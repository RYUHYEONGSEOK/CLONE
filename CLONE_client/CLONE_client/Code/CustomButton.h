#pragma once

#include "Obj.h"

class CLoading;
class CTexture;
class CCustomButton
	: public CObj
{
public:
	// 모든 버튼들
	enum eCustomButtonType {
		CUS_BUTTON_INFO, CUS_BUTTON_SETTING, CUS_BUTTON_EXIT,
		CUS_BUTTON_CHANGE, CUS_BUTTON_READY, CUS_BUTTON_START,
		CUS_BUTTON_L_UP_SLIDE, CUS_BUTTON_R_UP_SLIDE,
		CUS_BUTTON_L_MID_SLIDE, CUS_BUTTON_R_MID_SLIDE,
		CUS_BUTTON_L_DOWN_SLIDE, CUS_BUTTON_R_DOWN_SLIDE,
		CUS_ICON_HOST, /*CUS_ICON_MODETEXT_SINGLE, CUS_ICON_MODETEXT_TEAM,*/ CUS_STAGEIMG, CUS_BUTTON_END
	};
	// 버튼의 상태
	enum eCustomButtonState { CUS_BUTTON_STATE_NORMAL, CUS_BUTTON_STATE_MOUSEON, 
		CUS_BUTTON_STATE_CLICKED, CUS_BUTTON_STATE_CHECKED, CUS_STATE_BUTTON_END };

private:
	float				m_fSizeX[CUS_BUTTON_END], m_fSizeY[CUS_BUTTON_END];
	float				m_fX[CUS_BUTTON_END], m_fY[CUS_BUTTON_END];
	D3DXMATRIX			m_matLogoBackProj[CUS_BUTTON_END], m_matLogoBackView[CUS_BUTTON_END];
	CTexture*			m_pCustomTexture[CUS_BUTTON_END];

	bool				m_bButtonState[CUS_BUTTON_END][CUS_STATE_BUTTON_END];

	int					m_iStageNum;	// 스테이지 플래그
	bool				m_bStageChangeFlag; // 스테이지 중복 방지용

	vector<D3DXVECTOR2>	m_vecHostIconPos;	// Host Icon의 위치값 저장

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT	AddComponent(void);
	HRESULT ButtonSetting(void);
	HRESULT	InitHostPos(void);
	HRESULT HostIconSetting(void);

	bool	GetButtonState(const DWORD& dwButtonType, const DWORD& dwButtonState);
	void	ResetButtonState(void);


public:
	static CCustomButton* Create(CDevice* pDevice);

protected:
	explicit CCustomButton(CDevice* pDevice);
public:
	virtual ~CCustomButton();
};