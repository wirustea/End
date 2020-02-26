#include "LightHelper.hlsli"

Texture2D g_DiffuseMap : register(t0);	//��������
TextureCube g_TexCube : register(t1);	//��̬��պ�����
Texture2D g_ShadowMap : register(t2);	//��Ӱ����
Texture2D g_CarShadowMap : register(t3);//���Ʋ�������Ӱ����

SamplerState g_Sam : register(s0);
SamplerState g_Clamp : register(s1); 

cbuffer CBChangesEveryInstanceDrawing : register(b0)
{
    matrix g_World;
    matrix g_WorldInvTranspose;
	matrix g_CWorld;			// ������������������㳵��
}

cbuffer CBChangesEveryObjectDrawing : register(b1)
{
    Material g_Material;
}

cbuffer CBDrawingStates : register(b2)
{
    int g_TextureUsed;			// ��������
    int g_ReflectionEnabled;	// ��������
    int g_RefractionEnabled;	// ��������
    float g_Eta;				// ����/���������
	float4 g_FogColor;			// ����ɫ
	int g_FogEnabled;			// ������Ч
	float g_FogStart;			// ��ʼ��Χ
	float g_FogRange;			// ��Χ
	int g_Shadow;				// ������Ӱ
}

cbuffer CBChangesEveryFrame : register(b3)
{
    matrix g_View;
	matrix g_ProjView;			// ������Դ��ͼ����
	matrix g_CarView;			// �����Դ��ͼ����
    float3 g_EyePosW;	
    float g_Pad2;
}

cbuffer CBChangesOnResize : register(b4)
{
    matrix g_Proj;
	matrix g_ProjProj;			// ������ԴͶӰ����
	matrix g_CarProj;			// �����ԴͶӰ����
}

cbuffer CBChangesRarely : register(b5)
{
	CarLight g_CarLight[2];			// ����
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
    float3 PosW : POSITION0; // �������е�λ��
    float3 NormalW : NORMAL; // �������������еķ���
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

