// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo RefactionShader.Shader

Texture2D		shaderTexture  : register(t0);
SamplerState	SampleType : register(s0);

cbuffer MatrixBuffer  : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer ClipPlaneBuffer  : register(b1)
{
	float4 clipPlane;
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
	float3 normal : NORMAL;
	float clip : SV_ClipDistance0;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;

	input.position.w = 1.0f;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)World);
	output.normal = normalize(output.normal);
	output.clip = dot(mul(input.position, World), clipPlane);

	return output;
}

//////////////////////////////////////////////////////////////////////////////
cbuffer LightBuffer  : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float4 lightSpec;
	float4 lightPos;
};

float4 PS(VS_OUTPUT input) :SV_TARGET
{
	float4 textureColor;
	float3 lightDir;
	float lightIntensity;
	float4 color;

	textureColor = shaderTexture.Sample(SampleType, input.tex);
	color = ambientColor;
	lightDir = -lightDirection;
	lightIntensity = saturate(dot(input.normal, lightDir));

	if (lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
	}

	color = saturate(color);
	color = color * textureColor;

	return color;
}
