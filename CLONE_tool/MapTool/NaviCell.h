#pragma once
#include "VIBuffer.h"
class CNaviCell:
	public CVIBuffer
{

public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };
public:
	CNaviCell*			m_pNeighbor[NEIGHBOR_END];

public:
	D3DXVECTOR3			m_vPoint[POINT_END];
	DWORD				m_dwIndex;

public:
	const D3DXVECTOR3*	GetPoint(POINT ePointID);
	CNaviCell*	GetNeighbor(NEIGHBOR eNeighbor);
	DWORD		GetIndex(void);

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
	void Render(void);

public:
	static CNaviCell* Create(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC
		, const DWORD& dwIdx);
public:
	DWORD Release(void);

public:
	virtual CResources* CloneResource(void);
	VTXCOL*		Vtx;
	INDEX16*	Idx;


private:
	explicit CNaviCell(const D3DXVECTOR3* pPointA, const D3DXVECTOR3* pPointB, const D3DXVECTOR3* pPointC);

public:
	~CNaviCell(void);
};

