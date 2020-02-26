#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <cstring>
#include <DirectXMath.h>


// 方向光
struct DirectionalLight
{
	DirectionalLight() = default;

	DirectionalLight(const DirectionalLight&) = default;
	DirectionalLight& operator=(const DirectionalLight&) = default;

	DirectionalLight(DirectionalLight&&) = default;
	DirectionalLight& operator=(DirectionalLight&&) = default;

	DirectionalLight(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT3& _direction) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), direction(_direction), pad() {}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	DirectX::XMFLOAT3 direction;
	float pad;
};

// 点光
struct PointLight
{
	PointLight() = default;

	PointLight(const PointLight&) = default;
	PointLight& operator=(const PointLight&) = default;

	PointLight(PointLight&&) = default;
	PointLight& operator=(PointLight&&) = default;

	PointLight(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT3& _position, float _range, const DirectX::XMFLOAT3& _att) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), position(_position), range(_range), att(_att), pad(){}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// (position, range)
	DirectX::XMFLOAT3 position;
	float range;

	// (A0, A1, A2, pad)
	DirectX::XMFLOAT3 att;
	float pad; 

};

// 聚光灯
struct SpotLight
{
	SpotLight() = default;

	SpotLight(const SpotLight&) = default;
	SpotLight& operator=(const SpotLight&) = default;

	SpotLight(SpotLight&&) = default;
	SpotLight& operator=(SpotLight&&) = default;

	SpotLight(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT3& _position, float _range, const DirectX::XMFLOAT3& _direction,
		float _spot, const DirectX::XMFLOAT3& _att) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), 
		position(_position), range(_range), direction(_direction), spot(_spot), att(_att), pad() {}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// (position, range)
	DirectX::XMFLOAT3 position;
	float range;

	// (direction, spot)
	DirectX::XMFLOAT3 direction;
	float spot;

	// (att, pad)
	DirectX::XMFLOAT3 att;
	float pad;

};

// car light
struct CarLight
{
	CarLight() = default;

	CarLight(const CarLight&) = default;
	CarLight& operator=(const CarLight&) = default;

	CarLight(CarLight&&) = default;
	CarLight& operator=(CarLight&&) = default;

	CarLight(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT3& _position, float _range, const DirectX::XMFLOAT3& _direction,
		float _spot, const DirectX::XMFLOAT3& _att) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular),
		bias(_position), range(_range), direction(_direction), spot(_spot), att(_att), pad() {}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// (position, range)
	DirectX::XMFLOAT3 bias;  // bias to car position
	float range;

	// (direction, spot)
	DirectX::XMFLOAT3 direction;
	float spot;

	// (att, pad)
	DirectX::XMFLOAT3 att;
	float pad;

};

// 物体表面材质
struct Material
{
	Material() = default;

	Material(const Material&) = default;
	Material& operator=(const Material&) = default;

	Material(Material&&) = default;
	Material& operator=(Material&&) = default;

	Material(const DirectX::XMFLOAT4& _ambient, const DirectX::XMFLOAT4& _diffuse, const DirectX::XMFLOAT4& _specular,
		const DirectX::XMFLOAT4& _reflect) :
		ambient(_ambient), diffuse(_diffuse), specular(_specular), Reflect(_reflect) {}

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular; // w = 镜面反射强度
	DirectX::XMFLOAT4 Reflect;
};

// shadow
struct PointShadow
{
	PointShadow() = default;

	PointShadow(const PointShadow&) = default;
	PointShadow& operator=(const PointShadow&) = default;

	PointShadow(PointShadow&&) = default;
	PointShadow& operator=(PointShadow&&) = default;

	PointShadow(const DirectX::XMFLOAT3& _position) :position(_position) {}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4X4 projView;
	DirectX::XMFLOAT4X4 projProj;
};

namespace LightHelper
{
	void GenerateViewMatrix(PointShadow& light, DirectX::XMFLOAT3 m_LookAt);
	void GenerateProjMatrix(PointShadow& light, float FiledOfView, float ScreenAspectRatio, float NearPlane, float FarPlane);
	DirectX::XMMATRIX GetViewMatrixXM(PointShadow& light);
	DirectX::XMMATRIX GetProjMatrixXM(PointShadow& light);
	DirectX::XMFLOAT4X4 GetViewMatrix(PointShadow& light);
	DirectX::XMFLOAT4X4 GetProjMatrix(PointShadow& light);
}




#endif
