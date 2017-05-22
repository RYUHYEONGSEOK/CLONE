#pragma once

#include "VIBuffer.h"

class CCubeColor
	: public CBuffer
{
private:
	virtual HRESULT CreateBuffer(const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXVECTOR4& vColor);

public:
	static CCubeColor* Create(CDevice* pDevice,
		const D3DXVECTOR3& vMin, const D3DXVECTOR3& vMax, const D3DXVECTOR4& vColor);

public:
	virtual CResource* Clone(void);

	virtual void Render(DXGIFORMATTYPE eFormatType);
	virtual void Update();
	virtual void Release();

public:
	void CreateRasterizerState(void);

private:
	explicit CCubeColor(CDevice* pDevice);
	explicit CCubeColor(const CCubeColor& rhs);
public:
	virtual ~CCubeColor(void);
};