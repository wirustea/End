#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"

#include "WICTextureLoader.h"

using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance),
	m_CameraMode(CameraMode::FirstPerson),
	m_FogEnabled(true)
{
}

GameApp::~GameApp()
{
}


/**
 * @brief 初始化
 * @param 
 *
 * @return
 */
bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	// 务必先初始化所有渲染状态，以供下面的特效使用
	RenderStates::InitAll(m_pd3dDevice.Get());

	if (!m_BasicEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_SkyEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	if (!m_DepthEffect.InitAll(m_pd3dDevice.Get()))
		return false;


	if (!InitResource())
		return false;

	// 初始化鼠标
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}


/**
 * @brief 窗口尺寸变化时，变更视口更资源
 * @param
 *
 * @return
 */
void GameApp::OnResize()
{
	assert(m_pd2dFactory);
	assert(m_pdwriteFactory);

	// 释放D2D的相关资源
	m_pColorBrush.Reset();
	m_pd2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugStringW(L"\n warning:Direct2D and Direct3D limited\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			m_pColorBrush.GetAddressOf()));
		HR(m_pdwriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
	}
	else
	{
		// 报告异常问题
		assert(m_pd2dRenderTarget);
	}

	// 摄像机变更显示
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
		m_pCamera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
		m_BasicEffect.SetProjMatrix(m_pCamera->GetProjXM());
	}
}


/**
 * @brief 帧更新
 * @param dt(float) 每一帧的时间，单位秒
 *
 * @return
 */
