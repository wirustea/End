#include "TextureRender.h"
#include "d3dUtil.h"
#include "DXTrace.h"
using namespace Microsoft::WRL;

DepthRender::DepthRender(ID3D11Device * device, int texWidth, int texHeight, bool generateMips)
	: m_GenerateMips(generateMips), m_CacheViewPort()
{
	
	// 创建纹理
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = texWidth;
	depthBufferDesc.Height = texHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //24位是为了深度缓存，8位是为了模板缓存
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;  //注意深度缓存(纹理)的绑定标志
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;
	HR(device->CreateTexture2D(&depthBufferDesc, NULL, m_pDepthTex.GetAddressOf()));
	
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(m_pDepthTex.Get(),&dsvDesc,m_pOutputTextureDSV.GetAddressOf()));


	// 创建纹理对应的着色器资源视图

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	HR(device->CreateShaderResourceView(
		m_pDepthTex.Get(),
		&srvDesc,
		m_pOutputTextureSRV.GetAddressOf()));


	// 初始化视口
	
	m_OutputViewPort.Width = static_cast<float>(texWidth);
	m_OutputViewPort.Height = static_cast<float>(texHeight);
	m_OutputViewPort.TopLeftX = 0.0f;
	m_OutputViewPort.TopLeftY = 0.0f;
	m_OutputViewPort.MinDepth = 0.0f;
	m_OutputViewPort.MaxDepth = 1.0f;
}

DepthRender::~DepthRender()
{
}

void DepthRender::Begin(ID3D11DeviceContext * deviceContext)
{
	// 缓存渲染目标和深度模板视图
	deviceContext->OMGetRenderTargets(1, m_pCacheRTV.GetAddressOf(), m_pCacheDSV.GetAddressOf());
	// 缓存视口
	UINT num_Viewports = 1;
	deviceContext->RSGetViewports(&num_Viewports, &m_CacheViewPort);


	// 清空缓冲区
	float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearDepthStencilView(m_pOutputTextureDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// 设置渲染目标和深度模板视图

	ID3D11RenderTargetView* renderTarget[1] = { 0 };
	deviceContext->OMSetRenderTargets(1, renderTarget, m_pOutputTextureDSV.Get());
	// 设置视口
	deviceContext->RSSetViewports(1, &m_OutputViewPort);
}

void DepthRender::End(ID3D11DeviceContext * deviceContext)
{
	// 恢复默认设定
	deviceContext->RSSetViewports(1, &m_CacheViewPort);
	deviceContext->OMSetRenderTargets(1, m_pCacheRTV.GetAddressOf(), m_pCacheDSV.Get());

	// 若之前有指定需要mipmap链，则生成
	if (m_GenerateMips)
	{
		deviceContext->GenerateMips(m_pOutputTextureSRV.Get());
	}
	
	// 清空临时缓存的渲染目标视图和深度模板视图
	m_pCacheDSV.Reset();
	m_pCacheRTV.Reset();
}

ID3D11ShaderResourceView * DepthRender::GetOutputTexture()
{
	return m_pOutputTextureSRV.Get();
}

void DepthRender::SetDebugObjectName(const std::string& name)
{
#if (defined(DEBUG) || defined(_DEBUG)) && (GRAPHICS_DEBUGGER_OBJECT_NAME)
	std::string DSVName = name + ".TextureDSV";
	std::string SRVName = name + ".TextureSRV";
	std::string RTVName = name + ".TextureRTV";
	m_pOutputTextureDSV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(DSVName.length()), DSVName.c_str());
	m_pOutputTextureSRV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(SRVName.length()), SRVName.c_str());
	m_pOutputTextureRTV->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<UINT>(RTVName.length()), RTVName.c_str());
#else
	UNREFERENCED_PARAMETER(name);
#endif
}