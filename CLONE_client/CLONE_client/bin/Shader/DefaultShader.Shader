// 쉐이더 컴파일방법!
//		  버텍스 함수									쉐이더파일이름
//  fxc /E VS /T vs_5_0 /Od /Zi /Fo CompiledShader.fxo DefaultShader.Shader

cbuffer MatrixBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
};

cbuffer Float4Buffer : register(b1)
{
	float4	g_fCollision;
};

struct VS_INPUT
{
	float4 pos  : POSITION;
	float4 Color : COLOR;
};

struct VS_OUTPUT
{
	float4 pos  : SV_POSITION;
	float4 Color : COLOR;
};

VS_OUTPUT VS(VS_INPUT vin)
{
	VS_OUTPUT vout = (VS_OUTPUT)0;

	vin.pos.w = 1.f;

	matrix matAll = mul(World, View);
	matAll = mul(matAll, Projection);

	vout.pos = mul(vin.pos, matAll);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;

	return vout;
}

float4 PS(VS_OUTPUT pin) : SV_Target
{
	if (g_fCollision.x == 0.f)
		return pin.Color;
	else
		return float4(1.0f, 0.f, 0.f, 1.f);
}

technique11 Default_Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}