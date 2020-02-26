cbuffer CBMatrix:register(b0)
{
	matrix WorldViewProj;
};

struct VertexIn
{
	float3 Pos:POSITION;
	float4 color:COLOR;
};


struct VertexOut
{
	float4 Pos:SV_POSITION;
	float4 color:COlOR;
};