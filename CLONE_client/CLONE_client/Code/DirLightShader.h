#pragma once

#include "Shader.h"

class CBuffer;
class CObj;
class CDirLightShader
	: public CShader
{
private:
	ID3D11Buffer*		m_pPixelMatrixBuffer;
	ID3D11Buffer*		m_pLightMatrixBuffer;
	//ID3D11Buffer*		m_pFogBuffer;

public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);

public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix,
		XMFLOAT4X4 matViewInvMatrix, XMFLOAT4X4 matProjInvMatrix,
		LightInfo* pLightInfo,
		LightMatrixStruct* pLightMatInfo,
		ID3D11ShaderResourceView** ppResource,
		const WORD& wCntResource,
		CObj* pCam);

public:
	static CDirLightShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CDirLightShader(CDevice* pDevice);
	explicit CDirLightShader(const CDirLightShader& rhs);

public:
	virtual ~CDirLightShader(void);
};