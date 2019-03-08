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
#define ArraySize(Array)  _countof(Array)


//生成版本号
#define MakeVersion(v1,v2,v3,v4) (UINT64)(((UINT64)(v4))|((UINT64)v3<<16)|((UINT64)v2<<32)|((UINT64)v1<<48))

#define MakeMiniVersion(v1,v2) (DWORD)(v2|(v1<<16))

#define __StrA(x) #x
#define __StrW(x) _T(__StrA(x))


#define DISM_MSG_PROGRESS   38008     //进度信息wParam为当前完成百分比


#ifdef ENABLE_BACKUP_RESTORE
	#if ENABLE_BACKUP_RESTORE >=2
	//该模式根据实际情况动态调整
	extern DWORD REG_OPTION;
	#else
	#define REG_OPTION REG_OPTION_BACKUP_RESTORE
	#endif
#else
	#define REG_OPTION 0
#endif

_Success_(return == ERROR_SUCCESS)
typedef DWORD ( __stdcall *BaseCallBack)(
	_In_     DWORD MessageId,
	_In_opt_ WPARAM wParam,
	_In_opt_ LPARAM lParam,
	_In_opt_ PVOID UserData
	);




void FindPath(
	_In_   CStringI FilePath,
	_In_z_ LPCWSTR SubStr,
	_Out_  std::set<CStringI>& FindList,
	_In_   DWORD cchRootPath
	);


_Check_return_
LSTATUS RunCmd(
	_In_opt_z_ LPCWSTR FilePath,
	_In_       CString CmdString,
	_Out_opt_  CString* pOutString
	);

_Check_return_
LSTATUS RunCmd(
	_In_opt_z_ LPCWSTR FilePath,
	_In_       CString CmdString,
	_In_       bool Async = false
	);

_Check_return_
LSTATUS RunCmd(
	_In_opt_z_ LPCWSTR FilePath,
	_In_       CString CmdString,
	_In_opt_   BaseCallBack CallBack,
	_In_opt_   LPVOID UserData
	);

_Check_return_  _Success_(return == STATUS_SUCCESS)
NTSTATUS QuerySymbolicLinkObject(
	_In_z_ LPCWSTR LinkName,
	_Out_  CString& LinkTarget
	);

#if _MSC_VER >1500

_Check_return_ _Success_(return == S_OK)
HRESULT IsoCreateFileByPath(
	_In_z_     LPCWSTR pIsoPath,
	_In_z_     LPCWSTR pSrcDir,
	_In_opt_z_ LPCWSTR VolumeName,
	_In_opt_   BaseCallBack callBack,
	_In_opt_   LPVOID pUserData
	);

#endif

_Check_return_
LSTATUS Base642Binary(
	_Out_                      CStringA& Binary,
	_In_NLS_string_(cchString) LPCSTR    Base64String,
	_In_                       DWORD     cchString
	);

_Check_return_
LSTATUS Base642Binary(
	_Out_                      CStringA& Binary,
	_In_NLS_string_(cchString) LPCWSTR Base64String,
	_In_                       DWORD cchString = -1
	);

_Check_return_
LSTATUS Binary2Base64(
	_In_reads_bytes_(ccbSrc) const void* Src,
	_In_                     DWORD ccbSrc,
	_Out_                    CString& Base64String
	);

_Check_return_
LSTATUS Base642Binary(
	_Out_writes_bytes_to_(ccbBinary, ccbBinary) BYTE* pBinary,
	_Inout_                                     DWORD& ccbBinary,
	_In_NLS_string_(cchString)                  LPCWSTR Base64String,
	_In_                                        DWORD cchString = -1
	);

void ReverseBinary(
	_Inout_updates_bytes_(ccbBinary) BYTE* pBinary,
	_In_                             DWORD ccbBinary
	);

_Check_return_
BOOL PathIsSameVolume(
	_In_z_ LPCWSTR Path1,
	_In_z_ LPCWSTR Path2
	);

_Check_return_ _Success_(return == STATUS_SUCCESS)
NTSTATUS AdjustPrivilege(
	_In_ ULONG Privilege,
	_In_ BOOL Enable = TRUE
	);

_Check_return_
LSTATUS GetHashByFilePath(
	_In_z_                                                 LPCWSTR FilePath,
	_In_                                                   ALG_ID Algid,
	_Out_writes_bytes_(cbHashData)                         BYTE* pHashData,
	_In_
		_When_(Algid == CALG_MD5, _In_range_(>= , 16))
		_When_(Algid == CALG_SHA1, _In_range_(>= , 20))
		_When_(Algid == CALG_SHA_256, _In_range_(>= , 32)) DWORD cbHashData
	);

