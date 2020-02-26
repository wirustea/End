#include "DepthShader.hlsli"

VertexOut VS(VertexIn ina)
{
	VertexOut outa;
	outa.Pos = mul(float4(ina.Pos, 1.0f), WorldViewProj);
	outa.color = ina.color;
	return outa;
}