#pragma once

#include "Shader.h"

class CBuffer;
class CBlendShader
	: public CShader
{
private:
	//ID3D11Buffer*		m_pFogBuffer;

public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer* pBuffer,
		LightInfo* pLightInfo,
		ID3D11ShaderResourceView** ppResource,
		const WORD& wCntResource);

public:
	static CBlendShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CBlendShader(CDevice* pDevice);
	explicit CBlendShader(const CBlendShader& rhs);

public:
	virtual ~CBlendShader(void);
};