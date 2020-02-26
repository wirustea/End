//***************************************************************************************
// 几何网格模型
//***************************************************************************************

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <vector>
#include <string>
#include <map>
#include <functional>
#include "Vertex.h"

namespace Geometry
{
	// 网格数据
	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	struct MeshData
	{
		std::vector<VertexType> vertexVec;	// 顶点数组
		std::vector<IndexType> indexVec;	// 索引数组

		MeshData()
		{
			// 需检验索引类型合法性
			static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "The size of IndexType must be 2 bytes or 4 bytes!");
			static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
		}
	};

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateSphere(float radius = 1.0f, UINT levels = 20, UINT slices = 20,
		const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateCarMask(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateCarLeftWheel(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateCarRightWheel(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateCarBody(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	MeshData<VertexType, IndexType> CreateCarBaffle(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	inline void GetCarSide(std::vector<DirectX::XMFLOAT3>& leftVec, std::vector<DirectX::XMFLOAT3>& rightVec,
		float sphere_radius=2.6,float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30);

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	inline MeshData<VertexType, IndexType> CreateCarLight(float sphere_radius = 2.6, 
		float cylinder_radius = 1.8, float cylinder_width = 5.2, const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	inline MeshData<VertexType, IndexType> CreateLeftCarLinker(float sphere_radius=2.6, UINT sphere_levels=40, UINT sphere_slices=30,
		float cylinder_radius=1.8, float cylinder_width=5.2, UINT cylinder_slices=30,
		const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	inline MeshData<VertexType, IndexType> CreateRightCarLinker(float sphere_radius = 2.6, UINT sphere_levels = 40, UINT sphere_slices = 30,
		float cylinder_radius = 1.8, float cylinder_width = 5.2, UINT cylinder_slices = 30,
		const DirectX::XMFLOAT4 & color = { 1.0f, 1.0f, 1.0f, 1.0f });

	template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
	inline MeshData<VertexType, IndexType> CreateInCar(float sphere_radius, float cylinder_radius, float cylinder_width, const DirectX::XMFLOAT4 & color);


	inline DirectX::XMFLOAT3 GetLeftWheelCenter(float sphere_radius = 2.6, float cylinder_radius = 1.8, float cylinder_width = 5.2);
	inline DirectX::XMFLOAT3 GetRightWheelCenter(float sphere_radius = 2.6, float cylinder_radius = 1.8, float cylinder_width = 5.2);
	inline DirectX::XMFLOAT3 GetLeftAxialCenter(float sphere_radius = 2.6, float cylinder_radius = 1.8, float cylinder_width = 5.2);
	inline DirectX::XMFLOAT3 GetRightAxialCenter(float sphere_radius = 2.6, float cylinder_radius = 1.8, float cylinder_width = 5.2);
}


namespace Geometry
{
	namespace Internal
	{
		//
		// 以下结构体和函数仅供内部实现使用
		//

		struct VertexData
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 tangent;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 tex;
		};

		// 根据目标顶点类型选择性将数据插入
		template<class VertexType>
		inline void InsertVertexElement(VertexType& vertexDst, const VertexData& vertexSrc)
		{
			static std::string semanticName;
			static const std::map<std::string, std::pair<size_t, size_t>> semanticSizeMap = {
				{"POSITION", std::pair<size_t, size_t>(0, 12)},
				{"NORMAL", std::pair<size_t, size_t>(12, 24)},
				{"TANGENT", std::pair<size_t, size_t>(24, 40)},
				{"COLOR", std::pair<size_t, size_t>(40, 56)},
				{"TEXCOORD", std::pair<size_t, size_t>(56, 64)}
			};

			for (size_t i = 0; i < ARRAYSIZE(VertexType::inputLayout); i++)
			{
				semanticName = VertexType::inputLayout[i].SemanticName;
				const auto& range = semanticSizeMap.at(semanticName);
				memcpy_s(reinterpret_cast<char*>(&vertexDst) + VertexType::inputLayout[i].AlignedByteOffset,
					range.second - range.first,
					reinterpret_cast<const char*>(&vertexSrc) + range.first,
					range.second - range.first);
			}
		}
	}
	
	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateSphere(float radius, UINT levels, UINT slices, const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;

		MeshData<VertexType, IndexType> meshData;
		UINT vertexCount = 2 + (levels - 1) * (slices + 1);
		UINT indexCount = 6 * (levels - 1) * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		float phi = 0.0f, theta = 0.0f;
		float per_phi = XM_PI / levels;
		float per_theta = XM_2PI / slices;
		float x, y, z;

		// 放入顶端点
		vertexData = { XMFLOAT3(0.0f, radius, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		for (UINT i = 1; i < levels; ++i)
		{
			phi = per_phi * i;
			// 需要slices + 1个顶点是因为 起点和终点需为同一点，但纹理坐标值不一致
			for (UINT j = 0; j <= slices; ++j)
			{
				theta = per_theta * j;
				x = radius * sinf(phi) * cosf(theta);
				y = radius * cosf(phi);
				z = radius * sinf(phi) * sinf(theta);
				// 计算出局部坐标、法向量、Tangent向量和纹理坐标
				XMFLOAT3 pos = XMFLOAT3(x, y, z), normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));

				vertexData = { pos, normal, XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}

		// 放入底端点
		vertexData = { XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
			XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);


		// 逐渐放入索引
		if (levels > 1)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = 0;
				meshData.indexVec[iIndex++] = j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = j;
			}
		}


		for (UINT i = 1; i < levels - 1; ++i)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;

				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
			}
		}

		// 逐渐放入索引
		if (levels > 1)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = (levels - 2) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = (levels - 2) * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = (levels - 1) * (slices + 1) + 1;
			}
		}


		return meshData;
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarMask(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = (2 + (sphere_levels - 1) * (sphere_slices + 1))*1;
		UINT indexCount = (6 * (sphere_levels - 1) * sphere_slices)*1;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		// sphere mask
		float phi = 0.0f, theta = 0.0f;
		float per_phi = XM_PI / sphere_levels;
		float per_theta = XM_2PI / sphere_slices;
		float x, y, z;

		// put top vertexes
		vertexData = { XMFLOAT3(sphere_radius, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		for (UINT i = 1; i < sphere_levels; ++i)
		{

			phi = per_phi * i;

			for (UINT j = 0; j <= sphere_slices; ++j)
			{
				theta = per_theta * j;
				x = sphere_radius * sinf(phi) * cosf(theta);
				y = sphere_radius * cosf(phi);
				z = sphere_radius * sinf(phi) * sinf(theta);

				XMFLOAT3 pos = XMFLOAT3(y, x, z), normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));

				vertexData = { pos, normal, XMFLOAT4(0.0f ,-sinf(theta) , cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}

		// put bottom vertexes
		vertexData = { XMFLOAT3(-sphere_radius, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
			XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);


		UINT st = 0;
		UINT ss = (UINT)(sphere_levels*0.9);

		for (UINT i = 1; i < sphere_levels - 2; ++i)
		{
			if (i > ss || i < (sphere_levels - ss))
			{
				continue;
			}
			for (UINT j = 1; j <= sphere_slices; ++j)
			{
				st = min(i, sphere_levels - i - 1);
				if (j< st || j>(sphere_slices - st))
				{
					meshData.indexVec[iIndex++] = (i - 1) * (sphere_slices + 1) + j;
					meshData.indexVec[iIndex++] = i * (sphere_slices + 1) + j % (sphere_slices + 1) + 1;
					meshData.indexVec[iIndex++] = (i - 1) * (sphere_slices + 1) + j % (sphere_slices + 1) + 1;

					meshData.indexVec[iIndex++] = i * (sphere_slices + 1) + j % (sphere_slices + 1) + 1;
					meshData.indexVec[iIndex++] = (i - 1) * (sphere_slices + 1) + j;
					meshData.indexVec[iIndex++] = i * (sphere_slices + 1) + j;
				}
			}
		}

		return meshData;
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarRightWheel(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = 2 + (sphere_levels - 1) * (sphere_slices + 1) + 2 * (cylinder_slices + 1);
		UINT indexCount = 6 * (sphere_levels - 1) * sphere_slices + 6 * cylinder_slices;

		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		float h2 = cylinder_width / 2;
		float theta = 0.0f;
		float per_theta = -XM_2PI / cylinder_slices;
		int shift = vIndex;
		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;
		float x, y, z;

		int right_wheel_sphere_inner_start = vIndex;
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2, (cylinder_radius*0.9) * cosf(theta) + y_shift, (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(0.0f,-cosf(theta), -sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		// right wheel sphere
		float wheel_sphere_radius = sphere_radius;
		UINT wheel_sphere_levels = sphere_levels / 2;
		UINT wheel_sphere_slices = cylinder_slices;

		float total_theta = asinf(cylinder_radius*0.9 / wheel_sphere_radius);

		float center_x = h2 + sqrtf(wheel_sphere_radius* wheel_sphere_radius - (cylinder_radius * 0.9)*(cylinder_radius * 0.9));
		center_x *= 0.97;
		float center_z = (sphere_radius - cylinder_radius) / 2;
		float center_y = cylinder_radius - sphere_radius;

		float phi = 0.0f;
		theta = 0.0f;
		float per_phi = total_theta / wheel_sphere_levels;
		per_theta = XM_2PI / wheel_sphere_slices;

		center_x = -center_x;

		shift = vIndex;

		vertexData = { XMFLOAT3(center_x + wheel_sphere_radius, center_y, center_z), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		UINT right_sphere_outter_start;
		for (UINT i = 1; i < wheel_sphere_levels; ++i)
		{
			phi = per_phi * i;

			if (i == wheel_sphere_levels - 1) right_sphere_outter_start = vIndex;

			for (UINT j = 0; j <= wheel_sphere_slices; ++j)
			{
				theta = per_theta * j;
				y = wheel_sphere_radius * sinf(phi) * cosf(theta) + center_y;
				x = wheel_sphere_radius * cosf(phi) + center_x;
				z = wheel_sphere_radius * sinf(phi) * sinf(theta) + center_z;

				XMFLOAT3 pos = XMFLOAT3(x, y, z), normal;
				XMVECTOR cc = {center_x, center_y, center_z};
				XMStoreFloat3(&normal, -XMVector3Normalize(XMLoadFloat3(&pos)-cc));

				vertexData = { pos, normal, XMFLOAT4(-sinf(theta),0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}


		if (wheel_sphere_levels > 1)
		{
			for (UINT j = 0; j <= wheel_sphere_slices; ++j)
			{
				meshData.indexVec[iIndex++] = 0 + shift;
				meshData.indexVec[iIndex++] = j % (wheel_sphere_slices + 1) + 1 + shift;
				meshData.indexVec[iIndex++] = j + shift;
			}
		}


		for (UINT i = 1; i < wheel_sphere_levels - 1; ++i)
		{
			for (UINT j = 1; j <= wheel_sphere_slices; ++j)
			{
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j + shift;
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;
				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;

				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;
				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j + shift;
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j + shift;
			}
		}

		for (UINT i = 0; i < cylinder_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + right_wheel_sphere_inner_start;
			meshData.indexVec[iIndex++] = i + 1 + right_wheel_sphere_inner_start;
			meshData.indexVec[iIndex++] = right_sphere_outter_start + cylinder_slices - (i + 1);

			meshData.indexVec[iIndex++] = right_sphere_outter_start + cylinder_slices - (i + 1);
			meshData.indexVec[iIndex++] = right_sphere_outter_start + cylinder_slices - i;
			meshData.indexVec[iIndex++] = i + right_wheel_sphere_inner_start;
		}

		int right_wheel_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= (cylinder_slices + 2) + 1; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2*0.95,cylinder_radius*0.9 * cosf(theta) + y_shift , cylinder_radius*0.9 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2 * 0.95,cylinder_radius*0.95 * cosf(theta) + y_shift , cylinder_radius*0.95 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put bottom vertexes
		for (UINT i = 0; i <= (cylinder_slices + 2) + 1; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2*0.65,cylinder_radius*0.9 * cosf(theta) + y_shift , cylinder_radius*0.9 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2 * 0.65, cylinder_radius*0.95 * cosf(theta) + y_shift, cylinder_radius*0.95 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		for (UINT i = right_wheel_start; i <= (cylinder_slices + 2) * 4 + right_wheel_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1;
			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) * 4 + 1) + i + 1;

			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) * 4 + 1) + i + 1;
			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) * 4 + 1) + i;
			meshData.indexVec[iIndex++] = i;
		}

		// wheel linker

		theta = 0.0f;
		per_theta = -XM_2PI / cylinder_slices;


		right_wheel_sphere_inner_start = vIndex;
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2,(cylinder_radius*0.9) * cosf(theta) + y_shift , (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(0.0f,-cosf(theta), -sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2*0.90,cylinder_radius*0.90 * cosf(theta) + y_shift , cylinder_radius*0.90 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		for (UINT i = right_wheel_sphere_inner_start; i < cylinder_slices + right_wheel_sphere_inner_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i;
		}

		return meshData;
	}

	inline DirectX::XMFLOAT3 GetLeftAxialCenter(float sphere_radius, float cylinder_radius, float cylinder_width)
	{
		float h2 = cylinder_width / 2;
		float wheel_sphere_radius = sphere_radius;
		float center_x = h2;
		float center_z = (sphere_radius - cylinder_radius) / 2;
		float center_y = cylinder_radius - sphere_radius;
		return DirectX::XMFLOAT3(center_x, center_y, center_z);
	}

	inline DirectX::XMFLOAT3 GetRightAxialCenter(float sphere_radius,float cylinder_radius, float cylinder_width)
	{
		DirectX::XMFLOAT3 center = GetLeftAxialCenter(sphere_radius, cylinder_radius, cylinder_width);

		return DirectX::XMFLOAT3(-center.x, center.y, center.z);
	}

	inline DirectX::XMFLOAT3 GetLeftWheelCenter(float sphere_radius, float cylinder_radius, float cylinder_width)
	{
		float h2 = cylinder_width / 2;
		float wheel_sphere_radius = sphere_radius;
		float center_x = h2 + sqrtf(wheel_sphere_radius* wheel_sphere_radius - (cylinder_radius * 0.9)*(cylinder_radius * 0.9));
		center_x *= 0.97;
		center_x -= wheel_sphere_radius;
		float center_z = (sphere_radius - cylinder_radius) / 2;
		float center_y = cylinder_radius - sphere_radius;
		return DirectX::XMFLOAT3(center_x, center_y, center_z);
	}

	inline DirectX::XMFLOAT3 GetRightWheelCenter(float sphere_radius,float cylinder_radius, float cylinder_width)
	{
		DirectX::XMFLOAT3 center = GetLeftWheelCenter(sphere_radius, cylinder_radius, cylinder_width);

		return DirectX::XMFLOAT3(-center.x, center.y, center.z);
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarLeftWheel(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = 2 + (sphere_levels - 1) * (sphere_slices + 1) + 2 * (cylinder_slices + 1) ;
		UINT indexCount = 6 * (sphere_levels - 1) * sphere_slices + 6 * cylinder_slices ;

		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		// wheel inner sphere
		float h2 = cylinder_width / 2;
		float theta = 0.0f;
		float per_theta = -XM_2PI / cylinder_slices;
		int shift = vIndex;
		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;
		float x, y, z;

		int left_wheel_sphere_inner_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,(cylinder_radius*0.9) * cosf(theta) + y_shift , (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(0.0f,-cosf(theta), -sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// left wheel sphere
		float wheel_sphere_radius = sphere_radius;
		UINT wheel_sphere_levels = sphere_levels / 2;
		UINT wheel_sphere_slices = cylinder_slices;

		float total_theta = asinf(cylinder_radius*0.9 / wheel_sphere_radius);
		float center_x = h2 + sqrtf(wheel_sphere_radius* wheel_sphere_radius - (cylinder_radius * 0.9)*(cylinder_radius * 0.9));
		center_x *= 0.97;
		float center_z = (sphere_radius - cylinder_radius) / 2;
		float center_y = cylinder_radius - sphere_radius;

		float phi = 0.0f;
		theta = 0.0f;
		float per_phi = total_theta / wheel_sphere_levels;
		per_theta = XM_2PI / wheel_sphere_slices;

		shift = vIndex;

		vertexData = { XMFLOAT3(center_x - wheel_sphere_radius, center_y, center_z), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		UINT left_sphere_outter_start;
		for (UINT i = 1; i < wheel_sphere_levels; ++i)
		{
			phi = per_phi * i;

			if (i == wheel_sphere_levels - 1) left_sphere_outter_start = vIndex;

			for (UINT j = 0; j <= wheel_sphere_slices; ++j)
			{
				theta = per_theta * j;
				y = wheel_sphere_radius * sinf(XM_PI - phi) * cosf(theta) + center_y;
				x = wheel_sphere_radius * cosf(XM_PI - phi) + center_x;
				z = wheel_sphere_radius * sinf(XM_PI - phi) * sinf(theta) + center_z;

				XMFLOAT3 pos = XMFLOAT3(x, y, z), normal;
				XMVECTOR cc = { center_x, center_y, center_z };
				XMStoreFloat3(&normal, -XMVector3Normalize(XMLoadFloat3(&pos) - cc));

				vertexData = { pos, normal, XMFLOAT4(-sinf(theta),0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}

		if (wheel_sphere_levels > 1)
		{
			for (UINT j = 0; j <= wheel_sphere_slices; ++j)
			{
				meshData.indexVec[iIndex++] = 0 + shift;
				meshData.indexVec[iIndex++] = j + shift;
				meshData.indexVec[iIndex++] = j % (wheel_sphere_slices + 1) + 1 + shift;
			}
		}


		for (UINT i = 1; i < wheel_sphere_levels - 1; ++i)
		{
			for (UINT j = 1; j <= wheel_sphere_slices; ++j)
			{
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j + shift;
				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;

				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j % (wheel_sphere_slices + 1) + 1 + shift;
				meshData.indexVec[iIndex++] = (i - 1) * (wheel_sphere_slices + 1) + j + shift;
				meshData.indexVec[iIndex++] = i * (wheel_sphere_slices + 1) + j + shift;
			}
		}

		for (UINT i = 0; i < cylinder_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + left_wheel_sphere_inner_start;
			meshData.indexVec[iIndex++] = left_sphere_outter_start + cylinder_slices - (i + 1);
			meshData.indexVec[iIndex++] = i + 1 + left_wheel_sphere_inner_start;

			meshData.indexVec[iIndex++] = left_sphere_outter_start + cylinder_slices - (i + 1);
			meshData.indexVec[iIndex++] = i + left_wheel_sphere_inner_start;
			meshData.indexVec[iIndex++] = left_sphere_outter_start + cylinder_slices - i;
		}


		int left_wheel_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= (cylinder_slices+2)+1; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.95,cylinder_radius*0.95 * cosf(theta) + y_shift , cylinder_radius*0.95 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.95,cylinder_radius*0.9 * cosf(theta) + y_shift , cylinder_radius*0.9 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put bottom vertexes
		for (UINT i = 0; i <= (cylinder_slices+2)+1; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.65, cylinder_radius*0.95 * cosf(theta) + y_shift, cylinder_radius*0.95 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.65,cylinder_radius*0.9 * cosf(theta) + y_shift , cylinder_radius*0.9 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		for (UINT i = left_wheel_start; i <= (cylinder_slices+2)*4+ left_wheel_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) *4 + 1) + i + 1;
			meshData.indexVec[iIndex++] = i + 1;

			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) *4 + 1) + i + 1;
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = ((cylinder_slices + 2) *4 + 1) + i;
		}



		// wheel linker

		theta = 0.0f;
		per_theta = -XM_2PI / cylinder_slices;


		left_wheel_sphere_inner_start = vIndex;
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,(cylinder_radius*0.9) * cosf(theta) + y_shift , (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(0.0f,-cosf(theta), -sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.90,cylinder_radius*0.90 * cosf(theta) + y_shift , cylinder_radius*0.90 * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		for (UINT i = left_wheel_sphere_inner_start; i < cylinder_slices + left_wheel_sphere_inner_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1 ;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i ;
			meshData.indexVec[iIndex++] = i;
		}

		return meshData;
	}


	inline void GetCarSide(std::vector<DirectX::XMFLOAT3>&leftVec, std::vector<DirectX::XMFLOAT3>&rightVec,
		float sphere_radius,float cylinder_radius, float cylinder_width, UINT cylinder_slices)
	{
		using namespace DirectX;

		int vIndex = 0;
		XMFLOAT3 vertexData;

		// wheel
		float h2 = cylinder_width / 2;
		float theta = 0.0f;
		float per_theta = -XM_2PI / cylinder_slices;
		int shift = vIndex;
		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;
		
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = XMFLOAT3(h2, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift);
			leftVec.push_back(vertexData);
		}

		
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = XMFLOAT3(-h2, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift);
			rightVec.push_back(vertexData);
		}
	}


	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarBody(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = (2 * (cylinder_slices + 1) * 2)*2;
		UINT indexCount = (6 * cylinder_slices * 2)*2;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		// wheel
		float h2 = cylinder_width / 2;
		float theta = 0.0f;
		float per_theta = -XM_2PI / cylinder_slices;
		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;

		int left_wheel_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,cylinder_radius * cosf(theta) + y_shift , cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2*0.7, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		int right_wheel_start = vIndex;
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2*0.7, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// put index

		for (UINT i = 0; i < cylinder_slices/2.5; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i;
			meshData.indexVec[iIndex++] = i;
		}
		for (UINT i = cylinder_slices *0.8; i < cylinder_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i;
			meshData.indexVec[iIndex++] = i;
		}

		// right
		for (UINT i = right_wheel_start; i < cylinder_slices / 2.5 + right_wheel_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i;
		}
		for (UINT i = cylinder_slices *0.8 + right_wheel_start; i < cylinder_slices+ right_wheel_start; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i + 1;

			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = (cylinder_slices + 1) + i;
		}

		// wheel outter
		h2 = cylinder_width / 2;
		theta = 0.0f;
		per_theta = -XM_2PI / cylinder_slices;
		y_shift = cylinder_radius - sphere_radius;
		z_shift = (sphere_radius - cylinder_radius) / 2;

		int left_wheel_outter_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,(cylinder_radius)* cosf(theta) + y_shift , (cylinder_radius)* sinf(theta) + z_shift), XMFLOAT3(1.0f,0.0f, 0.0f),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		int right_wheel_outter_start = vIndex;
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2, (cylinder_radius)* cosf(theta) + y_shift, (cylinder_radius)* sinf(theta) + z_shift), XMFLOAT3(-1.0f,0.0f, 0.0f),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// wheel inner
		h2 = cylinder_width / 2;
		theta = 0.0f;
		per_theta = -XM_2PI / cylinder_slices;
		y_shift = cylinder_radius - sphere_radius;
		z_shift = (sphere_radius - cylinder_radius) / 2;

		int left_wheel_inner_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,(cylinder_radius*0.9) * cosf(theta) + y_shift , (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(1.0f,0.0f, 0.0f),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		int right_wheel_inner_start = vIndex;
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2, (cylinder_radius*0.9) * cosf(theta) + y_shift, (cylinder_radius*0.9) * sinf(theta) + z_shift), XMFLOAT3(-1.0f,0.0f, 0.0f),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		// left 
		for (UINT i = 0; i < cylinder_slices/2.5; ++i)
		{
			meshData.indexVec[iIndex++] = i + left_wheel_inner_start;
			meshData.indexVec[iIndex++] = i + 1 + left_wheel_inner_start;
			meshData.indexVec[iIndex++] = left_wheel_outter_start + i + 1;

			meshData.indexVec[iIndex++] = left_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = left_wheel_outter_start + i;
			meshData.indexVec[iIndex++] = i + left_wheel_inner_start;
		}
		for (UINT i = cylinder_slices *0.7; i < cylinder_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + left_wheel_inner_start;
			meshData.indexVec[iIndex++] = i + 1 + left_wheel_inner_start;
			meshData.indexVec[iIndex++] = left_wheel_outter_start + i + 1;

			meshData.indexVec[iIndex++] = left_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = left_wheel_outter_start + i;
			meshData.indexVec[iIndex++] = i + left_wheel_inner_start;
		}

		//right
		for (UINT i = 0; i < cylinder_slices / 2.5; ++i)
		{
			meshData.indexVec[iIndex++] = i + right_wheel_inner_start;
			meshData.indexVec[iIndex++] = right_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = i + 1 + right_wheel_inner_start;

			meshData.indexVec[iIndex++] = right_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = i + right_wheel_inner_start;
			meshData.indexVec[iIndex++] = right_wheel_outter_start + i;
		}
		for (UINT i = cylinder_slices *0.7; i < cylinder_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + right_wheel_inner_start;
			meshData.indexVec[iIndex++] = right_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = i + 1 + right_wheel_inner_start;

			meshData.indexVec[iIndex++] = right_wheel_outter_start + i + 1;
			meshData.indexVec[iIndex++] = i + right_wheel_inner_start;
			meshData.indexVec[iIndex++] = right_wheel_outter_start + i;
		}

		return meshData;
	}


	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateLeftCarLinker(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;

		int levels = sphere_levels / 2;
		int slices = cylinder_slices;
		float radius = cylinder_radius;

		MeshData<VertexType, IndexType> meshData;
		UINT vertexCount = 2 + (levels ) * (slices + 1);
		UINT indexCount = 7 * (levels - 1) * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		float h2 = cylinder_width / 2 *0.7;

		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;

		float phi = 0.0f, theta = 0.0f;
		float per_phi = XM_PIDIV2 / levels;
		float per_theta = XM_2PI / slices;
		float x, y, z;

		for (UINT i = 0; i < levels; ++i)
		{
			phi = per_phi * i+XM_PIDIV2;

			for (UINT j = 0; j <= slices; ++j)
			{
				theta = per_theta * j;
				y = radius * sinf(phi) * cosf(theta)+ y_shift;
				x = radius * cosf(phi) + h2;
				z = radius * sinf(phi) * sinf(theta)+z_shift;

				XMFLOAT3 pos = XMFLOAT3(x,y,z), normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));

				vertexData = { pos, normal, XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}


		for (UINT i = 1; i < levels ; ++i)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;

				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j;
			}
		}

		//// 逐渐放入索引
		//if (levels > 1)
		//{
		//	for (UINT j = 1; j <= slices; ++j)
		//	{
		//		meshData.indexVec[iIndex++] = (levels - 2) * (slices + 1) + j;
		//		meshData.indexVec[iIndex++] = (levels - 2) * (slices + 1) + j % (slices + 1) + 1;
		//		meshData.indexVec[iIndex++] = (levels - 1) * (slices + 1) + 1;
		//	}
		//}


		return meshData;
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateRightCarLinker(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;

		int levels = sphere_levels / 2;
		int slices = cylinder_slices;
		float radius = cylinder_radius;

		MeshData<VertexType, IndexType> meshData;
		UINT vertexCount = 2 + (levels) * (slices + 1);
		UINT indexCount = 7 * (levels - 1) * slices;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;

		float h2 = -cylinder_width / 2 * 0.7;

		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;

		float phi = 0.0f, theta = 0.0f;
		float per_phi = XM_PIDIV2 / levels;
		float per_theta = XM_2PI / slices;
		float x, y, z;

		vertexData = { XMFLOAT3(h2+radius, y_shift, z_shift), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		for (UINT i = 1; i <= levels; ++i)
		{
			phi = per_phi * i;

			for (UINT j = 0; j <= slices; ++j)
			{
				theta = per_theta * j;
				y = radius * sinf(phi) * cosf(theta) + y_shift;
				x = radius * cosf(phi) + h2;
				z = radius * sinf(phi) * sinf(theta) + z_shift;

				XMFLOAT3 pos = XMFLOAT3(x, y, z), normal;
				XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&pos)));

				vertexData = { pos, normal, XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, phi / XM_PI) };
				Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
			}
		}

		if (levels > 1)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = 0;
				meshData.indexVec[iIndex++] = j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = j;
			}
		}

		for (UINT i = 1; i <= levels; ++i)
		{
			for (UINT j = 1; j <= slices; ++j)
			{
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j % (slices + 1) + 1;

				meshData.indexVec[iIndex++] = i * (slices + 1) + j % (slices + 1) + 1;
				meshData.indexVec[iIndex++] = (i - 1) * (slices + 1) + j;
				meshData.indexVec[iIndex++] = i * (slices + 1) + j;
			}
		}


		return meshData;
	}


	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarBaffle(float sphere_radius, UINT sphere_levels, UINT sphere_slices,
		float cylinder_radius, float cylinder_width, UINT cylinder_slices,
		const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = (2 + (sphere_levels - 1) * (sphere_slices + 1)) * 1;
		UINT indexCount = (6 * (sphere_levels - 1) * sphere_slices) * 1;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;


		float h2 = cylinder_width / 2;
		float theta = 0.0f;
		float per_theta = -XM_2PI / cylinder_slices;
		int shift = vIndex;
		float y_shift = cylinder_radius - sphere_radius;
		float z_shift = (sphere_radius - cylinder_radius) / 2;

		int left_wheel_start = vIndex;
		// put top vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(h2,cylinder_radius * cosf(theta) + y_shift , cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		int right_wheel_start = vIndex;
		// put bottom vertexes
		for (UINT i = 0; i <= cylinder_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-h2, cylinder_radius * cosf(theta) + y_shift, cylinder_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		//shell
		int shell_slices = (float)cylinder_slices / 4;
		float shell_radius = 2 * cylinder_radius;
		y_shift = -sphere_radius;
		z_shift = (sphere_radius - cylinder_radius) / 2;
		h2 = cylinder_width / 2;

		float h2_s = sphere_radius * 0.94;
		float h2_e = sphere_radius * 0.35;
		float k = 3.0;
		float poly_a = (h2_e - h2_s) / pow(shell_slices, k), poly_b = h2_s;

		theta = 0.0f;
		float b = shell_radius, c = sqrtf(y_shift * y_shift + z_shift * z_shift);
		float t1 = acosf((b*b + c * c - sphere_radius * sphere_radius) / (2 * b*c));
		b = sphere_radius, c = cylinder_radius * 2;
		float pa = (cylinder_radius * 2) - sphere_radius;
		float t2 = acosf((b*b + c * c - (z_shift*z_shift + pa * pa)) / (2 * b*c));
		float total_theta = (t1 + t2)*1.05;
		per_theta = -total_theta / shell_slices;
		shift = vIndex;

		int left_shell_start = vIndex;

		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(poly_b + poly_a * pow(i,k), shell_radius * cosf(theta) + y_shift , shell_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		int right_shell_start = vIndex;

		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta;
			vertexData = { XMFLOAT3(-(poly_b + poly_a * pow(i,k)), shell_radius * cosf(theta) + y_shift, shell_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		for (UINT i = shift; i < shell_slices + shift; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1;
			meshData.indexVec[iIndex++] = (shell_slices + 1) + i + 1;

			meshData.indexVec[iIndex++] = (shell_slices + 1) + i + 1;
			meshData.indexVec[iIndex++] = (shell_slices + 1) + i;
			meshData.indexVec[iIndex++] = i;

		}

		//baffle
		float baffle_radius = 4 * cylinder_radius;
		float sy = cylinder_radius * 2 - sphere_radius;
		float sz = (sphere_radius - cylinder_radius) / 2;
		h2_s = sphere_radius * 0.35;
		h2_e = sphere_radius * 0.15;
		k = 0.5;
		poly_a = (h2_e - h2_s) / pow(shell_slices, k);
		poly_b = h2_s;

		theta = 0.0f;

		float baffle_sz = (sz - z_shift) * cosf(total_theta) - (sy - y_shift) * sinf(total_theta) + z_shift;
		float baffle_sy = (sz - z_shift) * sinf(total_theta) + (sy - y_shift) * cosf(total_theta) + y_shift;
		float baffle_theta = asinf((sphere_radius + baffle_sy) / baffle_radius);
		baffle_theta = baffle_theta * 0.9;

		y_shift = baffle_sy;
		z_shift = baffle_sz + baffle_radius;

		per_theta = -baffle_theta / shell_slices;
		shift = vIndex;


		// 放入侧面顶端点
		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta - XM_2PI / 4;
			vertexData = { XMFLOAT3(poly_b + poly_a * pow(i,k), baffle_radius * cosf(theta) + y_shift , baffle_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}
		int left_baffle_start = vIndex;
		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta - XM_2PI / 4;
			vertexData = { XMFLOAT3(poly_b + poly_a * pow(i,k), baffle_radius * cosf(theta) + y_shift , baffle_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta), sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}


		// 放入侧面底端点
		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta - XM_2PI / 4;
			vertexData = { XMFLOAT3(-(poly_b + poly_a * pow(i,k)), baffle_radius * cosf(theta) + y_shift, baffle_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}
		int right_baffle_start = vIndex;
		for (UINT i = 0; i <= shell_slices; ++i)
		{
			theta = i * per_theta - XM_2PI / 4;
			vertexData = { XMFLOAT3(-(poly_b + poly_a * pow(i,k)), baffle_radius * cosf(theta) + y_shift, baffle_radius * sinf(theta) + z_shift), XMFLOAT3(0.0f, cosf(theta) , sinf(theta)),
				XMFLOAT4(0.0f , -sinf(theta), cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };

			Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		}

		for (UINT i = shift; i < shell_slices + shift; ++i)
		{
			meshData.indexVec[iIndex++] = i;
			meshData.indexVec[iIndex++] = i + 1;
			meshData.indexVec[iIndex++] = (shell_slices * 2 + 1) + i + 1;

			meshData.indexVec[iIndex++] = (shell_slices * 2 + 2) + i + 1;
			meshData.indexVec[iIndex++] = (shell_slices * 2 + 2) + i;
			meshData.indexVec[iIndex++] = i + 1;
		}


		// connect between shell and wheel
		for (UINT i = 0; i < shell_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + left_wheel_start;
			meshData.indexVec[iIndex++] = i + left_wheel_start + 1;
			meshData.indexVec[iIndex++] = (i + left_shell_start + 1);

			meshData.indexVec[iIndex++] = (i + left_shell_start);
			meshData.indexVec[iIndex++] = i + left_wheel_start;
			meshData.indexVec[iIndex++] = (i + left_shell_start + 1);

			meshData.indexVec[iIndex++] = i + right_wheel_start;
			meshData.indexVec[iIndex++] = (i + right_shell_start + 1);
			meshData.indexVec[iIndex++] = i + right_wheel_start + 1;

			meshData.indexVec[iIndex++] = (i + right_shell_start);
			meshData.indexVec[iIndex++] = (i + right_shell_start + 1);
			meshData.indexVec[iIndex++] = i + right_wheel_start;

		}

		// connect between baffle and wheel
		for (UINT i = 0; i < shell_slices; ++i)
		{
			meshData.indexVec[iIndex++] = i + left_wheel_start + shell_slices;
			meshData.indexVec[iIndex++] = i + left_wheel_start + 1 + shell_slices;
			meshData.indexVec[iIndex++] = (i + left_baffle_start + 1);

			meshData.indexVec[iIndex++] = (i + left_baffle_start);
			meshData.indexVec[iIndex++] = i + left_wheel_start + shell_slices;
			meshData.indexVec[iIndex++] = (i + left_baffle_start + 1);

			meshData.indexVec[iIndex++] = i + right_wheel_start + shell_slices;
			meshData.indexVec[iIndex++] = (i + right_baffle_start + 1);
			meshData.indexVec[iIndex++] = i + right_wheel_start + 1 + shell_slices;

			meshData.indexVec[iIndex++] = (i + right_baffle_start);
			meshData.indexVec[iIndex++] = (i + right_baffle_start + 1);
			meshData.indexVec[iIndex++] = i + right_wheel_start + shell_slices;

		}

		return meshData;
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateCarLight(float sphere_radius, float cylinder_radius, float cylinder_width, const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = 24;
		UINT indexCount = 70;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;


		float y1s = cylinder_radius - sphere_radius;
		float y1e = y1s- sphere_radius*0.06;

		float y2s = y1s * 0.8;
		float y2e = y2s - sphere_radius * 0.04;

		float xlefts = cylinder_width * 0.16;
		float xlefte = cylinder_width * 0.4;

		float xrights = -xlefts;
		float xrighte = -xlefte;

		float z1s1 = -sphere_radius * 0.98;
		float z1e1 = -sphere_radius * 0.90;

		float z1s2 = -sphere_radius * 0.97;
		float z1e2 = -sphere_radius * 0.90;

		float z2s1 = -sphere_radius * 0.70;
		float z2e1 = -sphere_radius * 0.60;

		float z2s2 = -sphere_radius * 0.70;
		float z2e2 = -sphere_radius * 0.60;

		//     5            1
		//   4           0
		//     7            3
		//   6           2

		// left light
		vertexData = { XMFLOAT3(xlefts,y1s,z1s1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1s,z1e1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1e,z1s2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1e,z1e2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xlefte,y2s,z2s1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2s,z2e1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2e,z2s2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2e,z2e2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xlefte,y2s,z2s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //4  ->8
		vertexData = { XMFLOAT3(xlefte,y2e,z2s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //6 ->9
		vertexData = { XMFLOAT3(xlefts,y1s,z1s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //0 ->10
		vertexData = { XMFLOAT3(xlefts,y1e,z1s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //2 ->11

		// right light
		//     1            5
		//   0            4
		//     3            6
		//   2            6

		vertexData = { XMFLOAT3(xrights,y1s,z1s1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1s,z1e1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1e,z1s2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1e,z1e2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xrighte,y2s,z2s1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2s,z2e1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2e,z2s2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2e,z2e2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xrighte,y2s,z2s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //4  ->8
		vertexData = { XMFLOAT3(xrighte,y2e,z2s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //6 ->9
		vertexData = { XMFLOAT3(xrights,y1s,z1s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //0 ->10
		vertexData = { XMFLOAT3(xrights,y1e,z1s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //2 ->11


		int bias = 12;
		meshData.indexVec =
		{ 0,2,1,
		2,3,1,
		5,0,1,
		4,0,5,
		5,7,4,
		7,6,4,
		7,3,6,
		3,2,6,
		8,9,10,
		9,11,10,

		12,13,14,
		14,13,15,
		17,13,12,
		16,17,12,
		17,16,19,
		19,16,18,
		19,18,15,
		15,18,14,

		20,22,21,
		21,22,23,
		};

		return meshData;
	}

	template<class VertexType, class IndexType>
	inline MeshData<VertexType, IndexType> CreateInCar(float sphere_radius, float cylinder_radius, float cylinder_width, const DirectX::XMFLOAT4 & color)
	{
		using namespace DirectX;
		MeshData<VertexType, IndexType> meshData;

		UINT vertexCount = 24;
		UINT indexCount = 70;
		meshData.vertexVec.resize(vertexCount);
		meshData.indexVec.resize(indexCount);

		Internal::VertexData vertexData;
		IndexType vIndex = 0, iIndex = 0;


		float y1s = cylinder_radius - sphere_radius;
		float y1e = y1s - sphere_radius * 0.06;

		float y2s = y1s * 0.8;
		float y2e = y2s - sphere_radius * 0.04;

		float xlefts = cylinder_width * 0.16;
		float xlefte = cylinder_width * 0.4;

		float xrights = -xlefts;
		float xrighte = -xlefte;

		float z1s1 = -sphere_radius * 0.98;
		float z1e1 = -sphere_radius * 0.90;

		float z1s2 = -sphere_radius * 0.97;
		float z1e2 = -sphere_radius * 0.90;

		float z2s1 = -sphere_radius * 0.70;
		float z2e1 = -sphere_radius * 0.60;

		float z2s2 = -sphere_radius * 0.70;
		float z2e2 = -sphere_radius * 0.60;

		//     5            1
		//   4           0
		//     7            3
		//   6           2

		// left light
		vertexData = { XMFLOAT3(xlefts,y1s,z1s1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1s,z1e1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1e,z1s2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefts,y1e,z1e2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xlefte,y2s,z2s1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2s,z2e1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2e,z2s2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xlefte,y2e,z2e2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xlefte,y2s,z2s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //4  ->8
		vertexData = { XMFLOAT3(xlefte,y2e,z2s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //6 ->9
		vertexData = { XMFLOAT3(xlefts,y1s,z1s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //0 ->10
		vertexData = { XMFLOAT3(xlefts,y1e,z1s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //2 ->11

		// right light
		//     1            5
		//   0            4
		//     3            6
		//   2            6

		vertexData = { XMFLOAT3(xrights,y1s,z1s1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1s,z1e1), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1e,z1s2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrights,y1e,z1e2), XMFLOAT3(-1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xrighte,y2s,z2s1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2s,z2e1), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2e,z2s2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);
		vertexData = { XMFLOAT3(xrighte,y2e,z2e2), XMFLOAT3(1.0f,0.0f,0.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

		vertexData = { XMFLOAT3(xrighte,y2s,z2s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //4  ->8
		vertexData = { XMFLOAT3(xrighte,y2e,z2s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 0.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //6 ->9
		vertexData = { XMFLOAT3(xrights,y1s,z1s1), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(1.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //0 ->10
		vertexData = { XMFLOAT3(xrights,y1e,z1s2), XMFLOAT3(0.0f,0.0f,-1.0f),XMFLOAT4(0.0f ,0.0,0.0, 1.0f), color, XMFLOAT2(0.0f, 1.0f) };
		Internal::InsertVertexElement(meshData.vertexVec[vIndex++], vertexData); //2 ->11


		int bias = 12;
		meshData.indexVec =
		{ 0,2,1,
		2,3,1,
		5,0,1,
		4,0,5,
		5,7,4,
		7,6,4,
		7,3,6,
		3,2,6,
		8,9,10,
		9,11,10,

		12,13,14,
		14,13,15,
		17,13,12,
		16,17,12,
		17,16,19,
		19,16,18,
		19,18,15,
		15,18,14,

		20,22,21,
		21,22,23,
		};

		return meshData;
	}

}






#endif


