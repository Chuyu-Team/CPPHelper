#pragma once
#include <Windows.h>
#include <atlstr.h>



HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* pData, LPDWORD pcbData);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* pData, DWORD cbData);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPBSTR pString);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, CString& Str);

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, LPDWORD pcbData);

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, BSTR* Str);

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, CString& Str);

//HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, DWORD cbData);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, DWORD* pType, CStringA& Data);

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, CStringA& Data);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pData);

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, PUINT64 pData);

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKey, LPCWSTR ValueName, LPDWORD pData);


HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Data);

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, UINT64 Data);

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Type, const void* pData, DWORD cbData);

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, LPCWSTR String);

HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Type, const void* Data, DWORD cbData);


HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Data);

HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, LPCWSTR String);





HRESULT RegDeleteTree(HKEY hKey);

HRESULT RegCopyTree(HKEY hSrc, HKEY hDst);

HRESULT RegCopyTree(HKEY hSrc, LPCWSTR SrcSubKey, HKEY hDst, LPCWSTR DstSubKey);

HRESULT RegDeleteLink(HKEY hKey, LPCWSTR DesPath);

HRESULT RegCreateLink(HKEY hKey, LPCWSTR DesPath, LPCWSTR SrcPath);

BOOL RegPathExists(HKEY Root, LPCWSTR RegPath, DWORD Flage = 0);