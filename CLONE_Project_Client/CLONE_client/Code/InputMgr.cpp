#include "stdafx.h"
#include "InputMgr.h"

IMPLEMENT_SINGLETON(CInput)

CInput::CInput()
	: m_pInput(NULL)
	, m_pKeyBoardDevice(NULL)
	, m_pMouseDevice(NULL)

	, m_bMouseDown(false)
	, m_bMousePressed(false)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * MAXPATH);
	ZeroMemory(&m_eMouseState, sizeof(DIMOUSESTATE));

	ZeroMemory(&m_bKeyDown, sizeof(bool) * MAXPATH);
	ZeroMemory(&m_bKeyPressd, sizeof(bool) * MAXPATH);
}

CInput::~CInput()
{
	Release();
}

HRESULT CInput::Init(HINSTANCE _hInst, HWND _hWnd)
{
	FAILED_CHECK_MSG(DirectInput8Create(
		_hInst
		, DIRECTINPUT_VERSION
		, IID_IDirectInput8
		, (void**)&m_pInput
		, NULL), L"다이렉트 인풋 생성 실패");

	FAILED_CHECK_MSG(InitKeyBoard(_hWnd), L"키보드 생성 실패");
	FAILED_CHECK_MSG(InitMouse(_hWnd), L"마우스 생성 실패");

	return S_OK;
}

HRESULT CInput::InitKeyBoard(HWND _hWnd)
{
	FAILED_CHECK(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoardDevice, NULL));

	m_pKeyBoardDevice->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyBoardDevice->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoardDevice->Acquire();

	return S_OK;
}

HRESULT CInput::InitMouse(HWND _hWnd)
{
	FAILED_CHECK(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouseDevice, NULL));

	m_pMouseDevice->SetCooperativeLevel(_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouseDevice->SetDataFormat(&c_dfDIMouse);
	m_pMouseDevice->Acquire();

	return S_OK;
}

void CInput::UpdateInputState()
{
	m_pKeyBoardDevice->GetDeviceState(256, m_byKeyState);
	m_pMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_eMouseState);
}

bool CInput::CheckKeyboardDown(BYTE _byKeyFlag)
{
	if (CheckKeyboardPressed(_byKeyFlag))
		m_bKeyDown[_byKeyFlag] = false;

	if (m_bKeyPressd[_byKeyFlag] && !(m_bKeyDown[_byKeyFlag]))
	{
		m_bKeyDown[_byKeyFlag] = true;
		return true;
	}

	return false;
}

bool CInput::CheckKeyboardPress(BYTE _byKeyFlag)
{
	if (m_byKeyState[_byKeyFlag] & 0x80)
		return true;

	return false;
}

bool CInput::CheckKeyboardPressed(BYTE _byKeyFlag)
{
	if (m_byKeyState[_byKeyFlag] & 0x80)
	{
		m_bKeyPressd[_byKeyFlag] = true;
		return false;
	}
	else if (m_bKeyPressd[_byKeyFlag])
	{
		if (!(m_byKeyState[_byKeyFlag] & 0x80))
		{
			m_bKeyPressd[_byKeyFlag] = false;
			return true;
		}
	}

	return false;
}

bool CInput::CheckMouseDown(MouseType _eKeyFlag)
{
	if (CheckMousePressed(_eKeyFlag))
		m_bMouseDown = false;

	if (m_bMousePressed && !(m_bMouseDown))
	{
		m_bMouseDown = true;
		return true;
	}

	return false;
}

bool CInput::CheckMousePress(MouseType _eKeyFlag)
{
	if (m_eMouseState.rgbButtons[_eKeyFlag] & 0x80)
		return true;

	return false;
}

bool CInput::CheckMousePressed(MouseType _eKeyFlag)
{
	if (m_eMouseState.rgbButtons[_eKeyFlag] & 0x80)
	{
		m_bMousePressed = true;
		return false;
	}
	else if (m_bMousePressed)
	{
		if (!(m_eMouseState.rgbButtons[_eKeyFlag] & 0x80))
		{
			m_bMousePressed = false;
			return true;
		}
	}

	return false;
}

void CInput::Release()
{
	Safe_Release(m_pMouseDevice);
	Safe_Release(m_pKeyBoardDevice);
	Safe_Release(m_pInput);
}