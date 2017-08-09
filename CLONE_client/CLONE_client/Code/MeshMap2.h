#pragma once

#include "Obj.h"

class CBuffer;
class CTexture;
class CMeshMap2
	: public CObj
{
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
	static CMeshMap2* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CMeshMap2(CDevice* pDevice);
public:
	virtual ~CMeshMap2();
};