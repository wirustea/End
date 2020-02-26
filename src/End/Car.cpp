#include "Car.h"

Car::Car():
m_WorldMatrix(
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 8.0f, 0.0f, 1.0f),
	m_Right(1.0f, 0.0f, 0.0f),
	m_Up(0.0f, 1.0f, 0.0f),
	m_Front(0.0f, 0.0f, 1.0f),
	m_Position(0.0f, 8.0f, 0.0f),
	m_ACCFRICTION(PARAMS::m_ACCFRICTION),
	m_ACC(0.0f),
	m_ACCAIR(PARAMS::m_ACCAIR), 
	m_ACCLAND(PARAMS::m_ACCLAND), 
	m_ACCG(PARAMS::m_ACCG), 
	m_MaxV(PARAMS::m_MaxV),
	m_MinV(PARAMS::m_MinV),
	m_leftLightPos({ 2.0f, -1.6f, -2.5f }),
	m_rightLightPos({ -2.0f, -1.6f, -2.5f }),
	m_State(STATE::AIR)
{
	using namespace DirectX;
	m_leftLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_leftLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_leftLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_leftLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_leftLight.spot = 12.0f;
	m_leftLight.range = 1000.0f;

	m_rightLight.ambient = m_leftLight.ambient;
	m_rightLight.diffuse = m_leftLight.diffuse;
	m_rightLight.specular = m_leftLight.specular;
	m_rightLight.att = m_leftLight.att;
	m_rightLight.spot = m_leftLight.spot;
	m_rightLight.range = m_leftLight.range;


	XMStoreFloat3(&m_leftLight.bias, m_leftLightPos);
	XMStoreFloat3(&m_rightLight.bias, m_rightLightPos);

	m_leftLight.direction = { -m_Front.x,-m_Front.y ,-m_Front.z };
	m_rightLight.direction = { -m_Front.x,-m_Front.y ,-m_Front.z };

}

void Car::init(ID3D11Device * device, Land *terrainPtr)
{
	using namespace DirectX;

	m_TerrainPtr = terrainPtr;

	Material modelPartMaterial;
	Model model;

	// 车球状玻璃罩
	modelPartMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	modelPartMaterial.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	modelPartMaterial.specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	modelPartMaterial.Reflect = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	model.AppendMesh(device, Geometry::CreateCarMask(), modelPartMaterial, "Texture\\car_mask.png");

	// 车主体
	modelPartMaterial.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 0.8f);
	modelPartMaterial.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.8f);
	modelPartMaterial.Reflect = XMFLOAT4(0.7f, 0.7f, 0.7f, 0.7f);
	model.AppendMesh(device, Geometry::CreateCarBody(), modelPartMaterial, "Texture\\stone.dds");
	model.AppendMesh(device, Geometry::CreateCarBaffle(), modelPartMaterial, "Texture\\stone.dds");
	model.AppendMesh(device, Geometry::CreateLeftCarLinker(), modelPartMaterial, "Texture\\stone.dds");
	model.AppendMesh(device, Geometry::CreateRightCarLinker(), modelPartMaterial, "Texture\\stone.dds");
	m_body.SetModel(std::move(model));

	// 车轮
	modelPartMaterial.ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	modelPartMaterial.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	modelPartMaterial.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.Reflect = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	model.AppendMesh(device, Geometry::CreateCarRightWheel(), modelPartMaterial, "Texture\\map_tex.png");
	m_rightWheel.SetModel(std::move(model));

	model.AppendMesh(device, Geometry::CreateCarLeftWheel(), modelPartMaterial, "Texture\\map_tex.png");
	m_leftWheel.SetModel(std::move(model));

	// 车灯（几何体）
	modelPartMaterial.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	modelPartMaterial.Reflect = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	model.AppendMesh(device, Geometry::CreateCarLight(), modelPartMaterial, "Texture\\car_light.jpg");
	m_light.SetModel(std::move(model));

	// 世界矩阵
	m_body.SetWorldMatrix(m_WorldMatrix);
	m_rightWheel.SetWorldMatrix(m_WorldMatrix);
	m_leftWheel.SetWorldMatrix(m_WorldMatrix);
	m_light.SetWorldMatrix(m_WorldMatrix);

	// 轮子边界点
	Geometry::GetCarSide(m_leftWheelVex, m_rightWheelVex);

	// 左右轮中心
	m_leftAxialCenterBias =Geometry::GetLeftAxialCenter();
	m_rightAxialCenterBias = Geometry::GetRightAxialCenter();

	// 车轴长度
	m_axialLength = XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_leftAxialCenterBias) - XMLoadFloat3(&m_rightAxialCenterBias)));

	m_leftTransMat = DirectX::XMMatrixTranslation(-m_leftAxialCenterBias.x, -m_leftAxialCenterBias.y, -m_leftAxialCenterBias.z);
	m_invLeftTransMat = DirectX::XMMatrixTranslation(m_leftAxialCenterBias.x, m_leftAxialCenterBias.y, m_leftAxialCenterBias.z);

	m_rightTransMat = DirectX::XMMatrixTranslation(-m_rightAxialCenterBias.x, -m_rightAxialCenterBias.y, -m_rightAxialCenterBias.z);
	m_invRightTransMat = DirectX::XMMatrixTranslation(m_rightAxialCenterBias.x, m_rightAxialCenterBias.y, m_rightAxialCenterBias.z);

	m_swingTransMat = DirectX::XMMatrixTranslation(0.0f, -m_rightAxialCenterBias.y, -m_rightAxialCenterBias.z);
	m_invSwingTransMat = DirectX::XMMatrixTranslation(0.0f, -m_rightAxialCenterBias.y, -m_rightAxialCenterBias.z);

	// 车轴中心点位置
	m_ActualPositionBias = { 0.0f, -m_rightAxialCenterBias.y, -m_rightAxialCenterBias.z };

	// 初始化
	float leftHeight;
	XMFLOAT3 leftPos, leftNorm;
	GetMinPoint(m_leftWheelVex, leftHeight, leftPos, leftNorm);
	float rightHeight;
	XMFLOAT3 rightPos, rightNorm;
	GetMinPoint(m_rightWheelVex, rightHeight, rightPos, rightNorm);
	m_leftWheelNorm = leftNorm;
	m_rightWheelNorm = rightNorm;
}

