#include "stdafx.h"
#include "ServerMgr.h"

#include "Player.h"
#include "Bot.h"
#include "Bead.h"
#include "StageUIFrame.h"

#include "Stage.h"

#include "SceneMgr.h"

IMPLEMENT_SINGLETON(CServerMgr)

CServerMgr::CServerMgr()
	: m_iClientID(-1)
	, m_iTeamIndex(-1)
	, m_dwTempPacketSize(0)
	, m_iSavePakcetSize(0)
	, m_bIsAllReady(false)
	, m_bIsChangeToRoomScene(false)
	, m_bIsChangeToInGameScene(false)
	, m_pMapObjList(nullptr)
	, m_iGameTime(PLAY_TIME)
	, m_iCountDown(GAME_START_COUNT)
	, m_eStageResultType(RESULT_END)
{
	InitializeCriticalSection(&m_ServerMgrCS);

	ZeroMemory(&m_tSendWSA, sizeof(m_tSendWSA));
	ZeroMemory(&m_tRecvWSA, sizeof(m_tRecvWSA));
	ZeroMemory(m_szSendBuf, sizeof(char) * MAX_BUFFER_SIZE);
	ZeroMemory(m_szRecvBuf, sizeof(char) * MAX_BUFFER_SIZE);
	ZeroMemory(m_szPacketBuf, sizeof(char) * MAX_BUFFER_SIZE);

	for (int i = 0; i < COMPLETE_END; ++i)
	{
		m_bIsSendInitData[i] = false;
	}

	ZeroMemory(m_bIsRedOrBlue, sizeof(bool) * MAX_USER);
	ZeroMemory(m_vPlayerPos, sizeof(D3DXVECTOR3) * MAX_USER);
	m_iBotCount = 0;
	ZeroMemory(m_vBotPos, sizeof(D3DXVECTOR3) * MAX_BOT_COUNT);
	ZeroMemory(m_vBeadPos, sizeof(D3DXVECTOR3) * BEAD_END);
}
CServerMgr::~CServerMgr()
{
	if (!Release())
	{
		cout << "Release() error" << endl;
	}

	DeleteCriticalSection(&m_ServerMgrCS);
}

bool CServerMgr::Initialize(char* _pIP, HWND _hWnd/* = NULL*/)
{
	//initialize wsa
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "WSAStartup() error" << endl;
		return false;
	}

	//socket()
	m_tMyClientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	if (m_tMyClientSocket == INVALID_SOCKET)
	{
		cout << "Sock err() : " << WSAGetLastError() << endl;
		return false;
	}

	// connect()
	SOCKADDR_IN tServerAddr;
	ZeroMemory(&tServerAddr, sizeof(tServerAddr));
	tServerAddr.sin_family = AF_INET;
	tServerAddr.sin_addr.s_addr = inet_addr(_pIP);
	tServerAddr.sin_port = htons(SERVER_PORT);

	int iResult = WSAConnect(m_tMyClientSocket, (sockaddr*)&tServerAddr, sizeof(tServerAddr), NULL, NULL, NULL, NULL);
	if (iResult == SOCKET_ERROR)
	{
		cout << "Connect err() : " << WSAGetLastError() << endl;
		return false;
	}

	WSAAsyncSelect(m_tMyClientSocket, _hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_tSendWSA.buf = m_szSendBuf;
	m_tSendWSA.len = MAX_BUFFER_SIZE;
	m_tRecvWSA.buf = m_szRecvBuf;
	m_tRecvWSA.len = MAX_BUFFER_SIZE;

	return true;
}
bool CServerMgr::Release(void)
{
	closesocket(m_tMyClientSocket);
	return true;
}

