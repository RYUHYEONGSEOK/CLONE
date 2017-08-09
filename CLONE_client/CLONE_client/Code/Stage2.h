#pragma once

#include "Scene.h"

class CDevice;
class CStage2
	: public CScene
{
public:
	//�� ���� �ʿ��� ���̾ �����Ѵ�.
	enum LAYERID { LAYER_ENVIRONMENT, LAYER_GAMELOGIC, LAYER_UI };

public:
	virtual HRESULT		InitScene(void);
	virtual void		Update(void);
	virtual void		Render(void);
	virtual void		Release(void);

public:
	static CStage2* Create(CDevice* pDevice);

private:
	HRESULT Add_Environment_Layer(void);
	HRESULT Add_GameLogic_Layer(void);
	HRESULT Add_UI_Layer(void);
	HRESULT Add_Light(void);
	HRESULT Add_RenderTarget(void);
	HRESULT Connect_Cam_To_Navi(void);

public:
	CObj*	GetStageUI(void) { return m_pStateUI; }

private:
	CObj*			m_pStateUI;
	bool			m_bKeyDown;
	STAGESTATETYPE	m_eStageState;

	//���� �÷���Ÿ��
private:
	int* m_pPlayTime;

private:
	explicit CStage2(CDevice* pDevice);
public:
	virtual ~CStage2(void);
};