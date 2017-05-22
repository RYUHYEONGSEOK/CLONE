#pragma once

#include "VIBuffer.h"

class CRenderTex
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CRenderTex* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CRenderTex(CDevice* pDevice);
	explicit CRenderTex(const CRenderTex& rhs);
public:
	virtual ~CRenderTex(void);
};