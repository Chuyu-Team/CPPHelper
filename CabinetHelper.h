#pragma once

#include <fdi.h>
#include <Fci.h>

#pragma comment(lib,"cabinet.lib")


#include <io.h>
#include <fcntl.h>

#include <comdef.h>



#include <atlstr.h>
#include <Strsafe.h>
#include "Base.h"
#include "StringHelper.h"

//#define _IsDots(FileName) (((FileName)[0] == L'.' &&((FileName)[1] == NULL ||((FileName)[1] == L'.'&&(FileName)[2] == NULL))))

static FNOPEN(fnFileOpen)
{
	DWORD dwDesiredAccess = 0;
	DWORD dwCreationDisposition = 0;

	UNREFERENCED_PARAMETER(pmode);

	if (oflag & _O_RDWR)
	{
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	}
	else if (oflag & _O_WRONLY)
	{
		dwDesiredAccess = GENERIC_WRITE;
	}
	else
	{
		dwDesiredAccess = GENERIC_READ;
	}

	if (oflag & _O_CREAT)
	{
		dwCreationDisposition = CREATE_ALWAYS;
	}
	else
	{
		dwCreationDisposition = OPEN_EXISTING;
	}

	return (INT_PTR)CreateFile(UTF8ToUnicode( pszFile),
		dwDesiredAccess,
		FILE_SHARE_READ,
		NULL,
		dwCreationDisposition,
		FILE_ATTRIBUTE_NORMAL| FILE_FLAG_BACKUP_SEMANTICS,
		NULL);
}

static FNREAD(fnFileRead)
{
	DWORD dwBytesRead = 0;

	return ReadFile((HANDLE)hf, pv, cb, &dwBytesRead, NULL) ? dwBytesRead : -1;
}

static FNWRITE(fnFileWrite)
{
	DWORD dwBytesWritten = 0;

	if (WriteFile((HANDLE)hf, pv, cb, &dwBytesWritten, NULL) == FALSE)
	{
		dwBytesWritten = (DWORD)-1;
	}

	return dwBytesWritten;
}

static FNCLOSE(fnFileClose)
{
	return CloseHandle((HANDLE)hf) ? 0 : -1;
}

static FNSEEK(fnFileSeek)
{
	return SetFilePointer((HANDLE)hf, dist, NULL, seektype);
}


#define FDICreate2(pErf) FDICreate((PFNALLOC)malloc,(PFNFREE)free,(PFNOPEN)fnFileOpen,(PFNREAD)fnFileRead,(PFNWRITE)fnFileWrite,(PFNCLOSE)fnFileClose,(PFNSEEK)fnFileSeek,cpuUNKNOWN,pErf)

static FNFCIFILEPLACED(fnFCIFilePlaced)
{
	UNREFERENCED_PARAMETER(pv);

	return 0;
}

static FNFCIOPEN(fnFCIFileOpen)
{
	UNREFERENCED_PARAMETER(pv);

	auto hFile=fnFileOpen(pszFile, oflag, pmode);

	if (hFile == -1)
	{
		*err = GetLastError();
	}

	return (INT_PTR)hFile;
}


static FNFCIREAD(fnFCIFileRead)
{
	DWORD dwBytesRead = 0;

	UNREFERENCED_PARAMETER(pv);

	if (!ReadFile((HANDLE)hf, memory, cb, &dwBytesRead, NULL))
	{
		dwBytesRead = (DWORD)-1;
		*err = GetLastError();
	}

	return dwBytesRead;
}


static FNFCIWRITE(fnFCIFileWrite)
{
	DWORD dwBytesWritten = 0;

	UNREFERENCED_PARAMETER(pv);

	if (!WriteFile((HANDLE)hf, memory, cb, &dwBytesWritten, NULL))
	{
		dwBytesWritten = (DWORD)-1;
		*err = GetLastError();
	}

	return dwBytesWritten;
}

static FNFCIDELETE(fnFCIFileDelete)
{
	INT iResult = 0;

	UNREFERENCED_PARAMETER(pv);

	if (DeleteFileW(UTF8ToUnicode( pszFile)) == FALSE)
	{
		*err = GetLastError();
		iResult = -1;
	}

	return iResult;
}

