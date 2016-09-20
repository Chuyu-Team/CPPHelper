#pragma once
#include <Windows.h>

HRESULT DownloadFile(LPCWSTR Url, CStringA& Buffer, DWORD* pCharSet , BaseCallBack callBack, LPVOID pUserData );

HRESULT DownloadFile(LPCWSTR Url, LPCWSTR FilePath, BaseCallBack callBack, LPVOID pUserData);

HRESULT GetUrlString(LPCWSTR Url, CString& String, DWORD CodePage = -1/* -1为自动识别编码*/);

HRESULT WinInetGetFileSize(LPCWSTR Url, UINT64& FileSize);

HRESULT DownloadFile(LPCWSTR Url, CStringA& Buffer, LPCWSTR lpszAgent, DWORD* pCharSet, BaseCallBack callBack, LPVOID pUserData);

HRESULT DownloadStream(LPCWSTR Url, IReadWriteStream* pStream, LPCWSTR lpszAgent, DWORD* pCharSet, BaseCallBack callBack, LPVOID pUserData);

class WinInetURLHelper
{
public:
	HINTERNET m_hSession;
	HINTERNET m_hConnect;
	HINTERNET hUrlFile;

	DWORD dwService;
	DWORD cServerName;
	LPCWSTR lpszServerName;
	LPCWSTR lpszObjectName;
	INTERNET_PORT nServerPort;

	HRESULT ParseURLW(LPCWSTR URL);

	WinInetURLHelper()
		: m_hSession(NULL)
		, m_hConnect(NULL)
		, hUrlFile(NULL)
	{

	}

	void Close()
	{
		if (m_hSession)
		{
			if (m_hConnect)
			{
				if (hUrlFile)
				{
					InternetCloseHandle(hUrlFile);
					hUrlFile = NULL;
				}
				InternetCloseHandle(m_hConnect);
				m_hConnect = NULL;
			}
			InternetCloseHandle(m_hSession);
			//m_hSession = NULL;
		}
	}

	~WinInetURLHelper()
	{
		Close();
	}

	HRESULT InternetOpenW(
		_In_opt_ LPCWSTR lpszAgent= L"Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko",
		_In_ DWORD dwAccessType= INTERNET_OPEN_TYPE_PRECONFIG,
		_In_opt_ LPCWSTR lpszProxy=NULL,
		_In_opt_ LPCWSTR lpszProxyBypass= INTERNET_INVALID_PORT_NUMBER,
		_In_ DWORD dwFlags=0
		)
	{
		Close();

		m_hSession = ::InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		return m_hSession ? S_OK : GetLastError();
	}

	HRESULT InternetConnectW(
		//_In_ HINTERNET hInternet,
		//_In_ LPCWSTR lpszServerName,
		//_In_ INTERNET_PORT nServerPort,
		_In_opt_ LPCWSTR lpszUserName=NULL,
		_In_opt_ LPCWSTR lpszPassword=NULL,
		//_In_ DWORD dwService,
		_In_ DWORD dwFlags = 0,
		_In_opt_ DWORD_PTR dwContext=NULL
		)
	{
		

		if (m_hConnect)
		{
			if (hUrlFile)
			{
				InternetCloseHandle(hUrlFile);

				hUrlFile = NULL;
			}

			InternetCloseHandle(m_hConnect);
		}
		m_hConnect = ::InternetConnectW(m_hSession, CString(lpszServerName, cServerName), nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);

		return m_hConnect ? S_OK : GetLastError();
	}

	HRESULT HttpOpenRequestW(
		//_In_ HINTERNET hConnect,
		_In_opt_ LPCWSTR lpszVerb=NULL,
		//_In_opt_ LPCWSTR lpszObjectName,
		_In_opt_ LPCWSTR lpszVersion= _T("HTTP/1.1"),
		_In_opt_ LPCWSTR lpszReferrer=NULL,
		_In_opt_z_ LPCWSTR FAR * lplpszAcceptTypes=NULL,
		_In_ DWORD dwFlags= INTERNET_FLAG_KEEP_CONNECTION,
		_In_opt_ DWORD_PTR dwContext=NULL
		)
	{
		if (hUrlFile)
		{
			InternetCloseHandle(hUrlFile);
		}

		hUrlFile = ::HttpOpenRequestW(m_hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);
		return hUrlFile ? S_OK : GetLastError();
	}
};