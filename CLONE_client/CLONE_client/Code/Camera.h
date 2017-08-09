#pragma once

#include "Obj.h"

class CDevice;
class CCamera
	: public CObj
{
protected:
	explicit CCamera(CDevice* pDevice);

public:
	virtual ~CCamera(void);

public:
	virtual HRESULT	Init();
	virtual int		Update();
	virtual void	Render();
	virtual void	Release();
	virtual void	RenderForShadow(LightMatrixStruct tLight);
	virtual void	RenderForWaterEffect(D3DXVECTOR4 vClipPlane, REFTYPE eRefType);

public:
	void Init_ViewBuffer(void);
	void Init_ProjBuffer(void);
	void Init_Viewprot(void);

public:
	void Invalidate_View(void);
	void Invalidate_Proj(void);
	void Invalidate_Ortho(void);

public:
	const ConstantBuffer* GetConstantBuffer(void) { return &m_pCB; }

	D3DXMATRIX*		GetViewMatrix(void)		{ return &m_matView; }
	D3DXMATRIX*		GetProjMatrix(void)		{ return &m_matProj; }
	D3DXMATRIX*		GetWorldMatrix(void)	{ return &m_matWorld; }
	D3DXMATRIX*		GetReflectViewMatrix(float fHeight);

	D3DXVECTOR3*	GetEye(void)			{ return &m_vEye; }
	float			GetDistEyeToAt(void);

protected:
	DWORD			m_dwNaviIdx;
	D3DXMATRIX		m_matWorld;

protected:
	D3DXMATRIX		m_matView;
	D3DXMATRIX		m_reflectionView;
	D3DXMATRIX		m_matOrtho;
	D3DXVECTOR3		m_vEye, m_vAt, m_vUp;

protected:
	D3DXMATRIX		m_matProj;
	float			m_fFovY, m_fAspect, m_fNear, m_fFar;

	ID3D11Buffer*		m_pViewBuffer;
	ID3D11Buffer*		m_pProjBuffer;

protected:
	D3D11_VIEWPORT		m_tViewport;
	ConstantBuffer		m_pCB;

protected:
	ID3D11DeviceContext*	m_pDeviceContext;
};