#pragma once
#include "Scene.h"

class CMapToolView;
class CObj;
class CMouseCol;
class CStaticMesh;
class CStaticObject;
class CComponent;
class CMeshTool;
class CBack :
	public CScene
{
public:
	CMapToolView* m_pMainView;
public:
	CBack();
	virtual ~CBack();

	// CScene을(를) 통해 상속됨
	virtual HRESULT Initialize(void);
	virtual int Update(void);
	virtual void Render(void);
	virtual void Release(void);
	
public:
	static CObj*		m_pMeshForDel;
	CMouseCol*			m_pMouseCol;
	CStaticObject*		m_pStatic;
	D3DXVECTOR3			m_vDestPos;
	CComponent*			pMeshComponent;
	//CStaticMesh*		m_pMesh;
	//CMeshTool*			m_pMeshTool;
	//CObj*				m_pObj;
	DWORD				m_dwTime;
	VTXTEX*				vtxtex;

public:
	static CBack* Create(void);
	void SetMainView(CMapToolView* pMainView);

public:
	static vector<CObj*> m_vecObject;


public:
	int DeleteMesh(void);
	void SelectMesh(void);
//	void NaviSelect(void);

public:
	HRESULT	CreateObj(void);
};

