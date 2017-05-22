// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo DepthShader.Shader

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT
{
	float4 position : POSITION;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

VS_OUT VS(VS_INPUT input)
{
	VS_OUT output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.depthPosition = output.position;
	output.position = mul(output.position, projectionMatrix);

	return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
	float depthValue;
	float4 color;

	depthValue = input.position.w / 200.f;

	color = float4(depthValue, 0.f, 0.f, 1.0f);

	return color;
}
