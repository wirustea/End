#include "LightHelper.hlsli"

Texture2D g_DiffuseMap : register(t0);	//物体纹理
TextureCube g_TexCube : register(t1);	//动态天空盒纹理
Texture2D g_ShadowMap : register(t2);	//阴影纹理
Texture2D g_CarShadowMap : register(t3);//车灯产生的阴影纹理

SamplerState g_Sam : register(s0);
SamplerState g_Clamp : register(s1); 

cbuffer CBChangesEveryInstanceDrawing : register(b0)
{
    matrix g_World;
    matrix g_WorldInvTranspose;
	matrix g_CWorld;			// 车体世界矩阵，用来计算车灯
}

cbuffer CBChangesEveryObjectDrawing : register(b1)
{
    Material g_Material;
}

cbuffer CBDrawingStates : register(b2)
{
    int g_TextureUsed;			// 启用纹理
    int g_ReflectionEnabled;	// 开启反射
    int g_RefractionEnabled;	// 开启折射
    float g_Eta;				// 空气/介质折射比
	float4 g_FogColor;			// 雾颜色
	int g_FogEnabled;			// 开启雾效
	float g_FogStart;			// 雾开始范围
	float g_FogRange;			// 雾范围
	int g_Shadow;				// 开启阴影
}

cbuffer CBChangesEveryFrame : register(b3)
{
    matrix g_View;
	matrix g_ProjView;			// 虚拟点光源视图矩阵
	matrix g_CarView;			// 车体光源视图矩阵
    float3 g_EyePosW;	
    float g_Pad2;
}

cbuffer CBChangesOnResize : register(b4)
{
    matrix g_Proj;
	matrix g_ProjProj;			// 虚拟点光源投影矩阵
	matrix g_CarProj;			// 车体光源投影矩阵
}

cbuffer CBChangesRarely : register(b5)
{
	CarLight g_CarLight[2];			// 车灯
    DirectionalLight g_DirLight[5];
    PointLight g_PointLight[5];
	SpotLight g_SpotLight[5];
	
}

struct VertexPosNormalTex
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VertexPosHWNormalTex
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION0; // 在世界中的位置
    float3 NormalW : NORMAL; // 法向量在世界中的方向
	float4 ProjPos : NORMAL1;
	float4 CarPos : NORMAL2;
    float2 Tex : TEXCOORD;
};

struct InstancePosNormalTex
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float2 Tex : TEXCOORD;
    matrix World : World;
    matrix WorldInvTranspose : WorldInvTranspose;
};

