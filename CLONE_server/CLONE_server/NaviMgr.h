#pragma once

class CNaviCell;

class CNaviMgr
{
	DECLARE_SINGLETON(CNaviMgr)

public:
	void ReserveCellContainerSize(const DWORD& dwSize, eMapType _eMapType);
	HRESULT AddCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, eMapType _eMapType);
	void LinkCell(void);
	DWORD MoveOnNaviMesh(D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, const DWORD& dwIndex, float _fFrameTime, eMapType _eMapType);
	bool GetNaviIndex(D3DXVECTOR3* pPos, DWORD& Index, eMapType _eMapType);

private:
	void Release(void);

private:
	XMMATRIX m_matWorld;
	DWORD	 m_dwIdxCnt[MAP_END];

public:
	typedef vector<CNaviCell*>		VECCELL;
	VECCELL							m_vecCell[MAP_END];

private:
	CNaviMgr(void);
	~CNaviMgr(void);
};