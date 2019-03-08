#pragma once
#include <Windows.h>
#include <atlstr.h>
#include "km.h"
#include <map>

/////////////////////////////////////////////////////////////////////////////
//
//                       提供文件基础操作
//
/////////////////////////////////////////////////////////////////////////////

//UTF16文件头
const BYTE BomUTF16[] = { 0xff,0xfe };

//UTF8文件头
const BYTE BomUTF8[] = { 0xEF ,0xBB ,0xBF };

#define YY_range_base_for_find_file_start(szFindPath, _FindFileData)         \
{                                                                            \
    WIN32_FIND_DATAW ___range_base_for_find_file_data;                       \
    WIN32_FIND_DATAW& _FindFileData = ___range_base_for_find_file_data;      \
    CHFileFind hFileFind = FindFirstFileW(szFindPath, &_FindFileData);       \
    if(hFileFind.IsInvalid() == false)                                       \
	{                                                                        \
        do


#define YY_range_base_for_find_file_end()                                    \
        while (FindNextFileW(hFileFind, &___range_base_for_find_file_data)); \
	}                                                                        \
}


#define YY_range_base_for_find_file_end_with_errorcode(errorcode)                                                                     \
        while (FindNextFileW(hFileFind, &___range_base_for_find_file_data) ? true : (errorcode = GetLastErrorFromBoolFalse(),false)); \
	}                                                                                                                                 \
    else                                                                                                                              \
    {                                                                                                                                 \
        errorcode = GetLastErrorFromBoolFalse();                                                                                      \
    }                                                                                                                                 \
}

//判断问是否是.以及..
_Check_return_
bool __fastcall IsDots(
	_In_z_ LPCWSTR FileName,
	_In_   DWORD   cbFileName
	);

_Check_return_
bool __fastcall IsDots(
	_In_z_ LPCWSTR FileName
	);

_Check_return_
bool __fastcall IsDots(
	_In_z_ LPCSTR FileName
	);

#define _IsDots IsDots

_Check_return_ _Success_(return == 0)
NTSTATUS __fastcall GetFileId(
	_In_z_     LPCWSTR                    FilePath,
	_Out_opt_  FILE_STANDARD_INFORMATION* pFileStandardInfo,
	_Out_opt_  FILE_INTERNAL_INFORMATION* pFileInternalInfo = NULL
	);

_Check_return_ _Success_(return == 0)
NTSTATUS __fastcall NtGetFileId(
	_In_      POBJECT_ATTRIBUTES         ObjectAttributes,
	_Out_opt_ FILE_STANDARD_INFORMATION* pFileStandardInfo,
	_Out_opt_ FILE_INTERNAL_INFORMATION* pFileInternalInfo
	);

/*无视权限删除文件或者文件夹
DeleteRootPath指示是否删除根目录，当Path为文件时此产生无效*/
_Check_return_ _Success_(return == 0)
NTSTATUS DeleteDirectory(
	_In_z_ LPCWSTR Path,
	_In_   BOOL    DeleteRootPath = TRUE
	);

_Check_return_
LSTATUS UpdateFile(
	_In_ CString lpExistingFileName,
	_In_ CString lpNewFileName
	);

_Check_return_ _Success_(return == 0)
NTSTATUS CopyDirectory(
	_In_z_ LPCWSTR ExistingDirectoryPath,
	_In_z_ LPCWSTR NewDirectoryInfoPath
	);

//DWORD MoveFileEx2(LPCTSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags = MOVEFILE_REPLACE_EXISTING);

//无视权限删除文件或者（空）文件夹
_Check_return_ _Success_(return == 0)
NTSTATUS DeleteFile2(
	_In_z_ LPCWSTR FilePath
	);

_Check_return_ _Success_(return == 0)
NTSTATUS NtDeleteFile2(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes
	);

_Check_return_ _Success_(return != 0xFFFFFFFF)
DWORD GetFileAttributes2(
	_In_z_ LPCTSTR FilePath
	);

_Check_return_ _Success_(return == 0)
NTSTATUS NtSetFileAttributes(
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_ DWORD FileAttributes
	);

_Check_return_ _Success_(return == 0)
NTSTATUS SetFileAttributes2(
	_In_z_ LPCTSTR FilePath,
	_In_ DWORD FileAttributes
	);

UINT64 GetFileAllocationSize(
	_In_z_ LPCWSTR FilePath
	);

UINT64 GetDirectorySize(
	_In_z_ LPCWSTR FilePath
	);

//可以NTFS压缩一个文件/文件夹
_Check_return_
LSTATUS CompressFile(
	_In_z_ LPCWSTR FilePath
	);

ULONG64 GetDriverFreeSize(
	_In_z_ LPCWSTR Latter
	);

ULONG64 GetDriverUseSize(
	_In_z_ LPCWSTR Latter
	);

_Check_return_
BOOL CheckUpdateFile(
	_In_      CString                  Src,
	_In_      CString                  Des,
	_Out_     std::map<UINT64, DWORD>& FileMap,
	_Out_opt_ UINT64*                  pSize
	);

_Check_return_
LSTATUS GetFileVersion(
	_In_                  HMODULE hFileMoudle,
	_Out_writes_bytes_(8) UINT16  Version[4],
	_In_                  WORD    wLanguage   = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
	);

_Check_return_
LSTATUS GetFileVersion(
	_In_z_                LPCWSTR FilePath,
	_Out_writes_bytes_(8) UINT16  Version[4],
	_In_                  WORD    wLanguage  = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)
	);

UINT64 GetDirectoryAllocationSize(
	_In_z_ LPCWSTR                  FilePath,
	_Out_  std::map<UINT64, DWORD>& FileMap
	);

_Check_return_ _Success_(return == S_OK)
LSTATUS CrateDirectorHandLink(
	_In_ CString To,
	_In_ CString From
	);

//Error Code https://msdn.microsoft.com/en-us/library/windows/desktop/bb762164(v=vs.85).aspx
_Check_return_ _Success_(return == 0)
int MoveDirectory(
	_In_ CString ExistingDirectoryPath,
	_In_ CString NewDirectoryInfoPath
	);


enum FilePathType
{
	PathIsUnkow = -1,
	PathNotExist,
	PathIsFile,
	PathIsDir,
};

_Check_return_
FilePathType GetFileType(
	_In_z_ LPCWSTR FilePath
	);

_Check_return_
FilePathType GetFileType(
	_In_z_ LPCSTR FilePath
	);