// The MIT License (MIT)
// Windows Registry, Folder and WMI Query Enumerator 1.0.0
// Copyright (C) 2022 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include <windows.h>
#include <objbase.h>
#include <atlbase.h>
#include <iostream>
#include <wbemidl.h>
#include <comutil.h>
#include <string>

#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "WbemUuid.Lib")

inline bool InitializeCOM()
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (FAILED(hr))
        return false;

    // setup process-wide security context
    hr = CoInitializeSecurity(NULL, // we're not a server
        -1, // we're not a server
        NULL, // dito
        NULL, // reserved
        RPC_C_AUTHN_LEVEL_DEFAULT, // let DCOM decide
        RPC_C_IMP_LEVEL_IMPERSONATE,
        NULL,
        EOAC_NONE,
        NULL);
    if (SUCCEEDED(hr) || hr == RPC_E_TOO_LATE)
        return true;

    return false;
}

class EnumWmi
{
public:
    EnumWmi(const std::wstring& query)
        : m_Initialized(false)
    {
        HRESULT hr = CoCreateInstance(CLSID_WbemAdministrativeLocator, NULL,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, reinterpret_cast<void**>(&m_Locator));
        if (FAILED(hr))
        {
            //std::cerr << "Instantiation of IWbemLocator failed" << std::endl;
            return;
        }

        // connect to local service with current credentials
        bstr_t networkResourceString(L"root\\cimv2");
        hr = m_Locator->ConnectServer(networkResourceString, NULL, NULL, NULL,
            WBEM_FLAG_CONNECT_USE_MAX_WAIT,
            NULL, NULL, &m_Service);
        if (SUCCEEDED(hr))
        {
            // execute a query
            bstr_t queryLanguageString(L"WQL");
            bstr_t queryString(query.c_str());
            hr = m_Service->ExecQuery(queryLanguageString, queryString,
                WBEM_FLAG_FORWARD_ONLY, NULL, &m_Enumerator);
            if (SUCCEEDED(hr))
            {
                m_Initialized = true;
            }
        }
    }
    bool Next()
    {
        if (!m_Initialized)
            return false;

        CComPtr< IWbemClassObject > processor = NULL;
        ULONG retcnt;
        HRESULT m_NextHR = m_Enumerator->Next(WBEM_INFINITE, 1L, reinterpret_cast<IWbemClassObject**>(&processor), &retcnt);

        m_Processor = processor;

        return !(m_NextHR == WBEM_S_FALSE || m_NextHR == WBEM_S_TIMEDOUT);
    }
    CComPtr< IWbemClassObject > GetProcessor()
    {
        return m_Processor;
    }
private:
    CComPtr< IWbemLocator > m_Locator;
    CComPtr< IWbemServices > m_Service;
    CComPtr< IEnumWbemClassObject > m_Enumerator;
    CComPtr< IWbemClassObject > m_Processor;
    bool m_Initialized;
};

class WmiProcessor
{
public:
    WmiProcessor(CComPtr< IWbemClassObject > processor)
        : m_Processor(processor)
    {}
    WmiProcessor(const WmiProcessor& other)
    {
        m_Processor = other.m_Processor;
    }
    _variant_t operator[](const std::wstring& find) const
    {
        _variant_t var_val;
        HRESULT hr = m_Processor->Get(find.c_str(), 0, &var_val, NULL, NULL);
        if (FAILED(hr))
            throw std::runtime_error("Failed to find");

        return var_val;
    }

private:
    CComPtr< IWbemClassObject > m_Processor;
};

class WmiIterator
{
public:
    WmiIterator(EnumWmi& enumWmi, INT_PTR index)
        : m_Index(index)
        , m_EnumWmi(enumWmi)
    {
        if (m_Index == 0)
            Enumerate();
    }

    bool operator!= (WmiIterator const& other) const
    {
        return m_Index != other.m_Index;
    }

    const WmiProcessor operator* () const
    {
        return WmiProcessor(m_EnumWmi.GetProcessor());
    }

    WmiProcessor operator* ()
    {
        return WmiProcessor(m_EnumWmi.GetProcessor());
    }
    WmiIterator const& operator++ ()
    {
        Enumerate();
        return *this;
    }
    void Enumerate()
    {
        ++m_Index;
        if (!m_EnumWmi.Next())
            m_Index = -1;
    }

private:
    INT_PTR m_Index;
    EnumWmi& m_EnumWmi;
};

inline WmiIterator begin(EnumWmi& enumWmi)
{
    return WmiIterator(enumWmi, 0);
}

inline WmiIterator end(EnumWmi& enumWmi)
{
    return WmiIterator(enumWmi, -1);
}
