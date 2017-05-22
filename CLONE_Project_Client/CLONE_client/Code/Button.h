#pragma once

#include "UI.h"

class CButton
	: public CUI
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
	static CButton* Create(CDevice* pDevice);

protected:
	explicit CButton(CDevice* pDevice);
public:
	virtual ~CButton();
};