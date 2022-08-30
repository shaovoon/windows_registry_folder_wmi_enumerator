// The MIT License (MIT)
// Windows Registry, Folder and WMI Query Enumerator 1.0.0
// Copyright (C) 2022 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "RegConstants.h"

class EnumRegistryValue
{
public:
    EnumRegistryValue(HKEY hKey, LPCTSTR lpSubKey)
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

            m_Count = cValues;
            m_ValidMainKey = true;
        }
    }
    ~EnumRegistryValue()
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
        std::vector<TCHAR> achValue(MAX_VALUE_NAME, L'\0');
        DWORD cchValue = MAX_VALUE_NAME;

        cchValue = MAX_VALUE_NAME;
        achValue[0] = '\0';
        DWORD retCode = RegEnumValue(m_MainKey, m_Index,
            achValue.data(),
            &cchValue,
            NULL,
            NULL,
            NULL,
            NULL);

        if (retCode == ERROR_SUCCESS)
            m_CurrentValue = achValue.data();

        if (m_Index < m_Count)
            ++m_Index;

        return retCode == ERROR_SUCCESS;
    }
    const std::wstring& GetCurrentValueName() const
    {
        return m_CurrentValue;
    }

private:
    HKEY m_MainKey;
    bool m_ValidMainKey;
    std::wstring m_CurrentValue;
    INT_PTR m_Index;
    INT_PTR m_Count;
};

// https://www.codeproject.com/Articles/835025/Enabling-MFC-Collections-to-Work-in-Range-based-fo
class EnumRegistryValueIterator
{
public:
    EnumRegistryValueIterator(EnumRegistryValue& collection, INT_PTR const index) :
        m_Index(index),
        m_Collection(collection)
    {
        if (m_Index == 0)
        {
            if (m_Collection.IsValid())
                m_Collection.Next();
        }
    }

    bool operator!= (EnumRegistryValueIterator const& other) const
    {
        return m_Index != other.m_Index;
    }

    const std::wstring& operator* () const
    {
        return m_Collection.GetCurrentValueName();
    }

    EnumRegistryValueIterator const& operator++ ()
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
    EnumRegistryValue& m_Collection;
};

inline EnumRegistryValueIterator begin(EnumRegistryValue& collection)
{
    return EnumRegistryValueIterator(collection, 0);
}

inline EnumRegistryValueIterator end(EnumRegistryValue& collection)
{
    return EnumRegistryValueIterator(collection, collection.Count());
}