cbuffer LineConstantBuffer : register(b0)
{
	matrix c_View;
	matrix c_Projection;
}

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Col : COLOR0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Col : COLOR0;
};

PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, c_View);
	output.Pos = mul(output.Pos, c_Projection);
	output.Col = input.Col;
	return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
	return input.Col;
}