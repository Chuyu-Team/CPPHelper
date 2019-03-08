#pragma once
#include <Windows.h>
#include <atlstr.h>


#define YY_range_based_for_registry_key_start(hKey, SubKeyName)                                                                          \
{                                                                                                                                        \
    wchar_t SubKeyName[256];                                                                                                             \
    LSTATUS lRangeBasedForStatus;                                                                                                        \
    for(DWORD dwIndex=0;(lRangeBasedForStatus = RegEnumKeyW(hKey, dwIndex, SubKeyName, _countof(SubKeyName))) == ERROR_SUCCESS;++dwIndex)


#define YY_range_based_for_registry_key_end()                                                                                            \
}

#define YY_range_based_for_registry_key_end_with_errorcode(errorcode)                                                                    \
    errorcode = lRangeBasedForStatus;                                                                                                    \
}



#define YY_range_based_for_registry_value_start(hKey, szValueName, dwType)                                                                  \
{                                                                                                                                           \
    wchar_t szValueName[16384];                                                                                                             \
    LSTATUS lRangeBasedForStatus;                                                                                                           \
    DWORD cchValueName;                                                                                                                     \
    DWORD dwType;                                                                                                                           \
    for(DWORD dwIndex=0;(cchValueName = _countof(szValueName)),(lRangeBasedForStatus = RegEnumValueW(hKey, dwIndex, szValueName, &cchValueName, NULL, &dwType, NULL, NULL)) == ERROR_SUCCESS;++dwIndex)

#define YY_range_based_for_registry_value_end YY_range_based_for_registry_key_end

#define YY_range_based_for_registry_value_end_with_errorcode YY_range_based_for_registry_key_end_with_errorcode



_Check_return_
LSTATUS RegGetData(
	_In_                                          HKEY    hKey,
	_In_opt_z_                                    LPCWSTR ValueName,
	_Out_opt_                                     LPDWORD pType,
	_Out_writes_bytes_to_opt_(*pcbData, *pcbData) void*   pData,
	_Inout_                                       LPDWORD pcbData
	);

_Check_return_
LSTATUS RegGetData(
	_In_                           HKEY    hKey,
	_In_opt_z_                     LPCWSTR ValueName,
	_Out_opt_                      LPDWORD pType,
	_Out_writes_bytes_all_opt_(cbData) void*   pData,
	_In_                           DWORD   cbData
	);

_Check_return_
LSTATUS RegGetData(
	_In_              HKEY    hKey,
	_In_opt_z_        LPCWSTR ValueName,
	_Outptr_result_z_ LPBSTR  pString
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY     hKey,
	_In_opt_z_ LPCWSTR  ValueName,
	_Out_      CString& Str
	);

_Check_return_
LSTATUS RegGetData(
	_In_                                          HKEY    hKey,
	_In_z_                                        LPCWSTR SubKeyPath,
	_In_opt_z_                                    LPCWSTR ValueName,
	_Out_opt_                                     DWORD*  pType,
	_Out_writes_bytes_to_opt_(*pcbData, *pcbData) void*   pData,
	_Inout_                                       LPDWORD pcbData
	);

_Check_return_
LSTATUS RegGetData(
	_In_              HKEY    hKey,
	_In_z_            LPCWSTR SubKeyPath,
	_In_opt_z_        LPCWSTR ValueName,
	_Outptr_result_z_ BSTR*   Str
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY     hKey,
	_In_z_     LPCWSTR  SubKeyPath,
	_In_opt_z_ LPCWSTR  ValueName,
	_Out_      CString& Str
	);

//HRESULT RegGetData(HKEY hKey, LPCWSTR SubKeyPath, LPCWSTR ValueName, DWORD* pType, void* pData, DWORD cbData);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY      hKey,
	_In_opt_z_ LPCWSTR   ValueName,
	_Out_opt_  DWORD*    pType,
	_Out_      CStringA& Data
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY      hKey,
	_In_z_     LPCWSTR   SubKeyPath,
	_In_opt_z_ LPCWSTR   ValueName,
	_Out_opt_  DWORD*    pType,
	_Out_      CStringA& Data
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY    hKey,
	_In_opt_z_ LPCWSTR ValueName,
	_Out_      LPDWORD pData
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY    hKey,
	_In_opt_z_ LPCWSTR ValueName,
	_Out_      PUINT64 pData
	);

