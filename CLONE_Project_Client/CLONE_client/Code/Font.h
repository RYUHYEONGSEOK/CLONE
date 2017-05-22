#pragma once

#include "Resources.h"
#include "FW1FontWrapper.h"

class CDevice;
class CFont : public CResource
{
public:
	enum FontType
	{
		FONT_TYPE_BASIC,
		FONT_TYPE_OUTLINE,
	};

private:
	IFW1FontWrapper*		m_pFW1FontWarpper;
	IFW1Factory*			m_pFW1FontFactory;
	IDWriteFactory*			m_pWriteFactory;

	ID3D11DeviceContext*	m_pDeivceContext;

public:
	FontType		m_eType;

	wstring			m_wstrText;
	float			m_fSize;
	UINT32			m_nColor;
	UINT			m_nFlag;

	D3DXVECTOR2		m_vPos;

	float			m_fOutlineSize;
	UINT32			m_nOutlineColor;

public:
	static  CResource*		Create(CDevice* pDevice, const wstring& wstPath);
	virtual CResource*		Clone();

private:
	HRESULT			Load_Font(const wstring& wstPath);

public:
	HRESULT			Init(const wstring& wstPath);
	virtual void	Update();
	void			Render();
	virtual void	Release();

private:
	explicit CFont(CDevice* pDevice);
	explicit CFont(const CFont& rhs);
public:
	virtual ~CFont(void);
};