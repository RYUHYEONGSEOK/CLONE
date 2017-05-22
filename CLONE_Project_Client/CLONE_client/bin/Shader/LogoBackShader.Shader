// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo LogoBackShader.Shader

Texture2D gTexture : register(t0);
SamplerState gSamplerState : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer Float4Buffer : register(b1)
{
	float4	g_fPercent;
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

float4 PS(VS_INPUT input) : SV_Target
{
	float4 fColor = gTexture.Sample(gSamplerState, input.tex2dcoord);

	if (input.tex2dcoord.x >= g_fPercent.x)
		fColor.rgb *= 0.5f;

	return fColor;
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
