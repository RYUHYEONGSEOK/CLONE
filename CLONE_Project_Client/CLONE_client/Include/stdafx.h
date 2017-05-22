// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>


// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.
// Waring -----------------------------
#pragma warning(disable : 4244) 
#pragma warning(disable : 4005) 
#pragma warning(disable : 4305)
#pragma warning(disable : 4838)
#pragma warning(disable : 4996)
#pragma warning(disable : 4316)

// Etc ------------------------------
#include <time.h>
#include <process.h>

// STL ------------------------------
#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <functional>

// FBX SDK --------------------------
#pragma  comment(lib, "libfbxsdk-md.lib")
#include <fbxsdk.h>

// Boost lib ------------------------
#include <boost/unordered_map.hpp>
using namespace boost;

// iostream -------------------------
#include <iostream>
using namespace  std;

// D3DX -----------------------------
#pragma  comment(lib, "d3dx11.lib")
#pragma  comment(lib, "d3d11.lib")

#pragma  comment(lib, "d3dx10.lib")
#pragma  comment(lib, "d3d10.lib")

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")

#pragma  comment(lib, "D3DCompiler.lib")
#pragma  comment(lib, "dxerr.lib")
#pragma  comment(lib, "dxgi.lib")

#pragma  comment(lib, "dinput8.lib")
#pragma  comment(lib, "dxguid.lib")

#pragma  comment(lib, "d2d1.lib")
#pragma  comment(lib, "dwrite.lib")

#ifdef _DEBUG
#pragma  comment(lib, "d3dx11d.lib")
#pragma  comment(lib, "Effects11d.lib")
#else
#pragma  comment(lib, "d3dx11.lib")
//#pragma  comment(lib, "Effects11.lib")
#endif

#include <D3D11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../Reference/Headers/D3DX11.h"	//#include <D3DX11.h>

#include "../Reference/Headers/D3DX10.h"	//#include <D3DX10.h>
#include <D3D10.h>

#include "../Reference/Headers/d3dx9.h"		//#include <D3DX9.h>

#include "../Reference/Headers/xnamath.h"	//#include <xnamath.h>

// Dx Input ---------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

// Effect11 ---------------------
#include "../Reference/Headers/d3dx11effect.h"
#include "../Reference/Headers/d3dxGlobal.h"

// FW1FontWrapper ---------------------
#pragma  comment(lib, "FW1FontWrapper.lib")

// DX Input ---------------------------
#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>

// Debuging ---------------------------
#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
extern DWORD		g_dwClientDebugMode;
#endif
#include <crtdbg.h>

//Include
#include "Include.h"


// Server---------------------------------
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>
#include "ServerInlcude.h"