#pragma once

#include "Shader.h"

class CBuffer;
class CDepthDynamicShader
	: public CShader
{
public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		CBuffer* pBuffer);

public:
	static CDepthDynamicShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CDepthDynamicShader(CDevice* pDevice);
	explicit CDepthDynamicShader(const CDepthDynamicShader& rhs);

public:
	virtual ~CDepthDynamicShader(void);
};