HRESULT CServerMgr::AddClient(const wstring& wstrKey, CLIENT* pClient)
{
	if (pClient)
	{
		unordered_map<wstring, list<CLIENT*>>::iterator iter = g_mapClient.find(wstrKey);

		if (iter == g_mapClient.end())
			g_mapClient[wstrKey] = list<CLIENT*>();

		g_mapClient[wstrKey].push_back(pClient);
	}
	else
		return E_FAIL;

	return S_OK;
}
HRESULT CServerMgr::DeleteClient(const wstring& wstrKey)
{
	unordered_map<wstring, list<CLIENT*>>::iterator	iterMap = g_mapClient.find(wstrKey);

	if (iterMap == g_mapClient.end())
		return E_FAIL;

	list<CLIENT*>::iterator iter = iterMap->second.begin();
	list<CLIENT*>::iterator iter_end = iterMap->second.end();

	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(*iter);
	}

	iterMap->second.clear();

	g_mapClient.erase(wstrKey);

	return S_OK;
}
HRESULT CServerMgr::DeleteClient(const int _iID)
{
	//id setting
	char szTempName[MAXPATH];
	char szTempNum[10];
	ZeroMemory(&szTempName, sizeof(szTempName));
	ZeroMemory(&szTempNum, sizeof(szTempNum));
	_itoa(_iID, szTempNum, 10);
	strcpy(szTempName, "CLIENT_");
	strcat(szTempName, szTempNum);
	// Multibyte To Unicode
	string strTemp(szTempName);
	int nLen = MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), NULL, NULL);
	wstring wstrTemp(nLen, 0);
	MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), &wstrTemp[0], nLen);

	unordered_map<wstring, list<CLIENT*>>::iterator	iterMap = g_mapClient.find(wstrTemp);

	if (iterMap == g_mapClient.end())
		return E_FAIL;

	list<CLIENT*>::iterator iter = iterMap->second.begin();
	list<CLIENT*>::iterator iter_end = iterMap->second.end();

	for (iter; iter != iter_end; ++iter)
	{
		::Safe_Delete(*iter);
	}

	iterMap->second.clear();

	g_mapClient.erase(wstrTemp);

	return S_OK;
}

CLIENT* CServerMgr::FindClient(const int _iID)
{
	//id setting
	char szTempName[MAXPATH];
	char szTempNum[10];
	ZeroMemory(&szTempName, sizeof(szTempName));
	ZeroMemory(&szTempNum, sizeof(szTempNum));
	_itoa(_iID, szTempNum, 10);
	strcpy(szTempName, "CLIENT_");
	strcat(szTempName, szTempNum);
	// Multibyte To Unicode
	string strTemp(szTempName);
	int nLen = MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), NULL, NULL);
	wstring wstrTemp(nLen, 0);
	MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), &wstrTemp[0], nLen);

	unordered_map<wstring, list<CLIENT*>>::iterator	iterMap = g_mapClient.find(wstrTemp);

	if (iterMap == g_mapClient.end())
	{
		return nullptr;
	}

	return (*(iterMap->second.begin()));
}

void CServerMgr::SetChangeScene(int _iType, bool _bIsChange)
{
	switch (_iType)
	{
	case 0:	//room
		m_bIsChangeToRoomScene = _bIsChange;
		break;
	case 1:	//InGame
		m_bIsChangeToInGameScene = _bIsChange;
		break;
	}
}
void CServerMgr::ClearSendInitData(void)
{
	ZeroMemory(m_bIsRedOrBlue, sizeof(bool) * MAX_USER);
	ZeroMemory(m_vPlayerPos, sizeof(D3DXVECTOR3) * MAX_USER);
	m_iBotCount = 0;
	ZeroMemory(m_vBotPos, sizeof(D3DXVECTOR3) * MAX_BOT_COUNT);
	ZeroMemory(m_vBeadPos, sizeof(D3DXVECTOR3) * BEAD_END);

	ClearSendInitDataFlag();
}
void CServerMgr::ClearSendInitDataFlag(void)
{
	for (int i = 0; i < COMPLETE_END; ++i)
	{
		m_bIsSendInitData[i] = false;
	}
}
bool CServerMgr::CheckAllSendInitData(void)
{
	for (int i = 0; i < COMPLETE_END; ++i)
	{
		if (!m_bIsSendInitData[i])
			return false;
	}
	return true;
}

