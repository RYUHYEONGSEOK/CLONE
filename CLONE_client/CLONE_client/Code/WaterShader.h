#pragma once

#include "Shader.h"

class CBuffer;
class CObj;
class CWaterShader
	: public CShader
{
private:
	ID3D11Buffer*		m_pReflectionBuffer;
	ID3D11Buffer*		m_pWaterBuffer;

public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		D3DXMATRIX reflectionMatrix,
		CBuffer*	pBuffer,
		LightInfo * pLightInfo,
		ID3D11ShaderResourceView * pNormalTexture,
		ID3D11ShaderResourceView * reflectionTexture,
		ID3D11ShaderResourceView * refractionTexture,
		float waterTranslation, float reflectRefractScale);

public:
	static CWaterShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CWaterShader(CDevice* pDevice);
	explicit CWaterShader(const CWaterShader& rhs);

public:
	virtual ~CWaterShader(void);
};