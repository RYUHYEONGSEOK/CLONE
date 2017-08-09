#pragma once

class CDevice;
class CBuffer;
class CObj;
class CShader;
class CLight
{
private:
	explicit CLight(CDevice*	pDevice);

public:
	~CLight(void);

public:
	LightInfo* GetLightInfo(void);

public:
	HRESULT Init_Light(const LightInfo* pLightInfo, const WORD& dwLightIdx);
	void Render_Light(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj,
		XMFLOAT4X4 matViewInv, XMFLOAT4X4 matProjInv, LightMatrixStruct tLightMatInfo,
		ID3D11ShaderResourceView* pNormalResource,
		ID3D11ShaderResourceView* pDepthResource,
		ID3D11ShaderResourceView* pShadowResource);
	LightMatrixStruct GetLightMatrix(void);

public:
	static CLight* Create(CDevice* pDevice, const LightInfo* pLightInfo, const WORD& wLightIdx);
	void Release(void);

	void SetCam(CObj* pCam) { m_pCam = pCam; }

private:
	CObj*					m_pCam;
	CDevice*				m_pDevice;
	CShader*				m_pShader;
	CBuffer*				m_pTargetBuffer;

	//Light Info
	LightInfo				m_LightInfo;
	LightMatrixStruct		m_LightMatrixStruct;
};