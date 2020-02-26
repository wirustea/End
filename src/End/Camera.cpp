#include "Camera.h"
using namespace DirectX;

Camera::Camera()
	: m_Position(0.0f, 0.0f, 0.0f), m_Right(0.0f, 0.0f, 0.0f),
	m_Up(0.0f, 0.0f, 0.0f), m_Look(0.0f, 0.0f, 0.0f),
	m_NearZ(), m_FarZ(), m_FovY(), m_Aspect(),
	m_NearWindowHeight(), m_FarWindowHeight(),
	m_View(), m_Proj(), m_ViewPort(),m_autoMoveDely(PARAMS::camera_auto_move_dely),m_autoMoveCount(0)
{
}

Camera::~Camera()
{
}

DirectX::XMVECTOR Camera::GetPositionXM() const
{
	return XMLoadFloat3(&m_Position);
}

DirectX::XMFLOAT3 Camera::GetPosition() const
{
	return m_Position;
}

DirectX::XMVECTOR Camera::GetRightXM() const
{
	return XMLoadFloat3(&m_Right);
}

DirectX::XMFLOAT3 Camera::GetRight() const
{
	return m_Right;
}

DirectX::XMVECTOR Camera::GetUpXM() const
{
	return XMLoadFloat3(&m_Up);
}

DirectX::XMFLOAT3 Camera::GetUp() const
{
	return m_Up;
}

DirectX::XMVECTOR Camera::GetLookXM() const
{
	return XMLoadFloat3(&m_Look);
}

DirectX::XMFLOAT3 Camera::GetLook() const
{
	return m_Look;
}

float Camera::GetNearWindowWidth() const
{
	return m_Aspect * m_NearWindowHeight;
}

float Camera::GetNearWindowHeight() const
{
	return m_NearWindowHeight;
}

float Camera::GetFarWindowWidth() const
{
	return m_Aspect * m_FarWindowHeight;
}

float Camera::GetFarWindowHeight() const
{
	return m_FarWindowHeight;
}

DirectX::XMMATRIX Camera::GetViewXM() const
{
	return XMLoadFloat4x4(&m_View);
}

DirectX::XMMATRIX Camera::GetProjXM() const
{
	return XMLoadFloat4x4(&m_Proj);
}

DirectX::XMMATRIX Camera::GetViewProjXM() const
{
	return XMLoadFloat4x4(&m_View) * XMLoadFloat4x4(&m_Proj);
}

DirectX::XMFLOAT4X4 Camera::GetView() const
{
	return m_View;
}
DirectX::XMFLOAT4X4 Camera::GetProj() const
{
	return m_Proj;
}

D3D11_VIEWPORT Camera::GetViewPort() const
{
	return m_ViewPort;
}

void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = nearZ;
	m_FarZ = farZ;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);

	XMStoreFloat4x4(&m_Proj, XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ));
}

void Camera::SetViewPort(const D3D11_VIEWPORT & viewPort)
{
	m_ViewPort = viewPort;
}

void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
	m_ViewPort.TopLeftX = topLeftX;
	m_ViewPort.TopLeftY = topLeftY;
	m_ViewPort.Width = width;
	m_ViewPort.Height = height;
	m_ViewPort.MinDepth = minDepth;
	m_ViewPort.MaxDepth = maxDepth;
}

void Camera::ReSetAutoMoveCount()
{
	m_autoMoveCount = 0;
}

// ******************
// 第一人称/自由视角摄像机
//

FirstPersonCamera::FirstPersonCamera()
	: Camera()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

void FirstPersonCamera::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

void FirstPersonCamera::SetPosition(const DirectX::XMFLOAT3 & v)
{
	m_Position = v;
}

void XM_CALLCONV FirstPersonCamera::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up)
{
	LookTo(pos, target - pos, up);
}

