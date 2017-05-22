#pragma once

#include "UI.h"

class CFont;
class CKillLog
	: public CUI
{
private:
	float		m_fAppearTime;

	CFont*		m_pKillerFont;
	CFont*		m_pVictimFont;

public:
	wstring		m_wstrKiller;
	wstring		m_wstrVictim;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	void SetKillLogPos(D3DXVECTOR3 vPos);
	void SetLogStr(const wstring& wstrKiller, const wstring& wstrVictim);
	D3DXVECTOR3 GetKillLogPos(void);
	void FontUpdate(void);

public:
	HRESULT	AddComponent(void);

public:
	static CKillLog* Create(CDevice* pDevice);

protected:
	explicit CKillLog(CDevice* pDevice);
public:
	virtual ~CKillLog();
};