#pragma once

#include "VIBuffer.h"

class CSkyBoxBuffer
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(void);

public:
	static CSkyBoxBuffer* Create(CDevice* pDevice);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CSkyBoxBuffer(CDevice* pDevice);
	explicit CSkyBoxBuffer(const CSkyBoxBuffer& rhs);
public:
	virtual ~CSkyBoxBuffer(void);
};