void FirstPersonCamera::LookAt(const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & target, const DirectX::XMFLOAT3 & up)
{
	LookAt(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
}

void XM_CALLCONV FirstPersonCamera::LookTo(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR to, DirectX::FXMVECTOR up)
{
	XMVECTOR L = XMVector3Normalize(to);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(up, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&m_Position, pos);
	XMStoreFloat3(&m_Look, L);
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
}

void FirstPersonCamera::LookTo(const DirectX::XMFLOAT3 & pos, const DirectX::XMFLOAT3 & to, const DirectX::XMFLOAT3 & up)
{
	LookTo(XMLoadFloat3(&pos), XMLoadFloat3(&to), XMLoadFloat3(&up));
}

void FirstPersonCamera::Strafe(float d)
{
	XMVECTOR Pos = XMLoadFloat3(&m_Position);
	XMVECTOR Right = XMLoadFloat3(&m_Right);
	XMVECTOR Dist = XMVectorReplicate(d);
	// DestPos = Dist * Right + SrcPos
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(Dist, Right, Pos));
}

void FirstPersonCamera::Walk(float d)
{
	XMVECTOR Pos = XMLoadFloat3(&m_Position);
	XMVECTOR Right = XMLoadFloat3(&m_Right);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Front = XMVector3Normalize(XMVector3Cross(Right, Up));
	XMVECTOR Dist = XMVectorReplicate(d);
	// DestPos = Dist * Front + SrcPos
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(Dist, Front, Pos));
}

void FirstPersonCamera::MoveForward(float d)
{
	XMVECTOR Pos = XMLoadFloat3(&m_Position);
	XMVECTOR Look = XMLoadFloat3(&m_Look);
	XMVECTOR Dist = XMVectorReplicate(d);
	// DestPos = Dist * Look + SrcPos
	XMStoreFloat3(&m_Position, XMVectorMultiplyAdd(Dist, Look, Pos));
}

void FirstPersonCamera::Pitch(float rad)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_Right), rad);
	XMVECTOR Up = XMVector3TransformNormal(XMLoadFloat3(&m_Up), R);
	XMVECTOR Look = XMVector3TransformNormal(XMLoadFloat3(&m_Look), R);
	float cosPhi = XMVectorGetY(Look);
	// 将上下视野角度Phi限制在[2pi/9, 7pi/9]，
	// 即余弦值[-cos(2pi/9), cos(2pi/9)]之间
	if (fabs(cosPhi) > cosf(XM_2PI / 9))
		return;

	XMStoreFloat3(&m_Up, Up);
	XMStoreFloat3(&m_Look, Look);
}

void FirstPersonCamera::RotateY(float rad)
{
	XMMATRIX R = XMMatrixRotationY(rad);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void FirstPersonCamera::RotateZ(float rad)
{
	XMMATRIX R = XMMatrixRotationZ(rad);

	XMStoreFloat3(&m_Right, XMVector3TransformNormal(XMLoadFloat3(&m_Right), R));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(XMLoadFloat3(&m_Up), R));
	XMStoreFloat3(&m_Look, XMVector3TransformNormal(XMLoadFloat3(&m_Look), R));
}

void FirstPersonCamera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&m_Right);
	XMVECTOR U = XMLoadFloat3(&m_Up);
	XMVECTOR L = XMLoadFloat3(&m_Look);
	XMVECTOR P = XMLoadFloat3(&m_Position);

	// 保持摄像机的轴互为正交，且长度都为1
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L已经正交化，需要计算对应叉乘得到R
	R = XMVector3Cross(U, L);

	// 填充观察矩阵
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View = {
		m_Right.x, m_Up.x, m_Look.x, 0.0f,
		m_Right.y, m_Up.y, m_Look.y, 0.0f,
		m_Right.z, m_Up.z, m_Look.z, 0.0f,
		x, y, z, 1.0f
	};
}

