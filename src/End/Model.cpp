#include "Model.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace DirectX;

wchar_t *multiByteToWideChar(std::string pKey)
{
	const char* pCStrKey = pKey.c_str();
	//第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
	int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
	wchar_t *pWCStrKey = new wchar_t[pSize];
	//第二次调用将单字节字符串转换成双字节字符串
	MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
	return pWCStrKey;
}


Model::Model()
	: modelParts(), boundingBox(), vertexStride()
{
}


//Model::Model(ID3D11Device * device, const void* vertices, UINT vertexSize, UINT vertexCount,
//	const void * indices, UINT indexCount, DXGI_FORMAT indexFormat, Material& material)
//	: modelParts(), boundingBox(), vertexStride()
//{
//	SetMesh(device, vertices, vertexSize, vertexCount, indices, indexCount, indexFormat, material);
//}

void Model::SetMesh(ID3D11Device * device, const void * vertices, UINT vertexSize, UINT vertexCount, const void * indices,
				UINT indexCount, DXGI_FORMAT indexFormat, Material& material, std::string texFilename)
{
	//vertexStride = vertexSize;

	//modelParts.resize(1);

	//modelParts[0].vertexCount = vertexCount;
	//modelParts[0].indexCount = indexCount;
	//modelParts[0].indexFormat = indexFormat;

	//modelParts[0].material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//modelParts[0].material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//modelParts[0].material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//// 设置顶点缓冲区描述
	//D3D11_BUFFER_DESC vbd;
	//ZeroMemory(&vbd, sizeof(vbd));
	//vbd.Usage = D3D11_USAGE_IMMUTABLE;
	//vbd.ByteWidth = vertexSize * vertexCount;
	//vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbd.CPUAccessFlags = 0;
	//// 新建顶点缓冲区
	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = vertices;
	//HR(device->CreateBuffer(&vbd, &InitData, modelParts[0].vertexBuffer.ReleaseAndGetAddressOf()));

	//// 设置索引缓冲区描述
	//D3D11_BUFFER_DESC ibd;
	//ZeroMemory(&ibd, sizeof(ibd));
	//ibd.Usage = D3D11_USAGE_IMMUTABLE;
	//if (indexFormat == DXGI_FORMAT_R16_UINT)
	//{
	//	ibd.ByteWidth = indexCount * (UINT)sizeof(WORD);
	//}
	//else
	//{
	//	ibd.ByteWidth = indexCount * (UINT)sizeof(DWORD);
	//}
	//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ibd.CPUAccessFlags = 0;
	//// 新建索引缓冲区
	//InitData.pSysMem = indices;
	//HR(device->CreateBuffer(&ibd, &InitData, modelParts[0].indexBuffer.ReleaseAndGetAddressOf()));

	vertexStride = vertexSize;
	ModelPart modelpart;

	modelpart.vertexCount = vertexCount;
	modelpart.indexCount = indexCount;
	modelpart.indexFormat = indexFormat;

	modelpart.material = material;

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = vertexSize * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &InitData, modelpart.vertexBuffer.ReleaseAndGetAddressOf()));

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	if (indexFormat == DXGI_FORMAT_R16_UINT)
	{
		ibd.ByteWidth = indexCount * (UINT)sizeof(WORD);
	}
	else
	{
		ibd.ByteWidth = indexCount * (UINT)sizeof(DWORD);
	}
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &InitData, modelpart.indexBuffer.ReleaseAndGetAddressOf()));

	// texture
	if (texFilename.size() == 0) {}
	if (texFilename.substr(texFilename.find_last_of(".") + 1) == "dds")
	{
		wchar_t *wc = multiByteToWideChar(texFilename);

		HR(CreateDDSTextureFromFile(device,
			wc,
			nullptr,
			modelpart.texDiffuse.GetAddressOf()));
	}
	else
	{
		wchar_t *wc = multiByteToWideChar(texFilename);
		HR(CreateWICTextureFromFile(device, wc, nullptr, modelpart.texDiffuse.GetAddressOf()));
	}

	// append to model parts
	modelParts.push_back(modelpart);

	// bounding box
	BoundingBox bbs;
	BoundingBox::CreateFromPoints(bbs, vertexCount, (XMFLOAT3*)vertices, vertexStride);
	
	BoundingBox::CreateMerged(boundingBox, boundingBox, bbs);
}

void Model::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)

	size_t modelPartSize = modelParts.size();
	for (size_t i = 0; i < modelPartSize; ++i)
	{
		std::string vbName = name + ".part[" + std::to_string(i) + "].VertexBuffer";
		std::string ibName = name + ".part[" + std::to_string(i) + "].IndexBuffer";
		modelParts[i].vertexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(vbName.length()), vbName.c_str());
		modelParts[i].indexBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(ibName.length()), ibName.c_str());
	}

#else
	UNREFERENCED_PARAMETER(name);
#endif
}




