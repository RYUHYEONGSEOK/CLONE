#pragma once

#include "Obj.h"

class CTestWeapon
	: public CObj
{
private:
	bool			m_bCreate;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	HRESULT	AddComponent(void);
	void	Set_ConstantTable(void);

public:
	static CTestWeapon* Create(CDevice* pDevice);

protected:
	explicit CTestWeapon(CDevice* pDevice);
public:
	virtual ~CTestWeapon();
};