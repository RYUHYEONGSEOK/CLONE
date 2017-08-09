#pragma once

class CLight;
class CDevice;
class CObj;
class CBuffer;
class CLightMgr
{
public:
	DECLARE_SINGLETON(CLightMgr)

private:
	typedef list<CLight*>		LIGHTLIST;
	LIGHTLIST		m_Lightlist;

public:
	LightInfo* GetLightInfo(const UINT& iIndex = 0);

public:
	HRESULT AddLight(CDevice* pDevice, const LightInfo* pLightInfo, const WORD& wLightIdx, CObj* pCam);
	void Render_Light(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj,
		XMFLOAT4X4 matViewInv, XMFLOAT4X4 matProjInv,
		ID3D11ShaderResourceView* pNormalResource,
		ID3D11ShaderResourceView* pDepthResource,
		ID3D11ShaderResourceView* pShadowResource);
	LightMatrixStruct GetLightMatrix(void);

private:
	void Release(void);

private:
	CLightMgr(void);
	~CLightMgr(void);
};