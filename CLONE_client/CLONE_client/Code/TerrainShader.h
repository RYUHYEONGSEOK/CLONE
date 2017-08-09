#pragma once

#include "Shader.h"

class CTerrainShader
	: public CShader
{
public:
	HRESULT Ready_ShaderFile(const TCHAR* pFilePath);
	void BeginShader(const int& iPassIdx = 0);
	void EndShader(void);
	
public:
	HRESULT SetShaderParameter(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, 
		LightInfo* pLightInfo,
		ID3D11ShaderResourceView* pResource);

public:
	static CTerrainShader* Create(CDevice* pDevice, const TCHAR* pFilePath);
	virtual void Release(void);
	virtual	CResource*	Clone(void);
	virtual void		Update(void);

private:
	explicit CTerrainShader(CDevice* pDevice);
	explicit CTerrainShader(const CTerrainShader& rhs);

public:
	virtual ~CTerrainShader(void);
};