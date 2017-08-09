#include "stdafx.h"
#include "MainServer.h"

#include "NaviMgr.h"
#include "PacketMgr.h"
#include "TimerQMgr.h"
#include "GameMgr.h"
#include "ObjMgr.h"

#include "WorkerThread.h"
#include "ReservationThread.h"
#include "LogicThread.h"

CMainServer::CMainServer()
	: m_bIsShutDownServer(false)
	, m_pReservationThread(nullptr)
	, m_pLogicThread(nullptr)
	, m_iWorkerThreadCount(UNKNOWN_VALUE)
	, m_iPortNumber(UNKNOWN_VALUE)
{
	ZeroMemory(m_szIPAddress, sizeof(m_szIPAddress));

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

bool CMainServer::MakeManager(void)
{
	//objMgr Initialize
	if (!CObjMgr::GetInstance()->Initialize())
	{
		cout << "CObjMgr Initialize()" << endl;
		return false;
	}

	//TimerQ, Initialize
	CTimerQMgr::GetInstance();

	//gameMgr
	CGameMgr::GetInstance();

	//pakcetMgr Initialize
	CPacketMgr::GetInstance();

	//gameMgr Initailize
	if (!(CGameMgr::GetInstance()->Initialize()))
	{
		cout << "CGameMgr Intialize error" << endl;
		return false;
	}

	return true;
}
bool CMainServer::DestroyManager(void)
{
	//destroy mgr
	CTimerQMgr::DestroyInstance();
	CPacketMgr::DestroyInstance();
	CGameMgr::DestroyInstance();
	CObjMgr::DestroyInstance();

	return true;
}
bool CMainServer::MakeThread(void)
{
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

	//worker thread
	for (int i = 0; i < m_iWorkerThreadCount; ++i)
	{
		m_WorkerThreadVector.push_back(new thread{ WorkerThread });
	}

	return true;
}
bool CMainServer::ClearThread(void)
{
	m_pReservationThread->join();
	delete m_pReservationThread;
	m_pReservationThread = nullptr;

	m_pLogicThread->join();
	delete m_pLogicThread;
	m_pLogicThread = nullptr;

	for (int i = 0; i < m_iWorkerThreadCount; ++i)
	{
		m_WorkerThreadVector[i]->join();
		delete m_WorkerThreadVector[i];
		m_WorkerThreadVector[i] = nullptr;
	}

	return true;
}

bool CMainServer::LoadServerInfo(void)
{
	FILE* pFp = fopen("../Data/Information/ServerInformation.txt", "r");
	if (pFp == nullptr) return false;

	char szReadBuffer[SHORT_BUFFER]{};
	while (!feof(pFp))
	{
		ZeroMemory(szReadBuffer, sizeof(szReadBuffer));

		char szTitle[VERY_SHORT_BUFFER];
		char szValue[VERY_SHORT_BUFFER];
		ZeroMemory(szTitle, sizeof(szTitle));
		ZeroMemory(szValue, sizeof(szValue));

		fgets(szReadBuffer, sizeof(szReadBuffer), pFp);
		GetInfoInData(szReadBuffer, szTitle, szValue);

		if (stricmp(szTitle, "PORT") == 0)
		{
			m_iPortNumber = atoi(szValue);

			cout << "*** Complete Load Port Number ***" << endl;
		}
		else if (stricmp(szTitle, "WORKER_THREAD_COUNT") == 0)
		{
			int iValue = atoi(szValue);
			if (iValue == UNKNOWN_VALUE)
			{
				//get systeminfo
				GetSystemInfo(&m_tSystemInfo);
				m_iWorkerThreadCount = m_tSystemInfo.dwNumberOfProcessors;
			}
			else m_iWorkerThreadCount = iValue;

			cout << "*** Complete Load Worker Thread Count ***" << endl;
		}
		else if (stricmp(szTitle, "CONNECTION_RECV_BUFFER") == 0) { atoi(szValue); }
		else if (stricmp(szTitle, "IP_ADDRESS") == 0) 
		{
			strcpy(m_szIPAddress, szValue);

			cout << "*** Complete IP Address ***" << endl;
		}
	}

	fclose(pFp);
	return true;
}
bool CMainServer::GetInfoInData(char* _pInfoStr, char* _pTitle, char* _pValue)
{
	if (_pTitle == nullptr || _pInfoStr == nullptr) return false;

	char* pTempStr = strtok(_pInfoStr, ":");
	strcpy(_pTitle, pTempStr);

	pTempStr = strtok(nullptr, ":");
	strcpy(_pValue, pTempStr);

	return true;
}

bool CMainServer::LoadNavi(void)
{
	HANDLE hFile;

	//kjk plaza
	hFile = CreateFile(L"../Data/Navi/KJKPlazaNavi.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwByte = 0;
	int iSize = 0;
	int iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);
	CNaviMgr::GetInstance()->ReserveCellContainerSize(iCount, MAP_KJK_PLAZA);

	D3DXMATRIX matRotX;
	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, (float)D3DXToRadian(-90.f));

	while (iSize < iCount)
	{
		D3DXVECTOR3 vPoint1, vPoint2, vPoint3;

		ReadFile(hFile, &vPoint1, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint1, &vPoint1, &matRotX);
		ReadFile(hFile, &vPoint2, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint2, &vPoint2, &matRotX);
		ReadFile(hFile, &vPoint3, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint3, &vPoint3, &matRotX);

		CNaviMgr::GetInstance()->AddCell(&vPoint1, &vPoint2, &vPoint3, MAP_KJK_PLAZA);
		++iSize;
	}


	//kjk pool
	hFile = CreateFile(L"../Data/Navi/KJKPoolNavi.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	dwByte = 0;
	iSize = 0;
	iCount = 0;

	ReadFile(hFile, &iCount, sizeof(int), &dwByte, NULL);
	CNaviMgr::GetInstance()->ReserveCellContainerSize(iCount, MAP_KJK_POOL);

	D3DXMatrixIdentity(&matRotX);
	D3DXMatrixRotationX(&matRotX, (float)D3DXToRadian(-90.f));

	while (iSize < iCount)
	{
		D3DXVECTOR3 vPoint1, vPoint2, vPoint3;

		ReadFile(hFile, &vPoint1, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint1, &vPoint1, &matRotX);
		ReadFile(hFile, &vPoint2, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint2, &vPoint2, &matRotX);
		ReadFile(hFile, &vPoint3, sizeof(D3DXVECTOR3), &dwByte, NULL);
		D3DXVec3TransformNormal(&vPoint3, &vPoint3, &matRotX);

		CNaviMgr::GetInstance()->AddCell(&vPoint1, &vPoint2, &vPoint3, MAP_KJK_POOL);
		++iSize;
	}


	//loading end
	CloseHandle(hFile);
	CNaviMgr::GetInstance()->LinkCell();

	return true;
}
bool CMainServer::DeleteNavi(void)
{
	CNaviMgr::DestroyInstance();
	return true;
}

bool CMainServer::Initialize(void)
{
	//load server data in txt file
	if (!LoadServerInfo())
	{
		cout << "CMainServer LoadServerInfo() error" << endl;
	}
	else cout << "*** Complete Load ServerInfo ***" << endl;

	//network Initialize
	SOCKADDR_IN tListenAddr;
	ZeroMemory(&tListenAddr, sizeof(tListenAddr));
	tListenAddr.sin_family		= AF_INET;
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tListenAddr.sin_port		= htons(m_iPortNumber);

	//use tcp socket
	if (!CAcceptModule::Initialize(SOCK_STREAM, IPPROTO_IP, tListenAddr))
	{
		cout << "CMainServer CAcceptModule::Initialize error!" << endl;
		return false;
	}

	//IOCP Initiailize
	if (g_hIOCP == NULL)
	{
		g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	}

	//navi loading
	if (!LoadNavi())
	{
		cout << "CMainServer Initialize LoadNavi() error" << endl;
		return false;
	}
	else cout << "*** Complete Load Navi ***" << endl;

	//manager Initialize
	if (!(MakeManager()))
	{
		cout << "MakeManager() error" << endl;
		return false;
	}
	else cout << "*** Complete Make Manager ***" << endl;

	//thread Initialize
	if (!(MakeThread()))
	{
		cout << "MakeThread() error" << endl;
		return false;
	}
	else cout << "*** Complete Make Thread ***" << endl;

	return true;
}
bool CMainServer::Progress(void)
{
	//press VK_ESCAPE -> exit
	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		g_GlobalMutex.lock();
		m_bIsShutDownServer = true;
		g_GlobalMutex.unlock();

		cout << "====================" << endl;
		cout << "  Shut Down Server" << endl;
		cout << "====================" << endl;

		return false;
	}

	//accpet
	OnAccept();

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
	else cout << "*** Complete Clear Thread ***" << endl;

	//Destroy Manager
	if (!DestroyManager())
	{
		cout << "DestroyManager() error" << endl;
		return false;
	}
	else cout << "*** Complete Destroy Manager ***" << endl;

	//delete Navi
	if (!DeleteNavi())
	{
		cout << "CMainServer Release NaviDelete() error!" << endl;
		return false;
	}
	else cout << "*** Complete Delete Navi ***" << endl;

	WSACleanup();

	g_hIOCP = NULL;
	return true;
}