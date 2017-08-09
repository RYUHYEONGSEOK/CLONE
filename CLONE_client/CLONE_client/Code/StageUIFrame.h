#pragma once

#include "UI.h"

class CFont;
class CKillLog;
class CStageUIFrame
	: public CUI
{
public:
	enum UIFRAMETYPE { UIFRAME_UPFRAME, UIFRAME_LEFTFRAME, 
		UIFRAME_NOTICEFRAME, UIFRAME_NOTICECOUNTFRAME,
		UIFRAME_CHATFRAME, UIFRAME_CHECKVIC
		,UIFRAME_END };

	enum STATEFONTTYPE { FONT_TIME, FONT_PLAYERCNT, FONT_BOTCNT, FONT_END };

private:
	CTexture*			m_pUITexture[UIFRAME_END];
	// UI Frame 관련
	float				m_fSizeX[UIFRAME_END], m_fSizeY[UIFRAME_END];
	float				m_fX[UIFRAME_END], m_fY[UIFRAME_END];
	D3DXMATRIX			m_matUIProj[UIFRAME_END], m_matUIView[UIFRAME_END];

	// Bead Frame 관련
	float				m_fBeadSizeX, m_fBeadSizeY;
	float				m_fBeadX[BEAD_END], m_fBeadY[BEAD_END];
	D3DXMATRIX			m_matBeadProj[BEAD_END], m_matBeadView[BEAD_END];

	//Font 관련
	CFont*				m_pFont[FONT_END];

	//시간Font 관련
	bool				m_bReady;
	int*				m_pReadyCount;
	int*				m_pPlayTime;
	float				m_fShowResultDelay;

	//Player, Bot Font 관련
	unordered_map<wstring, list<CObj*>>*		m_pObjList;

	

	//구슬 수집관련
	BEADTYPE			m_eBeadType;
	D3DXVECTOR3			m_vBeadPos[5];
	D3DXVECTOR3			m_vBeadSize;

	//KillLog 관련
	vector<CKillLog*>	m_vKillLog;
	int					m_iKillLogCnt;

public:
	void RenderNotice(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj);
	void RenderBeadCheck(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj);
	void RenderKillLog(void);
	void FontSetting(void);
	void FontUpdate(void);
	void KillLogUpdate(void);


private:
	//채팅 관련
	bool				m_bChatOn;
	bool				m_bChatKeyDown;
public:
	void KeyCheck(void);
	void RenderChat(XMFLOAT4X4 matWorld, XMFLOAT4X4 matView, XMFLOAT4X4 matProj);




public:
	void AddKillLog(const wstring& wstrKiller, const wstring& wstrVictim);
	void SetPlayTime(int* fTime) { m_pPlayTime = fTime; }

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
	static CStageUIFrame* Create(CDevice* pDevice);

protected:
	explicit CStageUIFrame(CDevice* pDevice);
public:
	virtual ~CStageUIFrame();
};