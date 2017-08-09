#pragma once

class CBack;
class CCamera;
class CInfo;
class CMouseCol
{
public:
	CMouseCol();
public:
	~CMouseCol();

public:
	CBack*			m_pBack;
	D3DXVECTOR3		m_vPivotPos;
	D3DXVECTOR3		m_vRayDir;
	CCamera*		m_DyCam;
	int				m_iCntX;
	int				m_iCntZ;

public:
	void Translation_ViewSpace(void);
	bool Translation_ViewSpace2(void);
	void Translation_Local(const D3DXMATRIX* pmatWorld);
	bool PickObject(CInfo* m_pInfo, D3DXVECTOR3* pOut,  vector<VTXTEX>* pVertex, int iCnt);
	static POINT GetMousePos(void);
public:
	HRESULT		Initialize();
};

