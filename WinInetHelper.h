#pragma once
#include <Windows.h>
#include <WinInet.h>

_Check_return_ _Success_(return == S_OK)
HRESULT DownloadFile(
	_In_z_    LPCWSTR      Url,
	_Out_     CStringA&    Buffer,
	_Out_opt_ DWORD*       pCharSet ,
	_In_opt_  BaseCallBack callBack,
	_In_opt_  LPVOID       pUserData
	);

_Check_return_ _Success_(return == S_OK)
HRESULT DownloadFile(
	_In_z_   LPCWSTR      Url,
	_In_z_   LPCWSTR      FilePath,
	_In_opt_ BaseCallBack callBack,
	_In_opt_ LPVOID       pUserData
	);

_Check_return_ _Success_(return == S_OK)
HRESULT GetUrlString(
	_In_z_ LPCWSTR  Url,
	_Out_  CString& String,
	_In_   DWORD    CodePage = -1/* -1为自动识别编码*/
	);

_Check_return_ _Success_(return == S_OK)
HRESULT WinInetGetFileSize(
	_In_z_ LPCWSTR Url,
	_Out_  UINT64& FileSize
	);

_Check_return_ _Success_(return == S_OK)
HRESULT DownloadFile(
	_In_z_     LPCWSTR      Url,
	_Out_      CStringA&    Buffer,
	_In_opt_z_ LPCWSTR      lpszAgent,
	_Out_opt_  DWORD*       pCharSet,
	_In_opt_   BaseCallBack callBack,
	_In_opt_   LPVOID       pUserData
	);

_Check_return_ _Success_(return == S_OK)
HRESULT DownloadStream(
	_In_z_     LPCWSTR           Url,
	_In_       IReadWriteStream* pStream,
	_In_opt_z_ LPCWSTR           lpszAgent,
	_Out_opt_  DWORD*            pCharSet,
	_In_opt_   BaseCallBack      callBack,
	_In_opt_   LPVOID            pUserData
	);

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

	HRESULT ParseURLW(
		_In_z_ LPCWSTR URL
		);

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
		_In_opt_ LPCWSTR lpszAgent       = L"Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko",
		_In_     DWORD   dwAccessType    = INTERNET_OPEN_TYPE_PRECONFIG,
		_In_opt_ LPCWSTR lpszProxy       = NULL,
		_In_opt_ LPCWSTR lpszProxyBypass = INTERNET_INVALID_PORT_NUMBER,
		_In_     DWORD   dwFlags         = 0,
		_In_     DWORD   TimeOut         = 2 * 60 * 1000
		)
	{
		Close();

		if (m_hSession = ::InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags))
		{
			
			if (TimeOut)
			{
				::InternetSetOption(m_hSession, INTERNET_OPTION_CONNECT_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_CONTROL_SEND_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_DATA_SEND_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_DISCONNECTED_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_FROM_CACHE_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_LISTEN_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_RECEIVE_TIMEOUT, &TimeOut, sizeof(TimeOut));
				::InternetSetOption(m_hSession, INTERNET_OPTION_SEND_TIMEOUT, &TimeOut, sizeof(TimeOut));
			}
		}
		return m_hSession ? S_OK : HresultFromBool();
	}

	HRESULT InternetConnectW(
		//_In_ HINTERNET hInternet,
		//_In_ LPCWSTR lpszServerName,
		//_In_ INTERNET_PORT nServerPort,
		_In_opt_ LPCWSTR   lpszUserName = NULL,
		_In_opt_ LPCWSTR   lpszPassword = NULL,
		//_In_ DWORD dwService,
		_In_     DWORD     dwFlags      = 0,
		_In_opt_ DWORD_PTR dwContext    = NULL
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

		return m_hConnect ? S_OK : HresultFromBool();
	}

	HRESULT HttpOpenRequestW(
		//_In_ HINTERNET hConnect,
		_In_opt_ LPCWSTR lpszVerb=NULL,
		//_In_opt_ LPCWSTR lpszObjectName,
		_In_opt_ LPCWSTR lpszVersion= _T("HTTP/1.1"),
		_In_opt_ LPCWSTR lpszReferrer=NULL,
		_In_opt_z_ LPCWSTR FAR * lplpszAcceptTypes=NULL,
		_In_ DWORD dwFlags= INTERNET_FLAG_KEEP_CONNECTION| INTERNET_FLAG_NO_AUTH,
		_In_opt_ DWORD_PTR dwContext=NULL
		)
	{
		if (hUrlFile)
		{
			InternetCloseHandle(hUrlFile);
		}

		hUrlFile = ::HttpOpenRequestW(m_hConnect, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, lplpszAcceptTypes, dwFlags, dwContext);

		
		return hUrlFile ? S_OK : HresultFromBool();
	}

	HRESULT HttpSendRequestW(
		_In_reads_opt_(dwHeadersLength) LPCWSTR lpszHeaders,
		_In_ DWORD dwHeadersLength,
		_In_reads_bytes_opt_(dwOptionalLength) LPVOID lpOptional=NULL,
		_In_ DWORD dwOptionalLength=0,
		_In_ DWORD TryCount = 20 //服务器连接失败时的重试次数
		)
	{
		for (;;)
		{
			if (::HttpSendRequestW(hUrlFile, lpszHeaders, dwHeadersLength, lpOptional, dwOptionalLength))
			{
				return S_OK;
			}

			auto lStatus = GetLastError();

			if (ERROR_FILE_NOT_FOUND == lStatus && TryCount)
			{
				--TryCount;
				
				continue;
			}

			return lStatus != ERROR_SUCCESS ? __HRESULT_FROM_WIN32(lStatus) : E_FAIL;
		}
	}

	HRESULT GetStatusCode(
		_Out_ DWORD& Status
		)
	{
		wchar_t szStatus[8];
		DWORD cszStatus = ArraySize(szStatus);

		if (!HttpQueryInfoW(hUrlFile, HTTP_QUERY_STATUS_CODE, szStatus, &cszStatus, NULL))
		{
			return HresultFromBool();
		}

		Status = wcstoul(szStatus, NULL, 10);
		
		return S_OK;
	}
};