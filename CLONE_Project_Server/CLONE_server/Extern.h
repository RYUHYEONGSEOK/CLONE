#pragma once

#define WM_SOCKET		WM_USER+1

#define SERVER_PORT		9990

#define MAX_USER		6
#define ROUND_UNIT		2
#define BOT_UNIT		1
#define MIN_ROUND_COUNT 1
#define MIN_BOT_COUNT	1
#define MAX_ROUND_COUNT 5
#define MAX_BOT_COUNT	99
#define RED_COUNT		3
#define BLUE_COUNT		3

#define MAX_CHAT		100
#define MAX_NAME		64

#define UNKNOWN_VALUE	-1
#define NO_TARGET		0

#define GAME_START_COUNT 4	// 3, 2, 1, start

//server
extern HANDLE g_hIOCP;
extern mutex g_GlobalMutex;

//client
const D3DXVECTOR3	g_vLook = D3DXVECTOR3(0.f, 0.f, 1.f);
#define PLAY_TIME	120
#define HALFMAPX	80
#define HALFMAPZ	40


//key DWORD
const DWORD KEY_A			= 0x00000001;
const DWORD KEY_D			= 0x00000002;
const DWORD KEY_SPACE		= 0x00000004;
const DWORD KEY_S			= 0x00000008;
const DWORD KEY_W			= 0x00000010;
const DWORD KEY_LCONTROL	= 0x00000020;
const DWORD KEY_LSHIFT		= 0x00000040;
const DWORD KEY_LMOUSE		= 0x00000080;