void GameApp::UpdateScene(float dt)
{
	// 更新鼠标事件，获取相对偏移量
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	Keyboard::State keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	// --------------------------------------------------------------
	// 地图更新
	// 将当前坐标传入地图，以判断是否更新地图，如果更新则返回坐标偏移量（dx，dy）
	// --------------------------------------------------------------

	// 将坐标偏移传给地图类，进行地图与场景坐标更新
	double dx, dz;   
	bool res = m_Land.SetPosition(car.GetPosition(), dx, dz);
	if (res) car.MoveXZ(dx, dz);   //将偏移作用到物体

	// --------------------------------------------------------------
	// 运动控制
	// w/s分别为前向加速和后向加速
	// a/d分别为以车体中心为轴左右转向
	// --------------------------------------------------------------

	car.ReSetACC();  //重设加速度

	if (keyState.IsKeyDown(Keyboard::W))
		car.SpeedUp();   //加速，也就是将加速度设为地面加速度
		
	if (keyState.IsKeyDown(Keyboard::S))
	{
		car.brake();    //刹车，也就是将加速度设为反向地面加速度
	}
		
	if (keyState.IsKeyDown(Keyboard::A))
	{
		car.TurnTo(-dt);   
	}
	if (keyState.IsKeyDown(Keyboard::D))
	{
		car.TurnTo(dt);
	}

	car.UpdateSpeed(dt);     // 更新速度，这里不管加速还是刹车都会计算摩擦损耗
	car.MoveForward(dt);     // 根据加速度/位置/当前地形等移动
	car.TangentMove(dt);     // 根据地形斜率，车体前后方向摇摆

	// --------------------------------------------------------------
	// 第一人称相机模式
	// --------------------------------------------------------------

	if (m_CameraMode == CameraMode::FirstPerson)
	{
		if (mouseState.y != 0 || mouseState.x != 0)
		{
			cam1st->Pitch(mouseState.y * dt*PARAMS::st_factor * PARAMS::rotation_factor);
			cam1st->RotateY(mouseState.x * dt *PARAMS::st_factor* PARAMS::rotation_factor);
			cam1st->ReSetAutoMoveCount();
		}

		cam1st->SetPosition(car.GetPosition());					// 更新相机位置
		m_BasicEffect.SetEyePos(cam1st->GetPositionXM());		// 更新视线目标
		cam1st->MoveToFront(car.GetFront(), car.GetUp(),dt);    // 自动复位到车辆行驶方向
	}

	// --------------------------------------------------------------
	// 第三人称相机模式
	// --------------------------------------------------------------

	else if (m_CameraMode == CameraMode::ThirdPerson)
	{
		cam3rd->SetTarget(car.GetActualPosition());   //设置跟踪对象，也就是车体

		// 根据鼠标移动方向与距离，绕车体旋转
		if (mouseState.y != 0 || mouseState.x != 0)
		{
			cam3rd->RotateX(mouseState.y * dt*PARAMS::st_factor * PARAMS::rotation_factor);
			cam3rd->RotateY(mouseState.x * dt*PARAMS::st_factor * PARAMS::rotation_factor);
			cam3rd->ReSetAutoMoveCount();
		}
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);    // 拉近与拉远
		m_BasicEffect.SetEyePos(cam3rd->GetPositionXM());				// 将观察方向更新至 Effiect类中，用于光照等模拟
		cam3rd->MoveToFront(car.GetFront(), dt*PARAMS::st_factor);      // 自动复位到车辆行驶方向
		//cam3rd->Adjust(m_Land);
	}

	// ---------------------------------
	// 更新相机观察矩阵，并更新到Effect类
	// ---------------------------------

	m_pCamera->UpdateViewMatrix();
	m_BasicEffect.SetViewMatrix(m_pCamera->GetViewXM());
	m_BasicEffect.SetCarWorldMatrix(car.GetWorldMatrixXM());

	m_PointShadow.position = { car.GetPosition().x + 5,car.GetPosition().y + 15, car.GetPosition().z + 5 };
	LightHelper::GenerateViewMatrix(m_PointShadow, car.GetPosition());
	m_BasicEffect.SetProjViewMatrix(LightHelper::GetViewMatrixXM(m_PointShadow));

	m_BasicEffect.SetCarViewMatrix(car.GetViewMatrixXM());

	// 重置滚轮值
	m_pMouse->ResetScrollWheelValue();
	

	// --------------------------------------------
	// 摄像机模式切换
	// --------------------------------------------

	if (m_KeyboardTracker.IsKeyPressed(Keyboard::D1) && m_CameraMode != CameraMode::FirstPerson)
	{
		if (!cam1st)
		{
			cam1st.reset(new FirstPersonCamera);
			cam1st->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
			cam1st->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
			m_pCamera = cam1st;
		}
		XMFLOAT3 pos = car.GetPosition();
		XMFLOAT3 target = { -car.GetFront().x+pos.x, -car.GetFront().y+pos.y, -car.GetFront().z+pos.z};
		cam1st->LookAt(pos, target, XMFLOAT3(0.0f,1.0f,0.0f));

		m_CameraMode = CameraMode::FirstPerson;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D3) && m_CameraMode != CameraMode::ThirdPerson)
	{
		if (!cam3rd)
		{
			cam3rd.reset(new ThirdPersonCamera);
			cam3rd->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
			cam3rd->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam3rd;
		}
		XMFLOAT3 target = car.GetPosition();
		cam3rd->SetTarget(target);
		cam3rd->SetDistance(10.0f);
		cam3rd->SetDistanceMinMax(3.0f, 20.0f);

		m_CameraMode = CameraMode::ThirdPerson;
	}

	// 退出程序，这里应向窗口发送销毁信息
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	
}


/**
 * @brief 绘制场景
 * @param void
 *
 * @return
 */