_Check_return_
LSTATUS RegGetData(
	_In_       HKEY    hKey,
	_In_z_     LPCWSTR SubKey,
	_In_opt_z_ LPCWSTR ValueName,
	_Out_      LPDWORD pData
	);

_Check_return_
LSTATUS RegSetData(
	_In_       HKEY hKey,
	_In_opt_z_ LPCWSTR ValueName,
	_In_       DWORD Data
	);

_Check_return_
LSTATUS RegSetData(
	_In_       HKEY    hKey,
	_In_opt_z_ LPCWSTR ValueName,
	_In_       UINT64  Data
	);

_Check_return_
LSTATUS RegSetData(
	_In_                     HKEY        hKey,
	_In_opt_z_               LPCWSTR     ValueName,
	_In_                     DWORD       Type,
	_In_reads_bytes_(cbData) const void* pData,
	_In_                     DWORD       cbData
	);

template<DWORD Count,class Item>
__forceinline
_Check_return_
LSTATUS RegSetData(
	_In_              HKEY    hKey,
	_In_opt_z_        LPCWSTR ValueName,
	_In_              DWORD   Type,
	_In_              Item    (&Data)[Count]
	)
{
	return RegSetData(hKey, ValueName, Type, Data, sizeof(Data));
}

_Check_return_
LSTATUS RegSetData(
	_In_       HKEY    hKey,
	_In_opt_z_ LPCWSTR ValueName,
	_In_z_     LPCWSTR String
	);

_Check_return_
LSTATUS RegSetData(
	_In_                     HKEY        hKey,
	_In_z_                   LPCWSTR     SubKeyPath,
	_In_opt_z_               LPCWSTR     ValueName,
	_In_                     DWORD       Type,
	_In_reads_bytes_(cbData) const void* Data,
	_In_                     DWORD       cbData
	);

template<DWORD Count,class Item>
__forceinline
_Check_return_
LSTATUS RegSetData(
	_In_              HKEY    hKey,
	_In_z_            LPCWSTR SubKeyPath,
	_In_opt_z_        LPCWSTR ValueName,
	_In_              DWORD   Type,
	_In_              Item    (&Data)[Count]
	)
{
	return RegSetData(hKey, SubKeyPath, ValueName, Type, Data, sizeof(Data));
}


_Check_return_
LSTATUS RegSetData(
	_In_       HKEY    hKey,
	_In_z_     LPCWSTR SubKeyPath,
	_In_opt_z_ LPCWSTR ValueName,
	_In_       DWORD   Data
	);

_Check_return_
LSTATUS RegSetData(
	_In_       HKEY    hKey,
	_In_z_     LPCWSTR SubKeyPath,
	_In_opt_z_ LPCWSTR ValueName,
	_In_z_     LPCWSTR String
	);

//仅清空Key，但是不删除Key本身
_Check_return_
LSTATUS RegDeleteKey2(
	_In_ HKEY hKey
	);

_Check_return_
LSTATUS RegDeleteTree2(
	_In_   HKEY    hRootKey,
	_In_z_ LPCWSTR szSubKeyName
	);

_Check_return_
LSTATUS RegDeleteTree2(
	_In_ HKEY hKey
	);

_Check_return_
LSTATUS RegCopyTree2(
	_In_ HKEY hSrc,
	_In_ HKEY hDst
	);

_Check_return_
LSTATUS RegCopyTree2(
	_In_   HKEY    hSrc,
	_In_z_ LPCWSTR SrcSubKey,
	_In_   HKEY    hDst,
	_In_z_ LPCWSTR DstSubKey
	);

_Check_return_
LSTATUS RegDeleteLink(
	_In_   HKEY    hKey,
	_In_z_ LPCWSTR DesPath
	);

_Check_return_
LSTATUS RegCreateLink(
	_In_   HKEY    hKey,
	_In_z_ LPCWSTR DesPath,
	_In_z_ LPCWSTR SrcPath
	);

_Check_return_
BOOL RegPathExists(
	_In_     HKEY    Root,
	_In_z_   LPCWSTR RegPath,
	_In_opt_ DWORD   Flage   = 0
	);