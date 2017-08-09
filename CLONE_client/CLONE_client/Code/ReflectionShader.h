#pragma once

#include "Shader.h"

class CBuffer;
class CObj;
class CReflectionShader
	: public CShader
{
private:
	ID3D11Buffer*		m_pReflectionBuffer;

public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer*	pBuffer,
		LightInfo * pLightInfo,
		ID3D11ShaderResourceView * pResource,
		D3DXMATRIX matReflection);

public:
	static CReflectionShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CReflectionShader(CDevice* pDevice);
	explicit CReflectionShader(const CReflectionShader& rhs);

public:
	virtual ~CReflectionShader(void);
};