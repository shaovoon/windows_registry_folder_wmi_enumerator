# Windows Registry Folder and WMI Query Enumerator
Use C++11 range for-loop to enumerate registry keys/values, folder and WMI queries based on Marius Bancila's article: [Enabling MFC Collections to Work in Range-based for Loops](https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo). Marius enables range-based for-loop for MFC collections whereas this library applies his concept to non-collections to enumerate registry, folder and WMI queries with RAII. This is a header-only library.

## Enumerate Registry Keys Example

[MSDN Example of Registry Enumeration](https://docs.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys)

```Cpp
#include "EnumRegistryKey.h"

EnumRegistryKey enumRegistryKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft");
for (auto const& szKey : enumRegistryKey)
{
    std::wcout << szKey << L"\n";
}
```
## Enumerate Registry Values Example

```Cpp
#include "EnumRegistryValue.h"

EnumRegistryValue enumRegistryValue(HKEY_CURRENT_USER, L"Software\\7-Zip\\Compression");
for (auto const& szValueName : enumRegistryValue)
{
    std::wcout << szValueName << L"\n";
}
```
## Enumerate Folder Example

[MSDN Example of Folder Enumeration](https://docs.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory)

```Cpp
#include "EnumFolder.h"

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
```
## Enumerate WMI Example
```Cpp
#include "EnumWmi.h"

if (!InitializeCOM())
{
    std::cout << "InitializeCOM() fails! Program exits.\n";
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
```
