#pragma once

#include "Scene.h"

class CDevice;
class CStage
	: public CScene
{
public:
	//각 씬에 필요한 레이어를 정의한다.
	enum LAYERID { LAYER_ENVIRONMENT, LAYER_GAMELOGIC, LAYER_UI };

public:
	virtual HRESULT		InitScene(void);
	virtual void		Update(void);
	virtual void		Render(void);
	virtual void		Release(void);

public:
	static CStage* Create(CDevice* pDevice);

private:
	HRESULT Add_Environment_Layer(void);
	HRESULT Add_GameLogic_Layer(void);
	HRESULT Add_UI_Layer(void);
	HRESULT Add_Light(void);
	HRESULT Add_RenderTarget(void);
	

public:
	HRESULT	Add_NaviMesh(void);	//테스트용
	CObj*	GetStageUI(void) { return m_pStateUI; }

private:
	CObj*			m_pStateUI;
	bool			m_bKeyDown;
	STAGESTATETYPE	m_eStageState;

	//게임 플레이타임
private:
	int* m_pPlayTime;

private:
	explicit CStage(CDevice* pDevice);
public:
	virtual ~CStage(void);
};