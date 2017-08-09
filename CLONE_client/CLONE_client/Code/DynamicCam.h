#pragma once

#include "Camera.h"

class CDynamicCamera
	: public CCamera
{
private:
	explicit CDynamicCamera(CDevice*	pDevice);

public:
	virtual ~CDynamicCamera(void);

public:
	HRESULT InitCamera(const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt);
	void KeyCheck(void);

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	static CDynamicCamera* Create(CDevice*	pDevice
		, const D3DXVECTOR3* pEye, const D3DXVECTOR3* pAt);

private:
	void FixMouse(void);
	void MouseMove(void);
	void CamMove(void);
	void InitMove(void);

private:
	float		m_fCamSpeed;
	float		m_fCamRotY;
	float		m_fCamY;
	float		m_fTargetDistance;
	bool		m_bMouseShow;
	bool		m_bRClick;
	bool		m_bStaticCamKeyDown;
	bool		m_bStaticCam;
	bool		m_bFixKeyDown;
	bool		m_bMouseFix;

	//ÃÊ±â Ä·¹«ºù¿ë
	bool		m_bInitMove;
};