void FirstPersonCamera::MoveToFront(XMFLOAT3 targetFront, DirectX::XMFLOAT3 targetUp, float dt)
{
	if (m_autoMoveCount < m_autoMoveDely)
	{
		m_autoMoveCount += dt;
		return;
	}

	//rock
	//float y_r = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_Up), XMLoadFloat3(&targetUp)));
	//float x_r = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_Right), XMLoadFloat3(&targetUp)));

	//float angle = atan(x_r / (y_r + 1e-10));
	//RotateZ(angle * 0.01);

	// target front
	float target_theta, target_phi;
	target_phi = acos(targetFront.y);
	target_theta = atan2(targetFront.z, (targetFront.x + 1e-10f));

	//look direction
	float src_theta, src_phi;
	src_phi = acos(-m_Look.y);
	src_theta = atan2(-m_Look.z, (-m_Look.x + 1e-10f));

	float dphi = target_phi - src_phi;
	float dtheta = target_theta - src_theta;

	dtheta = fabs(dtheta) < fabs(dtheta + 2 * XM_PI) ?
		(fabs(dtheta) < fabs(dtheta - 2 * XM_PI) ? dtheta : (fabs(dtheta + 2 * XM_PI) < fabs(dtheta - 2 * XM_PI) ? dtheta + 2 * XM_PI : dtheta - 2 * XM_PI)) :
		(fabs(dtheta + 2 * XM_PI) < fabs(dtheta - 2 * XM_PI) ? (dtheta + 2 * XM_PI) : (fabs(dtheta) < fabs(dtheta - 2 * XM_PI) ? dtheta : dtheta - 2 * XM_PI));

	if (dphi != 0) Pitch(dphi > 0 ? max(-dt*PARAMS::first_person_camera_pitch_factor, -0.2*dphi) : min(dt*PARAMS::first_person_camera_pitch_factor, -0.2*dphi));
	if (dtheta != 0) RotateY(dtheta > 0 ? max(-dt*PARAMS::first_person_camera_pitch_factor, -0.2*dtheta) : min(dt*PARAMS::first_person_camera_pitch_factor, -0.2*dtheta));
}

// ******************
// 第三人称摄像机
//

ThirdPersonCamera::ThirdPersonCamera()
	: Camera(), m_Target(), m_Distance(), m_MinDist(), m_MaxDist(), m_Theta(), m_Phi()
{
}

ThirdPersonCamera::~ThirdPersonCamera()
{
}

DirectX::XMFLOAT3 ThirdPersonCamera::GetTargetPosition() const
{
	return m_Target;
}

DirectX::XMVECTOR ThirdPersonCamera::GetTargetPositionXM() const
{
	return XMLoadFloat3(&m_Target);
}

float ThirdPersonCamera::GetDistance() const
{
	return m_Distance;
}

float ThirdPersonCamera::GetRotationX() const
{
	return m_Phi;
}

float ThirdPersonCamera::GetRotationY() const
{
	return m_Theta;
}

void ThirdPersonCamera::RotateX(float rad)
{
	m_Phi -= rad;
	// Phi range [pi/6, pi/2]，
	if (m_Phi < XM_PI / 8)
		m_Phi = XM_PI / 8;
	else if (m_Phi > XM_PIDIV2)
		m_Phi = XM_PIDIV2;

}

void ThirdPersonCamera::RotateY(float rad)
{
	m_Theta = XMScalarModAngle(m_Theta - rad);
}

void ThirdPersonCamera::Approach(float dist)
{
	m_Distance += dist;
	// distance range [m_MinDist, m_MaxDist]
	if (m_Distance < m_MinDist)
		m_Distance = m_MinDist;
	else if (m_Distance > m_MaxDist)
		m_Distance = m_MaxDist;
}

void ThirdPersonCamera::SetRotationX(float phi)
{
	m_Phi = XMScalarModAngle(phi);
	// Phi range [pi/6, pi/2]，
	if (m_Phi < XM_PI/8)
		m_Phi = XM_PI / 8;
	else if (m_Phi > XM_PIDIV2)
		m_Phi = XM_PIDIV2;
}

