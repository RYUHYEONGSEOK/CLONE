#pragma once

#include "VIBuffer.h"

class CTerrainCol
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);

public:
	static CTerrainCol* Create(CDevice* pDevice,
		const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);

public:
	NormalVertex*	GetTerrainVtx(void) { return vertices; }

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);
 
private:
	explicit CTerrainCol(CDevice* pDevice,
		const WORD& wCntX, const WORD& wCntZ, const WORD& wItv);
	explicit CTerrainCol(const CTerrainCol& rhs);
public:
	virtual ~CTerrainCol(void);

};