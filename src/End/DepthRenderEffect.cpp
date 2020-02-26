// *****************************************************
// 深度获取特效管理
// *****************************************************

#include "Effects.h"
#include "d3dUtil.h"
#include "EffectHelper.h"
#include "DXTrace.h"
#include "Vertex.h"
using namespace DirectX;


class DepthRenderEffect::Impl : public AlignedType<DepthRenderEffect::Impl>
{
public:

	struct CBChangesEveryFrame
	{
		XMMATRIX worldViewProj;
	};

public:
	// 必须显式指定
	Impl() : m_IsDirty() {}
	~Impl() = default;

public:
	CBufferObject<0, CBChangesEveryFrame>	m_CBFrame;	        // 每帧绘制的常量缓冲区

	BOOL m_IsDirty;										        // 是否有值变更
	std::vector<CBufferBase*> m_pCBuffers;				        // 统一管理上面所有的常量缓冲区

	ComPtr<ID3D11VertexShader> m_pDepthVS;
	ComPtr<ID3D11PixelShader> m_pDepthPS;

	ComPtr<ID3D11InputLayout> m_pVertexPosLayout;

	ComPtr<ID3D11ShaderResourceView> m_pTexture;
};


namespace
{
	// SkyEffect单例
	static DepthRenderEffect * g_pInstance = nullptr;
}


DepthRenderEffect::DepthRenderEffect()
{
	if (g_pInstance)
		throw std::exception("BasicEffect is a singleton!");
	g_pInstance = this;
	pImpl = std::make_unique<DepthRenderEffect::Impl>();
}

DepthRenderEffect::~DepthRenderEffect()
{
}

DepthRenderEffect::DepthRenderEffect(DepthRenderEffect && moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
}

DepthRenderEffect & DepthRenderEffect::operator=(DepthRenderEffect && moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

DepthRenderEffect & DepthRenderEffect::Get()
{
	if (!g_pInstance)
		throw std::exception("BasicEffect needs an instance!");
	return *g_pInstance;
}

bool DepthRenderEffect::InitAll(ID3D11Device * device)
{
	if (!device)
		return false;

	if (!pImpl->m_pCBuffers.empty())
		return true;

	if (!RenderStates::IsInit())
		throw std::exception("RenderStates need to be initialized first!");

	ComPtr<ID3DBlob> blob;


	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\DepthShader_VS.cso", L"HLSL\\DepthShader_VS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pDepthVS.GetAddressOf()));
	// 创建顶点布局
	HR(device->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pVertexPosLayout.GetAddressOf()));


	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\DepthShader_PS.cso", L"HLSL\\DepthShader_PS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pDepthPS.GetAddressOf()));


	pImpl->m_pCBuffers.assign({
		&pImpl->m_CBFrame,
		});

	// 创建常量缓冲区
	for (auto& pBuffer : pImpl->m_pCBuffers)
	{
		HR(pBuffer->CreateBuffer(device));
	}

	// 设置调试对象名
	D3D11SetDebugObjectName(pImpl->m_pVertexPosLayout.Get(), "DepthRenderEffect.VertexPosColorLayout");
	D3D11SetDebugObjectName(pImpl->m_pCBuffers[0]->cBuffer.Get(), "DepthRenderEffect.CBFrame");
	D3D11SetDebugObjectName(pImpl->m_pDepthVS.Get(), "DepthRenderEffect.DepthShader_VS");
	D3D11SetDebugObjectName(pImpl->m_pDepthPS.Get(), "DepthRenderEffect.DepthShader_PS");

	return true;
}

void DepthRenderEffect::SetRenderDefault(ID3D11DeviceContext * deviceContext)
{
	deviceContext->IASetInputLayout(pImpl->m_pVertexPosLayout.Get());
	deviceContext->VSSetShader(pImpl->m_pDepthVS.Get(), nullptr, 0);
	deviceContext->PSSetShader(pImpl->m_pDepthPS.Get(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->GSSetShader(nullptr, nullptr, 0);
	deviceContext->RSSetState(nullptr);

	deviceContext->OMSetDepthStencilState(RenderStates::DSSLessEqual.Get(), 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV DepthRenderEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.worldViewProj = XMMatrixTranspose(W * V * P);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV DepthRenderEffect::SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.worldViewProj = XMMatrixTranspose(WVP);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}


void DepthRenderEffect::Apply(ID3D11DeviceContext * deviceContext)
{
	auto& pCBuffers = pImpl->m_pCBuffers;
	pCBuffers[0]->BindVS(deviceContext);

	if (pImpl->m_IsDirty)
	{
		pImpl->m_IsDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}

