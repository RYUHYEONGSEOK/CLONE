#include "stdafx.h"
#include "AcceptModule.h"

#include "GameMgr.h"
#include "ObjMgr.h"
#include "PacketMgr.h"

#include "Player.h"

CAcceptModule::CAcceptModule()
{
	m_tListenSocket = INVALID_SOCKET;
}
CAcceptModule::~CAcceptModule()
{
	Release();
}

bool CAcceptModule::Initialize(int _iType, int _iProtocol, SOCKADDR_IN _tAddr, int _iWaitSocket/* = SOMAXCONN*/)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_tAddr = _tAddr;
	m_tListenSocket = WSASocket(AF_INET, _iType, _iProtocol, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (m_tListenSocket == INVALID_SOCKET)
	{
		return false;
	}
	if (::bind(m_tListenSocket, (SOCKADDR*)&m_tAddr, sizeof(m_tAddr)) == SOCKET_ERROR)
	{
		return false;
	}
	if (listen(m_tListenSocket, _iWaitSocket) == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}
void CAcceptModule::OnAccept(void)
{
	int iAddrSize = sizeof(m_tClientAddr);

	SOCKET tClientSocket = WSAAccept(m_tListenSocket, reinterpret_cast<sockaddr*>(&m_tClientAddr), &iAddrSize, NULL, NULL);
	if (tClientSocket == INVALID_SOCKET)
	{
		cout << "client accept() error" << endl;
		exit(-1);
	}

	//max client check
	int iClientID = CObjMgr::GetInstance()->GetClientID();
	if (iClientID == UNKNOWN_VALUE)
	{
		cout << "player count over" << endl;
		return;
	}

	//game state check
	if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
	{
		cout << "game state is not ROOM" << endl;
		return;
	}

	//third : 완료된 IO정보의 전달을 위한 매개변수
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(tClientSocket), g_hIOCP, iClientID, 0);


	//accept client setting
	g_GlobalMutex.lock();
	CObj* pPlayer = new CPlayer(iClientID);
	reinterpret_cast<CPlayer*>(pPlayer)->SetSocket(tClientSocket);
	g_GlobalMutex.unlock();

	//socket state set recv's state.
	DWORD dwRecvFlag = 0;
	int iResult = WSARecv(reinterpret_cast<CPlayer*>(pPlayer)->GetSocket(),
		&(reinterpret_cast<CPlayer*>(pPlayer)->GetOverLapped()->wsaBuf), 1, nullptr, &dwRecvFlag,
		&(reinterpret_cast<CPlayer*>(pPlayer)->GetOverLapped()->m_tOverLapped), nullptr);
	if (iResult == SOCKET_ERROR)
	{
		int iErrCode = WSAGetLastError();
		if (iErrCode != WSA_IO_PENDING)
		{
			cout << "Accept(WSARecv) error" << endl;
			return;
		}
	}


	//insert player's information
	CObjMgr::GetInstance()->ConnectPlayer(iClientID, reinterpret_cast<CPlayer*>(pPlayer));
	cout << "★ Player ID : " << iClientID << "(Connect IP : " << inet_ntoa(m_tClientAddr.sin_addr) << ") ★" << endl;


	g_GlobalMutex.lock();
	//Host check
	int iHostID = CGameMgr::GetInstance()->GetHostID();
	//RoomIndex check
	int iRoomIndex = CGameMgr::GetInstance()->GetEmptyRoomIndex(iClientID);
	g_GlobalMutex.unlock();

	//id setting
	g_GlobalMutex.lock();
	m_tConnectionPacket.m_iID = iClientID;
	//roomindex setting
	reinterpret_cast<CPlayer*>(pPlayer)->SetRoomIndex(iRoomIndex);
	m_tConnectionPacket.m_iTeamIndex = iRoomIndex;
	//room setting
	m_tConnectionPacket.m_iBotCount = CGameMgr::GetInstance()->GetBotCount();
	//maptype setting
	switch (CGameMgr::GetInstance()->GetMapType())
	{
	case MAP_KJK_PLAZA: m_tConnectionPacket.m_bIsMapType = false; break;
	case MAP_KJK_POOL: m_tConnectionPacket.m_bIsMapType = true; break;
	}
	//GameMode setting
	switch (CGameMgr::GetInstance()->GetGameModeType())
	{
	case MODE_SOLO: m_tConnectionPacket.m_bIsGameMode = false; break;
	case MODE_TEAM: m_tConnectionPacket.m_bIsGameMode = true; break;
	}
	//host id setting
	if (iHostID == UNKNOWN_VALUE)
	{
		reinterpret_cast<CPlayer*>(pPlayer)->SetIsHost(true);
		m_tConnectionPacket.m_bIsHost = true;
		g_GlobalMutex.unlock();

		CGameMgr::GetInstance()->SetHostID(iClientID);
	}
	else
	{
		reinterpret_cast<CPlayer*>(pPlayer)->SetIsHost(true);
		m_tConnectionPacket.m_bIsHost = false;
		g_GlobalMutex.unlock();
	}

	
	//initilaize client packet send to all
	CPacketMgr::GetInstance()->SendPacket(iClientID, &m_tConnectionPacket);


	//setting sync my packet + setting other packet + send to all
	unordered_map<int, CObj*>::iterator sync_iter = CObjMgr::GetInstance()->GetPlayerList()->begin();
	unordered_map<int, CObj*>::iterator sync_iter_end = CObjMgr::GetInstance()->GetPlayerList()->end();
	for (sync_iter; sync_iter != sync_iter_end; ++sync_iter)
	{
		if (sync_iter->first != iClientID)
		{
			//my packet
			g_GlobalMutex.lock();
			m_tSyncRoomPacket.m_iID = sync_iter->first;
			m_tSyncRoomPacket.m_bIsHost = reinterpret_cast<CPlayer*>(sync_iter->second)->GetIsHost();
			m_tSyncRoomPacket.m_bIsReady = reinterpret_cast<CPlayer*>(sync_iter->second)->GetIsReady();
			m_tSyncRoomPacket.m_iTeamIndex = reinterpret_cast<CPlayer*>(sync_iter->second)->GetRoomIndex();
			g_GlobalMutex.unlock();
			CPacketMgr::GetInstance()->SendPacket(iClientID, &m_tSyncRoomPacket);


			//other packet
			g_GlobalMutex.lock();
			m_tSyncRoomPacket.m_iID = iClientID;
			m_tSyncRoomPacket.m_bIsHost = reinterpret_cast<CPlayer*>(pPlayer)->GetIsHost();
			m_tSyncRoomPacket.m_bIsReady = reinterpret_cast<CPlayer*>(pPlayer)->GetIsReady();
			m_tSyncRoomPacket.m_iTeamIndex = iRoomIndex;
			g_GlobalMutex.unlock();
			CPacketMgr::GetInstance()->SendPacket(sync_iter->first, &m_tSyncRoomPacket);
		}
	}
}

void CAcceptModule::Release(void)
{
	closesocket(m_tListenSocket);
	m_tListenSocket = INVALID_SOCKET;
}