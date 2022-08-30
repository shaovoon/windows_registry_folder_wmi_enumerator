// The MIT License (MIT)
// Windows Registry, Folder and WMI Query Enumerator 1.0.0
// Copyright (C) 2022 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#include <Windows.h>
#include <string>
#include "RegConstants.h"

class EnumRegistryKey
{
public:
    EnumRegistryKey(HKEY hKey, LPCTSTR lpSubKey)
        : m_MainKey(0)
        , m_ValidMainKey(false)
        , m_Index(0)
        , m_Count(0)
    {
        if (RegOpenKeyEx(hKey,
            lpSubKey,
            0,
            KEY_READ,
            &m_MainKey) == ERROR_SUCCESS)
        {
            TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
            TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
            DWORD    cchClassName = MAX_PATH;  // size of class string 
            DWORD    cSubKeys = 0;               // number of subkeys 
            DWORD    cbMaxSubKey;              // longest subkey size 
            DWORD    cchMaxClass;              // longest class string 
            DWORD    cValues;              // number of values for key 
            DWORD    cchMaxValue;          // longest value name 
            DWORD    cbMaxValueData;       // longest value data 
            DWORD    cbSecurityDescriptor; // size of security descriptor 
            FILETIME ftLastWriteTime;      // last write time 

            DWORD retCode = RegQueryInfoKey(
                m_MainKey,                    // key handle 
                achClass,                // buffer for class name 
                &cchClassName,           // size of class string 
                NULL,                    // reserved 
                &cSubKeys,               // number of subkeys 
                &cbMaxSubKey,            // longest subkey size 
                &cchMaxClass,            // longest class string 
                &cValues,                // number of values for this key 
                &cchMaxValue,            // longest value name 
                &cbMaxValueData,         // longest value data 
                &cbSecurityDescriptor,   // security descriptor 
                &ftLastWriteTime);       // last write time 

            m_Count = cSubKeys;
            m_ValidMainKey = (cSubKeys>0);
        }
    }
    ~EnumRegistryKey()
    {
        if (m_MainKey)
        {
            RegCloseKey(m_MainKey);
            m_MainKey = 0;
        }
    }
    bool IsValid() const
    {
        return m_ValidMainKey;
    }
    INT_PTR Count() const
    {
        return m_Count;
    }
    bool Next()
    {
        TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
        DWORD cbName = MAX_KEY_LENGTH;
        FILETIME ftLastWriteTime;      // last write time 

        DWORD retCode = RegEnumKeyEx(m_MainKey, m_Index,
            achKey,
            &cbName,
            NULL,
            NULL,
            NULL,
            &ftLastWriteTime);

        if (retCode == ERROR_SUCCESS)
            m_CurrentKey = achKey;

        if (m_Index < m_Count)
            ++m_Index;

        return retCode == ERROR_SUCCESS;
    }
    const std::wstring& GetCurrentKeyName() const
    {
        return m_CurrentKey;
    }

private:
    HKEY m_MainKey;
    bool m_ValidMainKey;
    std::wstring m_CurrentKey;
    INT_PTR m_Index;
    INT_PTR m_Count;
};

// https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo
class EnumRegistryKeyIterator
{
public:
    EnumRegistryKeyIterator(EnumRegistryKey& collection, INT_PTR const index) :
        m_Index(index),
        m_Collection(collection)
    {
        if (m_Index == 0)
        {
            if (m_Collection.IsValid())
                m_Collection.Next();
        }
    }

    bool operator!= (EnumRegistryKeyIterator const& other) const
    {
        return m_Index != other.m_Index;
    }

    const std::wstring& operator* () const
    {
        return m_Collection.GetCurrentKeyName();
    }

    EnumRegistryKeyIterator const& operator++ ()
    {
        if (m_Index < m_Collection.Count())
        {
            if (m_Collection.Next())
                ++m_Index;
            else
                m_Index = m_Collection.Count();
        }

        return *this;
    }

private:
    INT_PTR        m_Index;
    EnumRegistryKey& m_Collection;
};

inline EnumRegistryKeyIterator begin(EnumRegistryKey& collection)
{
    return EnumRegistryKeyIterator(collection, 0);
}

inline EnumRegistryKeyIterator end(EnumRegistryKey& collection)
{
    return EnumRegistryKeyIterator(collection, collection.Count());
}