static FNFCIGETTEMPFILE(fnFCIGetTempFileName)
{
	BOOL bSucceeded = FALSE;
	CHAR pszTempPath[MAX_PATH];
	CHAR pszTempFile[MAX_PATH];

	UNREFERENCED_PARAMETER(pv);
	UNREFERENCED_PARAMETER(cbTempName);

	if (GetTempPathA(MAX_PATH, pszTempPath) != 0)
	{
		if (GetTempFileNameA(pszTempPath, "CABINET", 0, pszTempFile) != 0)
		{
			DeleteFileA(pszTempFile);

		
			bSucceeded = SUCCEEDED(StringCbCopyA(pszTempName, cbTempName, pszTempFile));
		}
	}

	return bSucceeded;
}


static FNFCICLOSE(fnFCIFileClose)
{
	INT iResult = 0;

	UNREFERENCED_PARAMETER(pv);

	if (CloseHandle((HANDLE)hf) == FALSE)
	{
		*err = GetLastError();
		iResult = -1;
	}

	return iResult;
}

static FNFCISEEK(fnFCIFileSeek)
{
	INT iResult = 0;

	UNREFERENCED_PARAMETER(pv);

	iResult = SetFilePointer((HANDLE)hf, dist, NULL, seektype);

	if (iResult == -1)
	{
		*err = GetLastError();
	}

	return iResult;
}

static FNFCIGETNEXTCABINET(fnFDIGetNextCabinet)
{
	return FALSE;
}

static FNFCISTATUS(fnFCIStatus)
{
	return ERROR_SUCCESS;
}

static FNFCIGETOPENINFO(fnFCIGetOpenInfo)
{
	FILETIME fileTime;
	BY_HANDLE_FILE_INFORMATION fileInfo;

	auto hFile = (HANDLE)fnFCIFileOpen(pszName, _O_RDONLY, 0, err, pv);

	if (hFile != (HANDLE)-1)
	{
		if (GetFileInformationByHandle(hFile, &fileInfo)
			&& FileTimeToLocalFileTime(&fileInfo.ftCreationTime, &fileTime)
			&& FileTimeToDosDateTime(&fileTime, pdate, ptime))
		{
			*pattribs = (USHORT)fileInfo.dwFileAttributes;
			*pattribs &= (_A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);
		}
		else
		{
			fnFCIFileClose((INT_PTR)hFile, err, pv);
			hFile = (HANDLE)-1;
		}
	}

	return (INT_PTR)hFile;
}


#define FCICreate2(pErf,ccab) FCICreate(pErf,fnFCIFilePlaced,(PFNALLOC)malloc,(PFNFREE)free,(PFNFCIOPEN)fnFCIFileOpen,(PFNFCIREAD)fnFCIFileRead,(PFNFCIWRITE)fnFCIFileWrite,(PFNFCICLOSE)fnFCIFileClose,(PFNFCISEEK)fnFCIFileSeek,fnFCIFileDelete,fnFCIGetTempFileName,&ccab,NULL)