/**
 * @brief 前后向运动
 * @param dt(float) 帧时间，单位秒
 */
void Car::MoveForward(float dt)
{
	using namespace DirectX;

	//
	//1. 根据运动法向量与速度，计算车轮运动方向
	//
	XMVECTOR leftWheelDirection = XMVector3Normalize(-XMVector3Cross(XMLoadFloat3(&m_Right), XMLoadFloat3(&m_leftWheelNorm)));
	XMVECTOR rightWheelDirection = XMVector3Normalize(-XMVector3Cross(XMLoadFloat3(&m_Right), XMLoadFloat3(&m_rightWheelNorm)));
	
	leftWheelDirection *= m_V >= 0 ? 1.0f : -1.0f;
	rightWheelDirection *= m_V >= 0 ? 1.0f : -1.0f;
	
	//
	//2. 更新位置
	//
	XMFLOAT3 leftAxialCenterBias = GetLeftAxialCenterBias();
	XMFLOAT3 rightAxialCenterBias = GetRightAxialCenterBias();
	
	XMVECTOR oldAxi = XMLoadFloat3(&leftAxialCenterBias) - XMLoadFloat3(&rightAxialCenterBias);
	
	// 获得左右轮的预计运动向量（位移）
	XMVECTOR leftResVec = leftWheelDirection * XMVector3Length(XMLoadFloat3(&m_Front) * m_V * dt *PARAMS::st_factor) / XMVector3Length(XMLoadFloat3(&m_Front) * leftWheelDirection);
	XMVECTOR rightResVec = rightWheelDirection * XMVector3Length(XMLoadFloat3(&m_Front) * m_V * dt *PARAMS::st_factor) / XMVector3Length(XMLoadFloat3(&m_Front) * rightWheelDirection);
	
	// 根据左右轮高度，将两轮之间的距离限制在车轴长度
	bool leftOrigin = false;
	if (XMVectorGetY(leftResVec) + leftAxialCenterBias.y < XMVectorGetY(rightResVec) + rightAxialCenterBias.y)
	{
		rightResVec = (-oldAxi + rightResVec - leftResVec);
		rightResVec = XMVector3Normalize(rightResVec)*m_axialLength;
		rightResVec = leftResVec + rightResVec + oldAxi;
		leftOrigin = false;
	}
	else
	{
		leftResVec = (oldAxi + leftResVec - rightResVec);
		leftResVec = XMVector3Normalize(leftResVec)*m_axialLength;
		leftResVec = rightResVec + leftResVec - oldAxi;
		leftOrigin = true;
	}

	// 求出车体的位移
	XMVECTOR newDirection = (leftResVec + rightResVec) / 2;
	XMVECTOR newAxi = (leftResVec - rightResVec) + oldAxi;

	// 求出车体运动后的旋转角度并修正
	XMVECTOR angles = XMVector3AngleBetweenVectors(newAxi, oldAxi);
	float angle = XMVectorGetX(angles);
	
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Front), leftOrigin ? -angle : angle);

	// 更新向量信息，并更新世界矩阵
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Position, XMLoadFloat3(&m_Position) + newDirection);
	Car::UpdateWorldMatrix();
	
	//
	//3.计算纵向位移
	//

	// 分别找出左右两轮与地面差值（轮子顶点的高度减去对应的地面高度）最小的顶点
	float leftHeight;
	XMFLOAT3 leftPos, leftNorm;
	GetMinPoint(m_leftWheelVex, leftHeight, leftPos, leftNorm);
	float rightHeight;
	XMFLOAT3 rightPos, rightNorm;
	GetMinPoint(m_rightWheelVex, rightHeight, rightPos, rightNorm);
	
	// 计算左右轮与地面的偏差向量
	leftResVec = { 0.0f, -leftHeight ,0.0f };
	rightResVec = { 0.0f, -rightHeight ,0.0f };
	
	// 根据左右轮高度修正
	if (leftHeight < rightHeight)
	{
		leftOrigin = true;
	}
	else
	{
		leftOrigin = false;
	}
	rightResVec = (-oldAxi + rightResVec - leftResVec);
	float vlen = XMVectorGetX(XMVector3Length(rightResVec));
	rightResVec = XMVector3Normalize(rightResVec)*((vlen - m_axialLength) / 2 + m_axialLength);
	leftResVec = leftResVec + XMVector3Normalize(rightResVec)*(vlen - m_axialLength) / 2;
	rightResVec = leftResVec + rightResVec + oldAxi;

	//
	// 4.更新纵向位置和角度
	//

	// 计算车体位移
	newAxi = (leftResVec - rightResVec) + newAxi;
	newDirection = leftOrigin ? leftResVec : rightResVec;

	// 计算重力影响
	if (XMVectorGetY(newDirection)<-0.05 && m_GV*dt*PARAMS::st_factor < XMVectorGetX(XMVector3Length(newDirection)))
	{
		// 如果纵向位移长度比高度差值小，说明该步更新之后车体还是位于空中
		m_GV += m_ACCG * dt*PARAMS::st_factor;											// 更新纵向速度
		newDirection = XMVector3Normalize(newDirection) * m_GV*dt*PARAMS::st_factor;	// 根据速度计算纵向位移
		m_State = STATE::AIR;
	}
	else {
		// 如果纵向位移长度比高度差值大，说明该步更新之后车体触地
		m_State = STATE::LAND;
	}

	
	if (m_State == STATE::LAND)
	{
		// 当车体触地时，计算车体偏移角度（例如左轮先着地，这个时候需要以左轮为中心旋转车体）
		angles = XMVector3AngleBetweenVectors(newAxi, oldAxi);
		angle = XMVectorGetX(angles);

		// 根据重力加速度计算当前旋转角度
		if (XMVectorGetY(newDirection) >= -0.05)
		{
			m_GV = 0;
		}
		else
		{
			if (angle > 0.1)
			{
				m_GV += m_ACCG * dt*PARAMS::st_factor;
				angle *= m_GV *dt*PARAMS::st_factor; //
			}
			else
			{
				m_GV = 0;
			}
		}

		R = XMMatrixRotationAxis(XMLoadFloat3(&m_Front), leftOrigin ? angle : -angle);
		XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
		
	}
	XMStoreFloat3(&m_Position, XMLoadFloat3(&m_Position) + newDirection);
	Car::UpdateWorldMatrix();
	
	//
	// 5.更新轮胎所在处的未来运动方向，如果已经着地那么就是着地点的切向量，如果在空中，则维持不变
	//
	if (m_State == STATE::LAND)
	{
		m_leftWheelNorm = leftNorm;
		m_rightWheelNorm = rightNorm;
	}

	//
	// 6.轮胎旋转
	//
	m_leftWheelAngle -= m_V * dt*PARAMS::wheel_rotation_factor*PARAMS::st_factor;
	m_rightWheelAngle -= m_V * dt*PARAMS::wheel_rotation_factor*PARAMS::st_factor;
	m_leftWheelAngle = fmod(m_leftWheelAngle, XM_PI * 2);
	m_rightWheelAngle = fmod(m_rightWheelAngle, XM_PI * 2);
}

