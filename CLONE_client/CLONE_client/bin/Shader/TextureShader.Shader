Texture2D gTexture : register(t0);
SamplerState gSamplerState : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};	

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct GBuffer
{
	float4 Albedo : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Depth : SV_Target2;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);
	output.normal = input.normal;
	output.tex2dcoord = input.tex2dcoord;

	return output;
}

GBuffer PS(VS_INPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	output.Albedo = gTexture.Sample(gSamplerState, input.tex2dcoord);
	output.Normal = float4(input.normal.xyz * 0.5f + 0.5f, input.pos.w / 200.f);
	//output.Depth = input.pos.z / input.pos.w ;
	output.Depth = input.pos.w / 200.f;

	return output;
}

technique11 ColorTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
