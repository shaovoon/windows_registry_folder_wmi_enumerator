# Windows Registry Folder and WMI Query Enumerator
Wouldn't it be nice if we can use C++11 range for-loop to enumerate Windows registry keys/values, files in a folder and Windows Management Instrumentation (WMI) queries without the boilerplate initialization code or knowing the underlying Win32 API. This library does just that based on Marius Bancila's article: [Enabling MFC Collections to Work in Range-based for Loops](https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo). Marius enables range-based for-loop for MFC collections whereas this library applies his concept to non-collections to like registry, files in a folder and WMI queries. This is a header-only library.

__Pros__
* Resource management is accomplished with RAII: handles and resource are released in the Enumerator's destructor.
* The Windows API usage details is abstracted away from the user.
* Less boilerplate code to be written and just focus on your business logic.

__Cons__
* Iterators produced by the Enumerator cannot be used in STL algorithms because the underlying enumerated object is not a collection.
* This cannot be applied to enumeration done with asynchronous callbacks like [EnumWindows](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-enumwindows) because callbacks cannot be refactored into range for-loop iterators.

## Enumerate Folder Example

`EnumFolder` class took [MSDN Example of Raw Win32 API Folder Enumeration](https://docs.microsoft.com/en-us/windows/win32/fileio/listing-the-files-in-a-directory) to enumerate folder and is a thin wrapper over that example. `EnumFolder` implementation is shown in the later section.

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
        std::wcout << L"  " << ffd.cFileName << "   " << GetFileSize(ffd) << L" bytes\n";
    }
}
```

## Enumerate Registry Keys Example

Comparing with [MSDN Example of Raw Win32 Registry Enumeration](https://docs.microsoft.com/en-us/windows/win32/sysinfo/enumerating-registry-subkeys) with this concise example below, developer is saved from writing that many lines of boilerplate code.

```Cpp
#include "EnumRegistryKey.h"

EnumRegistryKey enumRegistryKey(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft");
for (auto const& szKey : enumRegistryKey)
{
    std::wcout << szKey << L"\n";
}
```
## Enumerate Registry Values Example

This is the example of enumerate registry values with ranged for-loop.

```Cpp
#include "EnumRegistryValue.h"

EnumRegistryValue enumRegistryValue(HKEY_CURRENT_USER, L"Software\\7-Zip\\Compression");
for (auto const& szValueName : enumRegistryValue)
{
    std::wcout << szValueName << L"\n";
}
```
## Enumerate WMI Example

This is the example of enumerating results of WMI query of processes running in the system with ranged for-loop. You can try changing the SQL query and see what results it returns.

```Cpp
#include "EnumWmi.h"

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
```

## Enumerate Topmost Window Handle Example

This is the example of enumerating topmost Window on the desktop. It accomplish this task by saving all the HWNDs in a vector during the callback.

```Cpp
#include "EnumWindowHandle.h"

EnumWindowHandle enumWindowHandle;
for (HWND hwnd : enumWindowHandle.GetResults())
{
    std::wstring title = GetWindowTitle(hwnd);
    if (!title.empty())
    {
        std::wcout << L"Windows title: " << title << "\n";
    }
}
```

## How EnumFolder class is implemented

To enable C++11 range for-loop, basically 2 class needs to written: the enumerator class that acts as a _'collection'_ class and an iterator class that iterates over the _'collection'_. For its simplicity, only `EnumFolder` class is explained here. Other Enumerator class involving registry and WMI requires a deep understanding of their workings. We used the iterator class for MFC `CStringArray` featured in [Marius Bancila's article](https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo) is used as a reference to enable for C++11 range for-loop.

```Cpp
class CStringArrayIterator
{
public:
   CStringArrayIterator(CStringArray& collection, INT_PTR const index):
      m_index(index),
      m_collection(collection)
   {
   }

   bool operator!= (CStringArrayIterator const & other) const
   {
      return m_index != other.m_index;
   }

   CString& operator* () const
   {
      return m_collection[m_index];
   }

   CStringArrayIterator const & operator++ ()
   {
      ++m_index;
      return *this;
   }

private:
   INT_PTR        m_index;
   CStringArray&  m_collection;
};

inline CStringArrayIterator begin(CStringArray& collection)
{
   return CStringArrayIterator(collection, 0);
}

inline CStringArrayIterator end(CStringArray& collection)
{
   return CStringArrayIterator(collection, collection.GetCount());
}
```

C++ Range for loop calls the `begin()` and `end()` behind the scene to get the beginning and ending iterators. 

`EnumFolder` class is implemented in this way with `FindFirstFile`, `FindNextFile` and `FindClose`.

```Cpp
class EnumFolder
{
public:
    EnumFolder(const std::wstring& folder)
        : m_Folder(folder)
        , m_hFind(INVALID_HANDLE_VALUE)
    {
        m_Folder += L"\\*";
        ::ZeroMemory(&m_Ffd, sizeof(m_Ffd));
    }
    ~EnumFolder()
    {
        if (m_hFind != INVALID_HANDLE_VALUE)
            FindClose(m_hFind);

        m_hFind = INVALID_HANDLE_VALUE;
    }
    bool Init()
    {
        m_hFind = FindFirstFile(m_Folder.c_str(), &m_Ffd);
        return m_hFind != INVALID_HANDLE_VALUE;
    }
    bool Next()
    {
        return FindNextFile(m_hFind, &m_Ffd) != 0;
    }
    const WIN32_FIND_DATA& GetFFD() const
    {
        return m_Ffd;
    }
private:
    std::wstring m_Folder;
    HANDLE m_hFind;
    WIN32_FIND_DATA m_Ffd;
};
```

While its iterator class, `EnumFolderIterator`, is implemented in this way.

```Cpp
class EnumFolderIterator
{
public:
    EnumFolderIterator(EnumFolder& collection, INT_PTR const index) :
        m_Index(index),
        m_Collection(collection)
    {
        if (index == 0)
        {
            if(!m_Collection.Init())
                m_Index = -1;
        }
    }

    bool operator!= (EnumFolderIterator const& other) const
    {
        return m_Index != other.m_Index;
    }

    const WIN32_FIND_DATA& operator* () const
    {
        return m_Collection.GetFFD();
    }

    EnumFolderIterator const& operator++ ()
    {
        if (m_Index != -1)
        {
            if (m_Collection.Next())
                ++m_Index;
            else
                m_Index = -1;
        }
        return *this;
    }

private:
    INT_PTR        m_Index;
    EnumFolder& m_Collection;
};

inline EnumFolderIterator begin(EnumFolder& collection)
{
    return EnumFolderIterator(collection, 0);
}

inline EnumFolderIterator end(EnumFolder& collection)
{
    return EnumFolderIterator(collection, -1);
}
```

The `begin()` returns with a `EnumFolderIterator` with 0 index while the `end()` gives it an index of -1 because the actual file count is not provided by WinAPI. This concludes our explanation of the design of the `EnumFolder` and its `iterator`.
