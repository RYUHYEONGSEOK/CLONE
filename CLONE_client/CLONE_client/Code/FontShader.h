#pragma once

#include "Shader.h"

class CBuffer;
class CFontShader : public CShader
{
public:
	static CResource*		Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual CResource*		Clone();

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer* pBuffer,
		LightInfo* pLightInfo,
		ID3D11ShaderResourceView* pResource);

public:
	virtual HRESULT			Ready_ShaderFile(const TCHAR* pFilePath);
	virtual void			Update();
private:
	virtual void			Release();

private:
	explicit CFontShader(CDevice* pDevice);
	explicit CFontShader(const CFontShader& rhs);

public:
	virtual ~CFontShader(void);
};
