#pragma once

#include "FileHelper.h"
#include "StringHelper.h"
#include "handle.h"
#pragma comment (lib,"Version.lib")
#pragma comment (lib,"KtmW32.lib")

#pragma warning(push)
#pragma warning(disable: 28251)

bool __fastcall IsDots(LPCWSTR FileName, DWORD cbFileName)
{
	switch (cbFileName)
	{
	case 4:
		return *FileName == L'.'&&FileName[1] == L'.';
	case 2:
		return *FileName == L'.';
	default:
		return false;
	}
}

bool __fastcall IsDots(LPCSTR FileName)
{
	return ((*FileName == '.' && ((FileName)[1] == NULL || ((FileName)[1] == '.' && (FileName)[2] == NULL))));
}

bool __fastcall IsDots(LPCWSTR FileName)
{
	return ((*FileName == L'.' && ((FileName)[1] == NULL || ((FileName)[1] == L'.' && (FileName)[2] == NULL))));
}


NTSTATUS __fastcall GetFileId(LPCWSTR FilePath, FILE_STANDARD_INFORMATION* pFileStandardInfo, FILE_INTERNAL_INFORMATION* pFileInternalInfo)
{
	UNICODE_STRING usFileName;

	if (!RtlDosPathNameToNtPathName_U(FilePath, &usFileName, NULL, NULL))
	{
		auto Status = RtlGetLastNtStatus();

		return Status ? Status : STATUS_INVALID_PARAMETER;
	}

	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &usFileName, OBJ_CASE_INSENSITIVE };

	auto Status = NtGetFileId(&ObjectAttributes, pFileStandardInfo, pFileInternalInfo);

	RtlFreeUnicodeString(&usFileName);


	return Status;
}

NTSTATUS __fastcall NtGetFileId(POBJECT_ATTRIBUTES ObjectAttributes, FILE_STANDARD_INFORMATION* pFileStandardInfo, FILE_INTERNAL_INFORMATION* pFileInternalInfo)
{
	HANDLE hFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), hRootDir, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;


	auto Status = NtOpenFile(&hFile, SYNCHRONIZE, ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
		return Status;
	if (pFileStandardInfo)
	{
		if (Status = NtQueryInformationFile(hFile, &IoStatusBlock, pFileStandardInfo, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation))
		{
			goto End;
		}
	}

	if (pFileInternalInfo)
	{
		if (Status = NtQueryInformationFile(hFile, &IoStatusBlock, pFileInternalInfo, sizeof(FILE_INTERNAL_INFORMATION), FileInternalInformation))
		{
			goto End;
		}
	}


End:

	NtClose(hFile);

	return Status;
}

NTSTATUS DeleteDirectoryInternal(OBJECT_ATTRIBUTES ObjectAttributes)
{
	HANDLE hFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), hRootDir, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;


	auto Status = NtOpenFile(&hFile, SYNCHRONIZE | FILE_LIST_DIRECTORY, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
		return Status;

	byte Buffer[sizeof(FILE_FULL_DIR_INFORMATION) + 512];


	FILE_FULL_DIR_INFORMATION & FileInfo = *(FILE_FULL_DIR_INFORMATION *)Buffer;

	UNICODE_STRING TempFileName = { 0,0,FileInfo.FileName };

	ObjectAttributes.RootDirectory = hFile;
	ObjectAttributes.ObjectName = &TempFileName;

	NTSTATUS Error = 0;

	while (ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, &IoStatusBlock, Buffer, sizeof(Buffer), FileFullDirectoryInformation, -1, NULL, 0) == ERROR_SUCCESS)
	{
		TempFileName.Length = TempFileName.MaximumLength = FileInfo.FileNameLength;

		if (FileInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if ((FileInfo.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT))
			{
				if (IsReparseTagNameSurrogate(FileInfo.EaSize))
					goto StartDelete;
			}
			else
			{
				if (IsDots(FileInfo.FileName, FileInfo.FileNameLength))
					continue;
			}

			if (Error = DeleteDirectoryInternal(ObjectAttributes))
			{
				goto OnError;
			}
		}

	StartDelete:

		if (FileInfo.FileAttributes&FILE_ATTRIBUTE_READONLY)
		{
			//取消只读属性
			if (Error = NtSetFileAttributes(&ObjectAttributes, FileInfo.FileAttributes^FILE_ATTRIBUTE_READONLY))
			{
				goto OnError;
			}
		}

		if (Error = NtDeleteFile2(&ObjectAttributes))
		{
			if (FileInfo.FileAttributes&FILE_ATTRIBUTE_READONLY)
			{
				//删除失败恢复只读属性
				NtSetFileAttributes(&ObjectAttributes, FileInfo.FileAttributes);
			}

		OnError:
			Status = Error;
		}
	}
