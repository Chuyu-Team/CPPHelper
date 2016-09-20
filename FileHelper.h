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



//判断问是否是.以及..
bool __fastcall IsDots(LPCWSTR FileName, DWORD cbFileName);
bool __fastcall IsDots(LPCWSTR FileName);
bool __fastcall IsDots(LPCSTR FileName);

#define _IsDots IsDots


NTSTATUS __fastcall GetFileId(LPCWSTR FilePath, FILE_STANDARD_INFORMATION* pFileStandardInfo, FILE_INTERNAL_INFORMATION* pFileInternalInfo = NULL);

NTSTATUS __fastcall NtGetFileId(POBJECT_ATTRIBUTES ObjectAttributes, FILE_STANDARD_INFORMATION* pFileStandardInfo, FILE_INTERNAL_INFORMATION* pFileInternalInfo);

/*无视权限删除文件或者文件夹
DeleteRootPath指示是否删除根目录，当Path为文件时此产生无效*/
NTSTATUS DeleteDirectory(LPCWSTR Path, BOOL DeleteRootPath = TRUE);

HRESULT UpdateFile(CString lpExistingFileName, CString lpNewFileName);

NTSTATUS CopyDirectory(LPCWSTR ExistingDirectoryPath, LPCWSTR NewDirectoryInfoPath);

//DWORD MoveFileEx2(LPCTSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags = MOVEFILE_REPLACE_EXISTING);

//无视权限删除文件或者（空）文件夹
NTSTATUS DeleteFile2(LPCWSTR FilePath);
NTSTATUS NtDeleteFile2(POBJECT_ATTRIBUTES ObjectAttributes);

DWORD GetFileAttributes2(LPCTSTR FilePath);
NTSTATUS NtSetFileAttributes(POBJECT_ATTRIBUTES ObjectAttributes, DWORD FileAttributes);

NTSTATUS SetFileAttributes2(LPCTSTR FilePath, DWORD FileAttributes);

UINT64 GetFileAllocationSize(LPCWSTR FilePath);

UINT64 GetDirectorySize(LPCWSTR FilePath);

//可以NTFS压缩一个文件/文件夹
HRESULT CompressFile(LPCWSTR FilePath);

ULONG64 GetDriverFreeSize(LPCWSTR Latter);

ULONG64 GetDriverUseSize(LPCWSTR Latter);


BOOL CheckUpdateFile(CString Src, CString Des, std::map<UINT64, DWORD>&FileMap, UINT64* pSize);

HRESULT GetFileVersion(HMODULE hFileMoudle, UINT16 Version[4]);

HRESULT GetFileVersion(LPCWSTR FilePath, UINT16 Version[4]);

UINT64 GetDirectoryAllocationSize(LPCWSTR FilePath, std::map<UINT64, DWORD>& FileMap);

HRESULT CrateDirectorHandLink(CString To, CString From);

HRESULT MoveDirectory(CString ExistingDirectoryPath, CString NewDirectoryInfoPath);


enum FilePathType
{
	PathIsUnkow = -1,
	PathNotExist,
	PathIsFile,
	PathIsDir,
};

FilePathType GetFileType(LPCWSTR FilePath);

FilePathType GetFileType(LPCSTR FilePath);