void CServerMgr::ReadPacket(SOCKET _socket)
{
	DWORD dwIOByte = 0;
	DWORD dwIOFlag = 0;
	int iResult = WSARecv(_socket, &m_tRecvWSA, 1, &dwIOByte, &dwIOFlag, NULL, NULL);
	if (iResult)
	{
		cout << "Recv Error!!! " << WSAGetLastError() << endl;
		return;
	}

	BYTE* pTemp = reinterpret_cast<BYTE*>(m_szRecvBuf);
	while (dwIOByte != 0)
	{
		if (m_dwTempPacketSize == 0)
		{
			m_dwTempPacketSize = reinterpret_cast<HEADER*>(pTemp)->size;
		}
		if (dwIOByte + m_iSavePakcetSize >= m_dwTempPacketSize)
		{
			memcpy(m_szPacketBuf + m_iSavePakcetSize, pTemp, m_dwTempPacketSize - m_iSavePakcetSize);
			ProcessPacket(m_szPacketBuf);
			pTemp += m_dwTempPacketSize - m_iSavePakcetSize;
			dwIOByte -= m_dwTempPacketSize - m_iSavePakcetSize;
			m_dwTempPacketSize = 0;
			m_iSavePakcetSize = 0;
		}
		else
		{
			memcpy(m_szPacketBuf + m_iSavePakcetSize, pTemp, dwIOByte);
			m_iSavePakcetSize += dwIOByte;
			dwIOByte = 0;
		}
	}
}
void CServerMgr::SendPacket(char* _pPacket)
{
	m_tSendWSA.buf = _pPacket;
	m_tSendWSA.len = reinterpret_cast<HEADER*>(_pPacket)->size;

	DWORD dwIOByte = 0;
	DWORD dwFlag = 0;
	int iResult = WSASend(m_tMyClientSocket, &(m_tSendWSA), 1, &dwIOByte, dwFlag, nullptr, nullptr);
	if (iResult)
	{
		cout << "Packet Sending error : " << WSAGetLastError() << endl;
		ZeroMemory(&m_tSendWSA, sizeof(m_tSendWSA));
	}
}

