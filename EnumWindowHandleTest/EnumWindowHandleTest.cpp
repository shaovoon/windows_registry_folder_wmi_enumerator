// EnumWindowHandleTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "EnumWindowHandle.h"

int main()
{
    EnumWindowHandle enumWindowHandle;
    for (HWND hwnd : enumWindowHandle.GetResults())
    {
        std::wstring title = GetWindowTitle(hwnd);
        if (!title.empty())
        {
            std::wcout << L"Windows title: " << title << "\n";
        }
    }
    std::cout << "Done!\n";
}
