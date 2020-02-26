#include "LightHelper.h"

using namespace DirectX;

//Generateº¯Êý
void LightHelper::GenerateViewMatrix(PointShadow& light , XMFLOAT3 m_LookAt)
{
	XMVECTOR EyePostion = XMLoadFloat3(&light.position);
	XMVECTOR LookAt = XMLoadFloat3(&m_LookAt);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&light.projView, XMMatrixLookAtLH(EyePostion, LookAt, Up));
}

void LightHelper::GenerateProjMatrix(PointShadow& light ,float fovY, float aspect, float nearZ, float farZ)
{
	XMStoreFloat4x4(&light.projProj, XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ));
}

XMMATRIX LightHelper::GetViewMatrixXM(PointShadow& light)
{
	return XMLoadFloat4x4(&light.projView);
}

XMMATRIX LightHelper::GetProjMatrixXM(PointShadow& light)
{
	return XMLoadFloat4x4(&light.projProj);
}

XMFLOAT4X4 LightHelper::GetViewMatrix(PointShadow& light)
{
	return light.projView;
}

XMFLOAT4X4 LightHelper::GetProjMatrix(PointShadow& light)
{
	return light.projProj;
}

