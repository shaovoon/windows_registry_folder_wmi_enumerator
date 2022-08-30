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
            LARGE_INTEGER filesize;
            filesize.LowPart = ffd.nFileSizeLow;
            filesize.HighPart = ffd.nFileSizeHigh;
			std::wcout << L"  " << ffd.cFileName << "   " << filesize.QuadPart << L" bytes\n";
        }
    }
    return 0;
}
