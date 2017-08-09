#pragma once

#include "Scene.h"

class CDevice;
class CObj;
class CCustom
	: public CScene
{
public:
	//�� ���� �ʿ��� ���̾ �����Ѵ�.
	enum LAYERID { LAYER_ENVIRONMENT, LAYER_GAMELOGIC, LAYER_UI };

private:
	CObj*				m_pCustomBack;

public:
	virtual HRESULT		InitScene(void);
	virtual void		Update(void);
	virtual void		Render(void);
	virtual void		Release(void);

public:
	static CCustom* Create(CDevice* pDevice);

private:
	HRESULT Add_Environment_Layer(void);
	HRESULT Add_GameLogic_Layer(void);
	HRESULT Add_UI_Layer(void);
	HRESULT Add_Light(void);

private:
	explicit CCustom(CDevice* pDevice);
public:
	virtual ~CCustom(void);
};