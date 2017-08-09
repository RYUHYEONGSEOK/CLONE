#pragma once

class CObj;
class CFrustum
{
	DECLARE_SINGLETON(CFrustum)
public:
	HRESULT UpdateFrustum(CObj* pCam);
	bool SphereInFrustum(const D3DXVECTOR3* pCenter, const float& fRadius);

private:
	D3DXVECTOR3			m_vPoint[8];
	D3DXVECTOR3			m_vConPoint[8];

	D3DXPLANE			m_Plane[6];

private:
	CFrustum(void);
public:
	~CFrustum(void);
};