
// MapToolView.cpp : CMapToolView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
#ifndef SHARED_HANDLERS
#include "MapTool.h"
#endif

#include "MapToolDoc.h"
#include "MapToolView.h"
#include "Device.h"
#include "Include.h"
#include "MainFrm.h"
#include "TimeMgr.h"
#include "Camera.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "Input.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "Back.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "Device.h"
#include "MouseCol.h"
#include "StaticMesh.h"
#include "NaviMgr.h"
#include "StaticObject.h"
#include "NaviCell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMapToolView

IMPLEMENT_DYNCREATE(CMapToolView, CView)

BEGIN_MESSAGE_MAP(CMapToolView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMapToolView ����/�Ҹ�

CMapToolView::CMapToolView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CMapToolView::~CMapToolView()
{
	//destroy
}

BOOL CMapToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CMapToolView �׸���

void CMapToolView::OnDraw(CDC* /*pDC*/)
{
	CMapToolDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CMapToolView �μ�

BOOL CMapToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CMapToolView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CMapToolView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CMapToolView ����

#ifdef _DEBUG
void CMapToolView::AssertValid() const
{
	CView::AssertValid();
}

void CMapToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMapToolDoc* CMapToolView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMapToolDoc)));
	return (CMapToolDoc*)m_pDocument;
}
#endif //_DEBUG


// CMapToolView �޽��� ó����


void CMapToolView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	CMainFrame*	pFrame = (CMainFrame*)AfxGetMainWnd();

	RECT	rcWindow;
	pFrame->GetWindowRect(&rcWindow);

	SetRect(&rcWindow, 0, 0, rcWindow.right - rcWindow.left,
		rcWindow.bottom - rcWindow.top);

	RECT rcMainView;
	GetClientRect(&rcMainView);

	float fRowFrm = float(rcWindow.right - rcMainView.right);
	float fColFrm = float(rcWindow.bottom - rcMainView.bottom);


	pFrame->SetWindowPos(NULL, 0, 0,
		int(WINCX + fRowFrm),
		int(WINCY + fColFrm), SWP_NOZORDER);

	g_hWnd = m_hWnd;

	CDevice::GetInstance()->CreateDevice();


	CTimeMgr::GetInstance()->InitTime();

	if (FAILED(CCamera::GetInstance()->Initialize()))
	{
		AfxMessageBox(L"Camera Initialize Failed", MB_OK);
	}



	if (FAILED(CResourcesMgr::GetInstance()->ReserveContainerSize(RESOURCE_END)))
	{
		AfxMessageBox(L"Resource Container Reserve Failed", MB_OK);
		return;
	}

	//�Ϲ�

	if (CShaderMgr::GetInstance()->AddShaderFiles(L"VS", L"Shader.fx", "VS", "vs_5_0", SHADER_VS))
	{
		AfxMessageBox(L"Vertex Shader(Default) Create Failed", MB_OK);
		return;
	}

	pNaviCell = CNaviMgr::GetInstance();

	pNaviCell->Reserve_cellContainerSize(1000);

	if (CShaderMgr::GetInstance()->AddShaderFiles(L"PS", L"Shader.fx", "PS", "ps_5_0", SHADER_PS))
	{
		AfxMessageBox(L"PIXEL Shader(Default) Create Failed", MB_OK);
		return;
	}

	
	//���̳��� �Ž���
	if (CShaderMgr::GetInstance()->AddShaderFiles(L"VS_ANI", L"Shader.fx", "VS_ANI", "vs_5_0", SHADER_ANI))
	{
		AfxMessageBox(L"Vertex Shader(Ani) Create Failed", MB_OK);
		return;
	}

	if (CInput::GetInstance()->InitInputDevice(AfxGetInstanceHandle(), g_hWnd))
	{
		AfxMessageBox(L"DInput Create Failed", MB_OK);
		return;
	}

	CScene* pScene = NULL;
	pScene = CBack::Create();
	CSceneMgr::GetInstance()->AddScene(SCENE_BACK, pScene);

	CSceneMgr::GetInstance()->ChangeScene(SCENE_BACK);

	//Invalidate(NULL);

	CRenderMgr* pRenderer = CRenderMgr::GetInstance();
	////�ͷ���
	m_pMouseCol = new CMouseCol();
	CObj* pObj = NULL;
	m_pStatic = CStaticObject::Create();
	pObj = m_pStatic;

	CObjMgr::GetInstance()->AddObject(L"StaticObject", pObj);
	pRenderer->AddRenderGroup(TYPE_NONEALPHA, pObj);


	m_iNaviPickCount = 0;

	SetTimer(1, 10, NULL);
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
}


void CMapToolView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	CView::OnTimer(nIDEvent);

	CTimeMgr::GetInstance()->SetTime();
	CInput::GetInstance()->SetInputState();
	CCamera::GetInstance()->Update();

	CSceneMgr::GetInstance()->Update();


	CDevice::GetInstance()->BeginDevice();
	CSceneMgr::GetInstance()->Render();

	pNaviCell->Render();
	CDevice::GetInstance()->EndDevice();
}


void CMapToolView::PostNcDestroy()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	CDevice::GetInstance()->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CCamera::GetInstance()->DestroyInstance();
	CShaderMgr::GetInstance()->DestroyInstance();
	CInput::GetInstance()->DestroyInstance();
	CRenderMgr::GetInstance()->DestroyInstance();
	CObjMgr::GetInstance()->DestroyInstance();
	CSceneMgr::GetInstance()->DestroyInstance();
	CNaviMgr::GetInstance()->DestroyInstance();

	CView::PostNcDestroy();
}

void CMapToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	D3DXVECTOR3 vIndex;
	if (bNaviMode == true && bAddNavi == true)
	{
		if (m_pMouseCol->PickObject(m_pStatic->GetInfo(), &vIndex, &dynamic_cast<CStaticMesh*>(m_pStatic->m_pBuffer)->vecVTXTEX, dynamic_cast<CStaticMesh*>(m_pStatic->m_pBuffer)->m_iVertices / 2))
		{
			if (m_iNaviPickCount == 0)
			{
				NaviPos1 = vIndex;
				D3DXMATRIX pOut;
				D3DXMatrixIdentity(&pOut);
				D3DXMatrixRotationX(&pOut, D3DXToRadian(90));
				D3DXVec3TransformCoord(&NaviPos1, &NaviPos1, &pOut);
				//NaviPos1 = vIndex;
				//cout << "ù��° �� :" << &NaviPos1 << endl;
				pNaviCell->GetNearPoint(NaviPos1, 3);
			}
			else if (m_iNaviPickCount == 1)
			{
				NaviPos2 = vIndex;

				D3DXMATRIX pOut;
				D3DXMatrixIdentity(&pOut);
				D3DXMatrixRotationX(&pOut, D3DXToRadian(90));
				D3DXVec3TransformCoord(&NaviPos2, &NaviPos2, &pOut);
				//NaviPos2 = vIndex;
				//cout << "�ι�° �� :" << &NaviPos2 << endl;
				pNaviCell->GetNearPoint(NaviPos2, 3);
			}
			else if (m_iNaviPickCount == 2)
			{
				NaviPos3 = vIndex;
				D3DXMATRIX pOut;
				D3DXMatrixIdentity(&pOut);
				D3DXMatrixRotationX(&pOut, D3DXToRadian(90));
				D3DXVec3TransformCoord(&NaviPos3, &NaviPos3, &pOut);
				//cout << "����° �� :" << &NaviPos3 << endl;
				pNaviCell->GetNearPoint(NaviPos3, 3);

			}

			++m_iNaviPickCount;

			if (m_iNaviPickCount == 3)
			{
				m_iNaviPickCount = 0;
				D3DXVECTOR3 arrPointPos[3];
				arrPointPos[0] = NaviPos1;
				arrPointPos[1] = NaviPos2;
				arrPointPos[2] = NaviPos3;

				float fSmallZ = arrPointPos[0].z;
				float fBigZ = arrPointPos[0].z;
				DWORD dwSmallZIndex = 0;
				DWORD dwBigZIndex = 0;

				if (arrPointPos[1].z < fSmallZ)
				{
					fSmallZ = arrPointPos[1].z;
					dwSmallZIndex = 1;
				}

				if (arrPointPos[2].z < fSmallZ)
				{
					fSmallZ = arrPointPos[2].z;
					dwSmallZIndex = 2;
				}

				if (arrPointPos[1].z > fBigZ)
				{
					fBigZ = arrPointPos[1].z;
					dwBigZIndex = 1;
				}

				if (arrPointPos[2].z > fBigZ)
				{
					fBigZ = arrPointPos[2].z;
					dwBigZIndex = 2;
				}

				D3DXVECTOR3 vArg1;
				D3DXVECTOR3 vArg2;
				D3DXVECTOR3 vLine(arrPointPos[dwBigZIndex] - arrPointPos[dwSmallZIndex]);

				D3DXVECTOR3 vNormal(vLine.z, 0.f, -vLine.x);

				D3DXVec3Normalize(&vArg1, &(arrPointPos[3 - dwBigZIndex - dwSmallZIndex] - arrPointPos[dwSmallZIndex]));
				D3DXVec3Normalize(&vArg2, &vNormal);

				float fResult = D3DXVec3Dot(&vArg1, &vArg2);
				if (fResult > 0)
				{
					cout << "������ : " << fResult << endl;
					pNaviCell->Add_Cell(&arrPointPos[dwSmallZIndex]
						, &arrPointPos[dwBigZIndex]
						, &arrPointPos[3 - dwBigZIndex - dwSmallZIndex]);
				}
				else
				{
					cout << "����" << fResult << endl;
					pNaviCell->Add_Cell(&arrPointPos[dwSmallZIndex]
						, &arrPointPos[3 - dwBigZIndex - dwSmallZIndex]
						, &arrPointPos[dwBigZIndex]);
				}
				//	pNaviCell->Add_Cell(&D3DXVECTOR3(0.f, 0.f, 0.f), &D3DXVECTOR3(0.f, 5.f, 0.f), &D3DXVECTOR3(5.f, 0.f, 0.f));

			}

		}
	}

}

bool CMapToolView::RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius)
{
	float a, b, c, discriminant;

	a = (rayDirection.x * rayDirection.x) + (rayDirection.y * rayDirection.y) + (rayDirection.z * rayDirection.z);
	b = ((rayDirection.x * rayOrigin.x) + (rayDirection.y * rayOrigin.y) + (rayDirection.z * rayOrigin.z)) * 2.0f;
	c = ((rayOrigin.x * rayOrigin.x) + (rayOrigin.y * rayOrigin.y) + (rayOrigin.z * rayOrigin.z)) - (radius * radius);

	discriminant = (b*b) - (4 * a * c);
	
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}