_Check_return_
LSTATUS GetHashByData(
	_In_reads_bytes_(cbData)                              LPCBYTE pData,
	_In_                                                  DWORD cbData,
	_In_                                                  ALG_ID Algid,
	_Out_writes_bytes_(cbHashData)                        BYTE* pHashData,
	_In_
		_When_(Algid == CALG_MD5, _In_range_(>= , 16))
		_When_(Algid == CALG_SHA1, _In_range_(>= , 20))
		_When_(Algid == CALG_SHA_256, _In_range_(>= , 32)) DWORD cbHashData
	);

_Check_return_
LSTATUS GetMd5ByFilePath(
	_In_z_                 LPCWSTR FilePath,
	_Out_writes_bytes_(16) BYTE FileMd5[16]
	);

_Check_return_
LSTATUS GetMd5ByData(
	_In_reads_bytes_(cbData) LPCBYTE pData,
	_In_                     DWORD   cbData,
	_Out_writes_bytes_(16)   BYTE    FileMd5[16]
	);

_Check_return_
LSTATUS GetSha1ByFilePath(
	_In_z_                 LPCWSTR FilePath,
	_Out_writes_bytes_(20) BYTE    FileSha1[20]
	);

void PathCat(
	_In_       CString& Path,
	_In_opt_z_ LPCWSTR  Append
	);

CString PathCat(
	_In_z_ LPCWSTR Path,
	_In_opt_z_ LPCWSTR Append
	);




_Check_return_
LSTATUS CreateFileByData(
	_In_z_ LPCWSTR FilePath,
	_In_z_ LPCWSTR lpName,
	_In_z_ LPCWSTR lpType,
	_In_   HMODULE hModule
	);

_Check_return_
LSTATUS CreateFileByData(
	_In_z_                    LPCWSTR     FilePath,
	_In_reads_bytes_(ccbData) const void* pData,
	_In_                      DWORD       ccbData
	);

_Check_return_
LSTATUS CreateFileByZipData(
	_In_z_                    LPCWSTR     FilePath,
	_In_reads_bytes_(ccbData) const void* Data,
	_In_                      DWORD       ccbData
	);

_Check_return_
LSTATUS CreateRoot(
	_In_z_ LPCWSTR FilePath
	);

_Check_return_ _Success_(return != INVALID_HANDLE_VALUE)
HANDLE OpenDriver(
	_In_z_ LPCWSTR DriverPath,
	_In_   DWORD   dwDesiredAccess = GENERIC_READ
	);

_Check_return_
LSTATUS GetDriverLayout(
	_In_  HANDLE    hDevice,
	_Out_ CStringA& Buffer/*DRIVE_LAYOUT_INFORMATION_EX+ n * PARTITION_INFORMATION_EX*/
	);

_Check_return_
LSTATUS GetDriverLayout(
	_In_z_ LPCWSTR DriverPath,
	_Out_ CStringA& Buffer
	);

_Check_return_  _Success_(return == -1)
int GetPartitionDisk(
	_In_ HANDLE hDevice
	);

_Check_return_
LSTATUS GetPartitionInfomation(
	_In_  HANDLE                    hDevice,
	_Out_ PARTITION_INFORMATION_EX& PartitionInfo
	);

_Check_return_ _Success_(return == -1)
int GetPartition(
	_In_ HANDLE hDevice
	);

_Check_return_
LSTATUS GetVhdVolumeFilePath(
	_In_  HANDLE   hDevice,
	_Out_ CString& VHDFilePath
	);

_Check_return_
LSTATUS GetVhdVolumeFilePath(
	_In_z_ LPCWSTR  hDevicePath,
	_Out_  CString& VHDFilePath
	);

_Check_return_
LSTATUS GetDiskCount(
	_Out_ std::vector<CString>& pszDevicePath
	);

_Check_return_ _Success_(return != nullptr)
byte* IsProcExists(
	_In_   byte*  pBase,
	_In_z_ LPCSTR ProcName
	);

_Check_return_
bool IsProcExists(
	_In_   HANDLE hFile,
	_In_z_ LPCSTR ProcName
	);

_Check_return_ _Success_(return == 0)
NTSTATUS CompressBuffer(
	_In_reads_bytes_(cbSrc)                 const void* Src,
	_In_                                    DWORD       cbSrc,
	_Out_writes_bytes_to_(*pcbDst, *pcbDst) void*       Dst,
	_Inout_                                 DWORD*      pcbDst
	);

_Check_return_ _Success_(return == 0)
NTSTATUS DecompressBuffer(
	_In_reads_bytes_(cbSrc)                 const void* Src,
	_In_                                    DWORD       cbSrc,
	_Out_writes_bytes_to_(*pcbDst, *pcbDst) void*       Dst,
	_Inout_                                 DWORD*      pcbDst
	);

