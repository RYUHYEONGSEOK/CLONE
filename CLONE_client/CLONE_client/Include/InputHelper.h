#pragma once

#include "stdafx.h"
#include "InputMgr.h"

wstring GetKeyboardChar(void)
{
	CInput* pInputMgr = CInput::GetInstance();

	// NUMBER(Upper case)
	if (pInputMgr->CheckKeyboardPress(DIK_LSHIFT) || pInputMgr->CheckKeyboardPress(DIK_RSHIFT))
	{
		if (pInputMgr->CheckKeyboardPressed(DIK_1))
			return L"!";
		if (pInputMgr->CheckKeyboardPressed(DIK_2))
			return L"@";
		if (pInputMgr->CheckKeyboardPressed(DIK_3))
			return L"#";
		if (pInputMgr->CheckKeyboardPressed(DIK_4))
			return L"$";
		if (pInputMgr->CheckKeyboardPressed(DIK_5))
			return L"%";
		if (pInputMgr->CheckKeyboardPressed(DIK_6))
			return L"^";
		if (pInputMgr->CheckKeyboardPressed(DIK_7))
			return L"&";
		if (pInputMgr->CheckKeyboardPressed(DIK_8))
			return L"*";
		if (pInputMgr->CheckKeyboardPressed(DIK_9))
			return L"(";
		if (pInputMgr->CheckKeyboardPressed(DIK_0))
			return L")";
	}

	// NUMBER
	if (pInputMgr->CheckKeyboardPressed(DIK_1))
		return L"1";
	if (pInputMgr->CheckKeyboardPressed(DIK_2))
		return L"2";
	if (pInputMgr->CheckKeyboardPressed(DIK_3))
		return L"3";
	if (pInputMgr->CheckKeyboardPressed(DIK_4))
		return L"4";
	if (pInputMgr->CheckKeyboardPressed(DIK_5))
		return L"5";
	if (pInputMgr->CheckKeyboardPressed(DIK_6))
		return L"6";
	if (pInputMgr->CheckKeyboardPressed(DIK_7))
		return L"7";
	if (pInputMgr->CheckKeyboardPressed(DIK_8))
		return L"8";
	if (pInputMgr->CheckKeyboardPressed(DIK_9))
		return L"9";
	if (pInputMgr->CheckKeyboardPressed(DIK_0))
		return L"0";

	// CHAR(Upper case)
	if (pInputMgr->CheckKeyboardPress(DIK_LSHIFT) || pInputMgr->CheckKeyboardPress(DIK_RSHIFT))
	{
		if (pInputMgr->CheckKeyboardPressed(DIK_Q))
			return L"Q";
		if (pInputMgr->CheckKeyboardPressed(DIK_W))
			return L"W";
		if (pInputMgr->CheckKeyboardPressed(DIK_E))
			return L"E";
		if (pInputMgr->CheckKeyboardPressed(DIK_R))
			return L"R";
		if (pInputMgr->CheckKeyboardPressed(DIK_T))
			return L"T";
		if (pInputMgr->CheckKeyboardPressed(DIK_Y))
			return L"Y";
		if (pInputMgr->CheckKeyboardPressed(DIK_U))
			return L"U";
		if (pInputMgr->CheckKeyboardPressed(DIK_I))
			return L"I";
		if (pInputMgr->CheckKeyboardPressed(DIK_O))
			return L"O";
		if (pInputMgr->CheckKeyboardPressed(DIK_P))
			return L"P";
		if (pInputMgr->CheckKeyboardPressed(DIK_A))
			return L"A";
		if (pInputMgr->CheckKeyboardPressed(DIK_S))
			return L"S";
		if (pInputMgr->CheckKeyboardPressed(DIK_D))
			return L"D";
		if (pInputMgr->CheckKeyboardPressed(DIK_F))
			return L"F";
		if (pInputMgr->CheckKeyboardPressed(DIK_G))
			return L"G";
		if (pInputMgr->CheckKeyboardPressed(DIK_H))
			return L"H";
		if (pInputMgr->CheckKeyboardPressed(DIK_J))
			return L"J";
		if (pInputMgr->CheckKeyboardPressed(DIK_K))
			return L"K";
		if (pInputMgr->CheckKeyboardPressed(DIK_L))
			return L"L";
		if (pInputMgr->CheckKeyboardPressed(DIK_Z))
			return L"Z";
		if (pInputMgr->CheckKeyboardPressed(DIK_X))
			return L"X";
		if (pInputMgr->CheckKeyboardPressed(DIK_C))
			return L"C";
		if (pInputMgr->CheckKeyboardPressed(DIK_V))
			return L"V";
		if (pInputMgr->CheckKeyboardPressed(DIK_B))
			return L"B";
		if (pInputMgr->CheckKeyboardPressed(DIK_N))
			return L"N";
		if (pInputMgr->CheckKeyboardPressed(DIK_M))
			return L"M";
	}
	
	// CHAR
	if (pInputMgr->CheckKeyboardPressed(DIK_Q))
		return L"q";
	if (pInputMgr->CheckKeyboardPressed(DIK_W))
		return L"w";
	if (pInputMgr->CheckKeyboardPressed(DIK_E))
		return L"e";
	if (pInputMgr->CheckKeyboardPressed(DIK_R))
		return L"r";
	if (pInputMgr->CheckKeyboardPressed(DIK_T))
		return L"t";
	if (pInputMgr->CheckKeyboardPressed(DIK_Y))
		return L"y";
	if (pInputMgr->CheckKeyboardPressed(DIK_U))
		return L"u";
	if (pInputMgr->CheckKeyboardPressed(DIK_I))
		return L"i";
	if (pInputMgr->CheckKeyboardPressed(DIK_O))
		return L"o";
	if (pInputMgr->CheckKeyboardPressed(DIK_P))
		return L"p";
	if (pInputMgr->CheckKeyboardPressed(DIK_A))
		return L"a";
	if (pInputMgr->CheckKeyboardPressed(DIK_S))
		return L"s";
	if (pInputMgr->CheckKeyboardPressed(DIK_D))
		return L"d";
	if (pInputMgr->CheckKeyboardPressed(DIK_F))
		return L"f";
	if (pInputMgr->CheckKeyboardPressed(DIK_G))
		return L"g";
	if (pInputMgr->CheckKeyboardPressed(DIK_H))
		return L"h";
	if (pInputMgr->CheckKeyboardPressed(DIK_J))
		return L"j";
	if (pInputMgr->CheckKeyboardPressed(DIK_K))
		return L"k";
	if (pInputMgr->CheckKeyboardPressed(DIK_L))
		return L"l";
	if (pInputMgr->CheckKeyboardPressed(DIK_Z))
		return L"z";
	if (pInputMgr->CheckKeyboardPressed(DIK_X))
		return L"x";
	if (pInputMgr->CheckKeyboardPressed(DIK_C))
		return L"c";
	if (pInputMgr->CheckKeyboardPressed(DIK_V))
		return L"v";
	if (pInputMgr->CheckKeyboardPressed(DIK_B))
		return L"b";
	if (pInputMgr->CheckKeyboardPressed(DIK_N))
		return L"n";
	if (pInputMgr->CheckKeyboardPressed(DIK_M))
		return L"m";

	//SAPCE
	if (pInputMgr->CheckKeyboardPressed(DIK_SPACE))
		return L" ";

	// END
	return L"";
}