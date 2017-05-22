#include "stdafx.h"
#include "MainServer.h"

#include "NaviMgr.h"
#include "ClientMgr.h"
#include "TimerQMgr.h"
#include "GameMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"

#include "AcceptThread.h"
#include "WorkThread.h"
#include "ReservationThread.h"
#include "LogicThread.h"

IMPLEMENT_SINGLETON(CMainServer)

CMainServer::CMainServer()
	: m_pAcceptThread(nullptr)
	, m_pReservationThread(nullptr)
	, m_pLogicThread(nullptr)
	, m_iWorkThreadCount(UNKNOWN_VALUE)
{
	//get systeminfo
	GetSystemInfo(&m_tSystemInfo);

	/*
	the best workthread count is doubled processorscount.
	m_iWorkThreadCount = m_tSystemInfo.dwNumberOfProcessors * 2;
	But, i will test half count
	*/
	m_iWorkThreadCount = m_tSystemInfo.dwNumberOfProcessors;

	//rand num
	srand(unsigned(time(NULL)));
}
CMainServer::~CMainServer()
{
	if (!(Release()))
	{
		cout << "CMainServer Release error" << endl;
	}
}

bool CMainServer::MakeThread(void)
{
	//accept thread
	if (m_pAcceptThread == nullptr)
	{
		m_pAcceptThread = new thread{ AcceptThread };

		if (m_pAcceptThread == nullptr)
		{
			cout << "m_pAcceptThread make error" << endl;
			return false;
		}
	}

	//reservation thread
	if (m_pReservationThread == nullptr)
	{
		m_pReservationThread = new thread{ ReservationThread };

		if (m_pReservationThread == nullptr)
		{
			cout << "m_pReservationThread make error" << endl;
			return false;
		}
	}

	//work thread
	for (int i = 0; i < m_iWorkThreadCount; ++i)
	{
		m_WorkThreadVector.push_back(new thread{ WorkThread });
	}

	//logic thread
	if (m_pLogicThread == nullptr)
	{
		m_pLogicThread = new thread{ LogicThread };

		if (m_pLogicThread == nullptr)
		{
			cout << "m_pLogicThread make error" << endl;
			return false;
		}
	}

	return true;
}
bool CMainServer::ClearThread(void)
{
	m_pAcceptThread->join();
	delete m_pAcceptThread;
	m_pAcceptThread = nullptr;

	m_pReservationThread->join();
	delete m_pReservationThread;
	m_pReservationThread = nullptr;

	for (int i = 0; i < m_iWorkThreadCount; ++i)
	{
		m_WorkThreadVector[i]->join();
		delete m_WorkThreadVector[i];
		m_WorkThreadVector[i] = nullptr;
	}

	m_pLogicThread->join();
	delete m_pLogicThread;
	m_pLogicThread = nullptr;

	return true;
}
bool CMainServer::NaviLoad(void)
{
	HANDLE hFile;
	hFile = CreateFile(L"../Data/Navi/Navi.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwByte = 0;
	int iSize = 0;
	int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);

	CNaviMgr::GetInstance()->ReserveCellContainerSize(iCount);

	D3DXMATRIX matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, D3DXToRadian(-90.f));

	while (iSize < iCount)
	{
		D3DXVECTOR3 vPoint1, vPoint2, vPoint3;

		ReadFile(hFile, &vPoint1, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint1, &vPoint1, &matRotX);
		ReadFile(hFile, &vPoint2, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint2, &vPoint2, &matRotX);
		ReadFile(hFile, &vPoint3, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint3, &vPoint3, &matRotX);

		CNaviMgr::GetInstance()->AddCell(&vPoint1, &vPoint2, &vPoint3);
		++iSize;
	}

	CloseHandle(hFile);
	CNaviMgr::GetInstance()->LinkCell();

	return true;
}
bool CMainServer::NaviDelete(void)
{
	CNaviMgr::DestroyInstance();
	return true;
}

bool CMainServer::Initialize(void)
{
	//network Initialize
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "wsaData error!" << endl;
		return false;
	}

	//IOCP Initiailize
	if (g_hIOCP == NULL)
	{
		g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	}

	//navi loading
	if (!NaviLoad())
	{
		cout << "CMainServer Initialize NaviLoad() error" << endl;
		return false;
	}

	//ObjMgr Initialize
	if (!CObjMgr::GetInstance()->Initialize())
	{
		cout << "CObjMgr Initialize()" << endl;
		return false;
	}

	//TimeMgr Initialize
	if (!CTimeMgr::GetInstance()->InitTime())
	{
		cout << "CTimeMgr InitTime()" << endl;
		return false;
	}

	//TimerQ, TImerCS Initialize
	CTimerQMgr::GetInstance();

	//gameMgr
	CGameMgr::GetInstance();

	//clientMgr Initialize
	if (!(CClientMgr::GetInstance()->Initialize()))
	{
		cout << "CClientMgr Initialize error" << endl;
		return false;
	}

	//gameMgr Initailize
	if (!(CGameMgr::GetInstance()->Initialize()))
	{
		cout << "CGameMgr Intialize error" << endl;
		return false;
	}

	//thread Initialize
	if (!(MakeThread()))
	{
		cout << "MakeThread() error" << endl;
		return false;
	}

	return true;
}
bool CMainServer::Progress(void)
{
	//press VK_ESCAPE -> exit
	//if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	//{
	//	cout << "VK_ESCAPE -> exit" << endl;
	//	return false;
	//}

	return true;
}
bool CMainServer::Release(void)
{
	//clear thread
	if (!ClearThread())
	{
		cout << "ClearThread() error" << endl;
		return false;
	}

	//clear Navi
	if (!NaviDelete())
	{
		cout << "CMainServer Release NaviDelete() error!" << endl;
		return false;
	}

	//destroy mgr
	CTimerQMgr::DestroyInstance();
	CClientMgr::DestroyInstance();
	CTimeMgr::DestroyInstance();
	CGameMgr::DestroyInstance();
	CObjMgr::DestroyInstance();

	WSACleanup();

	g_hIOCP = NULL;
	return true;
}