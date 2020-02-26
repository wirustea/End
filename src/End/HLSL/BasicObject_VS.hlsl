#include "Basic.hlsli"

// ¶¥µã×ÅÉ«Æ÷
VertexPosHWNormalTex VS(VertexPosNormalTex vIn)
{
    VertexPosHWNormalTex vOut;
    
    matrix viewProj = mul(g_View, g_Proj);
    vector posW = mul(float4(vIn.PosL, 1.0f), g_World);

    vOut.PosW = posW.xyz;
    vOut.PosH = mul(posW, viewProj);
    vOut.NormalW = mul(vIn.NormalL, (float3x3) g_WorldInvTranspose);
    vOut.Tex = vIn.Tex;

	// projecter
	matrix projViewProj = mul(g_ProjView, g_ProjProj);
	vOut.ProjPos = mul(posW, projViewProj);

	// car
	matrix carViewProj = mul(g_CarView, g_CarProj);
	vOut.CarPos = mul(posW, carViewProj);



    return vOut;
}