void Car::MoveXZ(double x, double z)
{
	
	m_WorldMatrix._41 += x;
	m_WorldMatrix._43 += z;
	m_Position.x += x;
	m_Position.z += z;
}

void Car::TurnTo(float rad)
{
	using namespace DirectX;

	// 在空中是无法转向
	if (m_State == STATE::AIR) return;

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Up),rad);
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Front, XMVector3TransformNormal(XMLoadFloat3(&m_Front), R));

	// 转向
	m_leftWheelAngle -= rad*2;
	m_rightWheelAngle += rad*2;
	m_leftWheelAngle = fmod(m_leftWheelAngle, XM_PI * 2);
	m_rightWheelAngle = fmod(m_rightWheelAngle, XM_PI * 2);

	Car::UpdateWorldMatrix();
	
}

void  Car::SpeedUp()
{
	if (m_State == STATE::LAND)
	{
		m_ACC = m_ACCLAND;
	}

}

/**
 * @brief 寻找当前点集中，与地面差值最小点, min(P.y-L(P.x, P.z).y)
 * @param WheelVex(vector<XMFLOAT3>)    // <in>  三维点集
 * @param height(float&)				// <out> 与地面的最小差值（负数则表示在地面以下）
 * @param pos(XMFLOAT3&)				// <out> 最小差值点的坐标
 * @param grad(XMFLOAT3&)				// <out> 最小差值点处在地面坐标（P.x,, L(P.x,P.z).y, P.z）处的法向量
 */
