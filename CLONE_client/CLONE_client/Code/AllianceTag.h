#pragma once

#include "UI.h"

class CAllianceTag
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
	void SetDirection(void);

public:
	static CAllianceTag* Create(CDevice* pDevice);

protected:
	explicit CAllianceTag(CDevice* pDevice);
public:
	virtual ~CAllianceTag();
};