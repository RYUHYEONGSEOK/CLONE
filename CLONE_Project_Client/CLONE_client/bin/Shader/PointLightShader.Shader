// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E PS /T ps_5_0 /Od /Zi /Fo CompiledShader.fxo PointLightShader.Shader

Texture2D		g_NormalTexture : register(t0);
Texture2D		g_DepthTexture : register(t1);
Texture2D		g_ShadowTexture : register(t2);

SamplerState	g_SamplerState : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer LightBuffer : register(b1)
{
	float4	ambientColor;
	float4	diffuseColor;
	float4	lightDirection;
	float4	specularColor;
	float4	lightPos;
};

cbuffer PixelMatrixBuffer : register(b2)
{
	vector CamPos;
	matrix ViewInv;
	matrix ProjInv;
	matrix VPInv;
	vector Range;	// Range vector의 x값을 Range로 씀
};

struct VS_IN
{
	float4 pos			: POSITION;
	float3 normal		: NORMAL;
	float2 tex2dcoord	: TEXCOORD0;
};

struct VS_OUT
{
	float4 pos			: SV_POSITION;
	float3 normal		: NORMAL;
	float2 tex2dcoord	: TEXCOORD0;
};

struct GBuffer
{
	float4 Shade : SV_Target0;
	float4 Spec : SV_Target1;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT output = (VS_OUT)0;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(In.pos, matAll);
	output.normal = mul(In.normal, (float3x3)World);
	output.normal = normalize(output.normal);
	output.tex2dcoord = In.tex2dcoord;

	return output;
}

GBuffer PS(VS_OUT input) : SV_Target	// 0~7까지 랜더타겟 지원...
{
	GBuffer	Out = (GBuffer)0.f;

	vector	vNormal = g_NormalTexture.Sample(g_SamplerState, input.tex2dcoord);
	vector	vDepth	= g_DepthTexture.Sample(g_SamplerState, input.tex2dcoord);

	float	vViewZ			= vNormal.w * 200.f;
	vector	vWorldNormal	= vector(vNormal.xyz * 2.f - 1.f, 0.f);

	vector		vPosition;//픽셀의위치
	vPosition.x = (input.tex2dcoord.x * 2.f - 1.f) * vViewZ;
	vPosition.y = (input.tex2dcoord.y * -2.f + 1.f) * vViewZ;
	vPosition.z = vDepth.r / 10.f * vViewZ;
	vPosition.w = vViewZ;

	vPosition = mul(vPosition, ProjInv);
	vPosition = mul(vPosition, ViewInv);

	vector	vWorldLight = (vPosition - lightPos);
	float	fDistance	= length(vWorldLight);
	vWorldLight			= normalize(vWorldLight);

	float fAtt = max((Range.x - fDistance) / Range.x, 0.f);

	Out.Shade	= max(dot(vWorldLight * -1, vWorldNormal), 0.f) * fAtt;
	Out.Shade	= Out.Shade * diffuseColor + ambientColor;

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

// 예전코드 --------------------------------------------------

//float3	lightDir;
//float	lightIntensity;
//float4	color;

//// 모든 픽셀에 default값을 넣고 시작
//color = ambientColor;

//// 빛의 방향 거꾸로 계산
//lightDir = -lightDirection;

//// 픽셀에 라이트값을 넣는다
//lightIntensity = saturate(dot(input.normal, lightDir));

//if (lightIntensity > 0.0f)
//{
//	// 빛에대해 최종 색깔 값을 정한다
//	color += (diffuseColor * lightIntensity);
//}

//// 마지막 색깔값을 OUTPUT에 넣는다.
//color = saturate(color);

//// 텍스쳐 픽셀이랑 계산 + specular 곱해서 반사량적용
//color = color * textureColor * specularColor;

//이건 나중에 수정
//output.Light = textureColor;
//output.Spec = textureColor;