#pragma once

#include "Shader.h"

class CBuffer;
class CDeferredShader
	: public CShader
{
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
	static CDeferredShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CDeferredShader(CDevice* pDevice);
	explicit CDeferredShader(const CDeferredShader& rhs);

public:
	virtual ~CDeferredShader(void);
};