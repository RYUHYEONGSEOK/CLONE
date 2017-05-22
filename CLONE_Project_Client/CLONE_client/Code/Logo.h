#pragma once

#include "Scene.h"

class CDevice;
class CObj;
class CLogo
	: public CScene
{
public:
	//�� ���� �ʿ��� ���̾ �����Ѵ�.
	enum LAYERID { LAYER_ENVIRONMENT, LAYER_GAMELOGIC, LAYER_UI };

private:
	CObj*			m_pLogoBack;
	CObj*			m_pTitle;
	CObj*			m_pText;

private:
	bool			m_bIsChange;

public:
	virtual HRESULT		InitScene(void);
	virtual void		Update(void);
	virtual void		Render(void);
	virtual void		Release(void);

public:
	static CLogo* Create(CDevice* pDevice);

private:
	HRESULT Loading_LogoResource(void);

private:
	HRESULT Add_Environment_Layer(void);
	HRESULT Add_GameLogic_Layer(void);
	HRESULT Add_UI_Layer(void);
	HRESULT Add_Light(void);

private:
	explicit CLogo(CDevice* pDevice);
public:
	virtual ~CLogo(void);
};