#pragma once

#include "Obj.h"

class CTrapWater
	: public CObj
{
private:
	float			m_fTime;
	bool			m_bStartFill;
	float			m_fWaterTranslation;
	float			m_fWaterIntensity;
	VertexTexture*	m_parrVertex;

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
	static CTrapWater* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CTrapWater(CDevice* pDevice);
public:
	virtual ~CTrapWater();
};