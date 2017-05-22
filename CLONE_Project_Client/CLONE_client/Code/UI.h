#pragma once

#include "Obj.h"
#include "UIBuffer.h"

struct Sprite
{
	D3DXVECTOR2		vPos;
	D3DXVECTOR2		fHalfSize;

	CTexture*		pTexture;
	CUIBuffer*		pBuffer;

	D3DXMATRIX		m_matView;

	Sprite()
		: vPos(0.f, 0.f), fHalfSize(0.f, 0.f)
		, pTexture(NULL), pBuffer(NULL)
	{
		D3DXMatrixIdentity(&m_matView);
	}
};

class CUI
	: public CObj
{
protected:
	ID3D11Buffer*		m_pViewBuffer;
	ID3D11Buffer*		m_pProjBuffer;

protected:
	float				m_fSizeX, m_fSizeY;
	float				m_fX, m_fY;
	D3DXMATRIX			m_matUIProj, m_matUIView;

public:
	virtual HRESULT	Init()		PURE;
	virtual int		Update()	PURE;
	virtual void	Render()	PURE;
	virtual void	Release()	PURE;
	virtual void	RenderForShadow(LightMatrixStruct tLight)	PURE;

protected:
	explicit CUI(CDevice* pDevice);
public:
	virtual ~CUI();
};

