#include "stdafx.h"
#include "Layer.h"
//Manager
#include "SceneMgr.h"
//Obj
#include "Renderer.h"
#include "Obj.h"

CLayer::CLayer(CDevice* pDevice)
	: m_pDevice(pDevice)
{

}

CLayer::~CLayer()
{
	Release_Obj();
}

CLayer* CLayer::Create(CDevice* pDevice)
{
	return new CLayer(pDevice);
}

HRESULT CLayer::AddObj(const wstring& wstrObjKey, CObj* pObj)
{
	if (pObj)
	{
		unordered_map<wstring, ListObj>::iterator iter = m_mapObjList.find(wstrObjKey);

		if (iter == m_mapObjList.end())
			m_mapObjList[wstrObjKey] = ListObj();

		m_mapObjList[wstrObjKey].push_back(pObj);
	}
	else return E_FAIL;

	return S_OK;
}

const CObj* CLayer::GetObj(const wstring& wstrObjKey,
	const unsigned int& iIndex /*= 0*/)
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.find(wstrObjKey);

	if (iterMap == m_mapObjList.end())
		return NULL;

	if (iIndex >= iterMap->second.size())
		return NULL;


	ListObj::iterator	iterList = iterMap->second.begin();
	for (unsigned int i = 0; i < iIndex; ++iterList) {}
	return (*iterList);
}

const CComponent* CLayer::GetComponent(const wstring& wstrObjKey, const wstring& wstrComponentKey)
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.find(wstrObjKey);

	if (iterMap == m_mapObjList.end())
		return NULL;

	ListObj::iterator	iterList = iterMap->second.begin();
	ListObj::iterator	iterList_end = iterMap->second.end();

	for (; iterList != iterList_end; ++iterList)
	{
		const CComponent* pComponent = (*iterList)->GetComponent(wstrComponentKey);

		if (pComponent != NULL)
			return pComponent;
	}
	return NULL;
}

void CLayer::Update_Obj()
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.begin();
	unordered_map<wstring, ListObj>::iterator	iterMap_end = m_mapObjList.end();

	ListObj::iterator	iterList;
	ListObj::iterator	iterList_end;

	for (; iterMap != iterMap_end; ++iterMap)
	{
		iterList = iterMap->second.begin();
		iterList_end = iterMap->second.end();

		for (; iterList != iterList_end;)
		{
			if ((*iterList)->Update() == CObj::OBJ_STATE_DIE)
			{
				CRenderer* pRenderer = CSceneMgr::GetInstance()->GetRenderer();
				pRenderer->RemoveRenderGroup((*iterList)->m_eRenderType, (*iterList));

				Safe_Delete(*iterList);
				iterList = iterMap->second.erase(iterList);
			}
			else 
				++iterList;
		}
	}
}

void CLayer::Render_Obj()
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.begin();
	unordered_map<wstring, ListObj>::iterator iterMap_end = m_mapObjList.end();

	ListObj::iterator	iterList;
	ListObj::iterator	iterList_end;

	for (; iterMap != iterMap_end; ++iterMap)
	{
		iterList = iterMap->second.begin();
		iterList_end = iterMap->second.end();

		for (; iterList != iterList_end; ++iterList)
			(*iterList)->Render();
	}
}

void CLayer::ChangeState_Obj(STAGESTATETYPE eType)
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.begin();
	unordered_map<wstring, ListObj>::iterator iterMap_end = m_mapObjList.end();

	ListObj::iterator	iterList;
	ListObj::iterator	iterList_end;

	for (; iterMap != iterMap_end; ++iterMap)
	{
		iterList = iterMap->second.begin();
		iterList_end = iterMap->second.end();

		for (; iterList != iterList_end; ++iterList)
			(*iterList)->SetStageState(eType);
	}
}

void CLayer::Release_Obj()
{
	unordered_map<wstring, ListObj>::iterator	iterMap = m_mapObjList.begin();
	unordered_map<wstring, ListObj>::iterator iterMap_end = m_mapObjList.end();

	ListObj::iterator	iterList;
	ListObj::iterator	iterList_end;

	for (; iterMap != iterMap_end; ++iterMap)
	{
		iterList = iterMap->second.begin();
		iterList_end = iterMap->second.end();

		for (; iterList != iterList_end; ++iterList)
			::Safe_Delete(*iterList);

		iterMap->second.clear();
	}
	m_mapObjList.clear();
}




