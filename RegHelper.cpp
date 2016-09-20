#pragma once
#include "RegHelper.h"
#include "Base.h"
#include "StringHelper.h"
#include "handle.h"


#define MaxRegValueName 0x4000
#ifdef ENABLE_BACKUP_RESTORE
#define REG_OPTION REG_OPTION_BACKUP_RESTORE
#else
#define REG_OPTION 0
#endif



HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* Data, LPDWORD pcbData)
{
	return RegQueryValueEx(hKey, ValueName, NULL, pType, (BYTE*)Data, pcbData);
}

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Type, const void* Data, DWORD cbData)
{
	return  RegSetValueEx(hKey, ValueName, 0, Type, (BYTE*)Data, cbData);
}

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, DWORD Data)
{
	return RegSetData(hKey, ValueName, REG_DWORD, &Data, sizeof(Data));
}

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, UINT64 Data)
{
	return RegSetData(hKey, ValueName, REG_QWORD, &Data, sizeof(Data));
}

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pType, void* Data, DWORD cbData)
{
	return RegGetData(hKey, ValueName, pType, Data, &cbData);
}

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPDWORD pData)
{
	DWORD cbData = sizeof(DWORD);

	return RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_DWORD, NULL, pData, &cbData);
}

HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, PUINT64 pData)
{
	DWORD cbData = sizeof(UINT64);

	return RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_QWORD, NULL, pData, &cbData);
}



//根据名称获取数据
HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, LPBSTR pString)
{
	DWORD cbData = 0;

	HRESULT hr = RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, NULL, &cbData);

	if (hr == 0)
	{
		if (cbData == 0)
		{
			*pString = NULL;
			return S_OK;
		}

		BSTR String = SysAllocStringByteLen(NULL, cbData - 2);

		hr = RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, String, &cbData);

		if (hr)
		{
			SysFreeString(String);
		}
		else
		{
			*pString = String;
		}
	}

	return hr;
}


HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, CString& Str)
{
	DWORD cbData = 0;

	HRESULT hr = RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, NULL, &cbData);

	if (hr)
		return hr;


	hr = RegGetValue(hKey, NULL, ValueName, RRF_RT_REG_SZ | RRF_RT_REG_MULTI_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND, NULL, Str.GetBuffer(cbData / 2), &cbData);
	if (hr)
		return hr;

	Str.ReleaseBufferSetLength(cbData / 2 - 1);

	return S_OK;
}

HRESULT RegSetData(HKEY hKey, LPCWSTR ValueName, LPCWSTR String)
{
	return RegSetData(hKey, ValueName, REG_SZ, String, StrLen(String) * 2 + 2);
}






HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, LPDWORD pcbData)
{
#ifdef ENABLE_BACKUP_RESTORE
	HKEY hSubKey;
	HRESULT hr = RegOpenKeyEx(hKey, SubKeyPath, REG_OPTION_BACKUP_RESTORE, KEY_READ, &hSubKey);

	if (hr)
		return hr;

	hr = RegGetData(hSubKey, ValueName, pType, pData, pcbData);

	RegCloseKey(hSubKey);

	return hr;
#else
	return RegGetValue(hKey, SubKeyPath, ValueName, RRF_RT_ANY | RRF_NOEXPAND, pType, pData, pcbData);
#endif
}

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKey, LPCWSTR ValueName, LPDWORD pData)
{
#ifdef ENABLE_BACKUP_RESTORE
	HKEY hSubKey;
	HRESULT hr = RegOpenKeyEx(hKey, SubKey, REG_OPTION_BACKUP_RESTORE, KEY_READ, &hSubKey);

	if (hr)
		return hr;

	hr = RegGetData(hSubKey, ValueName, pData);

	RegCloseKey(hSubKey);
	return hr;
#else
	DWORD cbData = sizeof(DWORD);

	return RegGetValue(hKey, SubKey, ValueName, RRF_RT_REG_DWORD, NULL, pData, &cbData);
#endif
}

HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Type, const void* Data, DWORD cbData)
{
#ifdef ENABLE_BACKUP_RESTORE
	HRESULT hr = RegCreateKeyEx(hKey, SubKeyPath, 0, NULL, REG_OPTION_BACKUP_RESTORE, KEY_WRITE, NULL, &hKey, NULL);

	if (hr)
		return hr;

	hr = RegSetData(hKey, ValueName, Type, Data, cbData);

	RegCloseKey(hKey);

	return hr;
#else
	return RegSetKeyValue(hKey, SubKeyPath, ValueName, Type, Data, cbData);
#endif
}

HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD Data)
{
	return RegSetData(hKey, SubKeyPath, ValueName, REG_DWORD, &Data, sizeof(Data));
}


HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, BSTR* Str)
{
	HRESULT hr = RegOpenKeyEx(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hKey);

	if (hr)
		return hr;

	hr = RegGetData(hKey, ValueName, Str);

	RegCloseKey(hKey);

	return hr;
}

HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, CString& Str)
{
	HRESULT hr = RegOpenKeyEx(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hKey);

	if (hr)
		return hr;

	hr = RegGetData(hKey, ValueName, Str);

	RegCloseKey(hKey);

	return hr;
}

HRESULT RegSetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, LPCWSTR String)
{
	return RegSetData(hKey, SubKeyPath, ValueName, REG_SZ, String, StrLen(String) * 2 + 2);
}

//不返回大小
//HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, DWORD cbData)
//{
//	return RegGetData(hKey, SubKeyPath, ValueName, pType, pData, &cbData);
//}








