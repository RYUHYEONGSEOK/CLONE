// CLONE_server.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
#include "stdafx.h"
#include "MainServer.h"

HANDLE	g_hIOCP = NULL;
mutex	g_GlobalMutex;

int main(void)
{
	CMainServer CServer;
	if (!CServer.Initialize())
	{
		cout << "CMainServer Initialize() error" << endl;
		return 0;
	}

	while (true)
	{
		if (!CServer.Progress())
		{
			cout << "Server ShutDown!!" << endl;
			break;
		}
	}

    return 0;
}