void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	
	// ----------------------------------
	// 生成动态天空盒
	// ----------------------------------

	// 仅第三人称视角时车体上的动态天空盒
	if (m_CameraMode == CameraMode::ThirdPerson)   
	{
		// 缓存当前资源
		m_pDaylight->Cache(m_pd3dImmediateContext.Get(), m_BasicEffect);

		// 以车的视角，获取六个面的视图
		for (int i = 0; i < 6; ++i)
		{
			m_pDaylight->BeginCapture(
				m_pd3dImmediateContext.Get(), m_BasicEffect, car.GetPosition(), static_cast<D3D11_TEXTURECUBE_FACE>(i));
			
			// 这里不绘制车体自身
			DrawScene(false);
		}

		// 恢复资源
		m_pDaylight->Restore(m_pd3dImmediateContext.Get(), m_BasicEffect, *m_pCamera);
	}
	

	// --------------------------
	// 绘制场景
	// --------------------------

	// 预先清空目标视图和深度模板视图
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制车体
	DrawScene(true);

	// --------------------------
	// 绘制Direct2D部分
	// --------------------------
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		std::wstring text = L"Switch Camera Mode: 1- First Person 3- Third Person\n"
			L"Esc for Exit\n"
			L"Mouse Wheel for Observation Distance\n";

		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}

	HR(m_pSwapChain->Present(0, 0));
}


/**
 * @brief 绘制
 * @param drawCar(bool) 是否绘制小车
 * 
 * @return
 */
void GameApp::DrawScene(bool drawCar)
{
	// basic effect 初始化
	m_BasicEffect.SetRenderDefault(m_pd3dImmediateContext.Get(), BasicEffect::RenderObject);
	m_BasicEffect.SetTextureUsed(true);

	// 获取光源视角下的深度视图
	m_BasicEffect.SetReflectionEnabled(false);
	m_BasicEffect.SetRefractionEnabled(false);

	// 以该光源视角，获取深度图
	m_DepthEffect.SetRenderDefault(m_pd3dImmediateContext.Get());
	m_pDepthRender[0]->Begin(m_pd3dImmediateContext.Get());
	car.Draw(m_pd3dImmediateContext.Get(), m_DepthEffect,
		LightHelper::GetViewMatrix(m_PointShadow), LightHelper::GetProjMatrix(m_PointShadow));
	m_Land.Draw(m_pd3dImmediateContext.Get(), m_DepthEffect,
		LightHelper::GetViewMatrix(m_PointShadow), LightHelper::GetProjMatrix(m_PointShadow));
	m_pDepthRender[0]->End(m_pd3dImmediateContext.Get());

	// 以车灯视角获取深度值
	m_pDepthRender[1]->Begin(m_pd3dImmediateContext.Get());
	m_Land.Draw(m_pd3dImmediateContext.Get(), m_DepthEffect, car.GetViewMatrix(), car.GetProjMatrix());
	m_pDepthRender[1]->End(m_pd3dImmediateContext.Get());
	
	// 结合 depth effect 获取的深度图，绘制带有阴影的地面
	m_BasicEffect.SetTextureUsed(true);
	m_BasicEffect.SetProjTexture(m_pDepthRender[0]->GetOutputTexture());
	m_BasicEffect.SetCarLightTexture(m_pDepthRender[1]->GetOutputTexture());
	m_BasicEffect.SetRenderShadow(m_pd3dImmediateContext.Get(), BasicEffect::RenderObject);
	m_Land.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	// 绘制小车，仅第三人称视角绘制完整小车
	if (drawCar && m_CameraMode==CameraMode::ThirdPerson)
	{
		m_BasicEffect.SetReflectionEnabled(true);
		m_BasicEffect.SetRefractionEnabled(false);
		m_BasicEffect.SetRenderDefault(m_pd3dImmediateContext.Get(), BasicEffect::RenderObject);
		car.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	}
	// 第一人称时仅绘制车灯
	else if(drawCar)
	{
		m_BasicEffect.SetReflectionEnabled(false);
		m_BasicEffect.SetRefractionEnabled(false);
		car.GetCarLightObj().Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	}

	// 绘制天空
	m_SkyEffect.SetRenderDefault(m_pd3dImmediateContext.Get());
	m_pDaylight->Draw(m_pd3dImmediateContext.Get(), m_SkyEffect,
		(drawCar ? *m_pCamera : m_pDaylight->GetCamera()), car.GetPosition().x, car.GetPosition().z);
}


