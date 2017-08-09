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
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT	AddComponent(void);

public:
	static CButton* Create(CDevice* pDevice);

protected:
	explicit CButton(CDevice* pDevice);
public:
	virtual ~CButton();
};