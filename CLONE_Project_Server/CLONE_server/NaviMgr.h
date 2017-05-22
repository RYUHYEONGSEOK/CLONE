#pragma once

class CNaviCell;

class CNaviMgr
{
	DECLARE_SINGLETON(CNaviMgr)

public:
	void ReserveCellContainerSize(const DWORD& dwSize);
	HRESULT AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);
	void LinkCell(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex, float _fFrameTime);
	bool GetNaviIndex(D3DXVECTOR3 * pPos, DWORD& Index);

private:
	void Release(void);

private:
	DWORD			m_dwReserveSize;
	XMMATRIX		m_matWorld;

public:
	typedef vector<CNaviCell*>		VECCELL;
	VECCELL							m_vecCell;

private:
	DWORD			m_dwIdxCnt;

private:
	CNaviMgr(void);
public:
	~CNaviMgr(void);
};