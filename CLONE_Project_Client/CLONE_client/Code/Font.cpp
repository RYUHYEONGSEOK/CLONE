#include "stdafx.h"
#include "Font.h"

#include "ResourceMgr.h"
#include "Texture.h"

#include "GraphicDevice.h"

#include <D2D1.h>
#include <DWrite.h>

CFont::CFont(CDevice* pDevice)
	: CResource(pDevice)
	, m_pFW1FontWarpper(NULL)
	, m_pFW1FontFactory(NULL)
	, m_pWriteFactory(NULL)

	, m_pDeivceContext(NULL)

	, m_eType(FONT_TYPE_BASIC)

	, m_wstrText(L"")
	, m_fSize(0.f)
	, m_nColor(0xFFFFFFFF)
	, m_nFlag(FW1_LEFT | FW1_TOP | FW1_RESTORESTATE)

	, m_vPos(0.f, 0.f)

	, m_fOutlineSize(1.f)
	, m_nOutlineColor(0xFF000000)
{

}

CFont::CFont(const CFont & rhs)
	: CResource(rhs.m_pDevice)
{
	m_pFW1FontWarpper = rhs.m_pFW1FontWarpper;
	m_pFW1FontFactory = rhs.m_pFW1FontFactory;
	m_pWriteFactory = rhs.m_pWriteFactory;

	m_pDeivceContext = rhs.m_pDeivceContext;

	m_eType = rhs.m_eType;

	m_wstrText = rhs.m_wstrText;
	m_fSize = rhs.m_fSize;
	m_nColor = rhs.m_nColor;
	m_nFlag = rhs.m_nFlag;

	m_vPos = rhs.m_vPos;

	m_fOutlineSize = rhs.m_fOutlineSize;
	m_nOutlineColor = rhs.m_nOutlineColor;

	m_pRefCnt = rhs.m_pRefCnt;
}

CFont::~CFont()
{
	Release();
}

CResource* CFont::Create(CDevice* pDevice,
	const wstring& wstrPath)
{
	CFont* pFont = new CFont(pDevice);

	if (FAILED(pFont->Init(wstrPath)))
		Safe_Delete(pFont);

	return pFont;
}

CResource* CFont::Clone()
{
	++(*m_pRefCnt);
	return new CFont(*this);
}

HRESULT CFont::Init(const wstring& wstrPath)
{
	m_pDeivceContext = m_pDevice->GetDeviceContext();
	Load_Font(wstrPath);

	return S_OK;
}

HRESULT CFont::Load_Font(const wstring& wstrPath)
{
	if (FAILED(FW1CreateFactory(FW1_VERSION, &m_pFW1FontFactory)))
	{
		MSG_BOX(L"FAILED CreateFactory");
		return E_FAIL;
	}

	if (FAILED(m_pFW1FontFactory->CreateFontWrapper(
		m_pDevice->GetDevice(), wstrPath.c_str(), &m_pFW1FontWarpper)))
	{
		MSG_BOX(L"Not Find Font");
		return E_FAIL;
	}

	/*DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pWriteFactory);
	
	FW1_FONTWRAPPERCREATEPARAMS tFontData;
	ZeroMemory(&tFontData, sizeof(FW1_FONTWRAPPERCREATEPARAMS));
	tFontData.DefaultFontParams.FontStyle = DWRITE_FONT_STYLE_OBLIQUE;
	tFontData.DefaultFontParams.pszFontFamily = wstrPath.c_str();

	m_pFW1FontFactory->CreateFontWrapper(
		m_pDevice->GetDevice(),
		m_pWriteFactory,
		&tFontData,
		&m_pFW1FontWarpper);*/

	return S_OK;
}

void CFont::Render()
{
	if (!m_pFW1FontWarpper)
		return;

	if (FONT_TYPE_OUTLINE == m_eType)
	{
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);

		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x, m_vPos.y - m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x, m_vPos.y + m_fOutlineSize,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x + m_fOutlineSize, m_vPos.y,
			m_nOutlineColor, m_nFlag);
		m_pFW1FontWarpper->DrawString(
			m_pDeivceContext,
			m_wstrText.c_str(), m_fSize,
			m_vPos.x - m_fOutlineSize, m_vPos.y,
			m_nOutlineColor, m_nFlag);
	}

	m_pFW1FontWarpper->DrawString(
		m_pDeivceContext,
		m_wstrText.c_str(), m_fSize,
		m_vPos.x, m_vPos.y,
		m_nColor, m_nFlag);
}

void CFont::Update()
{

}

void CFont::Release()
{
	if ((*m_pRefCnt) == 0)
	{
		Safe_Release(m_pFW1FontWarpper);
		Safe_Release(m_pFW1FontFactory); 
		Safe_Release(m_pWriteFactory);
		CResource::Release_RefCnt();
	}
	else 
		--(*m_pRefCnt);
}
