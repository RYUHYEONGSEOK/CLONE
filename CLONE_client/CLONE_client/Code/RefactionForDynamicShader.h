#pragma once

#include "Shader.h"

class CBuffer;
class CRefactionForDynamicShader
	: public CShader
{
private:
	ID3D11Buffer*		m_pClipPlaneBuffer;

public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer* pBuffer,
		LightInfo* pLightInfo,
		ID3D11ShaderResourceView* pResource,
		D3DXVECTOR4 vClipPlane);

public:
	static CRefactionForDynamicShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CRefactionForDynamicShader(CDevice* pDevice);
	explicit CRefactionForDynamicShader(const CRefactionForDynamicShader& rhs);

public:
	virtual ~CRefactionForDynamicShader(void);
};