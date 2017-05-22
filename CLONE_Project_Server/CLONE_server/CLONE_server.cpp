// CLONE_server.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "MainServer.h"

HANDLE g_hIOCP = NULL;
mutex g_GlobalMutex;

int main(void)
{
	if (!(CMainServer::GetInstance()->Initialize()))
	{
		cout << "CMainServer Initialize() error" << endl;
		return 0;
	}

	DWORD dwTime = GetTickCount();

	while (true)
	{
		if (dwTime + 10 < GetTickCount())
		{
			dwTime = GetTickCount();

			if (!(CMainServer::GetInstance()->Progress()))
			{
				break;
			}
		}
	}

	CMainServer::DestroyInstance();
	return 0;
}