_Check_return_
static LSTATUS CabExtractFile(
	_In_z_ LPCWSTR CabFilePath,
	_In_z_ LPCWSTR ExtractPath
	)
{
	ERF  erf;

	auto hfdi= FDICreate2(&erf);

	if (!hfdi)
		return ERROR_FUNCTION_FAILED;

	auto szExtractPathUTF8 = Unicode2UTF8(ExtractPath);

	if (szExtractPathUTF8.IsEmpty())
		return ERROR_PATH_NOT_FOUND;

	if (szExtractPathUTF8[szExtractPathUTF8.GetLength() - 1] != '\\')
		szExtractPathUTF8 += '\\';

	LSTATUS lStatus = ERROR_SUCCESS;

	if (!FDICopy(hfdi, (LPSTR)Unicode2UTF8( CabFilePath).GetString(), (LPSTR)"", 0, [](FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION    pfdin)->INT_PTR
	{
		INT_PTR iResult = 0;


		switch (fdint)
		{
		case fdintCOPY_FILE:
		{
			CStringA FilePath = (LPCSTR)pfdin->pv;

			FilePath += pfdin->psz1;

			CreateRoot(UTF8ToUnicode(FilePath, FilePath.GetLength()));

			iResult = fnFileOpen(FilePath.GetBuffer(), _O_WRONLY | _O_CREAT, 0);

			break;
		}
		case fdintCLOSE_FILE_INFO:
		{
			//pfdin->hf

			//auto pData = (vector<BYTE>*) ((void**)pfdin->pv)[1];

			//pData->_Mylast = pData->_Myfirst + pfdin->cb;

			//pfdin->hf = NULL;
			iResult = !fnFileClose(pfdin->hf);
			break;
		}
		case fdintNEXT_CABINET:
			break;
		case fdintPARTIAL_FILE:
			iResult = 0;
			break;
		case fdintCABINET_INFO:
			iResult = 0;
			break;
		case fdintENUMERATE:
			iResult = 0;
			break;
		default:
			iResult = -1;
			break;
		}

		return iResult;

	}, NULL, (void*)szExtractPathUTF8.GetString()))
	{
		lStatus = ERROR_FUNCTION_FAILED;
	}


	FDIDestroy(hfdi);

	return lStatus;
}

_Check_return_
static LSTATUS CabExtractFile(
	_In_z_ LPCSTR CabFilePath,
	_In_z_ LPCSTR ExtractPath
	)
{
	return CabExtractFile(CStringW(CabFilePath), CStringW(ExtractPath));
}

//此接口仅内部调用
_Check_return_
static LSTATUS CabCreateFileInternal_U(
	_In_ HFCI     hfci,
	_In_ int      chRoot,
	_In_ CStringA FilePath,
	_In_ TCOMP    typeCompress
	)
{
	WIN32_FIND_DATAW FindData;

	HANDLE hFileFind = FindFirstFileW(UTF8ToUnicode( FilePath) + L'*', &FindData);

	if (hFileFind == INVALID_HANDLE_VALUE)
		return GetLastError();
	LSTATUS lStatus = ERROR_SUCCESS;
	do
	{
		auto TempFileName = FilePath + Unicode2UTF8( FindData.cFileName);

		if (FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			if (_IsDots(FindData.cFileName))
				continue;
			TempFileName += '\\';
			lStatus = CabCreateFileInternal_U(hfci, chRoot, TempFileName, typeCompress);
			if (lStatus != ERROR_SUCCESS)
				break;
		}
		else
		{
			if (!FCIAddFile(hfci, TempFileName.GetBuffer(), TempFileName.GetBuffer() + chRoot, FALSE, NULL, fnFCIStatus, fnFCIGetOpenInfo, typeCompress))
			{
				lStatus = ERROR_FUNCTION_FAILED;
				break;
			}
		}


	} while (FindNextFileW(hFileFind,&FindData));

	FindClose(hFileFind);

	return lStatus;
}

//将一个文件夹压缩为cab
_Check_return_
static LSTATUS CabCreateDirectory_U(
	_In_z_ LPCSTR   CabFilePath,
	_In_   CStringA RootPath,
	_In_   TCOMP    typeCompress = TCOMPfromLZXWindow(21)
	)
{
	if (RootPath.IsEmpty())
		return ERROR_INVALID_PARAMETER;

	if (RootPath[RootPath.GetLength() - 1] != '\\')
		RootPath += '\\';

	if (GetFileType(CabFilePath))
	{
		if (!DeleteFileW( UTF8ToUnicode( CabFilePath)))
			return GetLastError();
	}

	ERF erf;            //FCI error structure

	CCAB ccab = { 0 ,900000 };           //cabinet information structure
	ccab.setID = 12345;
	ccab.iCab = 1;
	ccab.iDisk = 0;

	auto FileName = PathFindFileNameA(CabFilePath);
	StrCpyA(ccab.szCab, FileName);
	StrCpyNA(ccab.szCabPath, CabFilePath, FileName - CabFilePath +1);

	HFCI hfci = FCICreate2(&erf, ccab);

	if (hfci == NULL)
	{
		return ERROR_FUNCTION_FAILED;
	}

	auto lStatus = CabCreateFileInternal_U(hfci, RootPath.GetLength(), RootPath, typeCompress);

	if (lStatus == ERROR_SUCCESS)
	{
		if (!FCIFlushCabinet(hfci, FALSE, NULL, fnFCIStatus))
		{
			lStatus = ERROR_FUNCTION_FAILED;
		}
	}

	FCIDestroy(hfci);



	return lStatus;
}

