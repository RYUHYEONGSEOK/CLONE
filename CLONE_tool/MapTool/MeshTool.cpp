// MeshTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MeshTool.h"
#include "afxdialogex.h"
#include "Back.h"
#include "Info.h"
#include "Obj.h"
#include "Test.h"
#include "BirdTest.h"
#include "RenderMgr.h"
#include "ObjMgr.h"
#include "Bridge.h"
#include "Building.h"
#include "Fountain.h"
#include "Camera.h"
#include "Device.h"
#include "MainFrm.h"



// CMeshTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMeshTool, CDialog)

CMeshTool::CMeshTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MESHTOOL, pParent)
	, m_bStart(true)
	, m_PosX(0.0f)
	, m_PosY(0.0f)
	, m_PosZ(0.0f)
	, m_RotationX(-90.0f)
	, m_RotationY(0.0f)
	, m_RotationZ(0.0f)	
	, m_ScaleX(1.0f)
	, m_ScaleY(1.0f)
	, m_ScaleZ(1.0f)
{
	ZeroMemory(m_bCheck, sizeof(bool) * 2);
}

CMeshTool::~CMeshTool()
{
}

void CMeshTool::SetPickPos(D3DXVECTOR3 vPos)
{
	UpdateData(TRUE);

	m_PosX = vPos.x;
	m_PosY = vPos.y;
	m_PosZ = vPos.z;

	UpdateData(FALSE);
}

void CMeshTool::SetPickScale(D3DXVECTOR3 vScale)
{
	UpdateData(TRUE);

	m_ScaleX = vScale.x;
	m_ScaleY = vScale.y;
	m_ScaleZ = vScale.z;

	UpdateData(FALSE);
}



void CMeshTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListBox);
	DDX_Text(pDX, IDC_PositionX, m_PosX);
	DDX_Text(pDX, IDC_PositionY, m_PosY);
	DDX_Text(pDX, IDC_PositionZ, m_PosZ);
	DDX_Control(pDX, IDC_MeshSelect, m_ModeRadio[0]);
	DDX_Control(pDX, IDC_MeshCreate, m_ModeRadio[1]);
	DDX_Control(pDX, IDC_MeshDelete, m_ModeRadio[2]);

	//m_pInfo = CInfo::Create(D3DXVECTOR3(0.f, 0.f, 0.f));

	if (m_bStart)
	{
		m_ModeRadio[1].SetCheck(1);
		m_bStart = false;
	}


	DDX_Text(pDX, IDC_RotationX, m_RotationX);
	DDX_Text(pDX, IDC_RotationY, m_RotationY);
	DDX_Text(pDX, IDC_RotationZ, m_RotationZ);
	DDX_Text(pDX, IDC_ScaleX, m_ScaleX);
	DDX_Text(pDX, IDC_ScaleY, m_ScaleY);
	DDX_Text(pDX, IDC_ScaleZ, m_ScaleZ);
}


BEGIN_MESSAGE_MAP(CMeshTool, CDialog)
	ON_BN_CLICKED(IDC_LoadMesh, &CMeshTool::OnBnClickedLoadmesh)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMeshTool::OnLbnSelchangeList1)
	ON_EN_CHANGE(IDC_PositionX, &CMeshTool::OnEnChangePositionx)
	ON_EN_CHANGE(IDC_PositionY, &CMeshTool::OnEnChangePositiony)
	ON_EN_CHANGE(IDC_PositionZ, &CMeshTool::OnEnChangePositionz)
	ON_BN_CLICKED(IDC_CreateMesh, &CMeshTool::OnBnClickedCreatemesh)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CMeshTool::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &CMeshTool::OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &CMeshTool::OnDeltaposSpin3)
	ON_EN_CHANGE(IDC_RotationX, &CMeshTool::OnEnChangeRotationx)
	ON_EN_CHANGE(IDC_RotationY, &CMeshTool::OnEnChangeRotationy)
	ON_EN_CHANGE(IDC_RotationZ, &CMeshTool::OnEnChangeRotationz)
	ON_EN_CHANGE(IDC_ScaleX, &CMeshTool::OnEnChangeScalex)
	ON_EN_CHANGE(IDC_ScaleY, &CMeshTool::OnEnChangeScaley)
	ON_EN_CHANGE(IDC_ScaleZ, &CMeshTool::OnEnChangeScalez)
	ON_BN_CLICKED(IDC_MeshSelect, &CMeshTool::OnMapMode)
	ON_BN_CLICKED(IDC_MeshCreate, &CMeshTool::OnMapMode)
	ON_BN_CLICKED(IDC_MeshDelete, &CMeshTool::OnMapMode)
	ON_BN_CLICKED(IDC_Update, &CMeshTool::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_LastDelete, &CMeshTool::OnBnClickedLastdelete)
