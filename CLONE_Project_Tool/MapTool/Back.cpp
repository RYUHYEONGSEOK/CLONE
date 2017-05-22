#include "stdafx.h"
#include "Back.h"
#include "SceneMgr.h"
#include "RenderMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "MapToolView.h"
#include "MainFrm.h"
#include "StaticObject.h"
#include "Test.h"
#include "BirdTest.h"
#include "MouseCol.h"
#include "Info.h"
#include "StaticMesh.h"
#include "StaticObject.h"
#include "Input.h"
#include "MeshTool.h"
#include "MyForm.h"
#include "MainFrm.h"


CObj* CBack::m_pMeshForDel = NULL;
vector<CObj*> CBack::m_vecObject;


CBack::CBack()
{
}


CBack::~CBack()
{
}

HRESULT CBack::Initialize(void)
{
	m_pStatic = NULL;
	pMeshComponent = NULL;
	if (FAILED(CreateObj()))
		return E_FAIL;

	m_pMouseCol = new CMouseCol();
	//CMouseCol::GetMousePos
	m_dwTime = GetTickCount();

	return S_OK;
}

int CBack::Update(void)
{
	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();
	if (GetAsyncKeyState(VK_LBUTTON) && bDelMode == true)
	{
		//DeleteMesh();
		if (m_dwTime + 150 < GetTickCount())
		{
			m_dwTime = GetTickCount();

			for (iter; iter != iter_end; ++iter)
			{
				D3DXVECTOR3			vIndex;
				pMeshComponent = const_cast<CComponent*>((*iter)->GetComponent((*iter)->m_wstrObjKey));


				if (m_pMouseCol->PickObject((*iter)->GetInfo(), &vIndex, &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
				{
					m_vecObject.erase(iter);
					return 0;
				}

			}
		}
	}

	//pMeshComponent = const_cast<CComponent*>((*CStaticObject)->GetComponent((*iter)->m_wstrObjKey));
	
	if (GetAsyncKeyState(VK_LBUTTON) && bSelMode == true )
		SelectMesh();
	

	for (iter; iter != iter_end; ++iter)
	{
		(*iter)->Update();
	}
	CObjMgr::GetInstance()->Update();

	return 0;
}

void CBack::Render(void)
{
	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();
	for (iter; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
	float fTime = CTimeMgr::GetInstance()->GetTime();
	CRenderMgr::GetInstance()->Render(fTime);
}

void CBack::Release(void)
{
	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();

	for (; iter != iter_end; ++iter)
	{
		::Safe_Delete(*iter);
	}
	CBack::m_vecObject.clear();

	::Safe_Delete(m_pMouseCol);
	::Safe_Delete(m_pStatic);
	::Safe_Delete(pMeshComponent);
}

CBack * CBack::Create(void)
{

	CBack* pLogo = new CBack;
	if (FAILED(pLogo->Initialize()))
	{
		::Safe_Delete(pLogo);
	}
	return pLogo;

}

int CBack::DeleteMesh(void)
{
	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();
	if (m_dwTime + 150 < GetTickCount())
	{
		m_dwTime = GetTickCount();

		for (iter; iter != iter_end; ++iter)
		{
			D3DXVECTOR3			vIndex;
			pMeshComponent = const_cast<CComponent*>((*iter)->GetComponent((*iter)->m_wstrObjKey));

			
			if (m_pMouseCol->PickObject((*iter)->GetInfo(), &vIndex, &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
			{
				m_vecObject.erase(iter);
				return 0;
			}
			
		}
	}
	return 0;
}

void CBack::SelectMesh(void)
{
	CMeshTool* pObjectTool = &((CMainFrame*)AfxGetMainWnd())->m_pMyForm->m_MeshTool;
	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();
	
	if (m_dwTime + 150 < GetTickCount())
	{
		m_dwTime = GetTickCount();

		for (iter; iter != iter_end; ++iter)
		{
			D3DXVECTOR3			vIndex;
			pMeshComponent = const_cast<CComponent*>((*iter)->GetComponent((*iter)->m_wstrObjKey));

			if (m_pMouseCol->PickObject((*iter)->GetInfo(), &vIndex, &((CStaticMesh*)pMeshComponent)->vecVTXTEX, ((CStaticMesh*)pMeshComponent)->m_iVertices / 2))
			{
				m_pMeshForDel = (*iter);
				pObjectTool->SetPickPos((*iter)->GetInfo()->m_vPos);
				pObjectTool->SetPickScale((*iter)->GetInfo()->m_vScale);
				pObjectTool->m_RotationX = ((*iter)->GetInfo()->m_fAngle[ANGLE_X]);
				pObjectTool->m_RotationY = ((*iter)->GetInfo()->m_fAngle[ANGLE_Y]);
				pObjectTool->m_RotationZ = ((*iter)->GetInfo()->m_fAngle[ANGLE_Z]);
				(*iter)->m_fSelect = 1;

			}
			else
				(*iter)->m_fSelect = 0;

		}
	}

	//bCheck == true;
}

HRESULT CBack::CreateObj(void)
{
	//CRenderMgr* pRenderer = CRenderMgr::GetInstance();
	////ÅÍ·¹ÀÎ
	//CObj* pObj = NULL;
	//m_pStatic = CStaticObject::Create();
	//pObj = m_pStatic;
	//if (pObj == NULL)
	//	return E_FAIL;



	//CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);
	//pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);




	return S_OK;
}


void CBack::SetMainView(CMapToolView* pMainView)
{
	m_pMainView = pMainView;
}
