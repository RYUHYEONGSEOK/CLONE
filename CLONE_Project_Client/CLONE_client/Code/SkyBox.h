#pragma once

#include "Obj.h"

class CSkyBox
	: public CObj
{
public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	HRESULT	AddComponent(void);

public:
	static CSkyBox* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CSkyBox(CDevice* pDevice);
public:
	virtual ~CSkyBox();
};