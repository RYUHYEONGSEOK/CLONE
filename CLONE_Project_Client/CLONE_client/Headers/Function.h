#pragma once

template <typename T> void Safe_Delete(T& pointer)
{
	if (NULL != pointer)
	{
		delete pointer;
		pointer = NULL;
	}
}


template <typename T> void Safe_Delete_Array(T& pointer)
{
	if (NULL != pointer)
	{
		delete[] pointer;
		pointer = NULL;
	}
}


template <typename T> void Safe_Release(T& pointer)
{
	if (NULL != pointer)
	{
		pointer->Release();
		pointer = NULL;
	}
}

template <typename T> void Safe_Single_Destory(T& pointer)
{
	if (NULL != pointer)
	{
		pointer->DestroyInstance();
		pointer = NULL;
	}
}

typedef std::basic_string<TCHAR> tstring;

TCHAR* StringToTCHAR(string& s)
{
	tstring tstr;
	const char* all = s.c_str();
	int len = 1 + strlen(all);
	wchar_t* t = new wchar_t[len];
	if (NULL == t) throw std::bad_alloc();
	mbstowcs(t, all, len);
	return (TCHAR*)t;
}

std::string TCHARToString(const TCHAR* ptsz)
{
	int len = wcslen((wchar_t*)ptsz);
	char* psz = new char[2 * len + 1];
	wcstombs(psz, (wchar_t*)ptsz, 2 * len + 1);
	std::string s = psz;
	delete[] psz;
	return s;
}

//std::wstring TCHARTowString(const char* const psz)
//{
//	std::size_t len = std::strlen(psz);
//	std::vector<wchar_t> result;
//	const int len = WideCharToMultiByte(CP_ACP
//		, 0
//		, reinterpret_cast<LPCWSTR>(psz)
//		, static_cast<int>(len)
//		, NULL
//		, 0
//		, NULL
//		, NULL
//	);
//
//	result.resize(len);
//	if (result.empty()) return std::wstring();
//	const int cbytes = WideCharToMultiByte(CP_ACP
//		, 0
//		, reinterpret_cast<LPCWSTR>(psz)
//		, static_cast<int>(len)
//		, reinterpret_cast<LPSTR>(&result[0])
//		, static_cast<int>(result.size())
//		, NULL
//		, NULL
//	);
//	assert(cbytes);
//	return std::wstring(result.begin(), result.begin() + cbytes);
//}