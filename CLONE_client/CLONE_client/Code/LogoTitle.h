#pragma once

#include "Obj.h"

class CLoading;
class CFont;
class CTexture;
class CLoading;
class CLoginButton;
class CTextBoard;
class CLogoTitle
	: public CObj
{
private:
	CLoading*			m_pLoading;
	CLoginButton*		m_pButton;
	CTextBoard*			m_pTextBoard;

	CTexture*			m_pTopTexture;
	CTexture*			m_pBotTexture;
	CTexture*			m_pProgressTexture;
	CTexture*			m_pNameTagTexture;

private:
	float				m_fSizeX, m_fSizeY;
	float				m_fProgSizeX, m_fProgSizeY;
	float				m_fNameTagSizeX, m_fNameTagSizeY;
	float				m_fTopX, m_fTopY;
	float				m_fBotX, m_fBotY;
	float				m_fProgX, m_fProgY;
	float				m_fNameTagX, m_fNameTagY;
	D3DXMATRIX			m_matTopUIProj, m_matTopUIView;
	D3DXMATRIX			m_matBotUIProj, m_matBotUIView;
	D3DXMATRIX			m_matProgUIProj, m_matProgUIView;
	D3DXMATRIX			m_matNameTagProj, m_matNameTagView;

private:
	bool				m_bLoadComplete;
	bool				m_bTitleMoveComplete;

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
	void SetComplete(bool bCheck) { m_bLoadComplete = bCheck; }
	void SetLoading(CLoading* pLoading) { m_pLoading = pLoading; }
	
	bool GetSceneChangeFlag(void);
	bool GetTitleMoveFlag(void) { return m_bTitleMoveComplete; }

public:
	static CLogoTitle* Create(CDevice* pDevice);

protected:
	explicit CLogoTitle(CDevice* pDevice);
public:
	virtual ~CLogoTitle();
};