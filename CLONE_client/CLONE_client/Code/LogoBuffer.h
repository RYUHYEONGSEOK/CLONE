#pragma once

#include "VIBuffer.h"

class CLogoBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CLogoBuffer* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CLogoBuffer(CDevice* pDevice);
	explicit CLogoBuffer(const CLogoBuffer& rhs);
public:
	virtual ~CLogoBuffer(void);
};