#pragma once

#include "../../CLONE_server/CLONE_server/Protocol.h"
#include "Obj.h"

class CServerMgr
{
	DECLARE_SINGLETON(CServerMgr)

public:
	bool Initialize(char* _pIP, HWND _hWnd = NULL);
	bool Release(void);



private:
	int					m_iClientID;
	bool				m_bIsHost;
	int					m_iTeamIndex;
	SOCKET				m_tMyClientSocket;
	CRITICAL_SECTION	m_ServerMgrCS;
private:
	HRESULT AddClient(const wstring& wstrKey, CLIENT* pClient);
	HRESULT DeleteClient(const wstring& wstrKey);
	HRESULT DeleteClient(const int _iID);
public:
	CLIENT* FindClient(const int _iID);

	

private:
	bool m_bIsAllReady;
	bool m_bIsSendInitData[COMPLETE_END];
private:
	bool m_bIsChangeToRoomScene;
	bool m_bIsChangeToInGameScene;
private:
	eStageResultType m_eStageResultType;
private:
	bool		m_bIsRedOrBlue[MAX_USER];
	D3DXVECTOR3 m_vPlayerPos[MAX_USER];
	int			m_iBotCount;
	D3DXVECTOR3 m_vBotPos[MAX_BOT_COUNT];
	D3DXVECTOR3 m_vBeadPos[BEAD_END];
public:
	void SetChangeScene(int _iType, bool _bIsChange);	//0: ToRoom / 1: ToInGame
	void SetStageResultType(eStageResultType _bResultType) { m_eStageResultType = _bResultType; }
public:
	inline int GetClientID(void) { return m_iClientID; }
	inline bool GetIsHost(void) { return m_bIsHost; }
	inline bool GetIsAllReady(void) { return m_bIsAllReady; }// using start button go
	inline bool GetIsChangeToRoomScene(void) { return m_bIsChangeToRoomScene; }
	inline bool GetIsChangeToInGameScene(void) { return m_bIsChangeToInGameScene; }
	inline bool* GetPlayerIsRedOrBlue(void) { return m_bIsRedOrBlue; }
	inline D3DXVECTOR3* GetPlayerPos(void) {return m_vPlayerPos;}
	inline int GetBotCount(void) { return m_iBotCount; }
	inline D3DXVECTOR3* GetBotPos(void) { return m_vBotPos; }
	inline D3DXVECTOR3* GetBeadPos(void) { return m_vBeadPos; }
	inline eStageResultType GetStageResultType(void) { return m_eStageResultType; }
public:
	void ClearSendInitData(void);
	void ClearSendInitDataFlag(void);
	bool CheckAllSendInitData(void);



private:
	int m_iGameTime;
	int m_iCountDown;
public:
	int* GetGameTime(void){return &m_iGameTime;}
	int* GetCountDown(void) { return &m_iCountDown; }


private:
	unordered_map<wstring, list<CObj*>>* m_pMapObjList;
public:
	void SetObjList(unordered_map<wstring, list<CObj*>>* _pList) { m_pMapObjList = _pList; }



private:
	WSABUF	m_tSendWSA;
	WSABUF	m_tRecvWSA;
	char	m_szSendBuf[MAX_BUFFER_SIZE];
	char	m_szRecvBuf[MAX_BUFFER_SIZE];
	char	m_szPacketBuf[MAX_BUFFER_SIZE];
	DWORD	m_dwTempPacketSize;
	int		m_iSavePakcetSize;
public:
	void ReadPacket(SOCKET _socket);
	void SendPacket(char* _pPacket);
private:
	void ProcessPacket(char* _pPacket);


private:
	CServerMgr();
	~CServerMgr();
};