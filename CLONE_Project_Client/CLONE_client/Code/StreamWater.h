#pragma once

#include "Obj.h"

class CStreamWater
	: public CObj
{
private:
	float			m_fTime;
	VertexTexture*	m_parrVertex;

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);

public:
	HRESULT	AddComponent(void);
	void	WaveAction(void);

public:
	static CStreamWater* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CStreamWater(CDevice* pDevice);
public:
	virtual ~CStreamWater();
};