_Check_return_ _Success_(return == S_OK)
HRESULT NtPath2DosPath(
	_In_z_ LPCWSTR  NtPath,
	_Out_  CString& DosPath
	);

_Check_return_ _Success_(return)
BOOL LoadString_s(
	_In_z_ LPCWSTR   FilePath,
	_In_   int       Index,
	_Out_  CStringW& String
	);

_Check_return_ _Success_(return)
BOOL LoadString_s(
	_In_z_            LPCWSTR FilePath,
	_In_              int     Index,
	_Outptr_result_z_ LPBSTR  pString
	);


//HRESULT LoadString(HMODULE hModule,DWORD Index,CString& String);

#define IMAGE_FIRST_DIRECTORY(ntheader) (IMAGE_DATA_DIRECTORY*)((byte*)IMAGE_FIRST_SECTION(ntheader)-sizeof(IMAGE_DATA_DIRECTORY)*IMAGE_NUMBEROF_DIRECTORY_ENTRIES)

_Check_return_ _Success_(return != PROCESSOR_ARCHITECTURE_UNKNOWN)
DWORD GetFileArchitecture(
	_In_z_ LPCWSTR FilePath
	);

//void GetCtlCode(DWORD Code, DWORD& DeviceType, DWORD& Function, DWORD& Method, DWORD& Access);

_Check_return_ _Success_(return != nullptr)
PVOID64 GetProcAddressEx(
	_In_   HANDLE  hProc,
	_In_   HMODULE hModule,
	_In_z_ LPCSTR  lpProcName
	);

//GetLongPathNameW的安全封装
_Check_return_
LSTATUS GetLongPathName_s(
	_In_  LPCWSTR   lpszShortPath,
	_Out_ CStringW& lpszLongPath
	);

//ExpandEnvironmentStringsW的安全封装
_Check_return_
LSTATUS __fastcall  ExpandEnvironmentStrings_s(
    _In_  LPCWSTR   lpszShortPath,
    _Out_ CStringW& lpszLongPath
    );

//GetVolumePathNameW的安全封装
_Check_return_
LSTATUS __fastcall GetVolumePathName_s(
	_In_z_  LPCWSTR   lpszFileName,
	_Out_   CStringW& lpszLongPath
	);

//GetModuleFileNameW的安全封装
LSTATUS __fastcall GetModuleFileName_s(
	_In_opt_ HMODULE   hModule,
	_Out_    CStringW& szModulePath
	);
//IsReparseTagNameSurrogate可以使用此函数代替下面函数功能
//bool IsSimpleReparesPoint(DWORD dwReserved0);

_Check_return_ _Success_(return <= 0xF)
byte Char2Hex(
	_In_ wchar_t ch
	);


CString Binary2String(
	_In_reads_bytes_(cbData) const byte* pBinaryData,
	_In_                     DWORD       cbData
	);

CStringA HexString2Binary(
	_In_z_ LPCWSTR HexString
	);

CStringA HexString2Binary(
	_In_z_ LPCSTR HexString
	);


HRESULT HresultFromBool(
	_In_ BOOL bSuccess = FALSE
	);

HRESULT HresultFromBoolFalse();

//安全版GetLastError，必定返回一个错误代码。
_Ret_
LSTATUS GetLastError_s(
	_In_ BOOL bSuccess = FALSE
	);

LSTATUS GetLastErrorFromBoolFalse();

//检测是否是兼容模式
_Check_return_ _Success_(return !=-1)
BOOL IsCompatibilityMode();

//获取2位小版本号，比如6.1
DWORD DirectGetOsMinVersion();

//获取3位版本号，最后一位必定为0，比如6.1.7601.0
UINT64 DirectGetOsVersion();

//LocalFree释放空间
_Check_return_
_CRTALLOCATOR LPCWSTR __cdecl FormatLongString(_In_z_ LPCWSTR _Format, ...);

LSTATUS __fastcall ModuleAddRef(
	_In_ HMODULE hModule
	);

namespace internal
{

	template<class T>
	class range_based_for_container_array
	{
	private:
		T* pBegin;
		T* pEnd;
	public:
		__forceinline __fastcall range_based_for_container_array(T* __pBegin, T* __pEnd)
			: pBegin(__pBegin)
			, pEnd(__pEnd)
		{

		}

		__forceinline T* begin()
		{
			return pBegin;
		}

		__forceinline T* end()
		{
			return pEnd;
		}
	};
}


template<class T>
static
__forceinline
internal::range_based_for_container_array<T> __fastcall range_based_for(T* pData, size_t Count)
{
	return internal::range_based_for_container_array<T>(pData, pData + Count);
}


template<class T>
static
__forceinline
internal::range_based_for_container_array<T> __fastcall range_based_for(T* pBegin, T* pEnd)
{
	return internal::range_based_for_container_array<T>(pBegin, pEnd);
}