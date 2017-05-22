#pragma once

#include "UI.h"

class CFont;
class CStatus
	: public CUI
{
private:
	CFont*		m_pFont;
	CFont*		m_pGrapichBrandFont;
	CFont*		m_pGrapichMemoryFont;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	HRESULT	AddComponent(void);

public:
	static CStatus* Create(CDevice* pDevice);

protected:
	explicit CStatus(CDevice* pDevice);
public:
	virtual ~CStatus();
};