void ThirdPersonCamera::Adjust(Land& land)
{
	double x,y,z;
	
	x = m_Distance * sin(m_Phi)*cos(m_Theta)+m_Position.x;
	z = m_Distance * sin(m_Phi) *sin(m_Theta)+m_Position.z;
	y = m_Distance * cos(m_Phi)+ m_Position.y;

	while (land.GetHeight(x, z) > y-6)
	{
		RotateX(0.01);
		UpdateViewMatrix();
		x = m_Distance * sin(m_Phi)*cos(m_Theta) + m_Position.x;
		z = m_Distance * sin(m_Phi) *sin(m_Theta) + m_Position.z;
		y = m_Distance * cos(m_Phi) + m_Position.y;
	}
}

void ThirdPersonCamera::SetRotationY(float theta)
{
	m_Theta = XMScalarModAngle(theta);
}

void ThirdPersonCamera::SetTarget(const DirectX::XMFLOAT3 & target)
{
	m_Target = target;
}

void ThirdPersonCamera::SetDistance(float dist)
{
	m_Distance = dist;
}


void ThirdPersonCamera::SetDistanceMinMax(float minDist, float maxDist)
{
	m_MinDist = minDist;
	m_MaxDist = maxDist;
}


void ThirdPersonCamera::UpdateViewMatrix()
{
	float x = m_Target.x + m_Distance * sinf(m_Phi) * cosf(m_Theta);
	float z = m_Target.z + m_Distance * sinf(m_Phi) * sinf(m_Theta);
	float y = m_Target.y + m_Distance * cosf(m_Phi);
	m_Position = { x, y, z };
	XMVECTOR P = XMLoadFloat3(&m_Position);
	XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&m_Target) - P);
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), L));
	XMVECTOR U = XMVector3Cross(L, R);

	// update vectors
	XMStoreFloat3(&m_Right, R);
	XMStoreFloat3(&m_Up, U);
	XMStoreFloat3(&m_Look, L);

	m_View = {
		m_Right.x, m_Up.x, m_Look.x, 0.0f,
		m_Right.y, m_Up.y, m_Look.y, 0.0f,
		m_Right.z, m_Up.z, m_Look.z, 0.0f,
		-XMVectorGetX(XMVector3Dot(P, R)), -XMVectorGetX(XMVector3Dot(P, U)), -XMVectorGetX(XMVector3Dot(P, L)), 1.0f
	};
}


void ThirdPersonCamera::MoveToFront(XMFLOAT3 targetFront, float dt)
{
	if (m_autoMoveCount<m_autoMoveDely)
	{
		m_autoMoveCount += dt;
		return;
	}

	float target_theta, target_phi;

	target_phi = XM_PI/2.6; 
	target_theta = atan2(targetFront.z , (targetFront.x + 1e-10f));
	
	float dphi = target_phi -  m_Phi;

	float dtheta = target_theta - m_Theta;
	dtheta = fabs(dtheta) < fabs(dtheta + 2 * XM_PI) ?
		(fabs(dtheta) < fabs(dtheta - 2 * XM_PI) ? dtheta : (fabs(dtheta + 2 * XM_PI) < fabs(dtheta - 2 * XM_PI) ? dtheta + 2 * XM_PI : dtheta - 2 * XM_PI)):
		(fabs(dtheta + 2 * XM_PI) < fabs(dtheta - 2 * XM_PI) ? (dtheta + 2 * XM_PI) :( fabs(dtheta) < fabs(dtheta - 2 * XM_PI) ? dtheta: dtheta - 2 * XM_PI));


	if (dphi!=0) RotateX(dphi>0?max(-dt*PARAMS::third_person_camera_pitch_factor ,-0.2*dphi) : min(dt*PARAMS::third_person_camera_pitch_factor, -0.2*dphi));
	if (dtheta!=0) RotateY(dtheta > 0 ? max(-dt*PARAMS::third_person_camera_pitch_factor, -0.2*dtheta) : min(dt*PARAMS::third_person_camera_pitch_factor, -0.2*dtheta));
}
