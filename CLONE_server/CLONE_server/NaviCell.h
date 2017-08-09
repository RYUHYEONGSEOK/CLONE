#pragma once

class CLine2D;
class CNaviCell
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

public:
	const D3DXVECTOR3* GetPoint(POINT ePointID) { return &m_vPoint[ePointID]; }
	const CNaviCell* GetNeighbor(NEIGHBOR eNeighborID) { return m_pNeighbor[eNeighborID]; }
	const DWORD GetIndex(void) const { return m_dwIndex; }

public:
	void SetNeighbor(NEIGHBOR eNeighbor, CNaviCell* pNeighbor)
	{
		m_pNeighbor[eNeighbor] = pNeighbor;
	}

public:
	HRESULT InitCell(const DWORD& dwIdx);
	bool ComparePoint(const D3DXVECTOR3* pFirstPoint, const D3DXVECTOR3* pSecondPoint
		, CNaviCell* pNeighbor);
	bool CheckPass(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir, NEIGHBOR* pNeighbor);
	CLine2D* GetPassLine(const D3DXVECTOR3* pPos, const D3DXVECTOR3* pDir);

public:
	D3DXVECTOR3* GetPoint(void) { return m_vPoint; }

public:
	static CNaviCell* Create(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC, const DWORD& dwIdx);

public:
	void Release(void);

private:
	CNaviCell*				m_pNeighbor[NEIGHBOR_END];
	CLine2D*				m_pLine2D[LINE_END];

private:
	D3DXVECTOR3				m_vPoint[POINT_END];
	LPD3DXLINE				m_pLine;
	DWORD					m_dwIndex;

private:
	explicit CNaviCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);
public:
	~CNaviCell(void);
};