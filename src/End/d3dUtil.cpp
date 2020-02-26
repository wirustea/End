#include "d3dUtil.h"

using namespace DirectX;




// ------------------------------
// CreateTexture2DArray函数
// ------------------------------
// 根据已有纹理创建纹理数组
// [In]d3dDevice			D3D设备
// [In]d3dDeviceContext		D3D设备上下文
// [In]srcTexVec			存放纹理的数组
// [In]usage				D3D11_USAGE枚举值
// [In]bindFlags			D3D11_BIND_FLAG枚举值
// [In]cpuAccessFlags		D3D11_CPU_ACCESS_FLAG枚举值
// [In]miscFlags			D3D11_RESOURCE_MISC_FLAG枚举值
// [OutOpt]textureArray		输出的纹理数组资源
// [OutOpt]textureCubeView	输出的纹理数组资源视图
static HRESULT CreateTexture2DArray(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	std::vector<ID3D11Texture2D*>& srcTexVec,
	D3D11_USAGE usage,
	UINT bindFlags,
	UINT cpuAccessFlags,
	UINT miscFlags,
	ID3D11Texture2D** textureArray,
	ID3D11ShaderResourceView** textureArrayView);

//
// 函数定义部分
//

HRESULT CreateShaderFromFile(
	const WCHAR* csoFileNameInOut,
	const WCHAR* hlslFileName,
	LPCSTR entryPoint,
	LPCSTR shaderModel,
	ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// 寻找是否有已经编译好的顶点着色器
	if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
	{
		return hr;
	}
	else
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
		// 设置 D3DCOMPILE_DEBUG 标志用于获取着色器调试信息。该标志可以提升调试体验，
		// 但仍然允许着色器进行优化操作
		dwShaderFlags |= D3DCOMPILE_DEBUG;

		// 在Debug环境下禁用优化以避免出现一些不合理的情况
		dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		ID3DBlob* errorBlob = nullptr;
		hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
			dwShaderFlags, 0, ppBlobOut, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			}
			SAFE_RELEASE(errorBlob);
			return hr;
		}

		// 若指定了输出文件名，则将着色器二进制信息输出
		if (csoFileNameInOut)
		{
			return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
		}
	}

	return hr;
}

HRESULT CreateTexture2DArray(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	std::vector<ID3D11Texture2D*>& srcTexVec,
	D3D11_USAGE usage,
	UINT bindFlags,
	UINT cpuAccessFlags,
	UINT miscFlags,
	ID3D11Texture2D** textureArray,
	ID3D11ShaderResourceView** textureArrayView)
{

	if (!textureArray && !textureArrayView || !d3dDevice || !d3dDeviceContext || srcTexVec.empty())
		return E_INVALIDARG;

	HRESULT hResult;
	UINT arraySize = (UINT)srcTexVec.size();
	bool generateMips = (bindFlags & D3D11_BIND_RENDER_TARGET) &&
		(miscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS);
	// ******************
	// 创建纹理数组
	//

	D3D11_TEXTURE2D_DESC texDesc;
	srcTexVec[0]->GetDesc(&texDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDesc.Width;
	texArrayDesc.Height = texDesc.Height;
	texArrayDesc.MipLevels = generateMips ? 0 : texDesc.MipLevels;
	texArrayDesc.ArraySize = arraySize;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;		// 不能使用多重采样
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = usage;
	texArrayDesc.BindFlags = bindFlags;
	texArrayDesc.CPUAccessFlags = cpuAccessFlags;
	texArrayDesc.MiscFlags = miscFlags;

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);
	if (FAILED(hResult))
	{
		return hResult;
	}

	texArray->GetDesc(&texArrayDesc);
	// ******************
	// 将所有的纹理子资源赋值到纹理数组中
	//

	UINT minMipLevels = (generateMips ? 1 : texArrayDesc.MipLevels);
	// 每个纹理元素
	for (UINT i = 0; i < texArrayDesc.ArraySize; ++i)
	{
		// 纹理中的每个mipmap等级
		for (UINT j = 0; j < minMipLevels; ++j)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			// 允许映射索引i纹理中，索引j的mipmap等级的2D纹理
			d3dDeviceContext->Map(srcTexVec[i],
				j, D3D11_MAP_READ, 0, &mappedTex2D);

			d3dDeviceContext->UpdateSubresource(
				texArray,
				D3D11CalcSubresource(j, i, texArrayDesc.MipLevels),	// i * mipLevel + j
				nullptr,
				mappedTex2D.pData,
				mappedTex2D.RowPitch,
				mappedTex2D.DepthPitch);
			// 停止映射
			d3dDeviceContext->Unmap(srcTexVec[i], j);
		}
	}

	// ******************
	// 创建纹理数组的SRV
	//
	if (textureArrayView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		viewDesc.Texture2DArray.MostDetailedMip = 0;
		viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
		viewDesc.Texture2DArray.FirstArraySlice = 0;
		viewDesc.Texture2DArray.ArraySize = arraySize;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureArrayView);

		// 生成mipmaps
		if (hResult == S_OK && generateMips)
		{
			d3dDeviceContext->GenerateMips(*textureArrayView);
		}
	}

	// 检查是否需要纹理数组
	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		SAFE_RELEASE(texArray);
	}

	return hResult;
}


