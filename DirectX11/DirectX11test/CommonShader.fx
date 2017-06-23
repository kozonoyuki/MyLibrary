//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
Texture2D ShadowMap : register(t1);

SamplerState samLinear : register(s0);
SamplerState samBorder : register(s1);

cbuffer ConvertConstantBuffer : register(b0)
{
	matrix scale;
	matrix rotation;
	matrix World;
	matrix View;
	matrix Projection;
	matrix SMWVP;
	float4 Diffuse;
	float4 EyePos;
	float4 FogColor;
	float3 Light;
	float near;
	float far;
	float ParamOne;
	float ParamTwo;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
	float4 PosLocal : POSITION;
	float3 PosView : POSVIEW;
	float3 PosSM : POSITION_SM;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(float4(input.Pos, 1.0), scale);
	output.Pos = mul(output.Pos, rotation);
	output.Pos = mul(output.Pos, World);
	output.PosLocal = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.PosView = output.Pos.xyz / output.Pos.w;
	output.Pos = mul(output.Pos, Projection);
	output.Tex = input.Tex;
	output.Norm = mul(input.Norm, (float3x3)scale);
	output.Norm = mul(output.Norm, (float3x3)rotation);
	output.Norm = mul(output.Norm, (float3x3)World);
	output.Norm = mul(output.Norm, (float3x3)View);
	//output.Norm = mul(input.Norm, (float3x3)rotation);
	//output.Norm = mul(output.Norm, (float3x3)View);

	float4 pos4 = mul(float4(input.Pos, 1.0), SMWVP);
	pos4.xyz = pos4.xyz / pos4.w;
	output.PosSM.x = (pos4.x + 1.0) / 2.0;
	output.PosSM.y = (-pos4.y + 1.0) / 2.0;
	output.PosSM.z = pos4.z;

	return output;
}

float GetDiffuseColor(float3 lightdir, float3 normal)
{
	float Out;
	// ハーフランバート
	Out = dot(lightdir, normal);
	Out = Out * 0.5f + 0.5f;
	Out *= Out;

	return Out;
}

float4 lighting(PS_INPUT input)
{
	float3 light = Light - input.PosView;
	return  GetDiffuseColor(normalize(light), normalize(input.Norm));
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	
	float dist = distance(input.PosLocal.xyz, EyePos.xyz);
	float fog = (far - dist) / (far - near);
	fog = clamp(fog, 0.0f, 1.0f);
	fog = pow(1.0f - pow(fog, ParamOne), ParamTwo);
	
	float bright = lighting(input);
	float4 col = txDiffuse.Sample(samLinear, input.Tex);
	//float4 col = ShadowMap.Sample(samLinear, input.Tex);
	float sm = ShadowMap.Sample(samBorder, input.PosSM.xy);
	float sma = (input.PosSM.z < sm+0.005f) ? 1.0 : 0.5;
	return saturate(col * Diffuse * sma) * (1.0f - fog) + FogColor * fog;
	//return saturate(col * Diffuse * sma * bright) * (1.0f - fog) + FogColor * fog;
}

//--------------------------------------------------------------------------------------
// ShadowMap
//--------------------------------------------------------------------------------------
float4 VS_SM(VS_INPUT input) : SV_POSITION {
	float4 pos = float4(input.Pos, 1.0);
	pos = mul(pos, scale);
	pos = mul(pos, rotation);
	pos = mul(pos, World);
	pos = mul(pos, View);
	pos = mul(pos, Projection);
	return pos;
}