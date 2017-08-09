#pragma once

#include "Obj.h"

class CTerrain
	: public CObj
{
public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT	AddComponent(void);

public:
	static CTerrain* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CTerrain(CDevice* pDevice);
public:
	virtual ~CTerrain();
};