void Car::GetMinPoint(std::vector<DirectX::XMFLOAT3>& WheelVex, float& height, DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& grad)
{
	height = INT_MAX;
	float temp_height,terrain_height;

	for (auto &vec : WheelVex)
	{
		// 获取在世界坐标中的位置
		DirectX::XMVECTOR temp_vec = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&vec),DirectX::XMLoadFloat4x4(&m_WorldMatrix));
		DirectX::XMFLOAT3 vector;
		DirectX::XMStoreFloat3(&vector, temp_vec);

		// 求地面（x，z）点处的高度
		terrain_height = m_TerrainPtr->GetHeight(vector.x, vector.z);
		temp_height = vector.y - terrain_height;
		if (temp_height < height)
		{
			height = temp_height;
			pos = vector;
			grad = m_TerrainPtr->GetNorm(vector.x, vector.z);
		}
	}
}

CarLight& Car::GetLights(bool left)
{
	if (left) return m_leftLight;
	else return m_rightLight;
}

void Car::TangentMove(float dt)
{
	using namespace DirectX;

	// 在空中则不做车身调整
	if (m_State == STATE::AIR) return;

	// 计算车体正方向与所处地面法向量的夹角
	XMVECTOR direction = XMVector3Normalize(XMLoadFloat3(&m_leftWheelNorm) + XMLoadFloat3(&m_rightWheelNorm));
	float dphi = XMVectorGetX( XMVector3AngleBetweenVectors(XMLoadFloat3(&m_Up), direction));
	if (XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_Front), direction)) > 0)
		dphi = -dphi;

	// 增量修正
	if (fabs(dphi) > 0.1) Car::BodySwing(dphi > 0 ? max(- dt*PARAMS::body_swing_factor, -dphi) : min( dt*PARAMS::body_swing_factor, -dphi));
}

void Car::BodySwing(float rad)
{
	using namespace DirectX;

	XMMATRIX R = m_swingTransMat * XMMatrixRotationAxis(XMLoadFloat3(&m_Right),rad) * m_invSwingTransMat;

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right),R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Front, XMVector3TransformNormal(XMLoadFloat3(&m_Front), R));
}

void Car::WheelRotate()
{
	using namespace DirectX;
	
	// 轮胎旋转，通过车体世界矩阵更新轮胎世界矩阵
	m_leftWheel.SetWorldMatrix(
		m_leftTransMat * XMMatrixRotationX(m_leftWheelAngle) * m_invLeftTransMat* XMLoadFloat4x4(&m_leftWheel.m_WorldMatrix));
	m_rightWheel.SetWorldMatrix(
		m_rightTransMat * XMMatrixRotationX(m_rightWheelAngle) * m_invRightTransMat * XMLoadFloat4x4(&m_rightWheel.m_WorldMatrix));
}

double Car::GetSpeed() const
{
	return m_V;
}

GameObject& Car::GetCarLightObj()
{
	return m_light;
}


