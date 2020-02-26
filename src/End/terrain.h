#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <fstream>
#include <string>
#include "Vertex.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "model.h"
#include <cmath>
#include <DirectXCollision.h>
#include "Effects.h"
#include "Geometry.h"

class Terrain
{
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	Terrain();

	/**
	 * @brief 从高度图读取高度信息
	 * @param filePath(string)  高度图位置
	 */
	bool ReadRawFile(std::string filePath);		

	/**
	 * @brief 初始化地形
	 * @param device   设备
	 * @param width	   地形宽度
	 * @param height   地形高度
	 * @param m        地形宽度方向上的顶点个数
	 * @param n		   地形高度方向上的顶点个数
	 * @param scale    高度比例
	 * @return bool    结果
	 */
	bool InitTerrain(ID3D11Device * device, float width, float height, UINT m, UINT n, float scale);

	/**
	 * @brief 计算法向量
	 */
	void ComputeNomal(VertexPosNormalTex& v1, VertexPosNormalTex& v2, VertexPosNormalTex& v3, DirectX::XMFLOAT3& normal);        //计算法线

	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);
	void Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);

	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	void SetMaterial(Material& material);
	void SetTexture(ID3D11Device * device, std::string texFilename);

	/**
	 * @brief 获取（x，y）点处的插值高度
	 * @param x(double)
	 * @param y(double)
	 */
	double GetHeight(double x, double z);

	double GetHeight(int x, int z);

	DirectX::XMFLOAT3 GetNorm(int x, int z);

	/**
	 * @brief 获取（x，y）点处的插值法向量
	 * @param x(double)
	 * @param y(double)
	 */
	DirectX::XMFLOAT3 GetNorm(double x, double z);

	void moveXZ(double dx, double dz);
	void setXZ(double dx, double dz);

	double m_dx;
	double m_dz;

private:
	std::vector<float> m_heightInfos;				//高度图高度信息
	int m_cellsPerRow;								//每行单元格数
	int m_cellsPerCol;								//每列单元格数
	int m_verticesPerRow;							//每行顶点数
	int m_verticesPerCol;							//每列顶点数
	int m_numsVertices;								//顶点总数
	int m_nIndices;
	double m_width;									//地形宽度
	double m_height;								//地形高度
	double m_heightScale;							//高度缩放系数
	std::vector<VertexPosNormalTex> m_vertices;     //顶点集合
	std::vector<UINT> m_indices;					//索引集合

	Material m_Material;
	ComPtr<ID3D11ShaderResourceView> m_TexDiffuse;

	// buffer
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

	DirectX::XMFLOAT4X4 m_WorldMatrix;
};

#endif