//***************************************************************************************
// 特效管理框架
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#include <memory>
#include "LightHelper.h"
#include "RenderStates.h"


class IEffect
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;

	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	IEffect(IEffect&& moveFrom) = default;
	IEffect& operator=(IEffect&& moveFrom) = default;

	virtual ~IEffect() = default;

	virtual void Apply(ID3D11DeviceContext * deviceContext) = 0;
};



class BasicEffect : public IEffect
{
public:

	enum RenderType { RenderObject, RenderInstance };

	BasicEffect();
	virtual ~BasicEffect() override;

	BasicEffect(BasicEffect&& moveFrom) noexcept;
	BasicEffect& operator=(BasicEffect&& moveFrom) noexcept;

	// 获取单例
	static BasicEffect& Get();

	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device * device);


	// 
	// 渲染模式的变更
	//

	void SetRenderDefault(ID3D11DeviceContext * deviceContext, RenderType type); // 默认状态来绘制
	void SetRenderShadow(ID3D11DeviceContext * deviceContext, RenderType type);  // 阴影状态来绘制
	

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetCarWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);
	void XM_CALLCONV SetProjViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjProjMatrix(DirectX::FXMMATRIX P);
	void XM_CALLCONV SetCarViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetCarProjMatrix(DirectX::FXMMATRIX P);

	//
	// 光照、材质和纹理相关设置
	//

	// 各种类型灯光允许的最大数目
	static const int maxLights = 5;

	void SetDirLight(size_t pos, const DirectionalLight& dirLight);
	void SetPointLight(size_t pos, const PointLight& pointLight);
	void SetSpotLight(size_t pos, const SpotLight& spotLight);
	void SetCarLight(size_t pos, const CarLight & carLight);		//车灯

	void SetMaterial(const Material& material);
	void SetTextureUsed(bool isUsed);

	// Render to Texture 相关纹理
	void SetTextureDiffuse(ID3D11ShaderResourceView * textureDiffuse);	// 物体纹理
	void SetTextureCube(ID3D11ShaderResourceView * textureCube);		// 天空盒
	void SetProjTexture(ID3D11ShaderResourceView * texture);			// 虚拟点光源视角深度值
	void SetCarLightTexture(ID3D11ShaderResourceView * texture);		// 车灯视角深度值

	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos);

	//
	// 阴影
	//
	void SetShadow(bool isOn);

	// 雾效
	void SetFogState(bool isOn);
	void SetFogStart(float fogStart);
	void SetFogColor(DirectX::XMVECTOR fogColor);
	void SetFogRange(float fogRange);

	
	//
	// 状态开关设置
	//

	void SetReflectionEnabled(bool isEnable);
	void SetRefractionEnabled(bool isEnable);
	void SetRefractionEta(float eta);				  // 空气/介质折射比
	void Apply(ID3D11DeviceContext * deviceContext);  // 应用常量缓冲区和纹理资源的变更
	
private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};



class SkyEffect : IEffect
{
public:
	SkyEffect();
	virtual ~SkyEffect() override;

	SkyEffect(SkyEffect&& moveFrom) noexcept;
	SkyEffect& operator=(SkyEffect&& moveFrom) noexcept;

	// 获取单例
	static SkyEffect& Get();

	// 初始化Sky.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device * device);

	// 
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ID3D11DeviceContext * deviceContext);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P);
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP);

	//
	// 纹理立方体映射设置
	//

	void SetTextureCube(ID3D11ShaderResourceView * textureCube);


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext * deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};




class DepthRenderEffect : public IEffect
{
public:
	DepthRenderEffect();
	virtual ~DepthRenderEffect() override;

	DepthRenderEffect(DepthRenderEffect&& moveFrom) noexcept;
	DepthRenderEffect& operator=(DepthRenderEffect&& moveFrom) noexcept;

	// 获取单例
	static DepthRenderEffect& Get();

	// 初始化Shadow.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device * device);

	// 
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ID3D11DeviceContext * deviceContext);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX W, DirectX::CXMMATRIX V, DirectX::CXMMATRIX P);
	void XM_CALLCONV SetWorldViewProjMatrix(DirectX::FXMMATRIX WVP);


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext * deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};


#endif


