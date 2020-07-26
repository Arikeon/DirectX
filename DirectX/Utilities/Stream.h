#pragma once
#include "Build.h"
#include <wchar.h>
#include <string>

namespace Stream
{
	static INLINE void ReadIniBool(LPCWSTR dir, LPCWSTR sectionname, LPCWSTR keyname, bool& value)
	{
		value = GetPrivateProfileInt(sectionname, keyname, -1, dir);
		if (value == -1) //resulted to default
			CONSOLE_LOG("ERROR: ReadIniInt returned -1");
	}

	static INLINE void ReadIniInt(LPCWSTR dir, LPCWSTR sectionname, LPCWSTR keyname, int& value)
	{
		value = GetPrivateProfileInt(sectionname, keyname, -1, dir);
		if (value == -1) //resulted to default
			CONSOLE_LOG("ERROR: ReadIniInt returned -1");
	}

	static INLINE void ReadIniString(LPCWSTR dir, LPCWSTR sectionname, LPCWSTR keyname, std::wstring& value)
	{
		int max = 255;
		WCHAR s[255];
		GetPrivateProfileString(sectionname, keyname, L"NULL", s, max, dir);
		value = s;
		if (value == L"NULL") //resulted to default
			CONSOLE_LOG("ERROR: ReadIniInt returned -1");
	}
}