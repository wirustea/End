#include "terrain.h"
#include "DXTrace.h"
#include "d3dUtil.h"

Terrain::Terrain():
	m_WorldMatrix(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f)
{
}

//读取高度图信息
bool Terrain::ReadRawFile(std::string filePath)
{
	std::ifstream inFile;
	//二进制方式打开文件
	inFile.open(filePath.c_str(), std::ios::binary);
	//文件指针移动到末尾
	inFile.seekg(0, std::ios::end);
	//大小为当前缓冲区大小
	std::vector<BYTE> inData(inFile.tellg());
	//文件指针移动到开头
	inFile.seekg(std::ios::beg);
	//读取高度信息
	inFile.read((char*)&inData[0], inData.size());
	inFile.close();

	m_heightInfos.resize(inData.size());
	for (int i = 0; i < inData.size(); ++i)
	{
		m_heightInfos[i] = inData[i];
	}

	return true;
}

//计算法线
void Terrain::ComputeNomal(VertexPosNormalTex& v1, VertexPosNormalTex& v2, VertexPosNormalTex& v3, DirectX::XMFLOAT3& normal)
{
	DirectX::XMFLOAT3 f1(v2.pos.x - v1.pos.x, v2.pos.y - v1.pos.y, v2.pos.z - v1.pos.z);
	DirectX::XMFLOAT3 f2(v3.pos.x - v1.pos.x, v3.pos.y - v1.pos.y, v3.pos.z - v1.pos.z);
	DirectX::XMVECTOR vec1 = XMLoadFloat3(&f1);
	DirectX::XMVECTOR vec2 = XMLoadFloat3(&f2);
	DirectX::XMVECTOR temp = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vec1, vec2));
	XMStoreFloat3(&normal, temp);
}

bool Terrain::InitTerrain(ID3D11Device * device,float width, float height, UINT m, UINT n, float scale)
{
	m_cellsPerRow = m;
	m_cellsPerCol = n;
	m_verticesPerRow = m + 1;
	m_verticesPerCol = n + 1;
	m_numsVertices = m_verticesPerRow * m_verticesPerCol;
	m_width = width;
	m_height = height;
	m_heightScale = scale;

	//得到缩放后的高度
	for (auto& item : m_heightInfos)
	{
		item *= m_heightScale;
	}

	//起始x z坐标
	float oX = -width * 0.5f;
	float oZ = height * 0.5f;
	//每一格坐标变化
	m_dx = width / m;
	m_dz = height / n;

	m_vertices.resize(m_numsVertices);
	//计算顶点
	for (UINT i = 0; i < m_verticesPerCol; ++i)
	{
		float tempZ = oZ - m_dz * i;
		for (UINT j = 0; j < m_verticesPerRow; ++j)
		{
			UINT index = m_verticesPerRow * i + j;
			m_vertices[index].pos.x = oX + m_dx * j;
			m_vertices[index].pos.y = m_heightInfos[index];
			m_vertices[index].pos.z = tempZ;

			m_vertices[index].tex = DirectX::XMFLOAT2(m_dx*i, m_dx*j);
		}
	}

	//计算索引和法线
	//总格子数量:m * n
	//因此总索引数量: 6 * m * n
	m_nIndices = m * n * 6;
	m_indices.resize(m_nIndices);
	UINT tmp = 0;
	for (UINT i = 0; i < n; ++i)
	{
		for (UINT j = 0; j < m; ++j)
		{
			m_indices[tmp] = i * m_verticesPerRow + j;
			m_indices[tmp + 1] = i * m_verticesPerRow + j + 1;
			m_indices[tmp + 2] = (i + 1) * m_verticesPerRow + j;

			//计算法线
			DirectX::XMFLOAT3 temp;
			ComputeNomal(m_vertices[m_indices[tmp]], m_vertices[m_indices[tmp + 1]],
				m_vertices[m_indices[tmp + 2]], temp);
			m_vertices[m_indices[tmp]].normal = temp;
			m_vertices[m_indices[tmp + 1]].normal = temp;
			m_vertices[m_indices[tmp + 2]].normal = temp;

			m_indices[tmp + 3] = i * m_verticesPerRow + j + 1;
			m_indices[tmp + 4] = (i + 1) * m_verticesPerRow + j + 1;
			m_indices[tmp + 5] = (i + 1) * m_verticesPerRow + j;

			ComputeNomal(m_vertices[m_indices[tmp + 3]], m_vertices[m_indices[tmp + 4]],
				m_vertices[m_indices[tmp + 5]], temp);
			m_vertices[m_indices[tmp + 3]].normal = temp;
			m_vertices[m_indices[tmp + 4]].normal = temp;
			m_vertices[m_indices[tmp + 5]].normal = temp;

			tmp += 6;
		}
	}

	// 设置顶点缓冲区描述
	UINT vertexSize = sizeof(VertexPosNormalTex);

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = vertexSize * m_numsVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &m_vertices[0];
	HR(device->CreateBuffer(&vbd, &InitData, vertexBuffer.ReleaseAndGetAddressOf()));

	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	if (sizeof(VertexPosNormalTex) == 2)
	{
		ibd.ByteWidth = m_nIndices * (UINT)sizeof(WORD);
	}
	else
	{
		ibd.ByteWidth = m_nIndices * (UINT)sizeof(DWORD);
	}
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = &m_indices[0];
	HR(device->CreateBuffer(&ibd, &InitData, indexBuffer.ReleaseAndGetAddressOf()));

	return true;
}

