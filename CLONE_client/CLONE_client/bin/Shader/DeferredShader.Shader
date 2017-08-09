// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo DeferredShader.Shader

Texture2D gAlbedoTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gDepthTexture : register(t2);
Texture2D gLightTexture : register(t3);
Texture2D gSpecularTexture : register(t4);

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

	input.pos.w = 1.f;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(input.pos, matAll);
	output.tex2dcoord = input.tex2dcoord;
	output.normal = input.normal;

	return output;
}

GBuffer PS(VS_OUTPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	output.Albedo = gAlbedoTexture.Sample(gSamplerState, input.tex2dcoord);
	output.Normal = gNormalTexture.Sample(gSamplerState, input.tex2dcoord);
	output.Depth = gDepthTexture.Sample(gSamplerState, input.tex2dcoord);

	return output;
}

technique11 Default_Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}