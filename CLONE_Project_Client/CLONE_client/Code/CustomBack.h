#pragma once

#include "Obj.h"

class CLoading;
class CFont;
class CCustomBack
	: public CObj
{
public:
	enum eCusFontType { CUS_FONT_MODE, CUS_FONT_ROUND, CUS_FONT_BOTCNT
						, CUS_FONT_CLIENT0, CUS_FONT_CLIENT1
						, CUS_FONT_CLIENT2, CUS_FONT_CLIENT3
						, CUS_FONT_CLIENT4, CUS_FONT_CLIENT5, CUS_FONT_END };
private:
	float				m_fSizeX, m_fSizeY;
	float				m_fX, m_fY;
	D3DXMATRIX			m_matLogoBackProj, m_matLogoBackView;
	CFont*				m_pFont[CUS_FONT_END];

	CObj*				m_pCusButton;		// 커스텀 버튼 연결 포인터

	bool				m_bIsStartClicked;
	bool				m_bButtonDown[3];	// 버튼 중복 방지용
	DWORD				m_dwDelayTime;		// 버튼 딜레이

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

	void			FontUpdate(void);

public:
	HRESULT	AddComponent(void);
	HRESULT FontSetting(void);

public:
	void SetButtonObj(CObj* pButton) { m_pCusButton = pButton; }

public:
	static CCustomBack* Create(CDevice* pDevice);

protected:
	explicit CCustomBack(CDevice* pDevice);
public:
	virtual ~CCustomBack();
};