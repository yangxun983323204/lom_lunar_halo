#pragma once
#include <string>
#include <locale>
#include <codecvt>

namespace YX 
{
	/// <summary>
	/// 将wstring转换到utf8编码的string
	/// </summary>
	inline std::string WStringToUtf8(std::wstring wstr) 
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t> > cvt;
		return cvt.to_bytes(wstr);
	}

	/// <summary>
	/// 将utf8编码的string转换到wstring
	/// </summary>
	inline std::wstring Utf8ToWString(std::string u8str)
	{
		std::wstring_convert< std::codecvt_utf8<wchar_t> > cvt;
		return cvt.from_bytes(u8str);
	}

	inline std::wstring A2WWithLocale(std::string str)
	{
		std::string curLocale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "chs");
		const char* _Source = str.c_str();
		size_t _Dsize = str.size() + 1;
		wchar_t* _Dest = new wchar_t[_Dsize];
		wmemset(_Dest, 0, _Dsize);
		mbstowcs(_Dest, _Source, _Dsize);
		std::wstring result = _Dest;
		delete[]_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}

	inline std::string W2AWithLocale(std::wstring wstr)
	{
		std::string curLocale = setlocale(LC_ALL, NULL);
		setlocale(LC_ALL, "chs");
		const wchar_t* _Source = wstr.c_str();
		size_t _Dsize = 2 * wstr.size() + 1;
		char* _Dest = new char[_Dsize];
		memset(_Dest, 0, _Dsize);
		wcstombs(_Dest, _Source, _Dsize);
		std::string result = _Dest;
		delete[]_Dest;
		setlocale(LC_ALL, curLocale.c_str());
		return result;
	}
}