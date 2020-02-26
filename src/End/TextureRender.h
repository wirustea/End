
#ifndef TEXTURERENDER_H
#define TEXTURERENDER_H

#include <d3d11_1.h>
#include <wrl/client.h>
#include <string>

class DepthRender
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


	DepthRender(ID3D11Device * device,
		int texWidth, 
		int texHeight,
		bool generateMips = false);
	~DepthRender();

	// 开始对当前纹理进行渲染
	void Begin(ID3D11DeviceContext * deviceContext);
	// 结束对当前纹理的渲染，还原状态
	void End(ID3D11DeviceContext * deviceContext);
	// 获取渲染好的纹理
	ID3D11ShaderResourceView * GetOutputTexture();

	// 设置调试对象名
	void SetDebugObjectName(const std::string& name);

private:
	ComPtr<ID3D11Texture2D> m_pDepthTex;
	ComPtr<ID3D11ShaderResourceView>	m_pOutputTextureSRV;	// 输出的纹理对应的着色器资源视图
	ComPtr<ID3D11RenderTargetView>		m_pOutputTextureRTV;	// 输出的纹理对应的渲染目标视图
	ComPtr<ID3D11DepthStencilView>		m_pOutputTextureDSV;	// 输出纹理所用的深度/模板视图
	D3D11_VIEWPORT						m_OutputViewPort;	    // 输出所用的视口

	ComPtr<ID3D11RenderTargetView>		m_pCacheRTV;		    // 临时缓存的后备缓冲区
	ComPtr<ID3D11DepthStencilView>		m_pCacheDSV;		    // 临时缓存的深度/模板缓冲区
	D3D11_VIEWPORT						m_CacheViewPort;	    // 临时缓存的视口

	bool								m_GenerateMips;	        // 是否生成mipmap链
};



#endif