// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo WaterShader.Shader

Texture2D normalTexture : register(t0);
Texture2D reflectionTexture : register(t1);
Texture2D refractionTexture : register(t2);
SamplerState SampleType : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer ReflectionBuffer  : register(b1)
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
	float4 refractionPosition : TEXCOORD2;
};

struct GBuffer
{
	float4 Albedo : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Depth : SV_Target2;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	matrix reflectProjectWorld;
	matrix viewProjectWorld;

	input.position.w = 1.0f;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.tex = input.tex;

	// 반사 월드 매트릭스를 만든다
	reflectProjectWorld = mul(reflectionMatrix, Projection);
	reflectProjectWorld = mul(World, reflectProjectWorld);
	output.reflectionPosition = mul(input.position, reflectProjectWorld);
	viewProjectWorld = mul(View, Projection);
	viewProjectWorld = mul(World, viewProjectWorld);

	output.refractionPosition = mul(input.position, viewProjectWorld);

	return output;
}

////////////////////////////////////////////////////////////////////////
cbuffer WaterBuffer : register(b0)
{
	float waterTranslation;
	float reflectRefractScale;
	float2 padding;
};

GBuffer PS(VS_OUTPUT input) : SV_Target
{
	GBuffer output = (GBuffer)0;

	float4 vNormal = normalTexture.Sample(SampleType, input.tex);

	float2 reflectTexCoord;
	float2 refractTexCoord;
	float4 normalMap;
	float3 normal;
	float4 reflectionColor;
	float4 refractionColor;
	float flipWaterRefracReflec = 0.5f;
	float distanceWaterRefracReflec = 2.0f;

	// 물 노멀 움직이기
	input.tex.y += waterTranslation;

	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;
	refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;
	refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / distanceWaterRefracReflec + flipWaterRefracReflec;

	normalMap = normalTexture.Sample(SampleType, input.tex);
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);
	reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
	refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	//여기서부터 뽑는다.
	if (padding.x == 0.f)
	{
		// 반사를 섞는다
		output.Albedo = lerp(reflectionColor, refractionColor, 0.6f);	// 여기 수정되있음
		output.Albedo.r *= 7.8f;
		output.Albedo.g *= 10.9f;
		output.Albedo.b *= 15.f;
	
	}
	else if (padding.x == 1.f)
	{
		// 반사를 섞는다
		output.Albedo = lerp(reflectionColor, refractionColor, 0.6f);	// 여기 수정되있음
		output.Albedo.r *= 1.6f;
		output.Albedo.g *= 2.3f;
		output.Albedo.b *= 3.14f;
	}
	
	output.Normal = float4(vNormal.xyz * 0.5f + 0.5f, input.position.w / 200.f);
	//output.Depth = input.position.z / input.position.w;
	output.Depth = input.position.w / 200.f;

	return output;
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
