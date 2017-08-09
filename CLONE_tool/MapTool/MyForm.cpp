// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MyForm.h"
#include "MeshTool.h"
#include "Back.h"
#include "Obj.h"
#include "Info.h"
#include "BirdTest.h"
#include "Test.h"
#include "Bridge.h"
#include "Building.h"
#include "Building.h"
#include "Fountain.h"
#include "MeshTool.h"
#include "NaviCell.h"
#include "NaviMgr.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(IDD_MYFORM)
{

}

CMyForm::~CMyForm()
{
	CNaviMgr::GetInstance()->DestroyInstance();
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_iRadio[0]);
	DDX_Control(pDX, IDC_RADIO2, m_iRadio[1]);
	DDX_Control(pDX, IDC_RADIO4, m_iNaviRadio[0]);
	DDX_Control(pDX, IDC_RADIO5, m_iNaviRadio[1]);


}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_MeshTool, &CMyForm::OnBnClickedMeshtool)
	ON_BN_CLICKED(IDC_MeshSave, &CMyForm::OnBnClickedMeshsave)
	ON_BN_CLICKED(IDC_MeshLoad, &CMyForm::OnBnClickedMeshload)
	ON_BN_CLICKED(IDC_RADIO1, &CMyForm::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CMyForm::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_STATIC4, &CMyForm::OnBnClickedStatic4)
	ON_BN_CLICKED(IDC_RADIO4, &CMyForm::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CMyForm::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_MeshSave2, &CMyForm::OnBnClickedMeshsave2)
	ON_BN_CLICKED(IDC_NaviSave, &CMyForm::OnBnClickedNavisave)
	ON_BN_CLICKED(IDC_NaviLoad, &CMyForm::OnBnClickedNaviload)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.

void CMyForm::OnInitialUpdate()
{

	CFormView::OnInitialUpdate();
	m_iRadio[0].SetCheck(1);

	m_Font.CreatePointFont(90, L"코버워치");
	//GetDlgItem(IDC_MeshTool)->SetFont(&m_Font);
	//GetDlgItem(IDC_NaviTool)->SetFont(&m_Font);
	//GetDlgItem(IDC_MeshSave)->SetFont(&m_Font);
	//GetDlgItem(IDC_MeshLoad)->SetFont(&m_Font);
	//GetDlgItem(IDC_NaviSave)->SetFont(&m_Font);
	//GetDlgItem(IDC_NaviLoad)->SetFont(&m_Font);
	//GetDlgItem(IDC_RADIO1)->SetFont(&m_Font);
	//GetDlgItem(IDC_RADIO2)->SetFont(&m_Font);
	////..GetDlgItem(IDC_RADIO3)->SetFont(&m_Font);
	//GetDlgItem(IDC_STATIC1)->SetFont(&m_Font);
	//GetDlgItem(IDC_STATIC2)->SetFont(&m_Font);
	//GetDlgItem(IDC_STATIC3)->SetFont(&m_Font);


	//pNavi->GetInstance();
	pNavi = CNaviMgr::GetInstance();
	pNavi->Reserve_cellContainerSize(1000);

	Invalidate(FALSE);



	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
}




void CMyForm::OnBnClickedMeshtool()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_MeshTool.GetSafeHwnd() == NULL)
	{
		m_MeshTool.Create(IDD_MESHTOOL);
	}

	m_MeshTool.ShowWindow(SW_SHOW);
}


