// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo ReflectionShader.Shader

Texture2D	shaderTexture : register(t0);
Texture2D	reflectionTexture : register(t1);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer ReflectionBuffer : register(b1)
{
	matrix reflectionMatrix;
};

struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	matrix reflectProjectWorld;

	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.tex = input.tex;


	reflectProjectWorld = mul(reflectionMatrix, Projection);
	reflectProjectWorld = mul(World, reflectProjectWorld);

	//output.reflectionPosition = World
	//output.reflectionPosition = mul(input.position, World);
	output.reflectionPosition = mul(input.position, reflectProjectWorld);

	return output;
}

//////////////////////////////////////////////////////////////////////////////

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 textureColor;
	float2 reflectTexCoord;
	float4 reflectionColor;
	float4 color;
	float flipReflection = 0.5f;
	float distanceReflection = 2.0f;

	textureColor = shaderTexture.Sample(SampleType, input.tex);
	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / distanceReflection + flipReflection;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / distanceReflection + flipReflection;

	reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

	return lerp(textureColor, reflectionColor, 0.2f);
}