END_MESSAGE_MAP()


// CMeshTool 메시지 처리기입니다.

void CMeshTool::OnMapMode()
{

	if (m_ModeRadio[0].GetCheck())
	{
		bSelMode = true;
		bDelMode = false;
		bCreMode = false;
	}
	else if (m_ModeRadio[1].GetCheck())
	{
		bSelMode = false;
		bDelMode = false;
		bCreMode = true;
	}
	else if (m_ModeRadio[2].GetCheck())
	{
		bSelMode = false;
		bDelMode = true;
		bCreMode = false;
	}
}

void CMeshTool::OnBnClickedLoadmesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	CFileFind finder;
	BOOL bWorking = finder.FindFile(_T("..//Resource/Land/*.*"));
	while (bWorking)
	{
		bWorking = finder.FindNextFileW();
		char str[256];
		memset(str, 0, 256);
		wcstombs_s(NULL, str, finder.GetFileName().GetLength() + 1,
			finder.GetFileName(), _TRUNCATE);

		WCHAR wstr[MAX_PATH];

		int nLen = (int)strlen(str) + 1;
		mbstowcs(wstr, str, nLen);

		if (!finder.IsDots())
			m_ListBox.AddString(wstr);
	}

	UpdateData(FALSE);
}


void CMeshTool::OnLbnSelchangeList1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	int iSelectIndex = m_ListBox.GetCurSel();

	if (iSelectIndex < 0)
		return;

	m_ListBox.GetText(iSelectIndex, m_strSelectName);

	for (int i = 0; i < 5; ++i)
	{
		m_bCheck[i] = false;
	}
	m_bCheck[iSelectIndex] = true;
	int i = 0;
}


void CMeshTool::OnEnChangePositionx()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fPosValue = m_PosX;
		
		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vPos.x = m_fPosValue;

		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangePositiony()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fPosValue = m_PosY;

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vPos.y = m_fPosValue;

		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangePositionz()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fPosValue = m_PosZ;

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vPos.z = m_fPosValue;
		

		UpdateData(FALSE);
	}
}






void CMeshTool::OnBnClickedCreatemesh()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	//CObj*	m_pObj = CBack::m_pMeshForDel;

	float m_fPosX = m_PosX;
	float m_fPosY =	m_PosY;
	float m_fPosZ = m_PosZ;

	float m_fRotX = m_RotationX;
	float m_fRotY = m_RotationY;
	float m_fRotZ = m_RotationZ;

	float m_fScaleX = m_ScaleX;
	float m_fScaleY = m_ScaleY;
	float m_fScaleZ = m_ScaleZ;

	//const D3DXVECTOR3 vPos(m_fPosX, m_fPosY, m_fPosZ);
	//const D3DXVECTOR3 

	if (m_ModeRadio[1].GetCheck() && bMapMode == true)
	{
		if (m_bCheck[0] == true)
		{
			CObj* pTest = NULL;
			pTest = CTest::Create();
			//const D3DXVECTOR3 vPos(m_fPosX, m_fPosY, m_fPosZ);
			//const D3DXVECTOR3 vScale(m_fScaleX, m_fScaleY, m_fScaleZ);
			pTest->m_pInfo->m_vPos = D3DXVECTOR3(m_fPosX, m_fPosY, m_fPosZ);
			pTest->m_pInfo->m_vScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
			pTest->m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(m_fRotX);
			pTest->m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fRotY);
			pTest->m_pInfo->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fRotZ);
			CBack::m_vecObject.push_back(pTest);
		}

		if (m_bCheck[1] == true)
		{
			CObj* pBird = NULL;
			pBird = CBirdTest::Create();
			pBird->m_pInfo->m_vPos = D3DXVECTOR3(m_fPosX, m_fPosY, m_fPosZ);
			pBird->m_pInfo->m_vScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
			pBird->m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(m_fRotX);
			pBird->m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fRotY);
			pBird->m_pInfo->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fRotZ);
			CBack::m_vecObject.push_back(pBird);
		}

		if (m_bCheck[2] == true)
		{
			CObj*	pBridge = NULL;
			pBridge = CBridge::Create();
			pBridge->m_pInfo->m_vPos = D3DXVECTOR3(m_fPosX, m_fPosY, m_fPosZ);
			pBridge->m_pInfo->m_vScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
			pBridge->m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(m_fRotX);
			pBridge->m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fRotY);
			pBridge->m_pInfo->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fRotZ);
			CBack::m_vecObject.push_back(pBridge);
		}

		if (m_bCheck[3] == true)
		{
			CObj*	pBuilding = NULL;
			pBuilding = CBuilding::Create();
			pBuilding->m_pInfo->m_vPos = D3DXVECTOR3(m_fPosX, m_fPosY, m_fPosZ);
			pBuilding->m_pInfo->m_vScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
			pBuilding->m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(m_fRotX);
			pBuilding->m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fRotY);
			pBuilding->m_pInfo->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fRotZ);
			CBack::m_vecObject.push_back(pBuilding);
		}


		if (m_bCheck[4] == true)
		{
			CObj*	pFountain = NULL;
			pFountain = CFountain::Create();
			pFountain->m_pInfo->m_vPos = D3DXVECTOR3(m_fPosX, m_fPosY, m_fPosZ);
			pFountain->m_pInfo->m_vScale = D3DXVECTOR3(m_fScaleX, m_fScaleY, m_fScaleZ);
			pFountain->m_pInfo->m_fAngle[ANGLE_X] = D3DXToRadian(m_fRotX);
			pFountain->m_pInfo->m_fAngle[ANGLE_Y] = D3DXToRadian(m_fRotY);
			pFountain->m_pInfo->m_fAngle[ANGLE_Z] = D3DXToRadian(m_fRotZ);
			CBack::m_vecObject.push_back(pFountain);
		}

	}


	
}


