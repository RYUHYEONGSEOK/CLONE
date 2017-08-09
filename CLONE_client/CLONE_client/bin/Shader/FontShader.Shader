Texture2D gTexture : register(t0);
SamplerState gSamplerState : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer cbPixelBuffer
{
	float4 gPixelColor : packoffset(c0);
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex2dcoord : TEXCOORD0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	input.pos.w = 1.f;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);
	output.tex2dcoord = input.tex2dcoord;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4	color = gTexture.Sample(gSamplerState, input.tex2dcoord);

	if (0.0f == color.r)
		color.a = 0.0f;
	else
	{
		color.a = 1.0f;
		color = color * gPixelColor;
	}

	return color;
}