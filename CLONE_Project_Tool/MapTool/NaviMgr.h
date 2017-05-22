#pragma once

class CNaviCell;
class CDevice;
class CNaviMgr
{
public:
	DECLARE_SINGLETON(CNaviMgr)
public:
	CNaviMgr();
	~CNaviMgr();

public:
	void Reserve_cellContainerSize(const DWORD& dwSize);
	HRESULT	Add_Cell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);
	void Link_Cell(void);
	void Render(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir
		, const DWORD& dwCurrentIdx);
	void GetNearPoint(D3DXVECTOR3& vPoint, float fRange);
private:
	void Release(void);

private:
	CDevice*					m_pDevice;

public:
	DWORD		m_dwReserveSize;

	typedef vector<CNaviCell*>		VECCELL;
	static VECCELL		m_vecNaviMesh;
public:
	VECCELL*		GetCell(void)
	{
		return &m_vecNaviMesh;
	}

public:
	DWORD		m_dwIdxCnt;
};

