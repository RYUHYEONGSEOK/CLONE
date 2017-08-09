#pragma once

#include "Obj.h"

class CLoading;
class CFont;
class CLogoBack
	: public CObj
{
private:
	CLoading*			m_pLoading;
	CFont*				m_pFont;

private:
	float				m_fSizeX, m_fSizeY;
	float				m_fX, m_fY;
	D3DXMATRIX			m_matLogoBackProj, m_matLogoBackView;

private:
	bool				m_bTextChange;

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
	static CLogoBack* Create(CDevice* pDevice);

public:
	void SetLoadingObj(CLoading* pLoading) { m_pLoading = pLoading; }
	void SetTextChange(bool bCheck) { m_bTextChange = bCheck; }

protected:
	explicit CLogoBack(CDevice* pDevice);
public:
	virtual ~CLogoBack();
};