HRESULT CreateDDSTexture2DArrayFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::vector<std::wstring>& fileNames,
	ID3D11Texture2D** textureArray,
	ID3D11ShaderResourceView** textureArrayView,
	bool generateMips)
{
	// 检查设备、着色器资源视图、文件名数组是否非空
	if (!d3dDevice || !textureArrayView || fileNames.empty())
		return E_INVALIDARG;

	HRESULT hResult;
	// ******************
	// 读取所有纹理
	//

	UINT arraySize = (UINT)fileNames.size();
	std::vector<ID3D11Texture2D*> srcTexVec(arraySize);
	std::vector<D3D11_TEXTURE2D_DESC> texDescVec(arraySize);
	for (UINT i = 0; i < arraySize; ++i)
	{
		// 由于这些纹理并不会被GPU使用，我们使用D3D11_USAGE_STAGING枚举值
		// 使得CPU可以读取资源
		hResult = CreateDDSTextureFromFileEx(d3dDevice,
			fileNames[i].c_str(), 0, D3D11_USAGE_STAGING, 0,
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
			0, false, (ID3D11Resource**)&srcTexVec[i], nullptr);

		// 读取失败则释放之前读取的纹理并返回
		if (FAILED(hResult))
		{
			for (UINT j = 0; j < i; ++j)
				SAFE_RELEASE(srcTexVec[j]);
			return hResult;
		}

		// 读取创建好的纹理信息
		srcTexVec[i]->GetDesc(&texDescVec[i]);

		// 需要检验所有纹理的mipLevels，宽度和高度，数据格式是否一致，
		// 若存在数据格式不一致的情况，请使用dxtex.exe(DirectX Texture Tool)
		// 将所有的图片转成一致的数据格式
		if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
			texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
		{
			for (UINT j = 0; j < i; ++j)
				SAFE_RELEASE(srcTexVec[j]);
			return E_FAIL;
		}
	}

	hResult = CreateTexture2DArray(d3dDevice, d3dDeviceContext, srcTexVec,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | (generateMips ? D3D11_BIND_RENDER_TARGET : 0),
		0,
		(generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0),
		textureArray,
		textureArrayView);

	for (UINT i = 0; i < arraySize; ++i)
		SAFE_RELEASE(srcTexVec[i]);
	return hResult;
}

HRESULT CreateWICTexture2DArrayFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::vector<std::wstring>& fileNames,
	ID3D11Texture2D** textureArray,
	ID3D11ShaderResourceView** textureArrayView,
	bool generateMips)
{
	// 检查设备、着色器资源视图、文件名数组是否非空
	if (!d3dDevice || !textureArrayView || fileNames.empty())
		return E_INVALIDARG;

	HRESULT hResult;
	// ******************
	// 读取所有纹理
	//

	UINT arraySize = (UINT)fileNames.size();
	std::vector<ID3D11Texture2D*> srcTexVec(arraySize);
	std::vector<D3D11_TEXTURE2D_DESC> texDescVec(arraySize);
	for (UINT i = 0; i < arraySize; ++i)
	{
		// 由于这些纹理并不会被GPU使用，我们使用D3D11_USAGE_STAGING枚举值
		// 使得CPU可以读取资源
		hResult = CreateWICTextureFromFileEx(d3dDevice,
			fileNames[i].c_str(), 0, D3D11_USAGE_STAGING, 0,
			D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
			0, WIC_LOADER_DEFAULT, (ID3D11Resource**)&srcTexVec[i], nullptr);

		// 读取失败则释放之前读取的纹理并返回
		if (FAILED(hResult))
		{
			for (UINT j = 0; j < i; ++j)
				SAFE_RELEASE(srcTexVec[j]);
			return hResult;
		}

		// 读取创建好的纹理信息
		srcTexVec[i]->GetDesc(&texDescVec[i]);

		// 需要检验所有纹理的mipLevels，宽度和高度，数据格式是否一致，
		// 若存在数据格式不一致的情况，请使用图像处理软件统一处理，
		// 将所有的图片转成一致的数据格式
		if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
			texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
		{
			for (UINT j = 0; j < i; ++j)
				SAFE_RELEASE(srcTexVec[j]);
			return E_FAIL;
		}
	}

	hResult = CreateTexture2DArray(d3dDevice, d3dDeviceContext, srcTexVec,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE | (generateMips ? D3D11_BIND_RENDER_TARGET : 0),
		0,
		(generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0),
		nullptr,
		textureArrayView);

	for (UINT i = 0; i < arraySize; ++i)
		SAFE_RELEASE(srcTexVec[i]);
	return hResult;
}

