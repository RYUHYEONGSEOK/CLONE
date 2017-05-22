Texture2D gTexture;
SamplerState gSamplerState : register(s0);
TextureCube gTextureCube : register(t0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex2dcoord : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float3 tex2dcoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);
	output.tex2dcoord = input.pos;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = gTextureCube.Sample(gSamplerState, input.tex2dcoord);
	color.a = 1.f;
	return color;
}