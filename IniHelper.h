#pragma once
#include <Windows.h>
#include <atlstr.h>

_Check_return_
BOOL IniReadString(
	_In_z_ LPCWSTR  FilePath,
	_In_z_ LPCWSTR  KeyPath,
	_In_z_ LPCWSTR  ValueName,
	_Out_  CString& Str
	);

_Check_return_
BOOL IniReadBinaryData(
	_In_z_ LPCWSTR   FilePath,
	_In_z_ LPCWSTR   KeyPath,
	_In_z_ LPCWSTR   ValueName,
	_Out_  CStringA& Data
	);

_Check_return_
BOOL IniWriteString(
	_In_z_     LPCWSTR FilePath,
	_In_z_     LPCWSTR KeyPath,
	_In_opt_z_ LPCWSTR ValueName,
	_In_opt_z_ LPCWSTR Str
	);

_Check_return_
BOOL IniWriteBinaryData(
	_In_z_                    LPCWSTR     FilePath,
	_In_z_                    LPCWSTR     KeyPath,
	_In_z_                    LPCWSTR     ValueName,
	_In_reads_bytes_(ccbData) const void* pBinaryData,
	_In_                      DWORD       ccbData
	);

_Check_return_
BOOL IniDeleteString(
	_In_z_     LPCWSTR FilePath,
	_In_z_     LPCWSTR KeyPath,
	_In_opt_z_ LPCWSTR ValueName
	);

_Check_return_
BOOL IniDeleteSection(
	_In_z_ LPCWSTR FilePath,
	_In_z_ LPCWSTR KeyPath
	);

_Check_return_
BOOL IniGetSectionNames(
	_In_z_ LPCWSTR  FilePath,
	_Out_  CString& Names
	);

_Check_return_
BOOL IniGetValues(
	_In_z_ LPCWSTR  FilePath,
	_In_z_ LPCWSTR  Path,
	_Out_  CString& Values
	);

//BOOL IniSetValues(LPCWSTR FilePath, LPCWSTR Path, LPCWSTR Values);
#define IniSetValues(FilePath, Path, Values) WritePrivateProfileSection(Path,Values,FilePath)