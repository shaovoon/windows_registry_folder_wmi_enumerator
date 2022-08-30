// https://docs.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys
#include <iostream>
#include <windows.h>
#include "EnumRegistryKey.h"
#include "EnumRegistryValue.h"


int main()
{
    EnumRegistryKey enumRegistryKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft");
    for (auto const& szKey : enumRegistryKey)
    {
		std::wcout << szKey << L"\n";
    }
    return 0;
}

/*
int main()
{
    EnumRegistryValue enumRegistryValue(HKEY_CURRENT_USER, L"Software\\7-Zip\\Compression");
    for (auto const& szValueName : enumRegistryValue)
    {
        std::wcout << szValueName << L"\n";
    }
    return 0;
}
*/