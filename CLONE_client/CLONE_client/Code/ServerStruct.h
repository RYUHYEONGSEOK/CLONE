#pragma once

#include "ServerEnum.h"
#include "ServerExtern.h"

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>

#define MAX_BUFFER_SIZE			4096
#define MAX_PACKET_SIZE			255

typedef struct tagOverLapped_EX
{
	WSAOVERLAPPED	m_tOverLapped;
	int				m_iOperationType;

	WSABUF	wsaBuf;

	char	m_szIOCPbuf[MAX_BUFFER_SIZE];
	char	m_szPacketBuf[MAX_PACKET_SIZE];

	unsigned int m_iPrevReceived;
	unsigned int m_iCurPacketSize;
}OVERLAPPED_EX;

//client
//typedef struct tagClient
//{
//	//network
//	SOCKET			m_tSock;
//	OVERLAPPED_EX	tOverLapped;
//
//	//클라이언트 위치,각도
//	D3DXVECTOR3		m_vPos;
//	D3DXVECTOR3		m_vDir;
//
//	//클라이언트 정보
//	bool	m_bIsHost;
//	bool	m_bIsReady;
//	int		m_iRoomIndex;
//	//int		m_iWin;
//	//int		m_iDraw;
//	//int		m_iLose;
//	char	m_szName[MAX_NAME];
//
//	//클라이언트 CS
//	CRITICAL_SECTION cs;
//
//	//생성자 및 소멸자
//	tagClient(void)
//	{
//		//information
//		m_bIsHost = false;
//		m_bIsReady = false;
//		m_iRoomIndex = UNKNOWN_VALUE;
//		ZeroMemory(m_szName, sizeof(char) * MAX_NAME);
//
//		//setting + init
//		InitializeCriticalSection(&cs);
//
//		m_tSock = NULL;
//
//		ZeroMemory(&tOverLapped, sizeof(OVERLAPPED_EX));
//		tOverLapped.m_iOperationType = OP_RECV;
//		tOverLapped.wsaBuf.buf = tOverLapped.m_szIOCPbuf;
//		tOverLapped.wsaBuf.len = sizeof(tOverLapped.m_szIOCPbuf);
//	}
//	~tagClient(void)
//	{
//		//release
//		DeleteCriticalSection(&cs);
//	}
//
//}CLIENT;

typedef struct tEventType
{
	int				m_iPlayerID;
	int				m_iTarget;
	int				m_iDoEvent;
	float			m_fTime;
}EVENTTYPE;

class CMyComparison
{
	bool m_bIsReverse;
public:
	CMyComparison() {}
	bool operator() (const EVENTTYPE _lhs, const EVENTTYPE _rhs) const
	{
		return (_lhs.m_fTime > _rhs.m_fTime);
	}
};

//client
//class CDeleteObj
//{
//public:
//	explicit CDeleteObj(void) {}
//	~CDeleteObj(void) {}
//public: // operator
//	template <typename T> void operator () (T& pInstance)
//	{
//		if (NULL != pInstance)
//		{
//			delete pInstance;
//			pInstance = NULL;
//		}
//	}
//};