void CMyForm::OnBnClickedMeshsave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	if (CBack::m_vecObject.size() == 0)
	{
		MessageBox(L"No Object", L"Save Failed", MB_OK);
		return;
	}

	//CFileDialog		Dlg(FALSE, L"dat", L"*.dat", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this);

	//if (Dlg.DoModal() == IDCANCEL)
	//	return;

	//TCHAR	szDirPath[MAX_PATH] = L"";

	//GetCurrentDirectory(sizeof(szDirPath), szDirPath);	//경로 얻어오기
	//SetCurrentDirectory(szDirPath);

	//PathRemoveFileSpec(szDirPath);	//경로 자르기

	//lstrcat(szDirPath, L"..\\Data");

	//Dlg.m_ofn.lpstrInitialDir = szDirPath;	//원하는 경로로 바꾸기

	HANDLE hFile = CreateFile(L"../Data/Map.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwCount = CBack::m_vecObject.size();

	vector<CObj*>::iterator iter = CBack::m_vecObject.begin();
	vector<CObj*>::iterator iter_end = CBack::m_vecObject.end();

	int iCount = CBack::m_vecObject.size();

	WriteFile(hFile, &iCount, sizeof(int), &dwCount, NULL);

	for (iter; iter != iter_end; ++iter)
	{
		WriteFile(hFile, (*iter)->GetInfo()->m_vPos, sizeof(D3DXVECTOR3), &dwCount , NULL);
		WriteFile(hFile, (*iter)->GetInfo()->m_vScale, sizeof(D3DXVECTOR3), &dwCount, NULL);
		WriteFile(hFile, (*iter)->GetInfo()->m_fAngle, sizeof(float) * 3, &dwCount, NULL);
		WriteFile(hFile, (*iter)->m_wstrObjKey.c_str(), sizeof(TCHAR) * 30, &dwCount, NULL);
	}

	if (iter == iter_end)
	{
		AfxMessageBox(L"Object Save Complete", MB_OK);
		return;
	}
	CloseHandle(hFile);

}


void CMyForm::OnBnClickedMeshload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	UpdateData(TRUE);

	/*CFileDialog		Dlg(TRUE, L"dat", L"*.dat",
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, L"*.dat", this);

	if (Dlg.DoModal() == IDCANCEL)
		return;

	TCHAR		szDirPath[MAX_PATH] = L"";

	GetCurrentDirectory(sizeof(szDirPath), szDirPath);

	SetCurrentDirectory(szDirPath);

	PathRemoveFileSpec(szDirPath);

	lstrcat(szDirPath, L"..\\Data");

	Dlg.m_ofn.lpstrInitialDir = szDirPath;

	HANDLE	hFile = CreateFile(Dlg.GetPathName(), GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
*/
	HANDLE hFile;
	hFile = CreateFile(L"../Data/Map.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte;
	int iSize = 0;
	int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);

	
		while (iSize < iCount)
		{
			TCHAR	tcObjKey[30];
			CObj* pObj = NULL;
			wstring wstrObjKey;
			D3DXVECTOR3 vPos, vScale;
			float fAngle[3] = { 0 };
			ReadFile(hFile, &vPos, sizeof(D3DXVECTOR3), &dwByte, NULL);
			ReadFile(hFile, &vScale, sizeof(D3DXVECTOR3), &dwByte, NULL);
			ReadFile(hFile, &fAngle, sizeof(float) * 3, &dwByte, NULL);
			ReadFile(hFile, &tcObjKey, sizeof(TCHAR) * 30, &dwByte, NULL);
			wstrObjKey = tcObjKey;

			if (wstrObjKey == L"1Unit")
				pObj = CTest::Create();
			else if (wstrObjKey == L"BirdTest")
				pObj = CBirdTest::Create();
			else if (wstrObjKey == L"Bridge")
				pObj = CBridge::Create();
			else if (wstrObjKey == L"Building")
				pObj = CBuilding::Create();
			else if (wstrObjKey == L"Fountain")
				pObj = CFountain::Create();

			pObj->GetInfo()->m_vPos = vPos;
			pObj->GetInfo()->m_vScale = vScale;
			pObj->GetInfo()->m_fAngle[ANGLE_X] = fAngle[0];
			pObj->GetInfo()->m_fAngle[ANGLE_Y] = fAngle[1];
			pObj->GetInfo()->m_fAngle[ANGLE_Z] = fAngle[2];

			CBack::m_vecObject.push_back(pObj);
			++iSize;
			if (iSize == iCount )
			{
				AfxMessageBox(L"Object Load Complete", MB_OK);
				//return;
			}
		}

	
		//AfxMessageBox(L"Object Load Complete", MB_OK);
		//return;


	CloseHandle(hFile);
	UpdateData(FALSE);
}

