#pragma once
#include "IniHelper.h"
#include "Base.cpp"

BOOL IniReadString(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CString& Str)
{
	int cchData = Str.GetAllocLength()>100 ? Str.GetAllocLength() : 100;

	while (auto chNewData = GetPrivateProfileStringW(KeyPath, ValueName, NULL, Str.GetBuffer(cchData), cchData, FilePath))
	{
		if (cchData - chNewData <= 2)
		{
			//持续加大缓冲区
			cchData *= 2;
		}
		else
		{
			Str.ReleaseBuffer(chNewData);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL IniReadBinaryData(LPCWSTR FilePath, LPCWSTR KeyPath, LPCWSTR ValueName, CStringA& Data)
{
	CString Buffer;
	if (IniReadString(FilePath, KeyPath, ValueName, Buffer) == 0)
	{
		return FALSE;
	}
	else
	{
		Data = HexString2Binary(Buffer);

		return TRUE;
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


BOOL IniDeleteSection(
	LPCWSTR FilePath,
	LPCWSTR KeyPath
	)
{
	return WritePrivateProfileStringW(KeyPath, NULL, NULL, FilePath);
}

BOOL IniGetSectionNames(LPCWSTR FilePath, CString& Names)
{
	int cchData = Names.GetAllocLength()>100 ? Names.GetAllocLength() : 100;

	while (auto chNewData = GetPrivateProfileSectionNamesW(Names.GetBuffer(cchData), cchData, FilePath))
	{
		if (cchData - chNewData == 2)
		{
			cchData *= 2;
		}
		else
		{
			Names.ReleaseBuffer(chNewData);
			return TRUE;
		}
	}

	return FALSE;
}


BOOL IniGetValues(LPCWSTR FilePath, LPCWSTR Path, CString& Values)
{
	Values.GetBuffer(1024);

	int cchData = Values.GetAllocLength();

	while (auto chNewData = GetPrivateProfileSectionW(Path, Values.GetBuffer(cchData), cchData, FilePath))
	{
		if (cchData - chNewData == 2)
		{
			cchData *= 2;
		}
		else
		{
			Values.ReleaseBuffer(chNewData);
			return TRUE;
		}
	}

	return FALSE;
}
