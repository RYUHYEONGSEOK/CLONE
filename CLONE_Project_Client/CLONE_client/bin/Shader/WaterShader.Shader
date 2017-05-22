// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo WaterShader.Shader

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

	vector vNormal = vector(input.normal.xyzw);
	vNormal = normalize(mul(vector(vNormal.xyz, 0.f), World));

	output.pos = mul(float4(input.pos.xyz, 1.f), matAll);
	output.normal = float4(vNormal);
	output.tex2dcoord = input.tex2dcoord;

	return output;
}

GBuffer PS(VS_OUTPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	//여기서부터 뽑는다.
	output.Albedo = gTexture.Sample(gSamplerState, input.tex2dcoord);
	output.Albedo.a = 0.9f;	//알파 Test
	output.Normal = float4(input.normal.xyz * 0.5f + 0.5f, input.pos.w / 200.f);
	//output.Depth = input.position.z / input.position.w;
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
