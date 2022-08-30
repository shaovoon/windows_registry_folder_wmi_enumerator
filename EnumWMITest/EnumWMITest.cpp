// EnumWMITest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "EnumWmi.h"

int main()
{
    if (!InitializeCOM())
    {
        std::cout << "InitializeCOM() fails! Program exits.\n";
        return 1;
    }
    EnumWmi enumWmi(L"SELECT * FROM Win32_Processor");
    for (const auto& processor : enumWmi)
    {
        _bstr_t str = processor[L"Name"].bstrVal;
        std::cout << "Processor name: " << str << std::endl;
    }

    std::cout << "Done!\n";
    return 0;
}