void CMeshTool::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fPosValue = m_PosX;
	if (pNMUpDown->iDelta < 0)
	{
		if (m_PosX >= 999999.f)
			return;
		m_fPosValue += 0.1f;
	}
	else
	{
		m_fPosValue -= 0.1f;
		
	}
	*pResult = 0;
}


void CMeshTool::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	m_fPosValue = m_PosY;
	if (pNMUpDown->iDelta < 0)
	{
		if (m_PosY >= 999999.f)
			return;
		m_fPosValue += 0.1f;
	
	}
	else
	{
		m_fPosValue -= 0.1f;
	
	}


	*pResult = 0;
}


void CMeshTool::OnDeltaposSpin3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fPosValue = m_PosZ;
	if (pNMUpDown->iDelta < 0)
	{
		if (m_PosZ >= 999999.f)
			return;
		m_fPosValue += 0.1f;
	
	}
	else
	{
		m_fPosValue -= 0.1f;
	
	}


	*pResult = 0;
}


void CMeshTool::OnEnChangeRotationx()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fRotValue = m_RotationX;

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_fAngle[ANGLE_X] = m_fRotValue;
	

		UpdateData(FALSE);
	}
}





void CMeshTool::OnEnChangeRotationy()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fRotValue = m_RotationY;

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_fAngle[ANGLE_Y] = m_fRotValue;

		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangeRotationz()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fRotValue = m_RotationZ;

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_fAngle[ANGLE_Z] = m_fRotValue;

		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangeScalex()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.


	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fScaleValue = (m_ScaleX);

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vScale.x = m_fScaleValue;


		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangeScaley()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fScaleValue = (m_ScaleY);

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vScale.y = m_fScaleValue;

		UpdateData(FALSE);
	}
}


void CMeshTool::OnEnChangeScalez()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialog::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (!m_ModeRadio[2].GetCheck() && CBack::m_pMeshForDel != NULL)
	{
		UpdateData(TRUE);
		m_fScaleValue = (m_ScaleZ);

		CInfo* pTransForm = CBack::m_pMeshForDel->m_pInfo;
		if (m_ModeRadio[0].GetCheck())
			pTransForm->m_vScale.z = m_fScaleValue;
		

		UpdateData(FALSE);
	}
}


void CMeshTool::OnBnClickedUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

}


void CMeshTool::OnBnClickedLastdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_ModeRadio[2].GetCheck())
	{
		size_t BackSize = CBack::m_vecObject.size();

		if (BackSize != 0)
		{
			vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
			iter += BackSize - 1;
			CBack::m_vecObject.erase(iter);
		}
	}
}




