//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D Tex2D : register(t0);
SamplerState samState : register(s0);

cbuffer SRVRendererConstantBuffer : register(b0)
{
	matrix projection;
	matrix world;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT Out;
	Out.Pos = float4(input.Pos, 1);
	Out.Pos = mul(Out.Pos, world);
	Out.Pos = mul(Out.Pos, projection);
	Out.Tex = input.Tex;
	return Out;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = Tex2D.Sample(samState, input.Tex);
	//float4 color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return color;
}
