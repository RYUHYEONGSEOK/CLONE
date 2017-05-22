#include "stdafx.h"
#include "Scene.h"

#include "GraphicDevice.h"
#include "Function.h"

#include "Camera.h"
#include "Loading.h"

CScene::CScene(CDevice* pDevice)
	: m_pDevice(pDevice)
	, m_pMainCamera(NULL)
	, m_pLoading(NULL)
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
		m_pLayer[i] = NULL;
}


CScene::~CScene()
{
	Release_Layer();
}

void CScene::Update_Layer()
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
	{
		if (NULL != m_pLayer[i])
			m_pLayer[i]->Update_Obj();
	}
}

void CScene::Render_Layer()
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
	{
		if (NULL != m_pLayer[i])
		{
			switch (i)
			{
			case CLayer::LAYERTYPE_ENVIRONMENT:
				m_pDevice->Blend_Begin();
				m_pLayer[i]->Render_Obj();
				m_pDevice->Blend_End();
				break;

			case CLayer::LAYERTYPE_GAMELOGIC:
				m_pLayer[i]->Render_Obj();
				break;

			case CLayer::LAYERTYPE_UI:
				/*if (m_pMainCamera != NULL)
					((CCamera*)m_pMainCamera)->Invalidate_Ortho();*/

				m_pDevice->Blend_Begin();
				m_pLayer[i]->Render_Obj();
				m_pDevice->Blend_End();

				/*if (m_pMainCamera != NULL)
				{
					((CCamera*)m_pMainCamera)->Invalidate_View();
					((CCamera*)m_pMainCamera)->Invalidate_Proj();
				}*/
				break;

			}
		}
	}
}

void CScene::ChangeState_Layer(STAGESTATETYPE eType)
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
	{
		if (NULL != m_pLayer[i])
			m_pLayer[i]->ChangeState_Obj(eType);
	}
}

const CComponent * CScene::GetComponent(CLayer::LayerType eType, const wstring & wstrObjKey, const wstring & wstrComponentKey)
{
	return m_pLayer[eType]->GetComponent(wstrObjKey, wstrComponentKey);
}

const CObj * CScene::GetObj(CLayer::LayerType eType, const wstring & wstrObjKey, const unsigned int & iIndex /*=0*/)
{
	return m_pLayer[eType]->GetObj(wstrObjKey, iIndex);
}

const unordered_map<wstring, list<CObj*>>* CScene::GetObjList(CLayer::LayerType eType)
{
	return m_pLayer[eType]->GetMapObjList();
}

void CScene::Release_Layer()
{
	for (int i = 0; i < CLayer::LAYERTYPE_END; ++i)
		::Safe_Delete(m_pLayer[i]);
}