void Car::UpdateSpeed(float dt)
{
	// 根据加速度更新速度
	m_V += m_ACC * dt* PARAMS::st_factor;
	
	// 加入摩擦损耗
	if (m_V > 0)
	{
		m_V -= m_ACCFRICTION * dt * PARAMS::st_factor;
		if (m_V < 0) m_V = 0;
	}
	else
	{
		m_V += m_ACCFRICTION * dt *PARAMS::st_factor;
		if (m_V > 0) m_V = 0;
		
	}

	// 将速度限制在 [m_MinV,m_MaxV]
	m_V = min(max(m_MinV, m_V), m_MaxV);
}

void Car::brake()
{
	// 直接设为速度负方向
	m_ACC = -m_ACCLAND;
}

void Car::ReSetACC()
{
	if (m_State == STATE::LAND)
	{
		m_ACC = 0;
	}
	
}

void Car::Draw(ID3D11DeviceContext * deviceContext, BasicEffect & effect)
{
	m_body.Draw(deviceContext, effect);
	m_leftWheel.Draw(deviceContext, effect);
	m_rightWheel.Draw(deviceContext, effect);
	m_light.Draw(deviceContext, effect);
}


void Car::Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	m_body.Draw(deviceContext, effect,view,proj);
	m_leftWheel.Draw(deviceContext, effect, view, proj);
	m_rightWheel.Draw(deviceContext, effect, view, proj);
	m_light.Draw(deviceContext, effect, view, proj);
}


void Car::UpdateWorldMatrix()
{
	Car::m_WorldMatrix = {
	Car::m_Right.x, Car::m_Right.y, Car::m_Right.z, 0.0f,
	Car::m_Up.x, Car::m_Up.y, Car::m_Up.z, 0.0f,
	Car::m_Front.x, Car::m_Front.y, Car::m_Front.z, 0.0f,
	Car::m_Position.x, Car::m_Position.y, Car::m_Position.z, 1.0f
	};

	m_body.SetWorldMatrix(m_WorldMatrix);
	m_leftWheel.SetWorldMatrix(m_WorldMatrix);
	m_rightWheel.SetWorldMatrix(m_WorldMatrix);
	m_light.SetWorldMatrix(m_WorldMatrix);
	Car::WheelRotate();

	Car::GenerateViewMatrix();
}

DirectX::XMMATRIX Car::GetWorldMatrixXM() const
{
	return DirectX::XMLoadFloat4x4(&m_WorldMatrix);
}


DirectX::XMFLOAT3 Car::GetPosition() const
{
	return m_Position;
}

DirectX::XMFLOAT3 Car::GetActualPosition() const
{
	using namespace DirectX;
	XMFLOAT3 Pos;
	XMStoreFloat3(&Pos, XMLoadFloat3(&m_ActualPositionBias) + XMLoadFloat3(&m_Position));
	return Pos;
}

DirectX::XMFLOAT3 Car::GetFront() const
{
	return m_Front;
}

DirectX::XMFLOAT3 Car::GetUp() const
{
	return m_Up;
}

DirectX::XMFLOAT3 Car::GetLeftAxialCenterBias() const
{
	DirectX::XMFLOAT3 res;
	DirectX::XMVECTOR v = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_leftAxialCenterBias), DirectX::XMLoadFloat4x4(&m_WorldMatrix));
	DirectX::XMStoreFloat3(&res, v);
	return res;
}

DirectX::XMFLOAT3 Car::GetRightAxialCenterBias() const
{
	DirectX::XMFLOAT3 res;
	DirectX::XMVECTOR v = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&m_rightAxialCenterBias), DirectX::XMLoadFloat4x4(&m_WorldMatrix));
	DirectX::XMStoreFloat3(&res, v);
	return res;
}

void Car::GenerateViewMatrix()
{
	using namespace DirectX;
	XMVECTOR EyePostion = XMLoadFloat3(&m_Position);
	XMVECTOR LookAt = XMLoadFloat3(&m_Front);
	XMVECTOR Up = XMLoadFloat3(&m_Up);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixLookAtLH(EyePostion, EyePostion -LookAt, Up));
}


void Car::GenerateProjMatrix(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMStoreFloat4x4(&m_ProjMatrix, DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ));
}

DirectX::XMFLOAT4X4 Car::GetViewMatrix() const
{
	return m_ViewMatrix;
}

DirectX::XMMATRIX Car::GetViewMatrixXM() const
{
	return DirectX::XMLoadFloat4x4(&m_ViewMatrix);
}

DirectX::XMFLOAT4X4 Car::GetProjMatrix() const
{
	return m_ProjMatrix;
}

DirectX::XMMATRIX Car::GetProjMatrixXM() const
{
	return DirectX::XMLoadFloat4x4(&m_ProjMatrix);
}