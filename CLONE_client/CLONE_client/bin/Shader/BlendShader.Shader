// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo BlendShader.Shader

Texture2D		g_AlbedoTexture : register(t0);
Texture2D		g_LightTexture : register(t1);
Texture2D		g_SpecularTexture : register(t2);

SamplerState	g_SamplerState : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

//cbuffer FogBuffer : register(b1)
//{
//	float4 FogInfo;
//}

struct VS_IN
{
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float  fogFactor : FOG;
};

struct PS_OUT
{
	float4		Color : COLOR0;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	Out.pos = mul(In.pos, matAll);
	Out.normal = In.normal;
	Out.tex2dcoord = In.tex2dcoord;

	//// -------------------------------- Fog Factor
	//float4 CamPosition;

	//// CamPosition Calc
	//CamPosition = mul(In.pos, World);
	//CamPosition = mul(CamPosition, View);

	//Out.fogFactor = saturate((FogInfo.y - CamPosition.z) / (FogInfo.y - FogInfo.x));

	return Out;
}

PS_OUT PS(VS_OUT In) : SV_Target	// 0~7까지 랜더타겟 지원...
{
	PS_OUT	Out = (PS_OUT)0.f;

	float4		Color[3];
	//float4		fogColor = float4(0.5f, 0.5f, 0.5f, 1.f);

	Color[0] = g_AlbedoTexture.Sample(g_SamplerState, In.tex2dcoord);
	Color[1] = g_LightTexture.Sample(g_SamplerState, In.tex2dcoord);
	Color[2] = g_SpecularTexture.Sample(g_SamplerState, In.tex2dcoord);

	// Fog
	//Color[0] = In.fogFactor * Color[0] + (1.f - In.fogFactor) * fogColor;
	
	Out.Color = Color[0] * Color[1] + Color[2];

	return Out;
}

technique11 Default_Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
};