HRESULT CreateWICTexture2DCubeFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::wstring & cubeMapFileName,
	ID3D11Texture2D ** textureArray,
	ID3D11ShaderResourceView ** textureCubeView,
	bool generateMips)
{
	// 检查设备、设备上下文是否非空
	// 纹理数组和纹理立方体视图只要有其中一个非空即可
	if (!d3dDevice || !d3dDeviceContext || !(textureArray || textureCubeView))
		return E_INVALIDARG;

	// ******************
	// 读取天空盒纹理
	//

	ID3D11Texture2D* srcTex = nullptr;
	ID3D11ShaderResourceView* srcTexSRV = nullptr;

	// 该资源用于GPU复制
	HRESULT hResult = CreateWICTextureFromFile(d3dDevice,
		(generateMips ? d3dDeviceContext : nullptr),
		cubeMapFileName.c_str(),
		(ID3D11Resource**)&srcTex,
		(generateMips ? &srcTexSRV : nullptr));

	// 文件未打开
	if (FAILED(hResult))
	{
		return hResult;
	}

	D3D11_TEXTURE2D_DESC texDesc, texArrayDesc;
	srcTex->GetDesc(&texDesc);

	// 要求宽高比4:3
	if (texDesc.Width * 3 != texDesc.Height * 4)
	{
		SAFE_RELEASE(srcTex);
		SAFE_RELEASE(srcTexSRV);
		return E_FAIL;
	}

	// ******************
	// 创建包含6个纹理的数组
	//

	UINT squareLength = texDesc.Width / 4;
	texArrayDesc.Width = squareLength;
	texArrayDesc.Height = squareLength;
	texArrayDesc.MipLevels = (generateMips ? texDesc.MipLevels - 2 : 1);	// 立方体的mip等级比整张位图的少2
	texArrayDesc.ArraySize = 6;
	texArrayDesc.Format = texDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	// 允许从中创建TextureCube

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);
	if (FAILED(hResult))
	{
		SAFE_RELEASE(srcTex);
		SAFE_RELEASE(srcTexSRV);
		return hResult;
	}

	// ******************
	// 选取原天空盒纹理的6个子正方形区域，拷贝到该数组中
	//

	D3D11_BOX box;
	// box坐标轴如下: 
	//    front
	//   / 
	//  /_____right
	//  |
	//  |
	//  bottom
	box.front = 0;
	box.back = 1;

	for (UINT i = 0; i < texArrayDesc.MipLevels; ++i)
	{
		// +X面拷贝
		box.left = squareLength * 2;
		box.top = squareLength;
		box.right = squareLength * 3;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_X, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// -X面拷贝
		box.left = 0;
		box.top = squareLength;
		box.right = squareLength;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_X, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// +Y面拷贝
		box.left = squareLength;
		box.top = 0;
		box.right = squareLength * 2;
		box.bottom = squareLength;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Y, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);


		// -Y面拷贝
		box.left = squareLength;
		box.top = squareLength * 2;
		box.right = squareLength * 2;
		box.bottom = squareLength * 3;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Y, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// +Z面拷贝
		box.left = squareLength;
		box.top = squareLength;
		box.right = squareLength * 2;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_POSITIVE_Z, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// -Z面拷贝
		box.left = squareLength * 3;
		box.top = squareLength;
		box.right = squareLength * 4;
		box.bottom = squareLength * 2;
		d3dDeviceContext->CopySubresourceRegion(
			texArray,
			D3D11CalcSubresource(i, D3D11_TEXTURECUBE_FACE_NEGATIVE_Z, texArrayDesc.MipLevels),
			0, 0, 0,
			srcTex,
			i,
			&box);

		// 下一个mipLevel的纹理宽高都是原来的1/2
		squareLength /= 2;
	}


	// ******************
	// 创建立方体纹理的SRV
	//
	if (textureCubeView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureCubeView);
	}

	// 检查是否需要纹理数组
	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		SAFE_RELEASE(texArray);
	}

	SAFE_RELEASE(srcTex);
	SAFE_RELEASE(srcTexSRV);

	return hResult;
}

