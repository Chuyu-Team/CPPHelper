#pragma once
#include "RegHelper.h"
#include "Base.h"
#include "StringHelper.h"
#include "handle.h"


#define MaxRegValueName 0x4000

#pragma warning(push)
#pragma warning(disable: 28251)


LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* Data, LPDWORD pcbData)
{
	return RegQueryValueExW(hKey, ValueName, NULL, pType, (BYTE*)Data, pcbData);
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Type, const void* Data, DWORD cbData)
{
	return RegSetValueExW(hKey, ValueName, 0, Type, (BYTE*)Data, cbData);
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Data)
{
	return RegSetData(hKey, ValueName, REG_DWORD, &Data, sizeof(Data));
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR ValueName, UINT64 Data)
{
	return RegSetData(hKey, ValueName, REG_QWORD, &Data, sizeof(Data));
}

LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* Data, DWORD cbData)
{
	return RegGetData(hKey, ValueName, pType, Data, &cbData);
}

LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pData)
{
	DWORD cbData = sizeof(DWORD);
#ifdef _ATL_XP_TARGETING
	DWORD Type;
	auto lStatus = RegQueryValueExW(hKey, ValueName, NULL, &Type, (BYTE*)pData, &cbData);
	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	if (Type != REG_DWORD)
		return ERROR_UNSUPPORTED_TYPE;

	return S_OK;
#else
	return RegGetValueW(hKey, NULL, ValueName, RRF_RT_REG_DWORD, NULL, pData, &cbData);
#endif
}

LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, PUINT64 pData)
{
	DWORD cbData = sizeof(UINT64);
#ifdef _ATL_XP_TARGETING
	DWORD Type;

	auto lStatus = RegQueryValueExW(hKey, ValueName, NULL, &Type, (BYTE*)pData, &cbData);
	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	if (Type != REG_QWORD)
		return ERROR_UNSUPPORTED_TYPE;

	return S_OK;
#else
	return RegGetValueW(hKey, NULL, ValueName, RRF_RT_REG_QWORD, NULL, pData, &cbData);
#endif
}



//根据名称获取数据
LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, LPBSTR pString)
{
	CString String;

	auto lStatus = RegGetData(hKey, ValueName, String);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	*pString = String.AllocSysString();

	return ERROR_SUCCESS;
}


LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, CString& Str)
{
	DWORD cbData = 0;
#ifdef _ATL_XP_TARGETING
	DWORD Type;
	auto lStatus = RegQueryValueExW(hKey, ValueName, NULL, &Type, NULL, &cbData);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	switch (Type)
	{
	case REG_SZ:
	case REG_MULTI_SZ:
	case REG_EXPAND_SZ:
		break;
	default:
		return ERROR_UNSUPPORTED_TYPE;
		break;
	}

	DWORD cString = (cbData+2) / 2;
	auto pBuffer = Str.GetBuffer(cString);

	lStatus = RegQueryValueEx(hKey, ValueName, NULL, NULL, (byte*)pBuffer, &cbData);
	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	cString = cbData / 2;

	if (cString)
	{
		if (pBuffer[cString - 1] == NULL)
			--cString;
	}

	Str.ReleaseBufferSetLength(cString);

	return ERROR_SUCCESS;
#else
	auto lStatus = RegGetValueW(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, NULL, &cbData);

	if (lStatus)
		return lStatus;

	DWORD cString = (cbData+2) / 2;

	lStatus = RegGetValueW(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, Str.GetBuffer(cString), &cbData);
	if (lStatus)
		return lStatus;

	cString = cbData / 2;

	if (cString)
		--cString;

	Str.ReleaseBufferSetLength(cString);

	return ERROR_SUCCESS;
#endif
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR ValueName, LPCWSTR String)
{
	return RegSetData(hKey, ValueName, REG_SZ, String, StrLen(String) * 2 + 2);
}






LSTATUS RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, LPDWORD pcbData)
{
#ifdef ENABLE_BACKUP_RESTORE
	HKEY hSubKey;
	LSTATUS lStatus = RegOpenKeyExW(hKey, SubKeyPath, REG_OPTION_BACKUP_RESTORE, KEY_READ, &hSubKey);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegGetData(hSubKey, ValueName, pType, pData, pcbData);

	RegCloseKey(hSubKey);

	return lStatus;
#elif defined(_ATL_XP_TARGETING)
	return SHGetValueW(hKey, SubKeyPath, ValueName, pType, pData, pcbData);
#else
	return RegGetValueW(hKey, SubKeyPath, ValueName, RRF_RT_ANY | RRF_NOEXPAND, pType, pData, pcbData);
#endif
}

