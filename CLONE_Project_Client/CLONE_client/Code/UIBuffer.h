#pragma once

#include "VIBuffer.h"

class CUIBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CUIBuffer* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CUIBuffer(CDevice* pDevice);
	explicit CUIBuffer(const CUIBuffer& rhs);
public:
	virtual ~CUIBuffer(void);
};