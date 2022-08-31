// EnumWMITest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "EnumWmi.h"

int main()
{
    if (!InitializeCOM())
    {
        std::cerr << "InitializeCOM() fails! Program exits.\n";
        return 1;
    }
    {
        EnumWmi enumWmi(L"SELECT * FROM Win32_Process");
        for (const auto& process : enumWmi)
        {
            _bstr_t str = process[L"Name"].bstrVal;
            std::cout << "Program name: " << str << std::endl;
        }
    }
    CoUninitialize();
    std::cout << "Done!\n";
    return 0;
}
