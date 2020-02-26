//***************************************************************************************
// 游戏
//***************************************************************************************

#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "Camera.h"
#include "GameObject.h"
#include "SkyRender.h"
#include "terrain.h"
#include "Car.h"
#include "TextureRender.h"
#include "Land.h"
#include "params.h"

class GameApp : public D3DApp
{
public:
	// 摄像机模式
	enum class CameraMode { FirstPerson, ThirdPerson};
	// 球体当前模式
	enum class SphereMode { None, Reflection, Refraction };

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();

	void DrawScene(bool drawCenterSphere);

private:

	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;				    // 单色笔刷
	ComPtr<IDWriteFont> m_pFont;								// 字体
	ComPtr<IDWriteTextFormat> m_pTextFormat;					// 文本格式

	Car car;

	BasicEffect m_BasicEffect;								    // 对象渲染特效管理
	SkyEffect m_SkyEffect;									    // 天空盒特效管理
	DepthRenderEffect m_DepthEffect;							// create depth texture for shadow


	std::unique_ptr<DepthRender> m_pDepthRender[2];				// for shadow
	std::unique_ptr<DynamicSkyRender> m_pDaylight;			    // 天空盒(白天)
	std::unique_ptr<DynamicSkyRender> m_pSunset;				// 天空盒(日落)
	std::unique_ptr<DynamicSkyRender> m_pDesert;				// 天空盒(沙漠)								// 天空盒模式

	Terrain m_Terrain;  // land
	Land m_Land;

	GameObject asteroidRing;

	PointShadow m_PointShadow;									// point light for sky and shadow

	SphereMode m_SphereMode;									// 球渲染模式
	float m_Eta;												// 空气/介质折射率
	bool m_FogEnabled;


	std::shared_ptr<Camera> m_pCamera;						    // 摄像机
	CameraMode m_CameraMode;									// 摄像机模式

};


#endif