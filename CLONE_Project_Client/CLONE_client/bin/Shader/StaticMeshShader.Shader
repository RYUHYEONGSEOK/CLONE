// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo StaticMeshShader.Shader

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

	output.pos = mul(float4(input.pos.xyz, 1.f), matAll);
	output.normal = normalize(mul(float4(input.normal.xyz, 0.f), World));
	output.tex2dcoord = input.tex2dcoord;

	return output;
}

GBuffer PS(VS_OUTPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	output.Albedo = gTexture.Sample(gSamplerState, input.tex2dcoord);
	output.Normal = float4(input.normal.xyz * 0.5f + 0.5f, input.pos.w / 200.f);
	//output.Depth = input.pos.z / input.pos.w ;
	output.Depth = input.pos.w / 200.f;

	return output;
}
