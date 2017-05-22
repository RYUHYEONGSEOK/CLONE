#pragma once

#include "Shader.h"

class CBuffer;
class CLogoBackShader
	: public CShader
{
public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer* pBuffer, ID3D11ShaderResourceView* pResource, XMFLOAT4 xmvParam);

public:
	static CLogoBackShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CLogoBackShader(CDevice* pDevice);
	explicit CLogoBackShader(const CLogoBackShader& rhs);

public:
	virtual ~CLogoBackShader(void);
};