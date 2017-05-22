#include "stdafx.h"
#include "AcceptThread.h"

#include "Protocol.h"

#include "GameMgr.h"
#include "ClientMgr.h"

DWORD WINAPI AcceptThread(void)
{
	//listen socket setting
	SOCKET tListenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);


	//socket address info
	sockaddr_in tListenAddr;
	sockaddr_in tClientAddr;
	ZeroMemory(&tListenAddr, sizeof(tListenAddr));
	ZeroMemory(&tClientAddr, sizeof(tClientAddr));


	//listenaddr setting
	tListenAddr.sin_family		= AF_INET;
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tListenAddr.sin_port		= htons(SERVER_PORT);


	//bind
	int iResult = ::bind(tListenSocket, reinterpret_cast<sockaddr*>(&tListenAddr), sizeof(tListenAddr));
	if (iResult == SOCKET_ERROR) 
	{
		cout << "bind() error" << endl;
		return 1;
	}


	//listen
	iResult = listen(tListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) 
	{
		cout << "listen() error" << endl;
		return 1;
	}


	while (true)
	{
		int iAddrSize = sizeof(tClientAddr);

		SOCKET tClientSocket = WSAAccept(tListenSocket, reinterpret_cast<sockaddr*>(&tClientAddr), &iAddrSize, NULL, NULL);
		if (tClientSocket == INVALID_SOCKET)
		{
			cout << "client accept() error" << endl;
			exit(-1);
		}

		//max client check
		int iClientID = CClientMgr::GetInstance()->GetClientID();
		if (iClientID == UNKNOWN_VALUE)
		{
			cout << "player count over" << endl;
			continue;
		}

		//game state check
		if (CGameMgr::GetInstance()->GetGameType() != GAME_ROOM)
		{
			cout << "game state is not ROOM" << endl;
			continue;
		}

		//third : 완료된 IO정보의 전달을 위한 매개변수
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(tClientSocket), g_hIOCP, iClientID, 0);


		//accept client setting
		CLIENT* pAcceptClient = new CLIENT;

		g_GlobalMutex.lock();
		pAcceptClient->m_tSock = tClientSocket;
		g_GlobalMutex.unlock();

		//socket state set recv's state.
		DWORD dwRecvFlag = 0;
		iResult = WSARecv(pAcceptClient->m_tSock, &(pAcceptClient->tOverLapped.wsaBuf), 1, nullptr, &dwRecvFlag, &(pAcceptClient->tOverLapped.m_tOverLapped), nullptr);
		if (iResult == SOCKET_ERROR) 
		{
			int iErrCode = WSAGetLastError();
			if (iErrCode != WSA_IO_PENDING)
			{
				cout << "Accept(WSARecv) error" << endl;
				return 1;
			}
		}

		//insert player's information
		CClientMgr::GetInstance()->InsertClient(iClientID, pAcceptClient);
		cout << "connect : " << inet_ntoa(tClientAddr.sin_addr) << " / ID : " << iClientID << endl;


		//initilaize client packet setting
		SC_ConnectRoom tConnectionPacket;
		//id setting
		tConnectionPacket.m_iID = iClientID;
		//Host check
		if (CGameMgr::GetInstance()->GetHostID() == UNKNOWN_VALUE)
		{
			g_GlobalMutex.lock();
			pAcceptClient->m_bIsHost = true;
			g_GlobalMutex.unlock();

			tConnectionPacket.m_bIsHost = true;

			CGameMgr::GetInstance()->SetHostID(iClientID);
		}
		else
		{
			g_GlobalMutex.lock();
			pAcceptClient->m_bIsHost = false;
			g_GlobalMutex.unlock();

			tConnectionPacket.m_bIsHost = false;
		}
		//RoomIndex check
		int iRoomIndex = CGameMgr::GetInstance()->GetEmptyRoomIndex(iClientID);

		g_GlobalMutex.lock();
		pAcceptClient->m_iRoomIndex = iRoomIndex;
		g_GlobalMutex.unlock();

		tConnectionPacket.m_iTeamIndex = iRoomIndex;
		//GameMode check
		switch (CGameMgr::GetInstance()->GetGameModeType())
		{
		case MODE_SOLO:
		{
			tConnectionPacket.m_bIsGameMode = MODE_SOLO;
			break;
		}
		case MODE_TEAM:
		{
			tConnectionPacket.m_bIsGameMode = MODE_TEAM;
			break;
		}
		default:
			cout << "GameMode error!" << endl;
			return 0;
		}
		//room setting
		tConnectionPacket.m_iBotCount = CGameMgr::GetInstance()->GetBotCount();
		tConnectionPacket.m_iRoundCount = CGameMgr::GetInstance()->GetRoundCount();
		//initilaize client packet send to all
		CClientMgr::GetInstance()->SendPacket(iClientID, &tConnectionPacket);


		//setting sync my packet + setting other packet + send to all
		unordered_map<int, CLIENT*>::iterator sync_iter = CClientMgr::GetInstance()->GetClientList()->begin();
		unordered_map<int, CLIENT*>::iterator sync_iter_end = CClientMgr::GetInstance()->GetClientList()->end();
		for (sync_iter; sync_iter != sync_iter_end; ++sync_iter)
		{
			if (sync_iter->first != iClientID)
			{
				//my packet
				SC_SyncRoom tPacket;
				tPacket.m_iID = sync_iter->first;
				tPacket.m_bIsHost = sync_iter->second->m_bIsHost;
				tPacket.m_bIsReady = sync_iter->second->m_bIsReady;
				tPacket.m_iTeamIndex = sync_iter->second->m_iRoomIndex;
				CClientMgr::GetInstance()->SendPacket(iClientID, &tPacket);


				//other packet
				SC_SyncRoom tAcceptPacket;
				tAcceptPacket.m_iID = iClientID;
				tAcceptPacket.m_bIsHost = pAcceptClient->m_bIsHost;
				tAcceptPacket.m_bIsReady = pAcceptClient->m_bIsReady;
				tAcceptPacket.m_iTeamIndex = iRoomIndex;
				CClientMgr::GetInstance()->SendPacket(sync_iter->first, &tAcceptPacket);
			}
		}
	}

	//listen socket release
	closesocket(tListenSocket);

	return 0;
}