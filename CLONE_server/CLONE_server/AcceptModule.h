#pragma once

#include "Protocol.h"

class CAcceptModule
{
private:
	SC_ConnectRoom	m_tConnectionPacket;
	SC_SyncRoom		m_tSyncRoomPacket;

protected:
	SOCKET		m_tListenSocket;
	SOCKADDR_IN m_tAddr;
	SOCKADDR_IN m_tClientAddr;

public:
	bool Initialize(int _iType, int _iProtocol, SOCKADDR_IN _tAddr, int _iWaitSocket = SOMAXCONN);
	void OnAccept(void);

private:
	void Release(void);

public:
	CAcceptModule();
	virtual ~CAcceptModule();
};