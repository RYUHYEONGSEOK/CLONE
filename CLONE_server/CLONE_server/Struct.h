#pragma once

#include "Enum.h"
#include "Extern.h"

#pragma comment(lib, "ws2_32")
#include <WinSock2.h>

#define MAX_BUFFER_SIZE			4096
#define MAX_PACKET_SIZE			255

typedef struct tagOverLapped_EX
{
	WSAOVERLAPPED	m_tOverLapped;
	int				m_iOperationType;
	int				m_iTargetID;

	WSABUF	wsaBuf;

	char	m_szIOCPbuf[MAX_BUFFER_SIZE];
	char	m_szPacketBuf[MAX_PACKET_SIZE];

	unsigned int m_iPrevReceived;
	unsigned int m_iCurPacketSize;
}OVERLAPPED_EX;

typedef struct tEventType
{
	int									m_iPlayerID;
	int									m_iTarget;
	int									m_iDoEvent;
	high_resolution_clock::time_point	m_ptExecTime;
}EVENTTYPE;


class CMyComparison
{
	bool m_bIsReverse;
public:
	CMyComparison() {}
	bool operator() (const EVENTTYPE _lhs, const EVENTTYPE _rhs) const
	{
		return (_lhs.m_ptExecTime > _rhs.m_ptExecTime);
	}
};

//client
class CDeleteObj
{
public:
	explicit CDeleteObj(void) {}
	~CDeleteObj(void) {}
public: // operator
	template <typename T> void operator () (T& pInstance)
	{
		if (NULL != pInstance)
		{
			delete pInstance;
			pInstance = NULL;
		}
	}
};