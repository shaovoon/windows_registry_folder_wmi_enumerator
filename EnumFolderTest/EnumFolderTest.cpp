#include <windows.h>
#include <tchar.h> 
#include <stdio.h>
#include <strsafe.h>
#include "EnumFolder.h"
#pragma comment(lib, "User32.lib")

int _tmain(int argc, TCHAR* argv[])
{
	EnumFolder enumFolder(L"c:\\temp");
	for (auto const& ffd : enumFolder)
	{
		if (IsFolder(ffd))
		{
			_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
		}
		else
		{
			LARGE_INTEGER filesize;
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
		}
	}
	return 0;
}
