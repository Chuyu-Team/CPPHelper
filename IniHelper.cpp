#pragma once
#include "IniHelper.h"
#include "Base.cpp"

BOOL IniReadString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CString& Str)
{
	//Str.GetAllocLength();

	int cchData = Str.GetAllocLength()>100 ? Str.GetAllocLength() : 100;

	while (cchData = GetPrivateProfileStringW(KeyPath, ValueName, NULL, Str.GetBuffer(cchData), cchData, FilePath))
	{
		if (GetLastError() == ERROR_MORE_DATA)
		{
			cchData = Str.GetAllocLength()*Str.GetAllocLength();
		}
		else
		{
			Str.ReleaseBuffer(cchData);
			return 1;
		}
	}

	return 0;
}

BOOL IniReadBinaryData(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CStringA& Data)
{
	CString Buffer;
	if (IniReadString(FilePath, KeyPath, ValueName, Buffer) == 0)
	{
		return 0;
	}
	else
	{
		Data = HexString2Binary(Buffer);

		return 1;
	}
}

BOOL IniWriteString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, LPCWSTR Str)
{
	return WritePrivateProfileStringW(KeyPath, ValueName, Str, FilePath);
}


BOOL IniWriteBinaryData(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, const void*pBinaryData, DWORD ccbData)
{
	return WritePrivateProfileStringW(KeyPath, ValueName, Binary2String((byte*)pBinaryData, ccbData), FilePath);
}

BOOL IniDeleteString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName)
{
	return WritePrivateProfileStringW(KeyPath, ValueName, NULL, FilePath);
}

BOOL IniGetSectionNames(LPCWSTR FilePath, CString& Names)
{
	//Str.GetAllocLength();

	int cchData = Names.GetAllocLength()>100 ? Names.GetAllocLength() : 100;

	while (cchData = GetPrivateProfileSectionNames(Names.GetBuffer(cchData), cchData, FilePath))
	{
		if (GetLastError() == ERROR_MORE_DATA)
		{
			cchData = Names.GetAllocLength()*Names.GetAllocLength();
		}
		else
		{
			Names.ReleaseBuffer(cchData);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL IniGetValues(LPCWSTR FilePath, LPCWSTR Path, CString& Values)
{
	//Str.GetAllocLength();
	Values.GetBuffer(1024);

	int cchData = Values.GetAllocLength();

	while (cchData = GetPrivateProfileSection(Path, Values.GetBuffer(cchData), cchData, FilePath))
	{
		if (cchData == Values.GetAllocLength() - 2)
		{
			cchData = Values.GetAllocLength()*Values.GetAllocLength();
		}
		else
		{
			Values.ReleaseBuffer(cchData);
			return TRUE;
		}
	}

	return FALSE;
}
