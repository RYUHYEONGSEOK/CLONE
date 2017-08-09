#pragma once

#include "VIBuffer.h"

class CFountainBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CFountainBuffer* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CFountainBuffer(CDevice* pDevice);
	explicit CFountainBuffer(const CFountainBuffer& rhs);
public:
	virtual ~CFountainBuffer(void);
};