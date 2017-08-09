#pragma once

#ifdef _WINDOWED
	const DWORD		WINCX = 1280;//1024;
	const DWORD		WINCY = 720;//768;
#else
	const DWORD		WINCX = GetSystemMetrics(SM_CXSCREEN);
	const DWORD		WINCY = GetSystemMetrics(SM_CYSCREEN);
#endif

const D3DXVECTOR3	g_vLook = D3DXVECTOR3(0.f, 0.f, 1.f);

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;

extern bool			g_bDraw;

extern tGameInfo	g_GameInfo;									// 게임 셋팅
extern unordered_map<wstring, list<CLIENT*>>	g_mapClient;	//클라이언트 map