void CServerMgr::ProcessPacket(char* _pPacket)
{
	EnterCriticalSection(&m_ServerMgrCS);
	HEADER* pHeader = reinterpret_cast<HEADER*>(_pPacket);

	switch (pHeader->type)
	{
	//room
	case SC_SYNC_ROOM:
	{
		SC_SyncRoom* pPacket = reinterpret_cast<SC_SyncRoom*>(_pPacket);

		//new client
		CLIENT* pClient = new CLIENT;
		pClient->m_iRoomIndex = pPacket->m_iTeamIndex;
		pClient->m_bIsHost = pPacket->m_bIsHost;
		pClient->m_bIsReady = pPacket->m_bIsReady;

		char szTemp[10];
		ZeroMemory(&szTemp, sizeof(szTemp));
		_itoa(pPacket->m_iID, szTemp, 10);
		strcpy(pClient->m_szName, "CLIENT_");
		strcat(pClient->m_szName, szTemp);

		// Multibyte To Unicode
		string strTemp(pClient->m_szName);
		int nLen = MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), NULL, NULL);
		wstring wstrTemp(nLen, 0);
		MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), &wstrTemp[0], nLen);

		FAILED_CHECK_RETURN(AddClient(wstrTemp, pClient), );
		break;
	}
	case SC_CONNECT_ROOM:
	{
		SC_ConnectRoom* pPacket = reinterpret_cast<SC_ConnectRoom*>(_pPacket);
		
		//game setting
		g_GameInfo.bMode = pPacket->m_bIsGameMode;
		g_GameInfo.bMapType = pPacket->m_bIsMapType;
		g_GameInfo.iBotCnt = pPacket->m_iBotCount;

		//serverMgr setting
		m_iClientID = pPacket->m_iID;
		m_bIsHost = pPacket->m_bIsHost;
		m_iTeamIndex = pPacket->m_iTeamIndex;

		//new client
		CLIENT* pClient = new CLIENT;
		pClient->m_iRoomIndex = pPacket->m_iTeamIndex;
		pClient->m_bIsHost = pPacket->m_bIsHost;
		pClient->m_bIsReady = false;

		char szTemp[10];
		ZeroMemory(&szTemp, sizeof(szTemp));
		_itoa(pPacket->m_iID, szTemp, 10);
		strcpy(pClient->m_szName, "CLIENT_");
		strcat(pClient->m_szName, szTemp);

		// Multibyte To Unicode
		string strTemp(pClient->m_szName);
		int nLen = MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), NULL, NULL);
		wstring wstrTemp(nLen, 0);
		MultiByteToWideChar(CP_ACP, 0, &strTemp[0], strTemp.size(), &wstrTemp[0], nLen);

		FAILED_CHECK_RETURN(AddClient(wstrTemp, pClient), );
		
		m_bIsChangeToRoomScene = true;
		break;
	}
	case SC_CLIENT_READY:
	{
		SC_Ready* pPacket = reinterpret_cast<SC_Ready*>(_pPacket);

		CLIENT* pClient = FindClient(pPacket->m_iID);
		pClient->m_bIsReady = pPacket->m_bReady;
		m_bIsAllReady = pPacket->m_bIsAll;
		break;
	}
	case SC_CLIENT_CHANGE:
	{
		SC_Change* pPacket = reinterpret_cast<SC_Change*>(_pPacket);

		CLIENT* pClient = FindClient(pPacket->m_iID);
		pClient->m_iRoomIndex = pPacket->m_iTeamIndex;

		if (pPacket->m_iID == m_iClientID)
		{
			m_iTeamIndex = pPacket->m_iTeamIndex;
		}
		break;
	}
	case SC_OPTION_CHANGE:
	{
		SC_OptionChange* pPacket = reinterpret_cast<SC_OptionChange*>(_pPacket);
		g_GameInfo.bMode = pPacket->m_bGameMode;
		g_GameInfo.iBotCnt = pPacket->m_iBotCount;
		g_GameInfo.bMapType = pPacket->m_bIsMapType;
		break;
	}
	case SC_CHAT_ROOM:
	{
		SC_ChatRoom* pPacket = reinterpret_cast<SC_ChatRoom*>(_pPacket);
		//수정필요
		//출력하는 부분 추가
		break;
	}
	case SC_ALL_UNREADY:
	{
		SC_AllUnready* pPacket = reinterpret_cast<SC_AllUnready*>(_pPacket);
		m_bIsAllReady = false;
		unordered_map<wstring, list<CLIENT*>>::iterator	iterMap = g_mapClient.begin();
		unordered_map<wstring, list<CLIENT*>>::iterator	iterMap_end = g_mapClient.end();
		for (iterMap; iterMap != iterMap_end; ++iterMap)
		{
			iterMap->second.front()->m_bIsReady = false;
		}
		break;
	}
	//room & game
	case SC_REMOVE_CLIENT:
	{
		SC_RemoveClient* pPacket = reinterpret_cast<SC_RemoveClient*>(_pPacket);
		//delete
		DeleteClient(pPacket->m_iID);
		//host change
		if (pPacket->m_iHostID == m_iClientID)
		{
			m_bIsHost = true;
		}
		CLIENT* pClient = FindClient(pPacket->m_iHostID);
		pClient->m_bIsHost = true;
		break;
	}
	//game
	case SC_SERVER_GAME_TIME:
	{
		SC_ServerGameTime* pPacket = reinterpret_cast<SC_ServerGameTime*>(_pPacket);
		m_iGameTime = pPacket->m_iTime;
		if (m_iGameTime < 0)
		{
			m_iGameTime = 0;
		}
		break;
	}
	case SC_CHAT_GAME:
	{
		SC_ChatGame* pPacket = reinterpret_cast<SC_ChatGame*>(_pPacket);
		//수정필요
		//출력하는 부분 추가
		break;
	}
	case SC_INIT_PLAYER:
	{
		SC_InitPlayer* pPacket = reinterpret_cast<SC_InitPlayer*>(_pPacket);
		memcpy(m_bIsRedOrBlue, pPacket->m_bIsRedOrBlue, sizeof(bool) * MAX_USER);
		memcpy(m_vPlayerPos, pPacket->m_vPlayerPos, sizeof(D3DXVECTOR3) * MAX_USER);

		m_bIsSendInitData[COMPLETE_PLAYER] = true;
		if (CheckAllSendInitData())
		{
			m_bIsChangeToInGameScene = true;
		}
		break;
	}
	case SC_INIT_BOT:
	{
		SC_InitBot* pPacket = reinterpret_cast<SC_InitBot*>(_pPacket);
		memcpy(m_vBotPos, pPacket->m_vBotPos, sizeof(D3DXVECTOR3) * MAX_BOT_COUNT);
		m_iBotCount = pPacket->m_iBotCount;
		if (m_iBotCount != g_GameInfo.iBotCnt)
		{
			cout << "Not Same Bot's Value error" << endl;
			g_GameInfo.iBotCnt = m_iBotCount;
		}

		m_bIsSendInitData[COMPLETE_BOT] = true;
		if (CheckAllSendInitData())
		{
			m_bIsChangeToInGameScene = true;
		}
		break;
	}
	case SC_INIT_BEAD:
	{
		SC_InitBead* pPacket = reinterpret_cast<SC_InitBead*>(_pPacket);
		memcpy(m_vBeadPos, pPacket->m_vBeadPos, sizeof(D3DXVECTOR3) * BEAD_END);
		
		m_bIsSendInitData[COMPLETE_BEAD] = true;
		if (CheckAllSendInitData())
		{
			m_bIsChangeToInGameScene = true;
		}
		break;
	}
	case SC_CHANGE_STATE_PLAYER:
	{
		SC_ChangeStatePlayer* pPacket = reinterpret_cast<SC_ChangeStatePlayer*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}

		if (pPacket->m_iPlayerID != m_iClientID)
		{
			list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
			list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
			for (iter; iter != iter_end; ++iter)
			{
				int iPlayerID = reinterpret_cast<CPlayer*>(*iter)->GetPlayerID();
				if (iPlayerID == pPacket->m_iPlayerID)
				{
					reinterpret_cast<CPlayer*>(*iter)->SetPlayerKeyValue(pPacket->m_iPlayerKeyState);
					(*iter)->SetPos(pPacket->m_vPlayerPos);
					(*iter)->SetAngleY(pPacket->m_fPlayerAngleY);
					break;
				}
			}
		}
		else if(pPacket->m_iPlayerID == m_iClientID)
		{
			list<CObj*>::iterator iter = (*m_pMapObjList)[L"Player"].begin();
			list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Player"].end();
			for (iter; iter != iter_end; ++iter)
			{
				int iPlayerID = reinterpret_cast<CPlayer*>(*iter)->GetPlayerID();
				if (iPlayerID == pPacket->m_iPlayerID)
				{
					reinterpret_cast<CPlayer*>(*iter)->SetPlayerKeyValue(pPacket->m_iPlayerKeyState);
					(*iter)->SetPos(pPacket->m_vPlayerPos);
					(*iter)->SetAngleY(pPacket->m_fPlayerAngleY);
					break;
				}
			}
		}
		break;
	}
	case SC_CHANGE_STATE_BOT:
	{
		SC_ChangeStateBot* pPacket = reinterpret_cast<SC_ChangeStateBot*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}

		list<CObj*>::iterator iter = (*m_pMapObjList)[L"Bot"].begin();
		list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Bot"].end();
		for (iter; iter != iter_end; ++iter)
		{
			if (pPacket->m_iBotID == reinterpret_cast<CBot*>(*iter)->GetBotID())
			{
				(*iter)->SetPos(pPacket->m_vBotPos);
				if (pPacket->m_iBotState == OBJ_STATE_IDEL)
				{
					(*iter)->SetAngleY(pPacket->m_fAngleY);
				}
				reinterpret_cast<CBot*>(*iter)->StateSetting((CObj::OBJSTATE)pPacket->m_iBotState, pPacket->m_iBotTimeCount, pPacket->m_fAngleY);
				break;
			}
		}
		break;
	}
	case SC_CHANGE_STATE_BEAD:
	{
		SC_ChangeStateBead* pPacket = reinterpret_cast<SC_ChangeStateBead*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}

		list<CObj*>::iterator iter = (*m_pMapObjList)[L"Bead"].begin();
		list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Bead"].end();
		for (iter; iter != iter_end; ++iter)
		{
			int iBeadID = reinterpret_cast<CBead*>(*iter)->GetBeadType();
			if (iBeadID == pPacket->m_iBeadID)
			{
				reinterpret_cast<CBead*>(*iter)->SetOwnerID(pPacket->m_iOwnerPlayerID);
				break;
			}
		}
		break;
	}
	case SC_SYNC_PLAYER:
	{
		SC_SyncPlayer* pPacket = reinterpret_cast<SC_SyncPlayer*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (i == m_iClientID)
			{
				((*m_pMapObjList)[L"Player"].front())->SetAngleY(pPacket->m_fPlayerAngleY[i]);
				((*m_pMapObjList)[L"Player"].front())->SetPos(pPacket->m_vPlayerPos[i]);
			}
			else if(pPacket->m_vPlayerPos[i].y < 0.5f)
			{
				list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
				list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
				for (iter; iter != iter_end; ++iter)
				{
					if (i == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
					{
						(*iter)->SetAngleY(pPacket->m_fPlayerAngleY[i]);
						(*iter)->SetPos(pPacket->m_vPlayerPos[i]);
						continue;
					}
				}
			}
		}
		break;
	}
	case SC_SYNC_BOT:
	{
		SC_SyncBot* pPacket = reinterpret_cast<SC_SyncBot*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}

		list<CObj*>::iterator iter = (*m_pMapObjList)[L"Bot"].begin();
		list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Bot"].end();
		for (iter; iter != iter_end; ++iter)
		{
			int iBotID = reinterpret_cast<CBot*>(*iter)->GetBotID();

			(*iter)->SetAngleY(pPacket->m_fBotAngleY[iBotID]);
			(*iter)->SetPos(pPacket->m_vBotPos[iBotID]);
		}
		break;
	}
	case SC_CHECK_COLLISION:
	{
		SC_CheckCollision* pPacket = reinterpret_cast<SC_CheckCollision*>(_pPacket);
		if (m_pMapObjList == nullptr)
		{
			cout << "m_pMapObjList nullptr" << endl;
			break;
		}
		
		//Player Player
		if (pPacket->m_iType == 1)
		{
			//add kill log
			CObj* pStageUI = ((CStage*)CSceneMgr::GetInstance()->GetScene())->GetStageUI();
			//내가 어떤 플레이어를 죽인 사람이 아니다
			if (pPacket->m_iMainID != m_iClientID)
			{
				wstring wstrTemp = L"";
				list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
				list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
				//대상 구분하기
				for (iter; iter != iter_end; ++iter)
				{
					if (pPacket->m_iMainID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
					{
						if (reinterpret_cast<CPlayer*>(*iter)->GetPlayerIsMyTeam() == true) wstrTemp += L"MyTeam";
						else wstrTemp += L"Enemy";
						break;
					}
				}

				//타겟 구분하기
				if (pPacket->m_iTargetID != m_iClientID)
				{
					list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
					list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
					for (iter; iter != iter_end; ++iter)
					{
						if (pPacket->m_iTargetID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
						{
							if (reinterpret_cast<CPlayer*>(*iter)->GetPlayerIsMyTeam() == true) ((CStageUIFrame*)pStageUI)->AddKillLog(wstrTemp, L"MyTeam");
							else ((CStageUIFrame*)pStageUI)->AddKillLog(wstrTemp, L"Enemy");
							break;
						}
					}
				}
				else if (pPacket->m_iTargetID == m_iClientID)
				{
					((CStageUIFrame*)pStageUI)->AddKillLog(wstrTemp, L"Player");
				}
			}
			//내가 어떤 플레이어를 죽인 사람이다
			else if (pPacket->m_iMainID == m_iClientID)
			{
				if (pPacket->m_iTargetID != m_iClientID)
				{
					//타켓 구분하기
					list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
					list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
					for (iter; iter != iter_end; ++iter)
					{
						if (pPacket->m_iTargetID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
						{
							if (reinterpret_cast<CPlayer*>(*iter)->GetPlayerIsMyTeam() == true) ((CStageUIFrame*)pStageUI)->AddKillLog(L"Player", L"MyTeam");
							else ((CStageUIFrame*)pStageUI)->AddKillLog(L"Player", L"Enemy");
							break;
						}
					}
				}
				else if (pPacket->m_iTargetID == m_iClientID)
				{
					((CStageUIFrame*)pStageUI)->AddKillLog(L"Player", L"Player");
				}
			}
			pStageUI = nullptr;


			if (pPacket->m_iTargetID == m_iClientID)
			{
				((*m_pMapObjList)[L"Player"].front())->SetObjState(CObj::OBJ_STATE_DIE);
			}
			else if (pPacket->m_iTargetID != m_iClientID)
			{
				list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
				list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
				for (iter; iter != iter_end; ++iter)
				{
					if (pPacket->m_iTargetID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
					{
						(*iter)->SetObjState(CObj::OBJ_STATE_DIE);
						break;
					}
				}
			}
		}
		//Player Bot
		else if (pPacket->m_iType == 2)
		{
			//add kill log
			CObj* pStageUI = ((CStage*)CSceneMgr::GetInstance()->GetScene())->GetStageUI();
			wstring wstrName = L"Bot_";
			wstring wstrNum = to_wstring(pPacket->m_iTargetID);
			wstrName += wstrNum;

			//봇을 죽인사람이 내가 아니다
			if (pPacket->m_iMainID != m_iClientID)
			{
				list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
				list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
				for (iter; iter != iter_end; ++iter)
				{
					if (pPacket->m_iMainID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
					{
						if (reinterpret_cast<CPlayer*>(*iter)->GetPlayerIsMyTeam() == true) ((CStageUIFrame*)pStageUI)->AddKillLog(L"MyTeam", wstrName);
						else ((CStageUIFrame*)pStageUI)->AddKillLog(L"Enemy", wstrName);
						break;
					}
				}
			}
			//봇을 죽인사람이 나다
			else if (pPacket->m_iMainID == m_iClientID)
			{
				((CStageUIFrame*)pStageUI)->AddKillLog(L"Player", wstrName);
			}
			pStageUI = nullptr;

			list<CObj*>::iterator iter = (*m_pMapObjList)[L"Bot"].begin();
			list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Bot"].end();
			for (iter; iter != iter_end; ++iter)
			{
				int iBotID = reinterpret_cast<CBot*>(*iter)->GetBotID();
				if (iBotID == pPacket->m_iTargetID)
				{
					//Bot's state change OBJ_STATE_DIE
					reinterpret_cast<CBot*>(*iter)->SetObjState(CObj::OBJ_STATE_DIE);
					break;
				}
			}
		}
		//Player Bead
		else if (pPacket->m_iType == 3)
		{
			//add kill log
			CObj* pStageUI = ((CStage*)CSceneMgr::GetInstance()->GetScene())->GetStageUI();
			wstring wstrName = L"Bead_";
			switch (pPacket->m_iTargetID)
			{
			case BEAD_C:
			{
				wstring wstrNum = L"C";
				wstrName += wstrNum;
				break;
			}
			case BEAD_L:
			{
				wstring wstrNum = L"L";
				wstrName += wstrNum;
				break;
			}
			case BEAD_O:
			{
				wstring wstrNum = L"O";
				wstrName += wstrNum;
				break;
			}
			case BEAD_N:
			{
				wstring wstrNum = L"N";
				wstrName += wstrNum;
				break;
			}
			case BEAD_E:
			{
				wstring wstrNum = L"E";
				wstrName += wstrNum;
				break;
			}
			default:
				cout << "Unknown Bead Type!" << endl;
				break;
			}

			//내가 아닌 사람이 먹음
			if (pPacket->m_iMainID != m_iClientID)
			{
				list<CObj*>::iterator iter = (*m_pMapObjList)[L"Enemy"].begin();
				list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Enemy"].end();
				for (iter; iter != iter_end; ++iter)
				{
					if (pPacket->m_iMainID == reinterpret_cast<CPlayer*>(*iter)->GetPlayerID())
					{
						if (reinterpret_cast<CPlayer*>(*iter)->GetPlayerIsMyTeam() == true) ((CStageUIFrame*)pStageUI)->AddKillLog(L"MyTeam", wstrName);
						else ((CStageUIFrame*)pStageUI)->AddKillLog(L"Enemy", wstrName);
						break;
					}
				}
			}
			//내가 먹음
			else if (pPacket->m_iMainID == m_iClientID)
			{
				((CStageUIFrame*)pStageUI)->AddKillLog(L"Player", wstrName);
			}

			pStageUI = nullptr;

			//change bead's state
			list<CObj*>::iterator iter = (*m_pMapObjList)[L"Bead"].begin();
			list<CObj*>::iterator iter_end = (*m_pMapObjList)[L"Bead"].end();
			for (iter; iter != iter_end; ++iter)
			{
				if (reinterpret_cast<CBead*>(*iter)->GetBeadType() == pPacket->m_iTargetID)
				{
					reinterpret_cast<CBead*>(*iter)->SetOwnerID(pPacket->m_iMainID);
					break;
				}
			}
		}
		break;
	}
	case SC_COUNT_DOWN:
	{
		SC_CountDown* pPacket = reinterpret_cast<SC_CountDown*>(_pPacket);
		m_iCountDown = pPacket->m_iCountDownNum;
		if (m_iCountDown < 0)
		{
			m_iCountDown = 0;
			break;
		}
		break;
	}
	case SC_GAME_STATE:
	{
		SC_GameState* pPacket = reinterpret_cast<SC_GameState*>(_pPacket);
		if (pPacket->m_iGameState == STAGE_READY)
		{
			m_iGameTime = PLAY_TIME;
			m_iCountDown = GAME_START_COUNT;

			m_bIsChangeToRoomScene = true;
		}
		else if (pPacket->m_iGameState == STAGE_RESULT)
		{
			if (pPacket->m_bGameWinOrLose == true) m_eStageResultType = RESULT_WIN;
			else if (pPacket->m_bGameWinOrLose == false) m_eStageResultType = RESULT_DEFEAT;
		}
		break;
	}
	default:
		cout << "unknown packet type!" << endl;
		break;
	}
	LeaveCriticalSection(&m_ServerMgrCS);
}