_Check_return_
static LSTATUS CabCreateDirectory(
	_In_z_ LPCWSTR CabFilePath,
	_In_z_ LPCWSTR RootPath,
	_In_   TCOMP   typeCompress = TCOMPfromLZXWindow(21)
	)
{
	return CabCreateDirectory_U(Unicode2UTF8(CabFilePath), Unicode2UTF8(RootPath), typeCompress);
}

_Check_return_
static LSTATUS CabCreateDirectory(
	_In_z_ LPCSTR CabFilePath,
	_In_z_ LPCSTR RootPath,
	_In_   TCOMP  typeCompress = TCOMPfromLZXWindow(21)
	)
{
	return CabCreateDirectory(CStringW(CabFilePath), CStringW(RootPath), typeCompress);
}

_Check_return_
static LSTATUS CabCreateFile(
	_In_z_ LPCWSTR CabFilePath,
	_In_z_ LPCWSTR SourceFile,
	_In_z_ LPCWSTR FileName,
	_In_   TCOMP   typeCompress = TCOMPfromLZXWindow(21)
	)
{
	if (GetFileType(CabFilePath))
	{
		if (!DeleteFileW(CabFilePath))
			return GetLastError();
	}

	ERF erf;            //FCI error structure

	CCAB ccab = { 0 ,900000 };           //cabinet information structure
	ccab.setID = 12345;
	ccab.iCab = 1;
	ccab.iDisk = 0;

	auto CabFilePath_U = Unicode2UTF8(CabFilePath);

	auto CabFileName = PathFindFileNameA(CabFilePath_U);
	StrCpyA(ccab.szCab, CabFileName);
	StrCpyNA(ccab.szCabPath, CabFilePath_U, CabFileName - CabFilePath_U + 1);

	HFCI hfci = FCICreate2(&erf, ccab);

	if (hfci == NULL)
	{
		return ERROR_FUNCTION_FAILED;
	}

	LSTATUS lStatus = ERROR_SUCCESS;
	if (FCIAddFile(hfci, (LPSTR)Unicode2UTF8(SourceFile).GetString(), (LPSTR)Unicode2UTF8(FileName).GetString(), FALSE, NULL, fnFCIStatus, fnFCIGetOpenInfo, typeCompress))
	{
		if (!FCIFlushCabinet(hfci, FALSE, NULL, fnFCIStatus))
		{
			lStatus = ERROR_FUNCTION_FAILED;
		}
	}

	FCIDestroy(hfci);

	return lStatus;
}

_Check_return_
static LSTATUS CabCreateFile(
	_In_z_ LPCSTR CabFilePath,
	_In_z_ LPCSTR SourceFile,
	_In_z_ LPCSTR FileName,
	_In_   TCOMP  typeCompress = TCOMPfromLZXWindow(21)
	)
{
	return CabCreateFile(CStringW(CabFilePath), CStringW(SourceFile), CStringW(FileName), typeCompress);
}

_Check_return_
static LSTATUS CabCreateFile(
	_In_z_ LPCSTR CabFilePath,
	_In_z_ LPCSTR SourceFile,
	_In_   TCOMP  typeCompress = TCOMPfromLZXWindow(21)
	)
{
	return CabCreateFile(CabFilePath, SourceFile,PathFindFileNameA(SourceFile), typeCompress);
}

_Check_return_
static LSTATUS CabCreateFile(
	_In_z_ LPCWSTR CabFilePath,
	_In_z_ LPCWSTR SourceFile,
	_In_   TCOMP   typeCompress = TCOMPfromLZXWindow(21)
	)
{
	return CabCreateFile(CabFilePath,SourceFile, PathFindFileNameW(SourceFile),typeCompress);
}
