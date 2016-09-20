#pragma once
#include <Windows.h>
#include <atlstr.h>

BOOL IniReadString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CString& Str);

BOOL IniReadBinaryData(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CStringA& Data);

BOOL IniWriteString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, LPCWSTR Str);

BOOL IniWriteBinaryData(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, const void*pBinaryData, DWORD ccbData);

BOOL IniDeleteString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName);

BOOL IniGetSectionNames(LPCWSTR FilePath, CString& Names);

BOOL IniGetValues(LPCWSTR FilePath, LPCWSTR Path, CString& Values);

//BOOL IniSetValues(LPCWSTR FilePath, LPCWSTR Path, LPCWSTR Values);
#define IniSetValues(FilePath, Path, Values) WritePrivateProfileSection(Path,Values,FilePath)