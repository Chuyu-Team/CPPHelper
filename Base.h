#pragma once
#define UMDF_USING_NTSTATUS
#include <Windows.h>
#include "StringI.h"
#include <set>
#include <map>
#include <vector>
#include <winioctl.h>
#include <winscard.h>
#include <wincrypt.h>
#include "km.h"

#define LDR_IS_DATAFILE(handle)      (((ULONG_PTR)(handle)) &  (ULONG_PTR)1)
#define LDR_IS_IMAGEMAPPING(handle)  (((ULONG_PTR)(handle)) & (ULONG_PTR)2)
#define LDR_IS_RESOURCE(handle)      (LDR_IS_IMAGEMAPPING(handle) || LDR_IS_DATAFILE(handle))


//计算数组长度
#define ArraySize(Array)  (sizeof(Array)/sizeof(*Array))


//生成版本号
#define MakeVersion(v1,v2,v3,v4) (((UINT64)(v4))|((UINT64)v3<<16)|((UINT64)v2<<32)|((UINT64)v1<<48))

#define MakeMiniVersion(v1,v2) (v2|(v1<<16))

#define __StrA(x) #x
#define __StrW(x) _T(__StrA(x))


#define DISM_MSG_PROGRESS   38008     //进度信息wParam为当前完成百分比


typedef DWORD(WINAPI *BaseCallBack)(DWORD MessageId, WPARAM wParam, LPARAM lParam, PVOID UserData);




void FindPath(CStringI FilePath, LPCWSTR SubStr, std::set<CStringI>&FindList, DWORD cchRootPath);



HRESULT RunCmd(LPCWSTR FilePath, CString CmdString, CString* pOutString);

HRESULT RunCmd(LPCWSTR FilePath, CString CmdString,bool Async =false);

HRESULT RunCmd(LPCWSTR FilePath, CString CmdString, DWORD(WINAPI *CallBack)(DWORD dwMessageId, WPARAM wParam, LPARAM lParam, PVOID UserData), LPVOID UserData);

HRESULT QuerySymbolicLinkObject(LPCWSTR LinkName, CString& LinkTarget);

#if _MSC_VER >1500

HRESULT IsoCreateFileByPath(LPCWSTR pIsoPath, LPCWSTR pSrcDir, LPCWSTR VolumeName, BaseCallBack callBack, LPVOID pUserData);

#endif

HRESULT Base642Binary(CStringA& Binary, LPCWSTR Base64String, DWORD cchString = -1);

void Binary2Base64(const void* Src, DWORD ccbSrc, CString& Base64String);

HRESULT Base642Binary(BYTE* pBinary, DWORD& ccbBinary, LPCWSTR Base64String, DWORD cchString = -1);

void ReverseBinary(BYTE* pBinary, DWORD ccbBinary);

BOOL PathIsSameVolume(LPCWSTR Path1, LPCWSTR Path2);

HRESULT AdjustPrivilege(ULONG Privilege, BOOL Enable = TRUE);

HRESULT GetHashByFilePath(LPCWSTR FilePath, ALG_ID Algid, BYTE* pHashData, DWORD cbHashData);

HRESULT GetHashByData(LPCBYTE pData, DWORD cbData, ALG_ID Algid, BYTE* pHashData, DWORD cbHashData);

HRESULT GetMd5ByFilePath(LPCWSTR FilePath, BYTE FileMd5[16]);

HRESULT GetMd5ByData(LPCBYTE pData, DWORD cbData, BYTE FileMd5[16]);

HRESULT GetSha1ByFilePath(LPCWSTR FilePath, BYTE FileSha1[20]);

void PathCat(CString& Path, LPCWSTR Append);

CString PathCat(LPCWSTR Path, LPCWSTR Append);





HRESULT CreateFileByData(LPCWSTR FilePath, LPCWSTR lpName, LPCWSTR lpType, HMODULE hModule);

HRESULT CreateFileByData(LPCWSTR FilePath, const void* pData, DWORD ccbData);

HRESULT CreateFileByZipData(LPCWSTR FilePath, const void* Data, DWORD ccbData);

HRESULT CreateRoot(LPCWSTR FilePath);

HANDLE OpenDriver(LPCWSTR DriverPath, DWORD dwDesiredAccess = GENERIC_READ);

HRESULT GetDriverLayout(HANDLE hDevice, CStringA &Buffer/*DRIVE_LAYOUT_INFORMATION_EX+ n * PARTITION_INFORMATION_EX*/);

int GetPartitionDisk(HANDLE hDevice);

HRESULT GetPartitionInfomation(HANDLE hDevice, PARTITION_INFORMATION_EX& PartitionInfo);

int GetPartition(HANDLE hDevice);

HRESULT GetVhdVolumeFilePath(HANDLE hDevice, CString& VHDFilePath);

HRESULT GetVhdVolumeFilePath(LPCWSTR hDevicePath, CString& VHDFilePath);

DWORD GetDiskCount();


byte* IsProcExists(byte* pBase, LPCSTR ProcName);

bool IsProcExists(HANDLE hFile, LPCSTR ProcName);

HRESULT CompressBuffer(const void* Src, DWORD cbSrc, void* Dst, DWORD* pcbDst);

HRESULT DecompressBuffer(const void* Src, DWORD cbSrc, void* Dst, DWORD* pcbDst);


HRESULT NtPath2DosPath(LPCWSTR NtPath,CString& DosPath);

HRESULT LoadString(LPCWSTR FilePath, int Index, CString& String);

HRESULT LoadString(LPCWSTR FilePath, int Index, LPBSTR pString);


//HRESULT LoadString(HMODULE hModule,DWORD Index,CString& String);


//PROCESSOR_ARCHITECTURE_UNKNOWN
DWORD GetFileArchitecture(LPCWSTR FilePath);

void GetCtlCode(DWORD Code, DWORD& DeviceType, DWORD& Function, DWORD& Method, DWORD& Access);


PVOID64 GetProcAddressEx(HANDLE hProc, HMODULE hModule, LPCSTR lpProcName);

//GetLongPathName的安全封装
HRESULT GetLongPathNameW(_In_ LPCTSTR lpszShortPath, _Inout_ CString& lpszLongPath);


//IsReparseTagNameSurrogate可以使用此函数代替下面函数功能
//bool IsSimpleReparesPoint(DWORD dwReserved0);


byte Char2Hex(wchar_t ch);


CString Binary2String(const byte*pBinaryData, DWORD cbData);

CStringA HexString2Binary(LPCWSTR HexString);


HRESULT HresultFromBool();

