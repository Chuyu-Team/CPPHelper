#pragma once
#include <Windows.h>
#include <atlstr.h>




/////////////////////////////////////////////////////////////////////////////
//
//                       提供Wof相关操作
//
/////////////////////////////////////////////////////////////////////////////





#pragma pack(push,1)

struct ExternalBacking
{
	WOF_EXTERNAL_INFO ExternalInfo;
	WIM_PROVIDER_EXTERNAL_INFO ProviderInfo;
};

#pragma pack(pop)

BOOL IsWofCompress(LPCWSTR FilePath);

HRESULT UnWofCompressFile(LPCWSTR FilePath);

#define WofXpress4k 0
#define WofXpress8k 2
#define WofXpress16k 3
#define WofLZX 1

HRESULT WofCompressFile(LPCWSTR FilePath, DWORD CompressType);

HRESULT UnWofCompressRoot(CString RootPath);