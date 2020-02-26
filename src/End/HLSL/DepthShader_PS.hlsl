#include "DepthShader.hlsli"


float4 PS(VertexOut outa) : SV_Target
{
	return outa.color;
}