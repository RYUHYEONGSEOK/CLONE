#pragma once

#include "Obj.h"

enum MAP_BUFFER_TYPE 
{
	MAP_OBJ_GROUND, MAP_OBJ_BRIDGE_UP, MAP_OBJ_BRIDGE_DOWN, MAP_OBJ_FOUNTAIN,
	MAP_OBJ_BUILDING_LEFT, MAP_OBJ_BUILDING_RIGHT, 
	MAP_OBJ_END
};

class CBuffer;
class CTexture;
class CMeshMap
	: public CObj
{
private:
	CTransform*		m_pMapInfo[MAP_OBJ_END];
	CBuffer*		m_pMapBuffer[MAP_OBJ_END];
	CTexture*		m_pMapTexture[MAP_OBJ_END];

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
	static CMeshMap* Create(CDevice* pDevice, OBJTYPE eType);

protected:
	explicit CMeshMap(CDevice* pDevice);
public:
	virtual ~CMeshMap();
};