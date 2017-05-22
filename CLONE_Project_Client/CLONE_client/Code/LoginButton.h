#pragma once

#include "UI.h"

class CFont;
class CLoginButton
	: public CUI
{
public:
	enum eButtonType { BUTTON_NORMAL, BUTTON_MOUSEON, BUTTON_CLICKED, BUTTON_UNSERVICE, BUTTON_END };

private:
	eButtonType		m_eType;
	bool			m_bClickDown;
	bool			m_bClickUp;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	HRESULT	AddComponent(void);
	bool	GetSceneChangeFlag(void) { return m_bClickUp; }

private:
	void KeyCheck(void);

public:
	static CLoginButton* Create(CDevice* pDevice);

protected:
	explicit CLoginButton(CDevice* pDevice);
public:
	virtual ~CLoginButton();
};