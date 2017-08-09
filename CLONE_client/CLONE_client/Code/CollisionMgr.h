#pragma once

class CObj;
class CCollisionMgr
{
public:
	DECLARE_SINGLETON(CCollisionMgr)

private:
	typedef struct tagOBB
	{
		D3DXVECTOR3		vPoint[8];
		D3DXVECTOR3		vCenter;
		D3DXVECTOR3		vProjAxis[3];
		D3DXVECTOR3		vAxis[3];
	}OBB;
	
private:
	unordered_map<wstring, list<CObj*>>* m_pmapObjList;

	OBB					m_OBB[2];
	NormalVertex*		m_pTerrainVtx;

public:
	void SetObjList(unordered_map<wstring, list<CObj*>>* pList) { m_pmapObjList = pList; }
	void SetTerrainVtx(NormalVertex* pTerrainVtx) { m_pTerrainVtx = pTerrainVtx; }

public:
	bool Collision_OBB(const wstring& wTargetTag, const D3DXVECTOR3* pDestMin, const D3DXVECTOR3* pDestMax, const D3DXMATRIX* pDestWorld
		, const D3DXVECTOR3* pSourMin, const D3DXVECTOR3* pSourMax, const D3DXMATRIX* pSourWorld);
	bool Collision_AABB(const wstring& wTargetTag, CObj* pObj);
	bool Collision_AABB_Bead(const wstring& wTargetTag, CObj* pObj);
	void Collision_Terrain(CObj* pObj);

private:
	void InitPoint(OBB* pOBB, const D3DXVECTOR3* pMin, const D3DXVECTOR3* pMax);
	void InitAxis(OBB* pOBB);
	void Release(void);

private:
	CCollisionMgr(void);
public:
	~CCollisionMgr(void);
};