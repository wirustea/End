//***************************************************************************************
// 存放模型数据
//***************************************************************************************

#ifndef MODEL_H
#define MODEL_H

#include <DirectXCollision.h>
#include <string>
#include "Effects.h"
#include "Geometry.h"

wchar_t *multiByteToWideChar(std::string pKey);

struct ModelPart
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	ModelPart() : material(), texDiffuse(), vertexBuffer(), indexBuffer(),
		vertexCount(), indexCount(), indexFormat() {}

	ModelPart(const ModelPart&) = default;
	ModelPart& operator=(const ModelPart&) = default;

	ModelPart(ModelPart&&) = default;
	ModelPart& operator=(ModelPart&&) = default;


	Material material;
	ComPtr<ID3D11ShaderResourceView> texDiffuse;
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;
	UINT vertexCount;
	UINT indexCount;
	DXGI_FORMAT indexFormat;
};

struct Model
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Model();

	void SetMesh(ID3D11Device * device, const void* vertices, UINT vertexSize, UINT vertexCount,
		const void * indices, UINT indexCount, DXGI_FORMAT indexFormat, Material& material, std::string texFilename= "");
	
	template<class VertexType, class IndexType>
	void AppendMesh(ID3D11Device * device, const Geometry::MeshData<VertexType, IndexType>& meshData, Material& material, std::string texFilename = "");

	void SetDebugObjectName(const std::string& name);

	std::vector<ModelPart> modelParts;
	DirectX::BoundingBox boundingBox;

	UINT vertexStride;
};


template<class VertexType, class IndexType>
inline void Model::AppendMesh(ID3D11Device * device, const Geometry::MeshData<VertexType, IndexType>& meshData, Material& material, std::string texFilename)
{
	static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4, "The size of IndexType must be 2 bytes or 4 bytes!");
	static_assert(std::is_unsigned<IndexType>::value, "IndexType must be unsigned integer!");
	SetMesh(device, meshData.vertexVec.data(), sizeof(VertexType),
		(UINT)meshData.vertexVec.size(), meshData.indexVec.data(), (UINT)meshData.indexVec.size(),
		(sizeof(IndexType) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), material, texFilename);
}



#endif
