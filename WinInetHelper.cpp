#pragma once
#include "WinInetHelper.h"
#include "Base.h"
#include <WinInet.h>
#pragma comment(lib,"Wininet.lib")
#include "StreamEx.h"
#include "ComHelper.h"
#include "StringHelper.h"

class CReadWriteMemStream : public IUnknownT<CReadWriteMemStream, IReadWriteStream>
{
public:
	CStringA Buffer;
	UINT64 Point;
	CReadWriteMemStream()
		:Point(0)
	{

	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		_In_opt_ UINT64* pByteOffset)
	{
		auto TempPoint = pByteOffset ? *pByteOffset : Point;

		auto Size= Buffer.GetLength();

		if (TempPoint >= Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		ULONG cbRead = min(cb, Size - TempPoint);

		memcpy(pv, Buffer.GetString() + TempPoint, cbRead);

		if (!pByteOffset)
			Point += cbRead;

		if (pcbRead)
			*pcbRead = cbRead;

		return cbRead == cb ? S_OK : S_FALSE;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition)
	{
		auto Size = Buffer.GetLength();

		switch (dwOrigin)
		{
		case FILE_CURRENT:
			dlibMove += Point;
			break;
		case FILE_BEGIN:
			break;
		case FILE_END:
			dlibMove += Size;
			break;
		default:
			return __HRESULT_FROM_WIN32(87);
			break;
		}

		if (dlibMove<0)
			return __HRESULT_FROM_WIN32(87);

		if (dlibMove>Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		Point = dlibMove;

		if (plibNewPosition)
			*plibNewPosition = Point;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize)
	{
		*pSize = Buffer.GetLength();

		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
		/* [annotation] */
		_In_reads_bytes_(cb)  const void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbWritten,
		_In_opt_ UINT64* pByteOffset)
	{
		auto TempPoint = pByteOffset ? *pByteOffset : Point;

		auto NewSize = TempPoint + cb;

		auto pData= Buffer.GetBuffer(NewSize);


		memcpy(pData + TempPoint, pv, cb);

		auto Size = Buffer.GetLength();

		if (NewSize >= Size)
		{
			Buffer.ReleaseBufferSetLength(NewSize);
		}

		if (pcbWritten)
		{
			*pcbWritten = cb;
		}

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE put_Size(_In_ UINT64 Size)
	{
		Buffer.Truncate(Size);

		return S_OK;
	}
};

HRESULT DownloadFile(LPCWSTR Url, CStringA& Buffer, LPCWSTR lpszAgent, DWORD* pCharSet, BaseCallBack callBack, LPVOID pUserData)
{
	CReadWriteMemStream MemStream;

	HRESULT hr = DownloadStream(Url, (IReadWriteStream*)&MemStream, lpszAgent, pCharSet, callBack, pUserData);

	if (hr!=S_OK)
		return hr;

	Buffer = MemStream.Buffer;

	return S_OK;
}

HRESULT DownloadFile(LPCWSTR Url, CStringA& Buffer, DWORD* pCharSet, BaseCallBack callBack, LPVOID pUserData)
{
	return DownloadFile(Url, Buffer, L"Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko", pCharSet, callBack, pUserData);
}

HRESULT DownloadStream(LPCWSTR Url, IReadWriteStream* pStream, LPCWSTR lpszAgent, DWORD* pCharSet, BaseCallBack callBack, LPVOID pUserData)
{
	if (callBack)
	{
		callBack(/*DISM_MSG_PROGRESS*/38008, 0, 0, pUserData);
	}

	HRESULT hr = 0;

	WinInetURLHelper Info;
		
	int TryCount = 5;

	Info.ParseURLW(Url);

Start:


	hr=Info.InternetOpenW(lpszAgent);
	
	if (hr!=S_OK)
		return hr;

	hr=Info.InternetConnectW();

	if (hr!=S_OK)
		return hr;

		


	hr=Info.HttpOpenRequestW();

	if (hr!=S_OK)
		return hr;

	
	UINT64 FileSize = 0, UsedSize = 0;
	//DWORD dwBytesRead = 0;
	//BYTE TempBuffer[10240];
	DWORD dwBytesRead;

	TCHAR TempBuffer[10240 / sizeof(TCHAR)] = L"Content-Type";

	if (pCharSet)
	{
		//请求字符集
		//DWORD WIndex = 0;
		*pCharSet = CP_ACP;
		dwBytesRead = sizeof(TempBuffer);

		if (HttpQueryInfoW(Info.hUrlFile, HTTP_QUERY_CUSTOM, TempBuffer, &dwBytesRead, NULL))
		{
			LPWSTR Str = StrStrI(TempBuffer, L"charset=");
			if (Str)
			{
				Str += 8;

				if (StrCmpI(Str, L"utf-8") == 0)
				{
					*pCharSet = CP_UTF8;
				}

			}
		}
	}

		
		
	UsedSize = dwBytesRead = 0;

	pStream->get_Size(&UsedSize);
	//pStream->Seek(LARGE_INTEGER{}, FILE_END, (ULARGE_INTEGER*)&UsedSize);

	//if (UsedSize)
	//{
		//调整指针
	{
		auto Rang = StrFormat(L"Range: bytes=%I64d-\r\n", UsedSize);

		//if (InternetSetFilePointer(hUrlFile, *((DWORD*)&UsedSize), ((PLONG)&UsedSize) + 1, FILE_BEGIN, NULL) == INVALID_SET_FILE_POINTER)
		//{
		//	//不支持断点续传，重新开始
		//	UsedSize = 0;
		//	pStream->Seek(LARGE_INTEGER{}, FILE_BEGIN, (ULARGE_INTEGER*)&UsedSize);
		//}
		hr = Info.HttpSendRequestW(Rang, Rang.GetLength());
	}
		if (hr!=S_OK)
		{
			return hr;
		}

		DWORD Status;

		hr=Info.GetStatusCode(Status);
		if (hr!=S_OK)
		{
			return hr;
		}

		switch (Status)
		{
		case HTTP_STATUS_PARTIAL_CONTENT:
			//支持断点续传
			break;
		case HTTP_STATUS_OK:
		case HTTP_STATUS_RESET_CONTENT:
			//不支持断点续传
		case 416:
			//超出了实际范围
			UsedSize = dwBytesRead = 0;
			pStream->put_Size(0);
			break;
		case 0:
			return E_FAIL;
		case HTTP_STATUS_NOT_FOUND:
			if (TryCount--)
			{
				//等待 1秒
				Sleep(1000);
				goto Start;
			}
		default:
			return Status;
			break;
		}

	//}
			
		dwBytesRead = sizeof(TempBuffer);
		if (HttpQueryInfoW(Info.hUrlFile, HTTP_QUERY_CONTENT_LENGTH, TempBuffer, &dwBytesRead, NULL))
		{
			//dwBytesRead = UsedSize;

			FileSize = _tcstoui64((wchar_t*)TempBuffer, NULL, 10) + UsedSize;
			//swscanf((wchar_t*)TempBuffer, L"%I64u", &FileSize);
		}
		else
		{
			FileSize = -1;
			callBack = NULL;
		}

		


		

	//while (true)
	for (;;)
	{
		if (!InternetReadFile(Info.hUrlFile, TempBuffer, sizeof(TempBuffer), &dwBytesRead))
		{
		Error:

			if (--TryCount)
			{
				Sleep(1000);

				hr = Info.InternetOpenW();

				if (hr!=S_OK)
					return hr;

				hr = Info.InternetConnectW();

				if (hr!=S_OK)
					return hr;

				hr = Info.HttpOpenRequestW();

				if (hr!=S_OK)
				{
					return hr;
				}

				//重新指定偏移

					
				auto Rang = StrFormat(L"Range: bytes=%I64d-\r\n", UsedSize);

				hr = Info.HttpSendRequestW(Rang, Rang.GetLength());

				if (hr!=S_OK)
				{
					return hr;
				}

				hr = Info.GetStatusCode(Status);
				if (hr!=S_OK)
				{
					return hr;
				}

				switch (Status)
				{
				case HTTP_STATUS_PARTIAL_CONTENT:
					//支持断点续传
					continue;
					break;
				case HTTP_STATUS_OK:
				case HTTP_STATUS_RESET_CONTENT:
					//不支持断点续传
				case 416:
					//超出了实际范围
					UsedSize = dwBytesRead = 0;
					pStream->put_Size(0);
					break;
				case 0:
					return E_FAIL;
				case HTTP_STATUS_NOT_FOUND:
					goto Error;
				default:
					return Status;
					break;
				}
			}
			else
			{
				return HresultFromBool();
			}
		}


		if (dwBytesRead == 0)
		{
			if(FileSize==-1|| FileSize== UsedSize)
				break;

			if (FileSize < UsedSize)
			{
				//pStream->put_Size(0);

				//意外的网络错误
				return __HRESULT_FROM_WIN32(ERROR_UNEXP_NET_ERR);
			}

			goto Error;
		}

		TryCount = 5;

		hr = pStream->Write(TempBuffer, dwBytesRead, &dwBytesRead, &UsedSize);
		if (hr!=S_OK)
		{
			return hr;
			break;
		}

		UsedSize += dwBytesRead;

		if (callBack)
		{
			callBack(/*DISM_MSG_PROGRESS*/38008, UsedSize * 100 / FileSize, 0, pUserData);

			if (callBack(/*Dism_MSG_QUERY_ABORT*/38030, 0, 0, pUserData))
			{
				return __HRESULT_FROM_WIN32(ERROR_CANCELLED);
			}
		}

			
	}

	return S_OK;
}

HRESULT DownloadFile(LPCWSTR Url, LPCWSTR FilePath, BaseCallBack callBack, LPVOID pUserData)
{
	CComPtr<IReadWriteStream> pStream;
	pStream.p = StreamCreate(FilePath, GENERIC_WRITE,FILE_SHARE_READ,OPEN_ALWAYS);
	if (!pStream.p)
		return HresultFromBool();

	return DownloadStream(Url, pStream, L"Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko", NULL, callBack, pUserData);
}

//
HRESULT GetUrlString(LPCWSTR Url, CString& String, DWORD CodePage /* -1为自动识别编码*/)
{
	CStringA Buffer;

	HRESULT hr = DownloadFile(Url, Buffer, CodePage == -1 ? &CodePage : NULL, NULL, NULL);

	if (hr!=S_OK)
	{
		return hr;
	}

	UTF8ToUnicode(Buffer, Buffer.GetLength(), String);

	return S_OK;
}

HRESULT WinInetGetFileSize(LPCWSTR Url, UINT64& FileSize)
{

	try
	{
		HRESULT hr = S_OK;

		HINTERNET m_hSession = ::InternetOpenW(L"Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko", INTERNET_OPEN_TYPE_DIRECT, NULL, INTERNET_INVALID_PORT_NUMBER, 0);

		if (m_hSession == 0)
		{
			return HresultFromBool();
		}

		HINTERNET hUrlFile = ::InternetOpenUrlW(m_hSession, Url, NULL, 0, 0, 0);

		if (hUrlFile == 0)
		{
			hr = HresultFromBool();
		}
		else
		{

			wchar_t szFileSize[100] = {};
			DWORD dwBytesRead = sizeof(szFileSize);
			if (HttpQueryInfoW(hUrlFile, HTTP_QUERY_CONTENT_LENGTH, szFileSize, &dwBytesRead, NULL))
			{
				FileSize = 0;
				swscanf(szFileSize, L"%I64u", &FileSize);
			}
			else
			{
				hr = HresultFromBool();
			}

			InternetCloseHandle(hUrlFile);
		}

		InternetCloseHandle(m_hSession);

		return hr;
	}
	catch (...)
	{
		return 5;
	}
}



HRESULT WinInetURLHelper::ParseURLW(LPCWSTR URL)
{
	dwService = INTERNET_SERVICE_HTTP;

	if (StrCmpNI(URL, L"http://", ArraySize(L"http://") - 1) == 0)
	{
		URL += ArraySize(L"http://") - 1;
	}

	lpszServerName = URL;

	if (auto _lpszObjectName = StrChr(lpszServerName, L'/'))
	{
		lpszObjectName = _lpszObjectName + 1;
		cServerName = _lpszObjectName - lpszServerName;
	}
	else
	{
		lpszObjectName = NULL;
		cServerName = _tcslen(lpszServerName);
	}


	auto szServerPort = StrRChr(lpszServerName, lpszServerName + cServerName, L':');
	if (szServerPort)
	{
		cServerName = szServerPort - lpszServerName;

		nServerPort = _tcstoul(szServerPort + 1, NULL, 10);
	}
	else
	{
		nServerPort = INTERNET_DEFAULT_HTTP_PORT;
	}

	return S_OK;
}