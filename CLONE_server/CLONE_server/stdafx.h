// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>


//warning
#pragma warning(disable : 4996)
#pragma warning(disable : 4838)


//network
#pragma comment(lib, "ws2_32")
#include <WinSock2.h>


//window
#include <Windows.h>
#include <iostream>
#include <chrono> 
using namespace std;
using namespace chrono;
#include <time.h>


//data structure
#include <vector>
#include <unordered_map>
#include <queue>


//thread
#include <thread>
#include <mutex>


//D3DX
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
#else
#pragma  comment(lib, "d3dx11.lib")
#endif

#include <D3D11_1.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "../Reference/Headers/D3DX11.h"	//#include <D3DX11.h>

#include "../Reference/Headers/D3DX10.h"	//#include <D3DX10.h>
#include <D3D10.h>

#include "../Reference/Headers/d3dx9.h"		//#include <D3DX9.h>
#include "../Reference/Headers/xnamath.h"	//#include <xnamath.h>


//include.h
#include "Inlcude.h"