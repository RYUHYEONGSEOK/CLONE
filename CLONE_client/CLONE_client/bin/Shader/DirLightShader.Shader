// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E PS /T ps_5_0 /Od /Zi /Fo CompiledShader.fxo DirLightShader.Shader

Texture2D		g_NormalTexture : register(t0);
Texture2D		g_DepthTexture : register(t1);
Texture2D		g_ShadowTexture : register(t2);

SamplerState	g_SamplerState : register(s0);
SamplerComparisonState	cmpSampler
{
	//Sampler State
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AdressU = BORDER;
	AdressV = BORDER;

	BorderColor[0] = 1.f;
	BorderColor[1] = 1.f;
	BorderColor[2] = 1.f;
	BorderColor[3] = 1.f;

	//Sampler Comparison State
	ComparisonFunc = LESS_EQUAL;
};

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer PixelMatrixBuffer : register(b1)
{
	vector CamPos;
	matrix ProjInv;
	matrix ViewInv;
	matrix VPInv;
};

//cbuffer FogBuffer : register(b2)
//{
//	float4 FogInfo;
//}

cbuffer LightBuffer : register(b3)
{
	float4	ambientColor;
	float4	diffuseColor;
	float4	lightDirection;
	float4	specularColor;
	float4	lightPos;
};

cbuffer LightMatrixBuffer : register(b4)
{
	matrix LightView;
	matrix LightProj;
};

cbuffer PixelMatrixBuffer2 : register(b5)
{
	vector CamPos2;
	matrix ProjInv2;
	matrix ViewInv2;
	matrix VPInv2;
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
	float4 lightViewPos : TEXCOORD1;
	float3 vRayDir		: TEXCOORD2;
	//float  fogFactor	: FOG;
};

struct GBuffer
{
	float4 Shade	: SV_Target0;
	float4 Spec		: SV_Target1;
};

VS_OUT VS(VS_IN In)
{
	VS_OUT output = (VS_OUT)0;

	In.pos.w = 1.f;

	// --------------------------------
	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	output.pos = mul(In.pos, matAll);

	// --------------------------------
	output.normal = mul(In.normal, (float3x3)World);
	output.normal = normalize(output.normal);

	// --------------------------------
	output.tex2dcoord = In.tex2dcoord;

	// -------------------------------- RayDir
	matrix matLightAll = mul(LightView, LightProj);

	output.lightViewPos = mul(In.pos, World);
	output.lightViewPos = mul(output.lightViewPos, matLightAll);

	float2 uv;
	uv.x = In.tex2dcoord.x * 2.f - 1.f;
	uv.y = In.tex2dcoord.y * -2.f + 1.f;

	output.vRayDir = mul(float4(uv.xy, 1.f, 1.f) * 200.f, VPInv).xyz;
	output.vRayDir -= CamPos.xyz;

	// -------------------------------- Fog Factor
	//output.fogFactor = saturate((FogInfo.y - CamPos.z) / (FogInfo.y - FogInfo.x));

	return output;
}

float2 texOffset(int u, int v)
{
	//					ShadowMapSize.x		ShadowMapSize.y
	return float2(u * 1.f / (1280.f), v * 1.f / (720.f));
}

