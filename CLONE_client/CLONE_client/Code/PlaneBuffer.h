#pragma once

#include "VIBuffer.h"

class CPlaneBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CPlaneBuffer* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CPlaneBuffer(CDevice* pDevice);
	explicit CPlaneBuffer(const CPlaneBuffer& rhs);
public:
	virtual ~CPlaneBuffer(void);
};