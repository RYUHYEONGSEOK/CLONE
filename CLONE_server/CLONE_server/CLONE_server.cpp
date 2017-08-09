// CLONE_server.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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