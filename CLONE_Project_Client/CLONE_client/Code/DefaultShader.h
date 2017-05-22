#pragma once

#include "Shader.h"

class CBuffer;
class CDefaultShader
	: public CShader
{
public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, 
		CBuffer* pBuffer, bool bCollision);

public:
	static CDefaultShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CDefaultShader(CDevice* pDevice);
	explicit CDefaultShader(const CDefaultShader& rhs);

public:
	virtual ~CDefaultShader(void);
};