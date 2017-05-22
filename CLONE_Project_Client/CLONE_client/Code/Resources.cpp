#include "stdafx.h"
#include "Resources.h"

CResource::CResource(CDevice* pDevice)
	: m_pDevice(pDevice)
	, m_pRefCnt(new WORD(0))
{

}

CResource::~CResource()
{

}

void CResource::Release_RefCnt()
{
	Safe_Delete(m_pRefCnt);
}