HRESULT RegGetData(HKEY hKey, LPCWSTR ValueName, DWORD* pType, CStringA& Data)
{
	Data.Empty();

	DWORD cbData = Data.GetAllocLength();

	HRESULT hr = RegGetData(hKey, ValueName, pType, Data.GetBuffer(), &cbData);

	if (hr)
	{
		if (hr != ERROR_MORE_DATA)
			return hr;

		hr = RegGetData(hKey, ValueName, pType, Data.GetBuffer(cbData), &cbData);

		if (hr)
			return hr;
	}

	Data.ReleaseBufferSetLength(cbData);

	return S_OK;

}



HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, CStringA& Data)
{
	HKEY hSubKey;
	HRESULT hr = RegOpenKeyEx(hKey, SubKeyPath, REG_OPTION, KEY_READ, &hSubKey);

	if (hr)
		return hr;

	hr = RegGetData(hSubKey, ValueName, pType, Data);

	RegCloseKey(hSubKey);
	return hr;
}


HRESULT RegDeleteTree(HKEY hKey)
{
	//wchar_t Buffer[MaxRegValueName];
	CString szValueName;
	auto pValueName = szValueName.GetBuffer(MaxRegValueName);

	HRESULT hr = S_OK, thr;

	DWORD KeySize, ValueSize;


	hr = RegQueryInfoKey(hKey, NULL, NULL, NULL, &KeySize, NULL, NULL, &ValueSize, NULL, NULL, NULL, NULL);

	if (hr)
		return hr;

	while (KeySize--)
	{
		thr = RegEnumKey(hKey, KeySize, pValueName, MaxRegValueName);

		if (thr == S_OK)
		{
			HKEY Item;
			thr = RegOpenKeyEx(hKey, pValueName, REG_OPTION, KEY_READ | KEY_WRITE | DELETE, &Item);
			if (thr == S_OK)
			{
				thr = RegDeleteTree(Item);

				RegCloseKey(Item);
			}
		}

		if (thr)
			hr = thr;
	}

	DWORD cchValueName;

	while (ValueSize--)
	{
		cchValueName = MaxRegValueName;
		thr = RegEnumValue(hKey, ValueSize, pValueName, &cchValueName, NULL, NULL, NULL, NULL);

		if (thr == S_OK)
		{
			thr = RegDeleteValue(hKey, pValueName);
		}

		if (thr)
			hr = thr;
	}

	if (hr == S_OK)
	{
		hr = NtDeleteKey(hKey);
	}

	return hr;
}

HRESULT RegCopyTree(HKEY hSrc, HKEY hDst)
{
	DWORD KeySize, ValueSize, MaxDataLen, Type;

	HRESULT hr = RegQueryInfoKey(hSrc, NULL, NULL, NULL, &KeySize, NULL, NULL, &ValueSize, NULL, &MaxDataLen, NULL, NULL), thr;

	if (hr)
		return hr;
	CString Buffer;
	auto pValueName = Buffer.GetBuffer(MaxRegValueName);

	DWORD cchValueName;
	DWORD ccbData;
	CStringA DataBuffer;

	auto pBuffer = DataBuffer.GetBuffer(MaxDataLen);

	while (ValueSize--)
	{
		cchValueName = MaxRegValueName;
		ccbData = MaxDataLen;

		thr = RegEnumValue(hSrc, ValueSize, pValueName, &cchValueName, NULL, &Type, (LPBYTE)pBuffer, &ccbData);

		if (thr == S_OK)
		{
			thr = RegSetData(hDst, pValueName, Type, pBuffer, ccbData);
		}

		if (thr)
			hr = thr;
	}


	while (KeySize--)
	{
		thr = RegEnumKey(hSrc, KeySize, pValueName, MaxRegValueName);

		if (thr)
		{
			hr = thr;
			continue;
		}

		thr = RegCopyTree(hSrc, pValueName, hDst, pValueName);

		if (thr)
		{
			hr = thr;
		}
	}

	return hr;
}

HRESULT RegCopyTree(HKEY hSrc, LPCWSTR SrcSubKey, HKEY hDst, LPCWSTR DstSubKey)
{
	CHKEY hSrcItem, hDstItem;

	HRESULT hr = RegOpenKeyEx(hSrc, SrcSubKey, REG_OPTION, KEY_READ, &hSrcItem);

	if (hr)
		return hr;

	hr = RegCreateKeyEx(hDst, DstSubKey, 0, NULL, REG_OPTION, KEY_WRITE | KEY_READ, NULL, &hDstItem, NULL);

	if (hr)
		return hr;

	return RegCopyTree(hSrcItem, hDstItem);
}

HRESULT RegDeleteLink(HKEY hKey, LPCWSTR DesPath)
{
	HKEY Item;
	HRESULT hr = RegOpenKeyExW(hKey, DesPath, REG_OPTION_OPEN_LINK | REG_OPTION, DELETE | KEY_ALL_ACCESS, &Item);
	if (hr)
	{
		return hr;
	}

	hr = RegDeleteValue(Item, L"SymbolicLinkValue");
	hr = NtDeleteKey(Item);
	//if (hr)
	RegCloseKey(Item);

	return hr < 0 ? hr : 0;
}

HRESULT RegCreateLink(HKEY hKey, LPCWSTR DesPath, LPCWSTR SrcPath)
{
	RegDeleteLink(hKey, DesPath);

	HRESULT hr = RegCreateKeyExW(hKey, DesPath, 0, NULL, REG_OPTION | REG_OPTION_VOLATILE | REG_OPTION_CREATE_LINK, KEY_ALL_ACCESS, NULL, &hKey, NULL);

	if (hr)
		return hr;

	hr = RegSetValueExW(hKey, L"SymbolicLinkValue", NULL, REG_LINK, (BYTE*)SrcPath, wcslen(SrcPath) * 2);

	RegCloseKey(hKey);

	return hr;
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


