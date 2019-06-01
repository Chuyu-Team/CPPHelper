#pragma once
#include <Windows.h>


//计算字符串长度
int __fastcall StrLen(
	_In_opt_z_ LPCSTR psz
	);

int __fastcall StrLen(
	_In_opt_z_ LPCWSTR psz
	);

#define StaticStrLen(str) (ArraySize(str)-1)

#define StrEmpty(str) ((str==NULL)||(*str==NULL))

//static LPWSTR StrSpet(LPCWSTR Str)
#define StrSpet(Str) (LPWSTR)(Str + StrLen(Str) + 1)


//过滤指定字符
LPCWSTR StrFilter(
	_In_z_ LPCWSTR SrcStr,
	_In_z_ LPCWSTR IgnoreStr
	);

//过滤不可见字符
//LPCWSTR StrFilterNotVisible(LPCWSTR Str)
#define StrFilterNotVisible(Str) StrFilter(Str, L" \t\r\n\xFEFF")

//获取指定字符串
bool StrGet(
	_Inout_z_ LPCWSTR& _Str,
	_In_z_    LPCWSTR  EndStr,
	_In_opt_  LPCWSTR  ErrorStr,
	_Out_     CString& data
	);

LPCWSTR StrExistI(
	_In_z_ LPCWSTR Str,
	_In_z_ LPCWSTR Find
	);

//void StrAppend(BSTR& Str, LPCWSTR Append);

size_t StrRemove(
	_In_z_ LPWSTR RemoveStr
	);


CString StrFormat(
	_In_z_ _Printf_format_string_ LPCWSTR Format, ...
	);


void StrDelete(
	_Inout_ CString& Scr,
	_In_z_  LPCWSTR Dst
	);


CString Str2MultiStr(
	_In_z_ LPCWSTR Str
	);


//将字节数按字符输出
CString StrFormatByte(
	_In_ LONGLONG ByteSize
	);


CStringA Unicode2UTF8(
	_In_z_ LPCWSTR Str
	);

CStringA Unicode2UTF8(
	_In_NLS_string_(cStr) LPCWSTR Str,
	_In_                    DWORD   cStr
	);

void UTF8ToUnicode(
	_In_NLS_string_(cchSrc) const char* Src,
	_In_                  DWORD       cchSrc,
	_Out_                 CString&    Dest
	);

CString UTF8ToUnicode(
	_In_NLS_string_(cchSrc) const char* Src,
	_In_                    DWORD       cchSrc
	);

CString UTF8ToUnicode(
	_In_z_ const char* Src
	);

BOOL StrRegexMatch(
	_In_z_ LPCWSTR Str,
	_In_z_ LPCWSTR MatchStr
	);


CString Guid2Str(
	_In_ const GUID& guid
	);


bool Str2Guid(
	_In_z_ LPCWSTR String,
	_Out_  GUID&   Guid
	);

bool Str2Guid(
	_In_z_ LPCSTR String,
	_Out_  GUID&  Guid
	);

GUID Str2Guid(
	_In_z_ LPCWSTR String
	);

CString StrCut(
	_In_ CString String,
	_In_ DWORD   MaxLen,
	_In_ wchar_t ch     = L'.'
	);

//"\0\0"结束
DWORD MultiStrLen(
	_In_ LPCWSTR String
	);