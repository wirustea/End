#include "Basic.hlsli"

// ������ɫ��(3D)
float4 PS(VertexPosHWNormalTex pIn) : SV_Target
{
    // ����ʹ��������ʹ��Ĭ�ϰ�ɫ
    float4 texColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (g_TextureUsed)
    {
        texColor = g_DiffuseMap.Sample(g_Sam, pIn.Tex);
        // ��ǰ���вü����Բ�����Ҫ������ؿ��Ա����������
        clip(texColor.a - 0.1f);
    }
    
    // ��׼��������
    pIn.NormalW = normalize(pIn.NormalW);

    // ����ָ���۾�������
    float3 toEyeW = normalize(g_EyePosW - pIn.PosW);
	float distToEye = distance(g_EyePosW, pIn.PosW);

    // ��ʼ��Ϊ0 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 ambient_carlight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse_carlight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec_carlight = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    int i;
	bool isShadowG = false;  // ������ԴͶӰ
	bool isShadowL = false;  // ����ͶӰ

    [unroll]
    for (i = 0; i < 5; ++i)
    {
        ComputeDirectionalLight(g_Material, g_DirLight[i], pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
        
    [unroll]
    for (i = 0; i < 5; ++i)
    {
        ComputePointLight(g_Material, g_PointLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    [unroll]
    for (i = 0; i < 5; ++i)
    {
        ComputeSpotLight(g_Material, g_SpotLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }


	[unroll]
	for (i = 0; i < 2; ++i)
	{
		ComputeCarLight(g_Material, g_CWorld, g_CarLight[i], pIn.PosW, pIn.NormalW, toEyeW, A, D, S);
		ambient_carlight += A;
		diffuse_carlight += D;
		spec_carlight += S;
	}
 


	if (g_Shadow)
	{
		float bias = 0.001f;
		float2 ShadowTex;
		float ShadowMapDepth; //a,g,b�洢�Ķ������
		float Depth;


		//�����Ӧ���������Ӧ��ShdowMap�ϵ����ֵ
		//��ȡͶӰ����µ�ͶӰ����ռ������ֵ[0.0,1.0]  u=0.5*x+0.5;   v=-0.5*y+0.5;   -w<=x<=w  -w<=y<=w  
		ShadowTex.x = (pIn.ProjPos.x / pIn.ProjPos.w)*0.5f + 0.5f;
		ShadowTex.y = (pIn.ProjPos.y / pIn.ProjPos.w)*(-0.5f) + 0.5f;

		if (saturate(ShadowTex.x) == ShadowTex.x&&saturate(ShadowTex.y) == ShadowTex.y)
		{
			//����������������Ӧ�����ֵ
			ShadowMapDepth = g_ShadowMap.Sample(g_Clamp, ShadowTex).r;

			//�������������Ӧ�����ֵ(���Դ����Ⱦ������ֵ)
			Depth = pIn.ProjPos.z / pIn.ProjPos.w;

			//��ȥ��Ӱƫб��
			Depth = Depth - bias;
			if (ShadowMapDepth < Depth)
			{
				isShadowG = true;
			}
		}


		ShadowTex.x = (pIn.CarPos.x / pIn.CarPos.w)*0.5f + 0.5f;
		ShadowTex.y = (pIn.CarPos.y / pIn.CarPos.w)*(-0.5f) + 0.5f;

		if (saturate(ShadowTex.x) == ShadowTex.x&&saturate(ShadowTex.y) == ShadowTex.y)
		{
			//����������������Ӧ�����ֵ
			ShadowMapDepth = g_CarShadowMap.Sample(g_Clamp, ShadowTex).r;

			//�������������Ӧ�����ֵ(���Դ����Ⱦ������ֵ)
			Depth = pIn.CarPos.z / pIn.CarPos.w;

			//��ȥ��Ӱƫб��
			Depth = Depth- bias;
			if (ShadowMapDepth < Depth)
			{
				isShadowL = true;
			}
		}
	}

	float4 litColor = texColor;

	if (isShadowL)
	{
		//litColor = litColor * ambient;
		litColor = litColor * (ambient + ambient_carlight + diffuse) + spec;
		
	}
	else if (isShadowG)
	{
		//litColor = litColor * ambient;
		litColor = litColor * (ambient + ambient_carlight + diffuse_carlight) + spec_carlight;
	}
	else
	{
		litColor = litColor * (ambient + ambient_carlight + diffuse + diffuse_carlight) + spec + spec_carlight;
	}


    // ����
    if (g_ReflectionEnabled)
    {
        float3 incident = -toEyeW;
        float3 reflectionVector = reflect(incident, pIn.NormalW);
        float4 reflectionColor = g_TexCube.Sample(g_Sam, reflectionVector);

        litColor += g_Material.Reflect * reflectionColor;
    }
    // ����
    if (g_RefractionEnabled)
    {
        float3 incident = -toEyeW;
        float3 refractionVector = refract(incident, pIn.NormalW, g_Eta);
        float4 refractionColor = g_TexCube.Sample(g_Sam, refractionVector);

        litColor += g_Material.Reflect * refractionColor;
    }

	// ��Ч����
	[flatten]
	if (g_FogEnabled)
	{
		// �޶���0.0f��1.0f��Χ
		float fogLerp = saturate((distToEye - g_FogStart) / g_FogRange);
		// ������ɫ�͹�����ɫ�������Բ�ֵ
		litColor = lerp(litColor, g_FogColor, fogLerp);
	}


    litColor.a = texColor.a * g_Material.Diffuse.a;
    return litColor;
}