//End:

	NtClose(hFile);

	return Status;
}


NTSTATUS DeleteDirectory(LPCWSTR Path, BOOL DeleteRootPath)
{
	UNICODE_STRING usFileName;

	if (!RtlDosPathNameToNtPathName_U(Path, &usFileName, NULL, NULL))
	{
		return RtlGetLastNtStatus();
	}

	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &usFileName, OBJ_CASE_INSENSITIVE };

	auto Stateus = DeleteDirectoryInternal(ObjectAttributes);

	if (!Stateus&&DeleteRootPath)
	{
		Stateus = NtDeleteFile2(&ObjectAttributes);
	}


	RtlFreeUnicodeString(&usFileName);

	return Stateus;
}

//获取文件物理大小
UINT64 GetFileAllocationSize(LPCWSTR FilePath)
{
	HANDLE hFile = CreateFile(FilePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

	FILE_STANDARD_INFORMATION info = { 0 };

	if (hFile != INVALID_HANDLE_VALUE)
	{
		IO_STATUS_BLOCK IoStatusBlock;

		NtQueryInformationFile(hFile, &IoStatusBlock, &info, sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
		//GetFileInformationByHandleEx(hFile, FileStandardInfo, &info, sizeof(info));

		CloseHandle(hFile);
	}

	return info.AllocationSize.QuadPart;
}

//bool IsSimpleReparesPoint(DWORD dwReserved0)
//{
//	switch (dwReserved0)
//	{
//	case IO_REPARSE_TAG_SYMLINK:
//	case IO_REPARSE_TAG_MOUNT_POINT:
//	case IO_REPARSE_TAG_NFS:
//		return true;
//		break;
//	default:
//		return false;
//		break;
//	}
//}

UINT64 GetDirectorySizeInternal(OBJECT_ATTRIBUTES ObjectAttributes)
{
	HANDLE hFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), hRootDir, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;


	auto Status = NtOpenFile(&hFile, SYNCHRONIZE | FILE_LIST_DIRECTORY, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
		return 0;

	UINT64 Space = 0;

	byte Buffer[sizeof(FILE_FULL_DIR_INFORMATION) + 512];


	FILE_FULL_DIR_INFORMATION & FileInfo = *(FILE_FULL_DIR_INFORMATION *)Buffer;

	UNICODE_STRING TempFileName = { 0,0,FileInfo.FileName };

	ObjectAttributes.RootDirectory = hFile;
	ObjectAttributes.ObjectName = &TempFileName;

	while (ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, &IoStatusBlock, Buffer, sizeof(Buffer), FileFullDirectoryInformation, -1, NULL, 0) == ERROR_SUCCESS)
	{
		if (FileInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FileInfo.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
			{
				if (IsReparseTagNameSurrogate(FileInfo.EaSize))
					continue;
			}
			else
			{
				if (IsDots(FileInfo.FileName, FileInfo.FileNameLength))
					continue;
			}

			TempFileName.Length = TempFileName.MaximumLength = FileInfo.FileNameLength;

			Space += GetDirectorySizeInternal(ObjectAttributes);
		}
		else
		{
			Space += FileInfo.AllocationSize.QuadPart;
		}
	}

	NtClose(hFile);

	return Space;
}

UINT64 GetDirectorySize(LPCWSTR FilePath)
{
	if (StrEmpty(FilePath))
		return 0;
	UNICODE_STRING NtName;

	if (!RtlDosPathNameToNtPathName_U(FilePath, &NtName, NULL, NULL))
		return 0;

	auto Space = GetDirectorySizeInternal(OBJECT_ATTRIBUTES{ sizeof(OBJECT_ATTRIBUTES), NULL, &NtName, OBJ_CASE_INSENSITIVE });

	RtlFreeUnicodeString(&NtName);

	return Space;
}


//static UINT64 GetDirectoryAllocationSize(CString _FilePath)
//{
//	UINT64 Space = 0;
//
//	if (_FilePath.GetLength() && _FilePath[_FilePath.GetLength() - 1] != L'\\')
//	{
//		_FilePath += L'\\';
//	}
//
//	WIN32_FIND_DATAW FindFileData;
//	auto hFileFind = FindFirstFileW(_FilePath+L'*', &FindFileData);
//
//	if (hFileFind != INVALID_HANDLE_VALUE)
//	{
//		do
//		{
//			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
//			{
//				if (_IsDots(FindFileData.cFileName) == 0)
//				{
//					Space += GetDirectoryAllocationSize(_FilePath + FindFileData.cFileName);
//				}
//			}
//			else
//			{
//				Space += GetFileAllocationSize(_FilePath + FindFileData.cFileName);
//			}
//		} while (FindNextFileW(hFileFind, &FindFileData));
//
//		FindClose(hFileFind);
//	}
//
//	return Space;
//}



//可以压缩一个文件/文件夹
HRESULT CompressFile(LPCWSTR FilePath)
{
	LSTATUS lSataus = ERROR_SUCCESS;
	USHORT Type = COMPRESSION_FORMAT_XPRESS_HUFF; //设置为极限压缩
	DWORD BytesReturned;

	HANDLE hFile = CreateFile(FilePath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		//char Buffer[sizeof(WOF_EXTERNAL_INFO) + sizeof(WIM_PROVIDER_EXTERNAL_INFO)];

		//((WOF_EXTERNAL_INFO*)Buffer)->Provider = WOF_PROVIDER_WIM;
		//((WOF_EXTERNAL_INFO*)Buffer)->Version = WOF_CURRENT_VERSION;
		//IO_STATUS_BLOCK IoStatusBlock;

		//auto hr=ZwFsControlFile(hFile, NULL, NULL, NULL, &IoStatusBlock, FSCTL_GET_EXTERNAL_BACKING, NULL, 0, Buffer, sizeof(Buffer));

		//if (DeviceIoControl(hFile, FSCTL_GET_EXTERNAL_BACKING, NULL, 0, Buffer,20, &BytesReturned, 0))
		{
			if (DeviceIoControl(hFile, FSCTL_SET_COMPRESSION, &Type, sizeof(Type), 0, 0, &BytesReturned, 0) == FALSE)
			{
				lSataus = GetLastError_s();
			}
		}




		CloseHandle(hFile);

		return lSataus;
	}
	else
	{
		return GetLastError_s();
	}


}

ULONG64 GetDriverFreeSize(LPCWSTR Latter)
{
	DWORD sectorsPerCluster = 0, bytesPerSector = 0, numberOfFreeCluster = 0, numberOfTotalCluster = 0;
	GetDiskFreeSpaceW(Latter, &sectorsPerCluster, &bytesPerSector, &numberOfFreeCluster, &numberOfTotalCluster);

	return (ULONG64)sectorsPerCluster*bytesPerSector * numberOfFreeCluster;
}


ULONG64 GetDriverUseSize(LPCWSTR Latter)
{
	DWORD sectorsPerCluster = 0, bytesPerSector = 0, numberOfFreeCluster = 0, numberOfTotalCluster = 0;
	GetDiskFreeSpaceW(Latter, &sectorsPerCluster, &bytesPerSector, &numberOfFreeCluster, &numberOfTotalCluster);

	return (ULONG64)sectorsPerCluster*bytesPerSector * (numberOfTotalCluster - numberOfFreeCluster);
}


NTSTATUS NtDeleteFile2(POBJECT_ATTRIBUTES ObjectAttributes)
{
	HANDLE hFile;

	IO_STATUS_BLOCK IoStatusBlock;

	auto Status = NtOpenFile(&hFile, SYNCHRONIZE | DELETE, ObjectAttributes, &IoStatusBlock, FILE_SHARE_DELETE | FILE_SHARE_READ, FILE_OPEN_REPARSE_POINT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
	{
		return Status;
	}

	FILE_DISPOSITION_INFORMATION Dispostion = { TRUE };

	Status = NtSetInformationFile(hFile, &IoStatusBlock, &Dispostion, sizeof(Dispostion), FileDispositionInformation);

	NtClose(hFile);

	if (Status == 0)
		return 0;

	//使用文件取代法删除
	auto cbData = ObjectAttributes->ObjectName->Length + sizeof(L"~$") - 2;
	UNICODE_STRING TempFileName = { cbData,cbData, (wchar_t*)new byte[cbData] };

	memcpy(TempFileName.Buffer, ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
	*(wchar_t*)(((byte*)TempFileName.Buffer) + ObjectAttributes->ObjectName->Length) = L'~';
	*(wchar_t*)(((byte*)TempFileName.Buffer) + ObjectAttributes->ObjectName->Length + 2) = L'$';


	OBJECT_ATTRIBUTES ObjectAttributes2 = { sizeof(OBJECT_ATTRIBUTES),ObjectAttributes->RootDirectory, &TempFileName, OBJ_CASE_INSENSITIVE };

	auto Status2 = NtCreateFile(&hFile, SYNCHRONIZE | DELETE, &ObjectAttributes2, &IoStatusBlock, 0, FILE_ATTRIBUTE_NORMAL, 0, FILE_CREATE, FILE_OPEN_FOR_BACKUP_INTENT | FILE_DELETE_ON_CLOSE, NULL, 0);

	delete[](byte*)TempFileName.Buffer;


	cbData = sizeof(FILE_RENAME_INFORMATION) + ObjectAttributes->ObjectName->Length;

	FILE_RENAME_INFORMATION* pRenameInfo = (FILE_RENAME_INFORMATION*)new byte[cbData];

	memcpy(pRenameInfo->FileName, ObjectAttributes->ObjectName->Buffer, ObjectAttributes->ObjectName->Length);
	pRenameInfo->ReplaceIfExists = TRUE;
	pRenameInfo->RootDirectory = ObjectAttributes->RootDirectory;
	pRenameInfo->FileNameLength = ObjectAttributes->ObjectName->Length;
	//FILE_RENAME_INFORMATION RenameInfo = {TRUE,hRootDir ,FileName.};

	Status2 = NtSetInformationFile(hFile, &IoStatusBlock, pRenameInfo, cbData, FileRenameInformation);

	delete[](byte*)pRenameInfo;

	NtClose(hFile);

	return Status2 == 0 ? 0 : Status;
}

NTSTATUS NtSetFileAttributes(POBJECT_ATTRIBUTES ObjectAttributes, DWORD FileAttributes)
{
	HANDLE hFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;

	auto Status = NtOpenFile(&hFile, FILE_WRITE_ATTRIBUTES | SYNCHRONIZE, ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
	{
		return Status;
	}
	FILE_BASIC_INFORMATION Info = { {},{},{},{}, FileAttributes };

	Status = NtSetInformationFile(hFile, &IoStatusBlock, &Info, sizeof(Info), FileBasicInformation);

	NtClose(hFile);

	return Status;
}

NTSTATUS SetFileAttributes2(LPCTSTR FilePath, DWORD FileAttributes)
{
	UNICODE_STRING NtName;

	if (!RtlDosPathNameToNtPathName_U(FilePath, &NtName, NULL, NULL))
	{
		return RtlGetLastNtStatus();
	}
	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &NtName, OBJ_CASE_INSENSITIVE };

	auto Status = NtSetFileAttributes(&ObjectAttributes, FileAttributes);

	RtlFreeUnicodeString(&NtName);

	return Status;
}

DWORD GetFileAttributes2(POBJECT_ATTRIBUTES ObjectAttributes)
{
	HANDLE hFile;
	IO_STATUS_BLOCK IoStatusBlock;

	auto Status = NtOpenFile(&hFile, FILE_READ_ATTRIBUTES | SYNCHRONIZE, ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
	{
		goto Error;
	}


	FILE_BASIC_INFORMATION Info;

	Status = NtQueryInformationFile(hFile, &IoStatusBlock, &Info, sizeof(Info), FileBasicInformation);

	NtClose(hFile);

	if (Status)
	{
	Error:

		SetLastError(RtlNtStatusToDosError(Status));

		return -1;
	}
	else
	{
		return Info.FileAttributes;
	}
}

DWORD GetFileAttributes2(LPCTSTR FilePath)
{
	UNICODE_STRING usFileName;

	if (!RtlDosPathNameToNtPathName_U(FilePath, &usFileName, NULL, NULL))
	{
		return -1;
	}

	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &usFileName, OBJ_CASE_INSENSITIVE };

	auto Attr= GetFileAttributes2(&ObjectAttributes);

	RtlFreeUnicodeString(&usFileName);


	return Attr;
}

NTSTATUS DeleteFile2(LPCWSTR FilePath)
{
	UNICODE_STRING usFileName;

	if (!RtlDosPathNameToNtPathName_U(FilePath, &usFileName, NULL, NULL))
	{
		return RtlGetLastNtStatus();
	}

	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &usFileName, OBJ_CASE_INSENSITIVE };

	HANDLE hFile;

	IO_STATUS_BLOCK IoStatusBlock;

	auto Status = NtOpenFile(&hFile, FILE_READ_ATTRIBUTES | SYNCHRONIZE, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_DELETE | FILE_SHARE_READ, FILE_OPEN_REPARSE_POINT | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
	{
		goto Error2;
	}

	FILE_ATTRIBUTE_TAG_INFORMATION FileAttributeTagInfo;

	Status = NtQueryInformationFile(hFile, &IoStatusBlock, &FileAttributeTagInfo, sizeof(FileAttributeTagInfo), FileAttributeTagInformation);

	NtClose(hFile);

	if (Status)
	{
		goto Error2;
	}

	if (FileAttributeTagInfo.FileAttributes&FILE_ATTRIBUTE_READONLY)
	{
		if (Status = NtSetFileAttributes(&ObjectAttributes, FileAttributeTagInfo.FileAttributes^FILE_ATTRIBUTE_READONLY))
			goto Error2;
	}

	if (FileAttributeTagInfo.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
	{
		if (IsReparseTagNameSurrogate(FileAttributeTagInfo.ReparseTag))
			goto Delete;
	}

	if (FileAttributeTagInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
	{
		if (Status = DeleteDirectoryInternal(ObjectAttributes))
			goto Error;
	}

Delete:
	Status = NtDeleteFile2(&ObjectAttributes);

Error:
	if (FileAttributeTagInfo.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
	{
		NtSetFileAttributes(&ObjectAttributes, FileAttributeTagInfo.FileAttributes);
	}

Error2:


	RtlFreeUnicodeString(&usFileName);

	return Status;
}

LSTATUS UpdateFile(CString lpExistingFileName, CString lpNewFileName)
{
	if (GetFileType(lpNewFileName) != PathIsDir)
	{
		return ERROR_FILE_NOT_FOUND;
	}

	LSTATUS lStatus = ERROR_FILE_NOT_FOUND;
	CString NewFile, OldFile;

	WIN32_FIND_DATAW FindFileData = {};
	HANDLE hFindFile = FindFirstFileW(lpNewFileName + L"*", &FindFileData);
	FILE_INTERNAL_INFORMATION IdNew, IdOld;

	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_IsDots(FindFileData.cFileName) == 0)
				{
					auto lStatusTmp = UpdateFile(lpExistingFileName + FindFileData.cFileName + L"\\", lpNewFileName + FindFileData.cFileName + L"\\");
					if (lStatusTmp)
					{
						lStatus = lStatusTmp;
					}
				}
			}
			else
			{
				//TempFile = TempPath + FindFileData.cFileName;


				OldFile = lpExistingFileName + FindFileData.cFileName;
				NewFile = lpNewFileName + FindFileData.cFileName;


				////////////////////////////////////////
				//先检查文件是否为同一个文件
				if (auto Status = GetFileId(NewFile, NULL, &IdNew))
				{
					lStatus = RtlNtStatusToDosError(Status);
					continue;
				}

				if (auto Status = GetFileId(OldFile, NULL, &IdOld))
				{
					lStatus = RtlNtStatusToDosError(Status);
					continue;
				}

				if (IdNew.IndexNumber.QuadPart != IdOld.IndexNumber.QuadPart)
				{
					if (DirectGetOsMinVersion() >= MakeMiniVersion(6, 1))
					{
						if (!MoveFileEx(NewFile, OldFile, MOVEFILE_CREATE_HARDLINK | MOVEFILE_REPLACE_EXISTING))
						{
							lStatus = GetLastError_s();
						}
					}
					else
					{
						//Vista不支持直接替换，在Host为Vista时则使用事物处理来保证操作原子性
						auto hTransaction = CreateTransaction(NULL, 0, 0, 0, 0, INFINITE, nullptr);
						if (hTransaction == INVALID_HANDLE_VALUE)
						{
							lStatus = GetLastError_s();
						}
						else
						{
							if (DeleteFileTransactedW(OldFile, hTransaction)==FALSE || CreateHardLinkTransactedW(OldFile, NewFile,nullptr, hTransaction)==FALSE)
							{
								lStatus = GetLastError_s();
							}
							else
							{
								if (!CommitTransactionAsync(hTransaction))
								{
									lStatus = GetLastError_s();
								}
							}
							CloseHandle(hTransaction);
						}

					}
				}
			}




		} while (FindNextFile(hFindFile, &FindFileData));


		FindClose(hFindFile);
	}

	return lStatus;
}

NTSTATUS NtCopyDirectory(OBJECT_ATTRIBUTES ExistingDirectoryPath, OBJECT_ATTRIBUTES NewDirectoryPath)
{
	HANDLE hExistingFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), hRootDir, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;


	auto Status = NtOpenFile(&hExistingFile, SYNCHRONIZE | FILE_LIST_DIRECTORY| FILE_READ_ATTRIBUTES, &ExistingDirectoryPath, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
		return Status;

	HANDLE hNewFile;

	Status = NtCreateFile(&hNewFile, SYNCHRONIZE | FILE_LIST_DIRECTORY| FILE_WRITE_ATTRIBUTES, &NewDirectoryPath, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_VALID_FLAGS, FILE_OPEN_IF, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT, NULL, NULL);

	if (Status == 0)
	{
		byte Buffer[sizeof(FILE_FULL_DIR_INFORMATION) + 512];


		FILE_FULL_DIR_INFORMATION & FileInfo = *(FILE_FULL_DIR_INFORMATION *)Buffer;

		UNICODE_STRING TempFileName = { 0,0,FileInfo.FileName };
		NewDirectoryPath.RootDirectory = hNewFile;
		ExistingDirectoryPath.RootDirectory = hExistingFile;
		ExistingDirectoryPath.ObjectName = NewDirectoryPath.ObjectName = &TempFileName;


		byte FileBuffer[1024];

		FILE_BASIC_INFORMATION BaseInfo;
		while (ZwQueryDirectoryFile(hExistingFile, NULL, NULL, NULL, &IoStatusBlock, Buffer, sizeof(Buffer), FileFullDirectoryInformation, -1, NULL, 0) == ERROR_SUCCESS)
		{
			TempFileName.Length = TempFileName.MaximumLength = FileInfo.FileNameLength;
			if (FileInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				if (IsDots(FileInfo.FileName, FileInfo.FileNameLength))
					continue;

				Status = NtCopyDirectory(ExistingDirectoryPath, NewDirectoryPath);
			}
			else
			{
				HANDLE hExistingFile;

				Status = NtOpenFile(&hExistingFile, FILE_GENERIC_READ, &ExistingDirectoryPath, &IoStatusBlock, FILE_SHARE_READ | FILE_SHARE_DELETE, FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

				if (Status)
					break;

				HANDLE hNewFile;

				Status = NtCreateFile(&hNewFile, SYNCHRONIZE | FILE_GENERIC_WRITE, &NewDirectoryPath, &IoStatusBlock, NULL, FileInfo.FileAttributes, FILE_SHARE_READ, FILE_OVERWRITE_IF, FILE_SEQUENTIAL_ONLY | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT, NULL, NULL);

				if (Status)
				{
					NtClose(hExistingFile);
					break;
				}

				DWORD cbData;

				while (true)
				{
					if (!ReadFile(hExistingFile, FileBuffer, sizeof(FileBuffer), &cbData, NULL))
					{
						Status = GetLastError_s();
						break;
					}

					if (!cbData)
						break;

					WriteFile(hNewFile, FileBuffer, cbData, &cbData, NULL);
				}

				if (NtQueryInformationFile(hExistingFile, &IoStatusBlock, &BaseInfo, sizeof(BaseInfo), FileBasicInformation)==0)
				{
					NtSetInformationFile(hNewFile,&IoStatusBlock, &BaseInfo, sizeof(BaseInfo), FileBasicInformation);
				}


				NtClose(hNewFile);
				NtClose(hExistingFile);
			}
		}

		if (NtQueryInformationFile(hExistingFile, &IoStatusBlock, &BaseInfo, sizeof(BaseInfo), FileBasicInformation) == 0)
		{
			NtSetInformationFile(hNewFile, &IoStatusBlock, &BaseInfo, sizeof(BaseInfo), FileBasicInformation);
		}

		NtClose(hNewFile);
	}

	NtClose(hExistingFile);

	return Status;
}

NTSTATUS CopyDirectory(LPCWSTR ExistingDirectoryPath, LPCWSTR NewDirectoryInfoPath)
{
	UNICODE_STRING usExistingDirectoryPath;

	if (!RtlDosPathNameToNtPathName_U(ExistingDirectoryPath, &usExistingDirectoryPath, NULL, NULL))
	{
		return RtlGetLastNtStatus();
	}

	UNICODE_STRING usNewDirectoryInfoPath;

	NTSTATUS Status;

	if (RtlDosPathNameToNtPathName_U(NewDirectoryInfoPath, &usNewDirectoryInfoPath, NULL, NULL))
	{
		Status = NtCopyDirectory(OBJECT_ATTRIBUTES{ sizeof(OBJECT_ATTRIBUTES),NULL,&usExistingDirectoryPath,OBJ_CASE_INSENSITIVE }, OBJECT_ATTRIBUTES{ sizeof(OBJECT_ATTRIBUTES),NULL,&usNewDirectoryInfoPath,OBJ_CASE_INSENSITIVE });

		RtlFreeUnicodeString(&usNewDirectoryInfoPath);
	}
	else
	{
		Status = RtlGetLastNtStatus();
	}

	RtlFreeUnicodeString(&usExistingDirectoryPath);

	return Status;
}

int MoveDirectory(CString ExistingDirectoryPath, CString NewDirectoryInfoPath)
{
	if (ExistingDirectoryPath[ExistingDirectoryPath.GetLength() - 1] == L'\\')
		ExistingDirectoryPath.ReleaseBufferSetLength(ExistingDirectoryPath.GetLength() - 1);

	ExistingDirectoryPath.AppendChar(NULL);

	if (NewDirectoryInfoPath[NewDirectoryInfoPath.GetLength() - 1] == L'\\')
		NewDirectoryInfoPath.ReleaseBufferSetLength(NewDirectoryInfoPath.GetLength() - 1);


	SHFILEOPSTRUCT FileOp = { 0 };
	FileOp.fFlags = FOF_NO_UI;
	FileOp.pFrom = ExistingDirectoryPath;
	FileOp.pTo = NewDirectoryInfoPath;
	FileOp.wFunc = FO_MOVE;
	return SHFileOperation(&FileOp);
}


LSTATUS GetFileVersion(HMODULE hFileMoudle, UINT16 Version[4], WORD wLanguage)
{
	HRSRC hRsrcVersion = FindResourceExW(hFileMoudle, RT_VERSION, MAKEINTRESOURCE(1), wLanguage);

	if (hRsrcVersion == NULL)
	{
		return GetLastError_s();
	}

	HGLOBAL hGlobal = LoadResource(hFileMoudle, hRsrcVersion);
	if (hGlobal == NULL)
	{
		return GetLastError_s();
	}

	VS_FIXEDFILEINFO* pFileInfo = NULL;
	UINT uLen;
#ifndef _ATL_XP_TARGETING

	if (!VerQueryValue(hGlobal, L"\\", (LPVOID*)&pFileInfo, &uLen))
	{
		return GetLastError_s();
	}
#else
	//XP系统不允许直接调用，需要先复制到一个内存块
	DWORD cbData = SizeofResource(hFileMoudle, hRsrcVersion);

	void* pData=alloca(cbData);

	memcpy(pData, hGlobal, cbData);

	if (!VerQueryValue(pData, L"\\", (LPVOID*)&pFileInfo, &uLen))
	{
		return GetLastError_s();
	}

#endif


	*((DWORD*)Version) = pFileInfo->dwFileVersionLS;
	((DWORD*)Version)[1] = pFileInfo->dwFileVersionMS;

	return ERROR_SUCCESS;
}

//获得文件版本号
LSTATUS GetFileVersion(LPCWSTR FilePath, UINT16 Version[4], WORD wLanguage)
{
	CHModule hFileMoudle = LoadLibraryEx(FilePath, NULL, LOAD_LIBRARY_AS_DATAFILE);

	return hFileMoudle.IsInvalid() ? GetLastError_s() : GetFileVersion(hFileMoudle, Version, wLanguage);
}


UINT64 GetDirectoryAllocationSizeInternal(OBJECT_ATTRIBUTES ObjectAttributes, std::map<UINT64, DWORD>& FileMap)
{
	HANDLE hFile;
	//OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), hRootDir, (UNICODE_STRING*)&FileName, OBJ_CASE_INSENSITIVE };
	IO_STATUS_BLOCK IoStatusBlock;


	auto Status = NtOpenFile(&hFile, SYNCHRONIZE | FILE_LIST_DIRECTORY, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT);

	if (Status)
		return 0;

	UINT64 Space = 0;

	byte Buffer[sizeof(FILE_FULL_DIR_INFORMATION) + 512];


	FILE_FULL_DIR_INFORMATION & FileInfo = *(FILE_FULL_DIR_INFORMATION *)Buffer;

	UNICODE_STRING TempFileName = { 0,0,FileInfo.FileName };
	Status = -1;
	ObjectAttributes.RootDirectory = hFile;
	ObjectAttributes.ObjectName = &TempFileName;

	FILE_STANDARD_INFORMATION FileStandardInfo;
	FILE_INTERNAL_INFORMATION FileInternalInfo;

	while (ZwQueryDirectoryFile(hFile, NULL, NULL, NULL, &IoStatusBlock, Buffer, sizeof(Buffer), FileFullDirectoryInformation, -1, NULL, 0) == ERROR_SUCCESS)
	{
		TempFileName.Length = TempFileName.MaximumLength = FileInfo.FileNameLength;

		if (FileInfo.FileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FileInfo.FileAttributes&FILE_ATTRIBUTE_REPARSE_POINT)
			{
				if (IsReparseTagNameSurrogate(FileInfo.EaSize))
					continue;
			}
			else
			{
				if (IsDots(FileInfo.FileName, FileInfo.FileNameLength))
					continue;
			}

			Space += GetDirectoryAllocationSizeInternal(ObjectAttributes, FileMap);
		}
		else
		{
			if (NtGetFileId(&ObjectAttributes, &FileStandardInfo, &FileInternalInfo))
				continue;

			if (FileStandardInfo.NumberOfLinks != 1)
			{
				auto T = FileMap.insert(std::pair<UINT64, DWORD>(FileInternalInfo.IndexNumber.QuadPart, FileStandardInfo.NumberOfLinks)).first;

				if (--(T->second))
					continue;
			}

			Space += FileInfo.AllocationSize.QuadPart;
		}
	}

	NtClose(hFile);

	return Space;
}

UINT64 GetDirectoryAllocationSize(LPCWSTR FileName, std::map<UINT64, DWORD>& FileMap)
{
	UNICODE_STRING usFileName;

	if (!RtlDosPathNameToNtPathName_U(FileName, &usFileName, NULL, NULL))
	{
		return 0;
	}

	auto Space = GetDirectoryAllocationSizeInternal(OBJECT_ATTRIBUTES{ sizeof(OBJECT_ATTRIBUTES), NULL,&usFileName, OBJ_CASE_INSENSITIVE }, FileMap);

	RtlFreeUnicodeString(&usFileName);

	return Space;
}

LSTATUS CrateDirectorHandLink(CString To, CString From)
{
	WIN32_FIND_DATAW FindFileData;
	CHFileFind hFileFind = FindFirstFileW(From + L"*", &FindFileData);

	if (hFileFind == INVALID_HANDLE_VALUE)
		return GetLastError_s();


	if (!GetFileType(To) && !CreateDirectory(To, NULL))
	{
		return GetLastError_s();
	}

	do
	{
		if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if (_IsDots(FindFileData.cFileName) == 0)
			{
				auto lStatus = CrateDirectorHandLink(To + FindFileData.cFileName + L"\\", From + FindFileData.cFileName + L"\\");

				if (lStatus)
					return lStatus;
			}
		}
		else
		{

			if (MoveFileExW(From + FindFileData.cFileName, To + FindFileData.cFileName, MOVEFILE_CREATE_HARDLINK) == FALSE)
			{
				return GetLastError_s();
			}
		}


	} while (FindNextFile(hFileFind, &FindFileData));

	return ERROR_SUCCESS;
}

FilePathType GetFileType(LPCWSTR FilePath)
{
	if(StrEmpty(FilePath))
		return FilePathType::PathNotExist;

	UNICODE_STRING usFileFile;
	if (!RtlDosPathNameToNtPathName_U(FilePath, &usFileFile, NULL, NULL))
	{
		return FilePathType::PathNotExist;
	}


	IO_STATUS_BLOCK IoStatusBlock;

	OBJECT_ATTRIBUTES ObjectAttributes = { sizeof(OBJECT_ATTRIBUTES), NULL, &usFileFile, OBJ_CASE_INSENSITIVE };


	HANDLE hFile;
	auto hr = NtOpenFile(&hFile, SYNCHRONIZE, &ObjectAttributes, &IoStatusBlock, FILE_SHARE_VALID_FLAGS, FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE | FILE_OPEN_FOR_BACKUP_INTENT);

	RtlFreeUnicodeString(&usFileFile);

	switch (hr)
	{
	case 0xC0000103:
		//目录无效，说明是个文件
		return FilePathType::PathIsFile;
#ifdef DEBUG
	case 0xc000003a:
		//系统找不到指定的路径
	case 0xc0000013:
		//设备未就绪。
	case 0xc0000034:
		//系统找不到指定的文件
	case 0xc0000240:
		//请求被终止
		return FilePathType::PathNotExist;
#endif
	case 0:
		//成功打开
		NtClose(hFile);
		return FilePathType::PathIsDir;
	default:
#ifdef DEBUG
		//assert(0);
		SetLastError(RtlNtStatusToDosError(hr));
#endif
		return FilePathType::PathNotExist;
		break;
	}
}


FilePathType GetFileType(LPCSTR FilePath)
{
	return GetFileType(CStringW(FilePath));
}

#pragma warning(pop)