#include "Land.h"

Land::Land()
{
	m_CenterID = 4;
}

void Land::Append(Terrain terrain)
{
	m_Terrains.push_back(terrain);
}

void Land::init(double width, double height)
{
	m_width = width;
	m_height = height;

	ResetTerrains();
}

void Land::ResetTerrains()
{
	m_Terrains[0].setXZ(-m_width, m_height);
	m_Terrains[1].setXZ(0, m_height);
	m_Terrains[2].setXZ(m_width, m_height);
	m_Terrains[3].setXZ(-m_width, 0);
	m_Terrains[5].setXZ(m_width, 0);
	m_Terrains[6].setXZ(-m_width, -m_height);
	m_Terrains[7].setXZ(0, -m_height);
	m_Terrains[8].setXZ(m_width, -m_height);
}

void Land::Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect)
{
	for (auto &T : m_Terrains)
	{
		T.Draw(deviceContext, effect);
	}
}

bool Land::SetPosition(DirectX::XMFLOAT3 position, double &dx, double &dz)
{
	dx = 0;
	dz = 0;
	double widthdiv2 = m_width / 2;
	double heightdiv2 = m_height / 2;

	bool isdirt = false;

	if (position.x < -widthdiv2)
	{
		//Relayout(LAND_LAYOUT::X_N);
		isdirt = true;
		dx = m_width;
	}
	if (position.x >= widthdiv2)
	{
		//Relayout(LAND_LAYOUT::X_P);
		isdirt = true;
		dx = -m_width;
	}
	if (position.z < -heightdiv2)
	{
		//Relayout(LAND_LAYOUT::Z_N);
		isdirt = true;
		dz = m_height;
	}
	if (position.z >= heightdiv2)
	{
		//Relayout(LAND_LAYOUT::Z_P);
		isdirt = true;
		dz = -m_height;
	}
	return isdirt;
}

void Land::Relayout(LAND_LAYOUT mode)
{
	ResetTerrains();

	// 0 1 2
	// 3 4 5
	// 6 7 8
	switch (mode)
	{
	case LAND_LAYOUT::X_N:
		// 2 0 1
		// 5 3 4
		// 8 6 7
		m_Terrains[0].moveXZ(2*m_width,0);
		m_Terrains[3].moveXZ(2*m_width, 0);
		m_Terrains[6].moveXZ(2*m_width, 0);

		m_Terrains[1].moveXZ(-m_width, 0);
		m_Terrains[4].moveXZ(-m_width, 0);
		m_Terrains[7].moveXZ(-m_width, 0);

		m_Terrains[2].moveXZ(-m_width, 0);
		m_Terrains[5].moveXZ(-m_width, 0);
		m_Terrains[8].moveXZ(-m_width, 0);
		break;
	case LAND_LAYOUT::X_P:
		// 1 2 0
		// 4 5 3
		// 7 8 6
		m_Terrains[0].moveXZ(m_width, 0);
		m_Terrains[3].moveXZ(m_width, 0);
		m_Terrains[6].moveXZ(m_width, 0);

		m_Terrains[1].moveXZ(m_width, 0);
		m_Terrains[4].moveXZ(m_width, 0);
		m_Terrains[7].moveXZ(m_width, 0);

		m_Terrains[2].moveXZ(-2 * m_width, 0);
		m_Terrains[5].moveXZ(-2 * m_width, 0);
		m_Terrains[8].moveXZ(-2 * m_width, 0);
		break;

	case LAND_LAYOUT::Z_N:
		// 3 4 5
		// 6 7 8
		// 0 1 2

		m_Terrains[0].moveXZ(0, -m_height);
		m_Terrains[1].moveXZ(0, -m_height);
		m_Terrains[2].moveXZ(0, -m_height);

		m_Terrains[3].moveXZ(0, -m_height);
		m_Terrains[4].moveXZ(0, -m_height);
		m_Terrains[5].moveXZ(0, -m_height);

		m_Terrains[6].moveXZ(0, 2 * m_height);
		m_Terrains[7].moveXZ(0, 2 * m_height);
		m_Terrains[8].moveXZ(0, 2 * m_height);
		break;

	case LAND_LAYOUT::Z_P:

		// 6 7 8
		// 0 1 2
		// 3 4 5

		m_Terrains[0].moveXZ(0, -2 * m_height);
		m_Terrains[1].moveXZ(0, -2 * m_height);
		m_Terrains[2].moveXZ(0, -2 * m_height);

		m_Terrains[3].moveXZ(0, m_height);
		m_Terrains[4].moveXZ(0, m_height);
		m_Terrains[5].moveXZ(0, m_height);

		m_Terrains[6].moveXZ(0, m_height);
		m_Terrains[7].moveXZ(0, m_height);
		m_Terrains[8].moveXZ(0, m_height);
		break;
	}

}

int Land::GetBlockId(double x, double z, double &dx, double &dz)
{
	double widthdiv2 = m_width / 2;
	double heightdiv2 = m_height / 2;

	dx = 0;
	dz = 0;
	int idx=1, idz=1;
	if (x < -widthdiv2)
	{
		idx += 1;
		dx = m_width;
	}
	if (x > widthdiv2)
	{
		idx -= 1;
		dx = -m_width;
	}
	if (z < -heightdiv2)
	{
		idz -= 1;
		dz = m_height;
	}
	if (z > heightdiv2)
	{
		idz += 1;
		dz = -m_height;
	}

	return idz*3+idx;
}

double Land::GetHeight(double x, double z)
{
	double dx, dz;
	int blickid = GetBlockId(x, z, dx, dz);

	return m_Terrains[blickid].GetHeight(x+dx, z+dz);
}

DirectX::XMFLOAT3 Land::GetNorm(double x, double z)
{
	double dx, dz;
	int blickid = GetBlockId(x, z, dx, dz);
	return m_Terrains[blickid].GetNorm(x + dx, z + dz);
}

void Land::Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj)
{
	for (auto &T : m_Terrains)
	{
		T.Draw(deviceContext, effect,view, proj);
	}
}