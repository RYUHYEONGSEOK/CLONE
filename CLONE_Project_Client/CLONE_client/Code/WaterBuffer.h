#pragma once

#include "VIBuffer.h"

class CWaterBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(const WORD& wCntX, const WORD& wCntZ);

public:
	static CWaterBuffer* Create(CDevice* pDevice, const WORD& wCntX, const WORD& wCntZ);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CWaterBuffer(CDevice* pDevice);
	explicit CWaterBuffer(const CWaterBuffer& rhs);
public:
	virtual ~CWaterBuffer(void);
};