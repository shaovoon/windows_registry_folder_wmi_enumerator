// The MIT License (MIT)
// Windows Registry, Folder and WMI Query Enumerator 1.0.0
// Copyright (C) 2022 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#include <Windows.h>
#include <string>

inline bool IsFolder(const WIN32_FIND_DATA& ffd)
{
    return (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;
}

inline LONGLONG GetFileSize(const WIN32_FIND_DATA& ffd)
{
	LARGE_INTEGER filesize;
	filesize.LowPart = ffd.nFileSizeLow;
	filesize.HighPart = ffd.nFileSizeHigh;

    return filesize.QuadPart;
}

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
