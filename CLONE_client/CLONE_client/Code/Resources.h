#pragma once

#include "Component.h"
#include "GraphicDevice.h"

class CResource : public CComponent
{
protected:
	CDevice*				m_pDevice;
	WORD*					m_pRefCnt;

public:
	virtual CResource*		Clone()		PURE;
	virtual void			Update()	PURE;
	virtual void			Release()	PURE;

protected:
	void Release_RefCnt();

protected:
	CResource(CDevice* pDevice);
public:
	virtual ~CResource();
};