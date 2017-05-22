Texture2D		g_SrcTexture;
Texture2D		g_DestTexture;
Texture2D		g_MultiTexture;
Texture2D		g_BrushTexture;

SamplerState SrcSampler;
SamplerState DestSampler;
SamplerState MultiSampler;
SamplerState BrushSampler;

vector		g_vCamPos;
vector		g_vBrushPos;
float		g_fBrushSize;

float		g_fDatail = 1.f;

cbuffer MatrixBuffer : register(b0)
{
	matrix g_matWorld;
	matrix g_matView;
	matrix g_matProj;
};

//sampler SrcSampler = sampler_state
//{
//	texture = g_SrcTexture;
//	minfilter = linear;
//	magfilter = linear;
//};
//
//sampler DestSampler = sampler_state
//{
//	texture = g_DestTexture;
//	minfilter = linear;
//	magfilter = linear;
//};
//
//sampler MultiSampler = sampler_state
//{
//	texture = g_MultiTexture;
//	minfilter = linear;
//	magfilter = linear;
//};
//
//sampler BrushSampler = sampler_state
//{
//	texture = g_BrushTexture;
//	minfilter = linear;
//	magfilter = linear;
//};


struct VS_IN
{
	vector		vPosition	: POSITION;
	vector		vNormal		: NORMAL;
	float2		vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	vector		vPosition		: SV_POSITION;
	float2		vTexUV			: TEXCOORD0;
	vector		vWorldPos		: TEXCOORD1;
	vector		vWorldNormal	: TEXCOORD2;
	vector		vProjPos		: TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;
	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(vector(In.vPosition.xyz, 1.f), g_matWorld);
	Out.vWorldNormal = mul(vector(In.vNormal.xyz, 0.f), g_matWorld);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	vector		vPosition		: SV_POSITION;
	float2		vTexUV			: TEXCOORD0;
	vector		vWorldPos		: TEXCOORD1;
	vector		vWorldNormal	: TEXCOORD2;
	vector		vProjPos		: TEXCOORD3;
};

struct PS_OUT
{
	vector		vColor		: COLOR0;
	vector		vNormal		: COLOR1;
	vector		vDepth		: COLOR2;
};

PS_OUT PS_MAIN(PS_IN In) : SV_Target
{
	PS_OUT	Out = (PS_OUT)0.0f;

	//Out.vColor = tex2D(BaseSampler, In.vTexUV * g_fDatail);
	vector		vColor[4];
	vColor[0] = tex2D(SrcSampler, In.vTexUV * g_fDatail);
	vColor[1] = tex2D(DestSampler, In.vTexUV * g_fDatail);
	vColor[2] = tex2D(MultiSampler, In.vTexUV);
	//vColor[3] = tex2D(BrushSampler, In.vTexUV);
	vColor[3] = (vector)0.f;


	if (g_vBrushPos.x - g_fBrushSize < In.vWorldPos.x
		&& In.vWorldPos.x < g_vBrushPos.x + g_fBrushSize
		&& g_vBrushPos.z - g_fBrushSize < In.vWorldPos.z
		&& In.vWorldPos.z < g_vBrushPos.z + g_fBrushSize)
	{
		float2		vNewUV = (float2)0.f;

		vNewUV.x = (In.vWorldPos.x - (g_vBrushPos.x - g_fBrushSize)) / (g_fBrushSize * 2.f);
		vNewUV.y = ((g_vBrushPos.z + g_fBrushSize) - In.vWorldPos.z) / (g_fBrushSize * 2.f);

		vColor[3] = tex2D(BrushSampler, vNewUV);	//0.5 0.5
	}

	Out.vColor = vColor[0] * vColor[2] + vColor[1] * (1.f - vColor[2]);

	Out.vNormal = vector(In.vWorldNormal.xyz * 0.5f + 0.5f, In.vProjPos.w / 1000.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 1000.f, 0.f, 0.f);

	return Out;
}

technique11 Default_Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_MAIN()));
	}
};