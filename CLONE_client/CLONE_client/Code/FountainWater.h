#pragma once

#include "Obj.h"

class CFountainWater
	: public CObj
{
private:
	float			m_fTime;

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
	static CFountainWater* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CFountainWater(CDevice* pDevice);
public:
	virtual ~CFountainWater();
};