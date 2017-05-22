#pragma once

#include "VIBuffer.h"
#include "NaviMgr.h"

class CLineBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(vector<CNaviCell*>* pNeviCell);

public:
	static CLineBuffer* Create(CDevice* pDevice, CNaviMgr::VECCELL* pNeviCell);

private:
	VertexColor*	vtx;
	INDEX16*		Idx;

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

	void Render_Lines(void);

public:
	void CreateRasterizerState(void);

private:
	explicit CLineBuffer(CDevice* pDevice, CNaviMgr::VECCELL* pNeviCell);
	explicit CLineBuffer(const CLineBuffer& rhs);
public:
	virtual ~CLineBuffer(void);
};