GBuffer PS(VS_OUT input) : SV_Target	// 0~7까지 랜더타겟 지원...
{
	GBuffer	Out = (GBuffer)0.f;

	// Bias를 줄이면 더 각져보이게 나온다
	float fBias = 0.00003f;

	vector	vNormal = g_NormalTexture.Sample(g_SamplerState, input.tex2dcoord);
	vector	vDepth = g_DepthTexture.Sample(g_SamplerState, input.tex2dcoord);

	// --------------------------------
	float vViewZ = vNormal.w * 200.f;

	vector		vWorldNormal = vector(vNormal.xyz * 2.f - 1.f, 0.f);
	vector		vWorldLight = normalize(lightDirection);

	Out.Shade = max(dot(vWorldLight * -1, vWorldNormal), 0.f);
	Out.Shade = Out.Shade * diffuseColor + ambientColor;
	// --------------------------------
	// Linear-PCF Shadow -------------------------------------------------------
	vector vPxlPosw = CamPos2 + (vector(input.vRayDir.xyz, 1.f) * vDepth.x);
	vPxlPosw.w = 1.f;
	vector vPxlV = mul(vPxlPosw, LightView);
	vector vPxlVP = mul(vPxlV, LightProj);

	float  fDepth = vPxlV.z / 200.f;
	float2 uv = vPxlVP.xy / vPxlVP.w;

	uv.y = -uv.y;
	uv = uv * 0.5f + 0.5f;

	vector	vShadow = g_ShadowTexture.Sample(g_SamplerState, uv);

	////3X3 Pixel 계산용
	//float sum = 0;
	//float x, y;

	//for (y = -1.5f; y <= 1.5f; y += 1.f)
	//{
	//	for (x = -1.5f; x <= 1.5f; x += 1.f)
	//	{
	//		sum += g_ShadowTexture.SampleCmpLevelZero(cmpSampler, uv.xy + texOffset(x, y), fDepth);
	//	}
	//}

	//float fShadowFactor = sum / 16.f;

	//Out.Shade *= fShadowFactor * 5.2f;
	//Out.Shade.a = 1.f;

	if (fDepth > vShadow.x + fBias)
	{
		Out.Shade = 0.6f;
		Out.Shade.a = 1.f;
	}

	Out.Shade *= 1.f;

	// -------------------------------- Fog
	/*float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.f);
	float fogFac = saturate((10.f - vDepth.x) / (10.f - 0.f));

	Out.Shade = fogFac * Out.Shade + (1.f - fogFac) * fogColor;*/

	// -------------------------------- 수정필요!
	vector		vReflect = normalize(reflect(vWorldLight, vWorldNormal));

	vector		vPosition;//픽셀의위치
	vPosition.x = (input.tex2dcoord.x * 2.f - 1.f) * vViewZ;
	vPosition.y = (input.tex2dcoord.y * -2.f + 1.f) * vViewZ;
	vPosition.z = vDepth.y * vViewZ;
	vPosition.w = vViewZ;

	vPosition = mul(vPosition, ProjInv2);
	vPosition = mul(vPosition, ViewInv2);

	vector		vAt = normalize(CamPos2 - vPosition);

	Out.Spec = pow(max(dot(vReflect, vAt), 0.f), 10.f);
	Out.Spec.a = 0.3f;

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

// 그림자 옛날꺼 --------------------------------------------------

//PS_OUT         Out = (PS_OUT)0;
/////// 그림자부분 //////
//float   fLinearDepth = tex2D(DepthSampler, In.vUV).r;
//vector   vPxlPosW = g_vWorldCamPos + (vector(In.vRayDir.xyz, 1.f) * fLinearDepth);
//////
//if (1 == fLinearDepth)
//{
//	Out.vColor = vector(1.f, 1.f, 1.f, g_fLightCnt);
//	return Out;
//}
//float sum = 0;
//if (1 != tex2D(DepthSampler, In.vUV).b)
//{
//	vector   vPxlPosLVP;
//	vector   vPxlPosLV = vPxlPosW;
//	vPxlPosLV.w = 1.f;
//	vPxlPosLV = mul(vPxlPosLV, g_matLightView);
//	vPxlPosLVP = mul(vPxlPosLV, g_matLightProj);

//	float   fDepth = vPxlPosLV.z * 0.001f;
//	float2 uv = vPxlPosLVP.xy / vPxlPosLVP.w;
//	uv.y = -uv.y;
//	uv = uv * 0.5f + 0.5f;
//	float shadow = 1.f;
//	float DepthSize = tex2D(ShadowMapSampler, uv).r;

//	if (fDepth > DepthSize + 0.0003f)
//	{
//		shadow = 0.5f;
//	}

//	Out.vColor *= shadow;


////////////////////////////////////////////////////////////////

////3X3 Pixel 계산용
//float sum = 0;
//float x, y;

//for (y = -1.5f; y <= 1.5f; y += 1.f)
//{
//	for (x = -1.5f; x <= 1.5f; x += 1.f)
//	{
//		sum += g_ShadowTexture.SampleCmpLevelZero(cmpSampler, uv.xy + texOffset(x, y), fDepth);
//	}
//}

//float fShadowFactor = sum / 16.f;

//Out.Shade *= fShadowFactor * 5.2f;
//Out.Shade.a = 1.f;

//if (fDepth > vShadow.x + fBias)
//{
//	Out.Shade = 0.6f;
//	Out.Shade.a = 1.f;
//}
//
//Out.Shade *= 1.f;