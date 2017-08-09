#pragma once

class CDevice;
class CComponent;
class CInfo;
class CVIBuffer;
class CStaticMesh;
class CObj
{
public:
	CObj();
	virtual ~CObj();

public:
	map<wstring, CComponent*>	m_mapComponent;
	CInfo*						m_pInfo;
	float						m_fViewZ;
	bool						m_bZSort;
	bool						m_bSelected;
	CDevice*					m_pGrapicDevice;
	ReleaseType					m_eReleaseType;
	vector<string>				vecAniName;
	CVIBuffer*					m_pBuffer;
	wstring						m_wstrObjKey;
	CComponent*					pComponent;
	float						m_fSelect;
public:
	TCHAR						m_tcKey[256];
	TCHAR						m_tcMeshKey[256];

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void) {};
	virtual void Release(void);
	void	SelectCheck(void);

public:
	static CObj* Create(ReleaseType _eType);
	wstring GetObjKey(void)
	{
		return m_wstrObjKey;
	}
	

public:
	const float&	GetViewZ(void) const;
	CInfo*			GetInfo(void) { return m_pInfo; }
	ReleaseType&	GetReleaseType(void) { return m_eReleaseType; }
	const CComponent*	GetComponent(const wstring& _wstrComponentKey);
	

public:
	void SetPos(D3DXVECTOR3 vPos);

protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
};

