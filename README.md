# Windows Registry Folder and WMI Query Enumerator
Use C++11 range for-loop to enumerate registry keys/values, folder and WMI queries based on Marius Bancila's article: [Enabling MFC Collections to Work in Range-based for Loops](https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo). Marius enables range-based for-loop for MFC collections whereas this library applies his concept to non-collections to enumerate registry, folder and WMI queries with RAII. This is a header-only library.

## Enumerate Registry Keys Example

[MSDN Example of Registry Enumeration](https://docs.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys)

```Cpp
#include "EnumRegistryKey.h"

EnumRegistryKey enumRegistryKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft");
for (auto const& szKey : enumRegistryKey)
{
    _tprintf(TEXT("%s\n"), szKey.c_str());
}
```
## Enumerate Registry Values Example

```Cpp
#include "EnumRegistryValue.h"

EnumRegistryValue enumRegistryValue(HKEY_CURRENT_USER, L"Software\\7-Zip\\Compression");
for (auto const& szValueName : enumRegistryValue)
{
    _tprintf(TEXT("%s\n"), szValueName.c_str());
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
```
## Enumerate WMI Example
```Cpp
#include "EnumWmi.h"

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
```
