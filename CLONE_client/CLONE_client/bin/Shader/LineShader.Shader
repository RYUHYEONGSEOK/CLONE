cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOUR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOUR;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);

	output.color = input.color;

	return output;
}

float4 PS(VS_OUTPUT input) : SV_Target
{
	return input.color;
}