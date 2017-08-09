#include "stdafx.h"
#include "UI.h"
//Manager
#include "ResourceMgr.h"
#include "LightMgr.h"
//Object
#include "TextureShader.h"

CUI::CUI(CDevice * pDevice)
	: CObj(pDevice)
	, m_fSizeX(0.f)
	, m_fSizeY(0.f)
	, m_fX(0.f)
	, m_fY(0.f)
{
	D3DXMatrixIdentity(&m_matUIView);
	D3DXMatrixIdentity(&m_matUIProj);
}

CUI::~CUI()
{
	
}