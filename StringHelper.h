#pragma once
#include <Windows.h>


//计算字符串长度
#define StrLenW ChTraitsCRT<wchar_t>::SafeStringLen
#define StrLenA ChTraitsCRT<char>::SafeStringLen

#ifdef UNICODE
#define StrLen StrLenW
#else
#define StrLen StrLenA
#endif

#define StaticStrLen(str) (ArraySize(str)-1)

#define StrEmpty(str) ((str==NULL)||(*str==NULL))

//static LPWSTR StrSpet(LPCWSTR Str)
#define StrSpet(Str) (LPWSTR)(Str + StrLen(Str) + 1)


//过滤指定字符
LPCWSTR StrFilter(LPCWSTR SrcStr, LPCWSTR IgnoreStr);

//过滤不可见字符
//LPCWSTR StrFilterNotVisible(LPCWSTR Str)
#define StrFilterNotVisible(Str) StrFilter(Str, L" \t\r\n\xFEFF")

//获取指定字符串
bool StrGet(LPCWSTR &_Str, LPCWSTR EndStr, LPCWSTR ErrorStr, CString&data);

LPCWSTR StrExistI(LPCWSTR Str, LPCWSTR Find);

void StrAppend(BSTR& Str, LPCWSTR Append);

DWORD StrRemove(LPWSTR RemoveStr);


CString StrFormat(_In_z_ _Printf_format_string_ LPCWSTR Format, ...);


void StrDelete(CString& Scr, LPCWSTR Dst);


CString Str2MultiStr(LPCWSTR Str);


//将字节数按字符输出
CString StrFormatByte(UINT64 ByteSize);


CStringA Unicode2UTF8(LPCWSTR Str);

void UTF8ToUnicode(const char* Src, DWORD cchSrc, CString& Dest);

CString UTF8ToUnicode(const char* Src, DWORD cchSrc);

CString UTF8ToUnicode(const char* Src);

BOOL StrRegexMatch(LPCWSTR Str, LPCWSTR MatchStr);


CString Guid2Str(const GUID& guid);



GUID Str2Guid(LPCWSTR String);

CString StrCut(CString String, DWORD MaxLen, wchar_t ch = L'.');

DWORD MultiStrLen(LPCWSTR String);