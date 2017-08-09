#pragma once
#include "VIBuffer.h"
#include "NaviMgr.h"

class CLineBuffer :
	public CVIBuffer
{
private:
	virtual HRESULT CreateBuffer(vector<CNaviCell*>* pNeviCell);

public:
	static CLineBuffer* Create(CNaviMgr::VECCELL* pNaviCell);

public:
	VTXCOL*		Vtx;
	INDEX16*	Idx;

public:
	virtual		CResources*		CloneResource(void);

	//virtual		void Render();
	virtual		int  Update();
	virtual		DWORD Release();

	void Render_Lines(void);

public:
	void CreateRasterizerState(void);


private:
	CLineBuffer(CNaviMgr::VECCELL* pNeviCell);
public:
	virtual ~CLineBuffer(void);
};

