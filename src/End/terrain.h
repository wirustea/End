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
	 * @brief �Ӹ߶�ͼ��ȡ�߶���Ϣ
	 * @param filePath(string)  �߶�ͼλ��
	 */
	bool ReadRawFile(std::string filePath);		

	/**
	 * @brief ��ʼ������
	 * @param device   �豸
	 * @param width	   ���ο��
	 * @param height   ���θ߶�
	 * @param m        ���ο�ȷ����ϵĶ������
	 * @param n		   ���θ߶ȷ����ϵĶ������
	 * @param scale    �߶ȱ���
	 * @return bool    ���
	 */
	bool InitTerrain(ID3D11Device * device, float width, float height, UINT m, UINT n, float scale);

	/**
	 * @brief ���㷨����
	 */
	void ComputeNomal(VertexPosNormalTex& v1, VertexPosNormalTex& v2, VertexPosNormalTex& v3, DirectX::XMFLOAT3& normal);        //���㷨��

	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);
	void Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);

	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	void SetMaterial(Material& material);
	void SetTexture(ID3D11Device * device, std::string texFilename);

	/**
	 * @brief ��ȡ��x��y���㴦�Ĳ�ֵ�߶�
	 * @param x(double)
	 * @param y(double)
	 */
	double GetHeight(double x, double z);

	double GetHeight(int x, int z);

	DirectX::XMFLOAT3 GetNorm(int x, int z);

	/**
	 * @brief ��ȡ��x��y���㴦�Ĳ�ֵ������
	 * @param x(double)
	 * @param y(double)
	 */
	DirectX::XMFLOAT3 GetNorm(double x, double z);

	void moveXZ(double dx, double dz);
	void setXZ(double dx, double dz);

	double m_dx;
	double m_dz;

private:
	std::vector<float> m_heightInfos;				//�߶�ͼ�߶���Ϣ
	int m_cellsPerRow;								//ÿ�е�Ԫ����
	int m_cellsPerCol;								//ÿ�е�Ԫ����
	int m_verticesPerRow;							//ÿ�ж�����
	int m_verticesPerCol;							//ÿ�ж�����
	int m_numsVertices;								//��������
	int m_nIndices;
	double m_width;									//���ο��
	double m_height;								//���θ߶�
	double m_heightScale;							//�߶�����ϵ��
	std::vector<VertexPosNormalTex> m_vertices;     //���㼯��
	std::vector<UINT> m_indices;					//��������

	Material m_Material;
	ComPtr<ID3D11ShaderResourceView> m_TexDiffuse;

	// buffer
	ComPtr<ID3D11Buffer> vertexBuffer;
	ComPtr<ID3D11Buffer> indexBuffer;

	DirectX::XMFLOAT4X4 m_WorldMatrix;
};

#endif