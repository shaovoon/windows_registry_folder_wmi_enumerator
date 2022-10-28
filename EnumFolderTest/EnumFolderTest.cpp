#include <iostream>
#include <windows.h>
#include <strsafe.h>
#include "EnumFolder.h"
#pragma comment(lib, "User32.lib")

int main(int argc, char* argv[])
{
    EnumFolder enumFolder(L"c:\\temp");
    for (auto const& ffd : enumFolder)
    {
        if (IsFolder(ffd))
        {
            std::wcout << L"  " << ffd.cFileName << "   <DIR>\n";
        }
        else
        {
            std::wcout << L"  " << ffd.cFileName << "   " << GetFileSize(ffd) << L" bytes\n";
        }
    }
    return 0;
}