LSTATUS RegGetData(HKEY hKey, LPCWSTR SubKey, LPCWSTR ValueName, LPDWORD pData)
{
#if defined(ENABLE_BACKUP_RESTORE)||defined(_ATL_XP_TARGETING)
	HKEY hSubKey;
	auto lStatus = RegOpenKeyExW(hKey, SubKey, REG_OPTION, KEY_READ, &hSubKey);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegGetData(hSubKey, ValueName, pData);

	RegCloseKey(hSubKey);
	return lStatus;
#else
	DWORD cbData = sizeof(DWORD);

	return RegGetValueW(hKey, SubKey, ValueName, RRF_RT_REG_DWORD, NULL, pData, &cbData);
#endif
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Type, const void* Data, DWORD cbData)
{
#ifdef ENABLE_BACKUP_RESTORE
	HKEY hSubKey;
	auto lStatus = RegCreateKeyExW(hKey, SubKeyPath, 0, NULL, REG_OPTION_BACKUP_RESTORE, KEY_WRITE, NULL, &hSubKey, NULL);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegSetData(hSubKey, ValueName, Type, Data, cbData);

	RegCloseKey(hSubKey);

	return lStatus;
#elif defined(_ATL_XP_TARGETING)
	return SHSetValueW(hKey, SubKeyPath, ValueName, Type, Data, cbData);
#else
	return RegSetKeyValueW(hKey, SubKeyPath, ValueName, Type, Data, cbData);
#endif
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Data)
{
	return RegSetData(hKey, SubKeyPath, ValueName, REG_DWORD, &Data, sizeof(Data));
}


LSTATUS RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, BSTR* Str)
{
	auto lStatus = RegOpenKeyExW(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hKey);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegGetData(hKey, ValueName, Str);

	RegCloseKey(hKey);

	return lStatus;
}

LSTATUS RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, CString& Str)
{
	auto lStatus = RegOpenKeyExW(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hKey);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegGetData(hKey, ValueName, Str);

	RegCloseKey(hKey);

	return lStatus;
}

LSTATUS RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, LPCWSTR String)
{
	return RegSetData(hKey, SubKeyPath, ValueName, REG_SZ, String, StrLen(String) * 2 + 2);
}

//不返回大小
//HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, DWORD cbData)
//{
//	return RegGetData(hKey, SubKeyPath, ValueName, pType, pData, &cbData);
//}








LSTATUS RegGetData(HKEY hKey, LPCWSTR ValueName, DWORD* pType, CStringA& Data)
{
	Data.Empty();

	DWORD cbData = Data.GetAllocLength();

	auto lStatus = RegGetData(hKey, ValueName, pType, Data.GetBuffer(), &cbData);

	if (lStatus != ERROR_SUCCESS)
	{
		if (lStatus != ERROR_MORE_DATA)
			return lStatus;

		lStatus = RegGetData(hKey, ValueName, pType, Data.GetBuffer(cbData), &cbData);

		if (lStatus != ERROR_SUCCESS)
			return lStatus;
	}

	Data.ReleaseBufferSetLength(cbData);

	return ERROR_SUCCESS;

}



LSTATUS RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, CStringA& Data)
{
	HKEY hSubKey;
	auto lStatus = RegOpenKeyExW(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hSubKey);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegGetData(hSubKey, ValueName, pType, Data);

	RegCloseKey(hSubKey);
	return lStatus;
}

LSTATUS RegDeleteTree2(HKEY hRootKey,LPCWSTR szSubKeyName)
{
#ifdef ENABLE_BACKUP_RESTORE
	CHKEY hItem;
	auto lStatus = RegOpenKeyExW(hRootKey, szSubKeyName, REG_OPTION, KEY_READ | KEY_WRITE | DELETE, &hItem);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	return RegDeleteTree2(hItem);
#elif defined(_ATL_XP_TARGETING)
	return SHDeleteKeyW(hRootKey, szSubKeyName);
#else
	return RegDeleteTreeW(hRootKey, szSubKeyName);
#endif
}

LSTATUS RegDeleteKey2(HKEY hKey)
{
#ifdef ENABLE_BACKUP_RESTORE
	CString szValueName;
	auto pValueName = szValueName.GetBuffer(MaxRegValueName);


	DWORD KeySize, ValueSize;


	auto lStatus = RegQueryInfoKey(hKey, NULL, NULL, NULL, &KeySize, NULL, NULL, &ValueSize, NULL, NULL, NULL, NULL);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	while (KeySize--)
	{
		LSTATUS Error = RegEnumKey(hKey, KeySize, pValueName, MaxRegValueName);

		if (Error == ERROR_SUCCESS)
		{
			Error = RegDeleteTree2(hKey, pValueName);
		}

		if (Error != ERROR_SUCCESS)
			lStatus = Error;
	}

	DWORD cchValueName;

	while (ValueSize--)
	{
		cchValueName = MaxRegValueName;
		LSTATUS Error = RegEnumValue(hKey, ValueSize, pValueName, &cchValueName, NULL, NULL, NULL, NULL);

		if (Error == ERROR_SUCCESS)
		{
			Error = RegDeleteValue(hKey, pValueName);
		}

		if (Error != ERROR_SUCCESS)
			lStatus = Error;
	}

	return lStatus;
#else
	return RegDeleteKey(hKey, NULL);
#endif
}

