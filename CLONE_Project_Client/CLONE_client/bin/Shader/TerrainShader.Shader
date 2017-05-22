//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo TerrainShader.Shader

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
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 vSpecular : COLOR1;
	float3 normal : NORMAL;
};

struct GBuffer
{
	float4 Albedo : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Depth : SV_Target2;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	//행렬 계산 전 w를 1로 만든다.
	input.position.w = 1.0f;

	// 파이프라인 계산
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);

	// 노멀벡터 계산
	output.normal = mul(input.normal, (float3x3)World);

	// 노멀벡터 정규화
	output.normal = normalize(output.normal);

	// 텍스쳐 저장
	output.tex = input.tex;

	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
GBuffer PS(VS_OUTPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	//여기서부터 뽑는다.
	output.Albedo = gTexture.Sample(gSamplerState, input.tex);
	output.Normal = float4(input.normal.xyz * 0.5f + 0.5f, input.position.w / 1000.f);
	output.Depth = float4(input.position.z / input.position.w, input.position.w / 200.f, 0.f, 0.f);

	return output;
}