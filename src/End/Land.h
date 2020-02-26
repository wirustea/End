#ifndef LAND_H
#define LAND_H

#include "terrain.h"
#include "Effects.h"
#include <vector>

enum LAND_LAYOUT { X_P, X_N, Z_P, Z_N };

class Land
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Land();
	void Append(Terrain terrain);
	void Draw(ID3D11DeviceContext * deviceContext, BasicEffect& effect);
	void Draw(ID3D11DeviceContext * deviceContext, DepthRenderEffect& effect, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 proj);

	void init(double width, double height);


	double GetHeight(double x, double z);
	DirectX::XMFLOAT3 GetNorm(double x, double z);

	bool SetPosition(DirectX::XMFLOAT3 position, double &dx, double &dz);

private:
	void Relayout(LAND_LAYOUT mode);
	void ResetTerrains();
	int GetBlockId(double x, double z, double &dx, double &dz);

	std::vector<Terrain> m_Terrains;
	DirectX::XMFLOAT3 m_ObjPosition;
	double m_width;
	double m_height;
	int m_CenterID;

	ComPtr<ID3D11DepthStencilView> m_pDynamicCubeMapDSV;	
	ComPtr<ID3D11RenderTargetView> m_pDynamicCubeMapRTV;

	ComPtr<ID3D11RenderTargetView> m_pCacheRTV;
	ComPtr<ID3D11DepthStencilView> m_pCacheDSV;

};
#endif