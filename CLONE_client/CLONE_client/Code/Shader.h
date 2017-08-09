#pragma once

#include "Resources.h"

class CDevice;
class CShader abstract : public CResource
{
protected:
	ID3D11VertexShader*			m_pVS;
	ID3D11PixelShader*			m_pPS;
	ID3D11InputLayout*			m_pLayout;

	ID3D11SamplerState*			m_pSampleState;
	ID3D11Buffer*				m_pMatrixBuffer;
	ID3D11Buffer*				m_pLightBuffer;

	ID3D11Buffer*				m_pFloat4Buffer;

public:	//±ÍÂúÀ¸´Ï ÆÛºí¸¯À¸·Î »©ÀÚ...
	//ID3DX11Effect*				m_pEffect;
	//ID3DX11EffectTechnique*		m_pTechnique;

public:
	HRESULT CompileShaderFromFile(const TCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob ** ppBlobOut);

public:
	ID3D11VertexShader*		GetVtxShader(void) { return m_pVS; }
	ID3D11PixelShader*		GetPxShader(void) { return m_pPS; }
	ID3D11InputLayout*		GetInputLayOut(void) { return m_pLayout; }
	ID3D11SamplerState*		GetSamplerState(void) { return m_pSampleState; }

public:
	virtual void	Render();
	void			Release();

protected:
	explicit CShader(CDevice* pDevice);
	explicit CShader(const CShader& rhs);
public:
	virtual ~CShader()			PURE;
};