void CMyForm::OnBnClickedNavisave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	UpdateData(TRUE);

	if (CNaviMgr::m_vecNaviMesh.size() == 0)
	{
		MessageBox(L"No Navi", L"Save Failed", MB_OK);
		return;
	}


	HANDLE hFile = CreateFile(L"../Data/Navi.dat", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwCount = CNaviMgr::m_vecNaviMesh.size();

	vector<CNaviCell*>::iterator iter = CNaviMgr::m_vecNaviMesh.begin();
	vector<CNaviCell*>::iterator iter_end = CNaviMgr::m_vecNaviMesh.end();

	int iCount = CNaviMgr::m_vecNaviMesh.size();

	WriteFile(hFile, &iCount, sizeof(int), &dwCount, NULL);

	for (iter; iter != iter_end; ++iter)
	{
		WriteFile(hFile, (*iter)->GetPoint(CNaviCell::POINT_A), sizeof(D3DXVECTOR3), &dwCount, NULL);
		WriteFile(hFile, (*iter)->GetPoint(CNaviCell::POINT_B), sizeof(D3DXVECTOR3), &dwCount, NULL);
		WriteFile(hFile, (*iter)->GetPoint(CNaviCell::POINT_C), sizeof(D3DXVECTOR3), &dwCount, NULL);
	}

	if (iter == iter_end)
	{
		AfxMessageBox(L"Navi Save Complete", MB_OK);
		return;
	}
	CloseHandle(hFile);
}

void CMyForm::OnBnClickedNaviload()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	HANDLE hFile;
	hFile = CreateFile(L"../Data/Navi.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD	dwByte;
	int iSize = 0;
	int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);
	cout << iCount << endl;

	while (iSize < iCount)
	{
		D3DXVECTOR3 vPoint1, vPoint2, vPoint3;

		ReadFile(hFile, &vPoint1, sizeof(D3DXVECTOR3), &dwByte, NULL);
		ReadFile(hFile, &vPoint2, sizeof(D3DXVECTOR3), &dwByte, NULL);
		ReadFile(hFile, &vPoint3, sizeof(D3DXVECTOR3), &dwByte, NULL);

		pNavi->Add_Cell(&vPoint1, &vPoint2, &vPoint3);
		++iSize;

	}


	CloseHandle(hFile);

	UpdateData(FALSE);
}





void CMyForm::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_iRadio[0].GetCheck())
	{
		bMapMode = true;
		bNaviMode = false;
	}

	
}


void CMyForm::OnBnClickedRadio2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_iRadio[1].GetCheck())
	{
		bMapMode = false;
		bNaviMode = true;
	}
}


void CMyForm::OnBnClickedStatic4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CMyForm::OnBnClickedRadio4()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_iNaviRadio[0].GetCheck())
	{
		bAddNavi = true;
		bDelNavi = false;
	}

}


void CMyForm::OnBnClickedRadio5()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_iNaviRadio[1].GetCheck())
	{
		bAddNavi = false;
		bDelNavi = true;
	}

}


void CMyForm::OnBnClickedMeshsave2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (bDelNavi == true)
	{
		//CNaviCell* pNaviCell = CNaviMgr::m_vecNaviMesh.back();

		size_t cellSize = CNaviMgr::m_vecNaviMesh.size();
		if (cellSize != 0)
		{
			vector<CNaviCell*>::iterator iter = CNaviMgr::m_vecNaviMesh.begin();
			iter += cellSize - 1;
			CNaviMgr::m_vecNaviMesh.erase(iter);
		}
	}
}