/**
 * @brief 初始化资源
 * @param void
 *
 * @return 
 *      true  
 *		false
 */
bool GameApp::InitResource()
{
	
	Material modelPartMaterial;
	Model model;

	// -----------------------
	// 初始化地面 
	// -----------------------

	modelPartMaterial.ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	modelPartMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	modelPartMaterial.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Terrain.ReadRawFile("Texture\\height.raw");

	m_Terrain.SetTexture(m_pd3dDevice.Get(), "Texture\\stone.dds");

	m_Terrain.SetMaterial(modelPartMaterial);
	m_Terrain.InitTerrain(m_pd3dDevice.Get(), 400, 400, 63, 63, 0.15f);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);
	m_Land.Append(m_Terrain);

	m_Land.init(400,400);



	// -----------------------
	// 初始化天空盒相关
	// -----------------------

	m_pDaylight = std::make_unique<DynamicSkyRender>(
		m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(),
		L"Texture\\universe.jpg",
		5000.0f, 256);
	m_BasicEffect.SetTextureCube(m_pDaylight->GetDynamicTextureCube());


	// 初始化深度渲染器
	m_pDepthRender[0] = std::make_unique<DepthRender>(m_pd3dDevice.Get(),
		m_ClientWidth, m_ClientHeight);
	m_pDepthRender[1] = std::make_unique<DepthRender>(m_pd3dDevice.Get(),
		m_ClientWidth, m_ClientHeight);

	// 初始化小车
	car.init(m_pd3dDevice.Get(), &m_Land);

	// 初始化相机
	m_CameraMode = CameraMode::ThirdPerson;
	auto camera = std::shared_ptr<ThirdPersonCamera>(new ThirdPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetTarget(XMFLOAT3(0.0f, 0.5f, 0.0f));
	camera->SetDistance(10.0f);
	camera->SetDistanceMinMax(2.0f, 14.0f);

	m_BasicEffect.SetViewMatrix(m_pCamera->GetViewXM());
	m_BasicEffect.SetEyePos(m_pCamera->GetPositionXM());

	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	m_BasicEffect.SetProjMatrix(m_pCamera->GetProjXM());

	// 初始化光源
	DirectionalLight dirLight[4];
	dirLight[0].ambient = XMFLOAT4(0.04f, 0.04f, 0.04f, 1.0f);
	dirLight[0].diffuse = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	dirLight[0].specular = XMFLOAT4(0.05f, 0.05f, 0.05f, 1.0f);
	dirLight[0].direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	dirLight[1] = dirLight[0];
	dirLight[1].direction = XMFLOAT3(0.577f, -0.577f, 0.577f);
	dirLight[2] = dirLight[0];
	dirLight[2].direction = XMFLOAT3(0.577f, -0.577f, -0.577f);
	dirLight[3] = dirLight[0];
	dirLight[3].direction = XMFLOAT3(-0.577f, -0.577f, -0.577f);
	// 将光源绑定到 basic effect 上
	for (int i = 0; i < 4; ++i)
		m_BasicEffect.SetDirLight(i, dirLight[i]);

	// 将车车灯绑定到basic effect 上
	m_BasicEffect.SetCarLight(0, car.GetLights(true));
	m_BasicEffect.SetCarLight(1, car.GetLights(false));

	// 初始化用于生成阴影的虚拟点光
	m_PointShadow.position = XMFLOAT3(5.0f, 10.0f, 5.0f);
	LightHelper::GenerateProjMatrix(m_PointShadow, XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	car.GenerateProjMatrix(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	m_BasicEffect.SetProjProjMatrix(LightHelper::GetProjMatrixXM(m_PointShadow));
	m_BasicEffect.SetCarProjMatrix(car.GetProjMatrixXM());

	//初始化雾效
	m_BasicEffect.SetFogState(m_FogEnabled);
	m_BasicEffect.SetFogColor(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	m_BasicEffect.SetFogStart(30.0f);
	m_BasicEffect.SetFogRange(300.0f);

	return true;
}


