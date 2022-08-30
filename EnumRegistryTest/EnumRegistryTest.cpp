// EnumRegistryTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// https://docs.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "EnumRegistryKey.h"
#include "EnumRegistryValue.h"

/*
int main()
{
	EnumRegistryKey enumRegistryKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft");
	for (auto const& szKey : enumRegistryKey)
	{
		_tprintf(TEXT("%s\n"), szKey.c_str());
	}
	return 0;
}
*/

int main()
{
	EnumRegistryValue enumRegistryValue(HKEY_CURRENT_USER, L"Software\\7-Zip\\Compression");
	for (auto const& szValueName : enumRegistryValue)
	{
		_tprintf(TEXT("%s\n"), szValueName.c_str());
	}
	return 0;
}