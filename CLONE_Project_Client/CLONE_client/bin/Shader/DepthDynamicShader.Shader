#define MAX		128

// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo DepthDynamicShader.Shader

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer cbBoneWorldMatrix : register(b1)
{
	row_major matrix gMtxBone[MAX] : packoffset(c0);
};

struct VS_INPUT
{
	float3 position		: POSITION;
	uint4 Bones1		: BONES0;
	uint4 Bones2		: BONES1;
	float4 Weights1		: WEIGHTS0;
	float4 Weights2		: WEIGHTS1;
};

struct VS_OUTPUT
{
	float4 position			: SV_POSITION;
	float4 depthPosition	: TEXTURE0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 Pos = float4(input.position, 1);

	uint iBone0 = input.Bones1.r;
	uint iBone1 = input.Bones1.g;
	uint iBone2 = input.Bones1.b;
	uint iBone3 = input.Bones1.a;
	uint iBone4 = input.Bones2.r;
	uint iBone5 = input.Bones2.g;
	uint iBone6 = input.Bones2.b;
	uint iBone7 = input.Bones2.a;

	float fWeight0 = input.Weights1.r;
	float fWeight1 = input.Weights1.g;
	float fWeight2 = input.Weights1.b;
	float fWeight3 = input.Weights1.a;
	float fWeight4 = input.Weights2.r;
	float fWeight5 = input.Weights2.g;
	float fWeight6 = input.Weights2.b;
	float fWeight7 = input.Weights2.a;

	matrix m0 = gMtxBone[iBone0];
	matrix m1 = gMtxBone[iBone1];
	matrix m2 = gMtxBone[iBone2];
	matrix m3 = gMtxBone[iBone3];
	matrix m4 = gMtxBone[iBone4];
	matrix m5 = gMtxBone[iBone5];
	matrix m6 = gMtxBone[iBone6];
	matrix m7 = gMtxBone[iBone7];

	if (fWeight0 > 0) output.position += fWeight0 * mul(Pos, m0);
	if (fWeight1 > 0) output.position += fWeight1 * mul(Pos, m1);
	if (fWeight2 > 0) output.position += fWeight2 * mul(Pos, m2);
	if (fWeight3 > 0) output.position += fWeight3 * mul(Pos, m3);
	if (fWeight4 > 0) output.position += fWeight4 * mul(Pos, m4);
	if (fWeight5 > 0) output.position += fWeight5 * mul(Pos, m5);
	if (fWeight6 > 0) output.position += fWeight6 * mul(Pos, m6);
	if (fWeight7 > 0) output.position += fWeight7 * mul(Pos, m7);


	output.position = mul(float4(output.position.xyz, 1.f), World);
	output.position = mul(output.position, View);
	output.depthPosition = output.position;
	output.position = mul(output.position, Projection);

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
	float depthValue;
	float4 color;

	depthValue = input.depthPosition.w / 200.f;

	color = float4(depthValue, 0.f, 0.f, 1.0f);

	return color;
}