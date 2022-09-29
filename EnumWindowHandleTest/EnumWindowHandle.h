// The MIT License (MIT)
// Windows Registry, Folder and WMI Query Enumerator 1.0.0
// Copyright (C) 2022 by Shao Voon Wong (shaovoon@yahoo.com)
//
// http://opensource.org/licenses/MIT

#pragma once
#include <vector>
#include <Windows.h>

struct EnumWindowHandleResults
{
	void AddHandle(HWND hwnd)
	{
		m_Windows.push_back(hwnd);
	}
	std::vector<HWND> m_Windows;
};

BOOL CALLBACK EnumWindowsProc(
	_In_ HWND   hwnd,
	_In_ LPARAM lParam
)
{
	EnumWindowHandleResults* results = reinterpret_cast<EnumWindowHandleResults*>(lParam);
	results->AddHandle(hwnd);
	return TRUE;
}

class EnumWindowHandle
{
public:
	const std::vector<HWND>& GetResults() const
	{
		::EnumWindows(EnumWindowsProc, (LPARAM)(&m_EnumWindowHandleResults));

		return m_EnumWindowHandleResults.m_Windows;
	}
private:
	EnumWindowHandleResults m_EnumWindowHandleResults;
};