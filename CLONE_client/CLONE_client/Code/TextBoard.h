#pragma once

#include "UI.h"

class CFont;
class CTextBoard
	: public CUI
{
private:
	CFont*			m_pFont;
	wstring			m_wstrText;

	bool			m_bIsActive;

	int				m_iMaxCnt;
	wstring			m_wstrInitText;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT Init_Text(D3DXVECTOR2 vPos, float fSize, wstring wstrInitWord, int iMaxCnt);
	void FontUpdate(void);
	void SetActive(bool bCheck) { m_bIsActive = bCheck; }

	wstring GetText(void) { m_wstrText; }

public:
	HRESULT	AddComponent(void);

public:
	static CTextBoard* Create(CDevice* pDevice, D3DXVECTOR2 vPos, float fSize, wstring wstrInitWord, int iMaxCnt);

protected:
	explicit CTextBoard(CDevice* pDevice);
public:
	virtual ~CTextBoard();
};