void Terrain::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
}

void XM_CALLCONV Terrain::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, world);
}

void Terrain::SetMaterial(Material& material)
{
	m_Material = material;
}

void Terrain::moveXZ(double dx, double dz)
{
	m_WorldMatrix._41 += dx;
	m_WorldMatrix._43 += dz;
}

void Terrain::setXZ(double dx, double dz)
{
	m_WorldMatrix._41 = dx;
	m_WorldMatrix._43 = dz;
}


void Terrain::SetTexture(ID3D11Device * device, std::string texFilename)
{
	if (texFilename.substr(texFilename.find_last_of(".") + 1) == "dds")
	{
		wchar_t *wc = multiByteToWideChar(texFilename);

		HR(DirectX::CreateDDSTextureFromFile(device,
			wc,
			nullptr,
			m_TexDiffuse.GetAddressOf()));
	}
	else
	{
		wchar_t *wc = multiByteToWideChar(texFilename);
		HR(DirectX::CreateWICTextureFromFile(device, wc, nullptr, m_TexDiffuse.GetAddressOf()));
	}

}

void Terrain::Draw(ID3D11DeviceContext * deviceContext, BasicEffect & effect)
{
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;

	// 设置顶点/索引缓冲区
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), (sizeof(VertexPosNormalTex) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), 0);

	// 更新数据并应用
	effect.SetWorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	effect.SetTextureDiffuse(m_TexDiffuse.Get());
	effect.SetMaterial(m_Material);

	effect.Apply(deviceContext);

	deviceContext->DrawIndexed(m_nIndices, 0, 0);
	
}


void Terrain::Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	UINT strides = sizeof(VertexPosNormalTex);
	UINT offsets = 0;

	// 设置顶点/索引缓冲区
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(indexBuffer.Get(), (sizeof(VertexPosNormalTex) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT), 0);

	// 更新数据并应用
	effect.SetWorldViewProjMatrix(XMLoadFloat4x4(&m_WorldMatrix), XMLoadFloat4x4(&view), XMLoadFloat4x4(&proj));

	effect.Apply(deviceContext);

	deviceContext->DrawIndexed(m_nIndices, 0, 0);

}


double Terrain::GetHeight(int x, int z)
{
	return m_heightInfos[m_verticesPerRow*z + x];
}

DirectX::XMFLOAT3 Terrain::GetNorm(int x, int z)
{
	return m_vertices[m_verticesPerRow*z + x].normal;
}


double Terrain::GetHeight(double x, double y)
{
	// 双线性差值 求出高度值
	x = 1.0*m_cellsPerRow/m_width *x + 1.0*m_cellsPerRow/2;
	y = -1.0*m_cellsPerRow / m_width * y + 1.0*m_cellsPerRow / 2;

	int x1 = floor(x), y1 = floor(y), x2 = ceil(x), y2 = ceil(y);

	if (x1 == x2)  // 顶点的x为整数
	{

		double Q1 = Terrain::GetHeight(x1, y1);
		double Q2 = Terrain::GetHeight(x1, y2);

		double y2_y = y2 - y;
		double y_y1 = y - y1;

		return y2_y * Q1 + y_y1 * Q2;
	}
	if (y1 == y2)  // 顶点的y为整数
	{
		double Q1 = Terrain::GetHeight(x1, y1);
		double Q2 = Terrain::GetHeight(x2, y1);

		double x2_x = x2 - x;
		double x_x1 = x - x1;

		return x2_x * Q1 + x_x1 * Q2;
	}

	double Q11 = Terrain::GetHeight(x1, y1);
	double Q12 = Terrain::GetHeight(x1, y2);
	double Q21 = Terrain::GetHeight(x2, y1);
	double Q22 = Terrain::GetHeight(x2, y2);

	double x2_x = x2 - x;
	double x_x1 = x - x1;
	double y2_y = y2 - y;
	double y_y1 = y - y1;

	return (x2_x*Q11 + x_x1 * Q21)*y2_y + (x2_x*Q12 + x_x1 * Q22)*y_y1;
}

DirectX::XMFLOAT3 Terrain::GetNorm(double x, double y)
{
	using namespace DirectX;

	x = 1.0*m_cellsPerRow / m_width * x + 1.0*m_cellsPerRow / 2;
	y = -1.0*m_cellsPerRow / m_width * y + 1.0*m_cellsPerRow / 2;

	int p1x = floor(x), p1z = floor(y), p2x = ceil(x), p2z = ceil(y);

	XMFLOAT3 p1y = Terrain::GetNorm(p1x, p1z), p2y = Terrain::GetNorm(p1x, p2z);
	XMFLOAT3 p3y = Terrain::GetNorm(p2x, p1z), p4y = Terrain::GetNorm(p2x, p2z);
	XMFLOAT3 res;
	XMStoreFloat3(&res, XMVector3Normalize(XMLoadFloat3(&p1y) + XMLoadFloat3(&p2y) + XMLoadFloat3(&p3y) + XMLoadFloat3(&p4y)));
	return res;
}