HRESULT CreateWICTexture2DCubeFromFile(
	ID3D11Device * d3dDevice,
	ID3D11DeviceContext * d3dDeviceContext,
	const std::vector<std::wstring>& cubeMapFileNames,
	ID3D11Texture2D ** textureArray,
	ID3D11ShaderResourceView ** textureCubeView,
	bool generateMips)
{
	// 检查设备与设备上下文是否非空
	// 文件名数目需要不小于6
	// 纹理数组和资源视图只要有其中一个非空即可
	UINT arraySize = (UINT)cubeMapFileNames.size();

	if (!d3dDevice || !d3dDeviceContext || arraySize < 6 || !(textureArray || textureCubeView))
		return E_INVALIDARG;

	// ******************
	// 读取纹理
	//

	HRESULT hResult;
	std::vector<ID3D11Texture2D*> srcTexVec(arraySize, nullptr);
	std::vector<ID3D11ShaderResourceView*> srcTexSRVVec(arraySize, nullptr);
	std::vector<D3D11_TEXTURE2D_DESC> texDescVec(arraySize);

	for (UINT i = 0; i < arraySize; ++i)
	{
		// 该资源用于GPU复制
		hResult = CreateWICTextureFromFile(d3dDevice,
			(generateMips ? d3dDeviceContext : nullptr),
			cubeMapFileNames[i].c_str(),
			(ID3D11Resource**)&srcTexVec[i],
			(generateMips ? &srcTexSRVVec[i] : nullptr));

		// 读取创建好的纹理信息
		srcTexVec[i]->GetDesc(&texDescVec[i]);

		// 需要检验所有纹理的mipLevels，宽度和高度，数据格式是否一致，
		// 若存在数据格式不一致的情况，请使用dxtex.exe(DirectX Texture Tool)
		// 将所有的图片转成一致的数据格式
		if (texDescVec[i].MipLevels != texDescVec[0].MipLevels || texDescVec[i].Width != texDescVec[0].Width ||
			texDescVec[i].Height != texDescVec[0].Height || texDescVec[i].Format != texDescVec[0].Format)
		{
			for (UINT j = 0; j < i; ++j)
			{
				SAFE_RELEASE(srcTexVec[j]);
				SAFE_RELEASE(srcTexSRVVec[j]);
			}
			return E_FAIL;
		}
	}

	// ******************
	// 创建纹理数组
	//
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texDescVec[0].Width;
	texArrayDesc.Height = texDescVec[0].Height;
	texArrayDesc.MipLevels = (generateMips ? texDescVec[0].MipLevels : 1);
	texArrayDesc.ArraySize = arraySize;
	texArrayDesc.Format = texDescVec[0].Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;	// 允许从中创建TextureCube

	ID3D11Texture2D* texArray = nullptr;
	hResult = d3dDevice->CreateTexture2D(&texArrayDesc, nullptr, &texArray);

	if (FAILED(hResult))
	{
		for (UINT i = 0; i < arraySize; ++i)
		{
			SAFE_RELEASE(srcTexVec[i]);
			SAFE_RELEASE(srcTexSRVVec[i]);
		}

		return hResult;
	}

	// ******************
	// 将原纹理的所有子资源拷贝到该数组中
	//
	texArray->GetDesc(&texArrayDesc);

	for (UINT i = 0; i < arraySize; ++i)
	{
		for (UINT j = 0; j < texArrayDesc.MipLevels; ++j)
		{
			d3dDeviceContext->CopySubresourceRegion(
				texArray,
				D3D11CalcSubresource(j, i, texArrayDesc.MipLevels),
				0, 0, 0,
				srcTexVec[i],
				j,
				nullptr);
		}
	}

	// ******************
	// 创建立方体纹理的SRV
	//
	if (textureCubeView)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
		viewDesc.Format = texArrayDesc.Format;
		viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		viewDesc.TextureCube.MostDetailedMip = 0;
		viewDesc.TextureCube.MipLevels = texArrayDesc.MipLevels;

		hResult = d3dDevice->CreateShaderResourceView(texArray, &viewDesc, textureCubeView);
	}

	// 检查是否需要纹理数组
	if (textureArray)
	{
		*textureArray = texArray;
	}
	else
	{
		SAFE_RELEASE(texArray);
	}

	// 释放所有资源
	for (UINT i = 0; i < arraySize; ++i)
	{
		SAFE_RELEASE(srcTexVec[i]);
		SAFE_RELEASE(srcTexSRVVec[i]);
	}

	return hResult;
}