LSTATUS RegDeleteTree2(HKEY hKey)
{
	//wchar_t Buffer[MaxRegValueName];
	auto lStatus = RegDeleteKey2(hKey);
	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	
	NTSTATUS Status = NtDeleteKey(hKey);
	if (Status!=0)
	{
		return RtlNtStatusToDosError(Status);
	}
	

	return ERROR_SUCCESS;
}

LSTATUS RegCopyTree2(HKEY hSrc, HKEY hDst)
{
	DWORD KeySize, ValueSize, MaxDataLen, Type;

	auto lStatus = RegQueryInfoKeyW(hSrc, NULL, NULL, NULL, &KeySize, NULL, NULL, &ValueSize, NULL, &MaxDataLen, NULL, NULL);

	if (lStatus)
		return lStatus;

	CString Buffer;
	LPWSTR pValueName = Buffer.GetBuffer(MaxRegValueName);

	DWORD cchValueName;
	DWORD ccbData;
	CStringA DataBuffer;

	LPSTR pBuffer = DataBuffer.GetBuffer(MaxDataLen);

	while (ValueSize--)
	{
		cchValueName = MaxRegValueName;
		ccbData = MaxDataLen;

		auto Error = RegEnumValueW(hSrc, ValueSize, pValueName, &cchValueName, NULL, &Type, (LPBYTE)pBuffer, &ccbData);

		if (Error == ERROR_SUCCESS)
		{
			Error = RegSetData(hDst, pValueName, Type, pBuffer, ccbData);
		}

		if (Error)
		{
			lStatus = Error;
		}
	}


	while (KeySize--)
	{
		auto Error = RegEnumKeyW(hSrc, KeySize, pValueName, MaxRegValueName);

		if (Error == ERROR_SUCCESS)
		{
			Error = RegCopyTree2(hSrc, pValueName, hDst, pValueName);
		}

		if (Error)
		{
			lStatus = Error;
		}
	}

	return lStatus;
}

LSTATUS RegCopyTree2(HKEY hSrc, LPCWSTR SrcSubKey, HKEY hDst, LPCWSTR DstSubKey)
{
	CHKEY hSrcItem, hDstItem;

	auto lStatus = RegOpenKeyExW(hSrc, SrcSubKey, REG_OPTION, KEY_READ, &hSrcItem);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	lStatus = RegCreateKeyExW(hDst, DstSubKey, 0, NULL, REG_OPTION, KEY_WRITE | KEY_READ, NULL, &hDstItem, NULL);

	if (lStatus != ERROR_SUCCESS)
		return lStatus;

	return RegCopyTree2(hSrcItem, hDstItem);
}

LSTATUS RegDeleteLink(HKEY hKey, LPCWSTR DesPath)
{
	HKEY Item;
	auto lStatus = RegOpenKeyExW(hKey, DesPath, REG_OPTION_OPEN_LINK | REG_OPTION, DELETE | KEY_ALL_ACCESS, &Item);
	if (lStatus != ERROR_SUCCESS)
	{
		return lStatus;
	}

	lStatus = RegDeleteValue(Item, L"SymbolicLinkValue");
	
	if (NTSTATUS Status = NtDeleteKey(Item))
	{
		lStatus = RtlNtStatusToDosError(Status);
	}

	RegCloseKey(Item);

	return lStatus;
}

LSTATUS RegCreateLink(HKEY hKey, LPCWSTR DesPath, LPCWSTR SrcPath)
{
	RegDeleteLink(hKey, DesPath);

	auto lStstus = RegCreateKeyExW(hKey, DesPath, 0, NULL, REG_OPTION | REG_OPTION_VOLATILE | REG_OPTION_CREATE_LINK, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (lStstus != ERROR_SUCCESS)
		return lStstus;

	lStstus = RegSetValueExW(hKey, L"SymbolicLinkValue", NULL, REG_LINK, (BYTE*)SrcPath, wcslen(SrcPath) * 2);

	RegCloseKey(hKey);

	return lStstus;
}

BOOL RegPathExists(HKEY Root, LPCWSTR RegPath, DWORD Flage)
{
	HKEY Temp;

	if (RegOpenKeyExW(Root, RegPath, REG_OPTION, Flage | KEY_ENUMERATE_SUB_KEYS, &Temp) == 0)
	{
		RegCloseKey(Temp);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


#pragma warning(pop)
#pragma warning(disable: 28251)
