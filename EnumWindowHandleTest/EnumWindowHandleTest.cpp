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
        const size_t len = static_cast<size_t>(::GetWindowTextLengthW(hwnd));
        if (len > 0)
        {
            std::wstring title(len + 1, L'\0');
            ::GetWindowTextW(hwnd, &title[0], len + 1);
            std::wcout << L"Windows title: " << title << "\n";
        }
    }
    std::cout << "Done!\n";
}
