#pragma once

#include "Obj.h"

class CStreamWater
	: public CObj
{
private:
	float			m_fTime;
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
	static CStreamWater* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CStreamWater(CDevice* pDevice);
public:
	virtual ~CStreamWater();
};