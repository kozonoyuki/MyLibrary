//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D Tex2D : register(t0);
SamplerState samLinear : register(s0);

cbuffer SpriteConstantBuffer : register(b0)
{
	matrix projection;
	matrix scale;
	matrix rotation;
	matrix world;
	float uv_left;
	float uv_top;
	float uv_width;
	float uv_height;
	float uv_alpha;
	float uv_pivot_x;
	float uv_pivot_y;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
	float4 Col : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = input.Pos;
	output.Pos.x -= uv_pivot_x;
	output.Pos.y -= uv_pivot_y;
	output.Pos = mul(output.Pos, scale);
	output.Pos = mul(output.Pos, rotation);
	output.Pos = mul(output.Pos, world);
	output.Pos += float4(uv_pivot_x, uv_pivot_y, 0.0f, 0.0f);
	output.Pos = mul(output.Pos, projection);

	output.Tex = input.Tex * float2(uv_width, uv_height) + float2(uv_left, uv_top);
	output.Col = input.Col;
	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = Tex2D.Sample(samLinear, input.Tex) * input.Col;
	color.a *= uv_alpha;
	return color;
}
