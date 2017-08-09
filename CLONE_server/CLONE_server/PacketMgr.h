#pragma once

#include "Protocol.h"

class CPacketMgr
{
	DECLARE_SINGLETON(CPacketMgr)

private:
	SC_Ready m_tReadyPacket;
	SC_Change m_tChangePacket;
	SC_OptionChange m_tOptionChangePacket;
	SC_ChatRoom m_tChatRoom;
	SC_ChangeStatePlayer m_tChangeStatePlayer;
	SC_ChatGame m_tChatGame;
	SC_ChangeStateBead m_tChangeStateBead;
	SC_CheckCollision m_tCheckCollision;

private:
	mutex m_mutexPacketMgr;

public:
	bool SendPacket(int _iID, void* _pPacket);
	bool ProcessPacket(char* _pPacket, int _iID);

private:
	CPacketMgr();
	~CPacketMgr();
};