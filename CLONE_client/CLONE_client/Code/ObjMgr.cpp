#include "stdafx.h"
#include "ObjMgr.h"

#include "Obj.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
		m_pLayer[i] = NULL;
}

CObjMgr::~CObjMgr()
{
	Release();
}

void CObjMgr::AddObj(const CLayer::LayerType& eLayerType,
	const wstring& wstrObjKey,
	CObj* pObj)
{
	if (m_pLayer[eLayerType] == NULL || pObj == NULL)
		return;

	m_pLayer[eLayerType]->AddObj(wstrObjKey, pObj);
}

const CObj* CObjMgr::GetObj(const CLayer::LayerType& eLayerType,
	const wstring& wstrObjKey,
	const int& iIdx /*= 0*/)
{
	if (m_pLayer[eLayerType] == NULL)
		return NULL;

	return m_pLayer[eLayerType]->GetObj(wstrObjKey, iIdx);
}

void CObjMgr::Release()
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
		m_pLayer[i] = NULL;
}
