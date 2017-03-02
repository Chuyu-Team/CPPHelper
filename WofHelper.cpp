#pragma once
#include "WofHelper.h"
#include "handle.h"
#include "FileHelper.h"

BOOL IsWofCompress(LPCWSTR FilePath)
{
	CHFile hFile = CreateFile(FilePath, /*0x181*/GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0x0A000000, 0);
	if (hFile.IsInvalid())
		return FALSE;

	ExternalBacking Buffer = {};
	DWORD BytesReturned = 0;

	return DeviceIoControl(hFile, FSCTL_GET_EXTERNAL_BACKING/*0x90310*/, NULL, 0, &Buffer, sizeof(ExternalBacking), &BytesReturned, NULL);
}

HRESULT UnWofCompressFile(LPCWSTR FilePath)
{
	CHFile hFile = CreateFile(FilePath, 0x181, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0x0A000000, 0);
	if (hFile.IsInvalid())
		return GetLastError();

	DWORD BytesReturned = 0;

	if (!DeviceIoControl(hFile, FSCTL_DELETE_EXTERNAL_BACKING/*0x90314*/, NULL, 0, NULL, 0, &BytesReturned, NULL))
		return GetLastError();

	return S_OK;
}

HRESULT WofCompressFile(LPCWSTR FilePath, DWORD CompressType)
{
	if (IsWofCompress(FilePath))
	{
		return S_OK;
	}

	CHFile hFile = CreateFile(FilePath, 0x81, FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0x0A000000, 0);
	if (hFile.IsInvalid())
		return GetLastError();
	ExternalBacking Temp = { WOF_EXTERNAL_INFO{ 1, 2 }, WIM_PROVIDER_EXTERNAL_INFO{ 1, CompressType } };
	DWORD Ret = 0;

	return DeviceIoControl(hFile, FSCTL_SET_EXTERNAL_BACKING, &Temp, 0x14, NULL, 0, &Ret, NULL) ? S_OK : GetLastError();
}

HRESULT UnWofCompressRoot(CString RootPath)
{
	WIN32_FIND_DATA FindData;

	CHFileFind hFileFind = FindFirstFile(RootPath, &FindData);

	if (hFileFind.IsInvalid())
		return GetLastError();

	RootPath.ReleaseBufferSetLength(RootPath.ReverseFind(L'\\') + 1);


	do
	{
		if (FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if (_IsDots(FindData.cFileName))
				continue;

			UnWofCompressRoot(RootPath + FindData.cFileName + L"\\*");
		}
		else
		{
			UnWofCompressFile(RootPath + FindData.cFileName);
		}

	} while (FindNextFile(hFileFind, &FindData));

	return S_OK;
}
