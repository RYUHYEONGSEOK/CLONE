#pragma once

#include "Obj.h"

class CDumpPlate
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
	static CDumpPlate* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CDumpPlate(CDevice* pDevice);
public:
	virtual ~CDumpPlate();
};