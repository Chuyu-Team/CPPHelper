#pragma once

#define __StrCat(a,b) a##b
#define __StrCat2(a,b) __StrCat(a,b)


//程序崩溃支持
//#include <DbgHelp.h>

#include <imagehlp.h> 
#pragma comment(lib, "imagehlp.lib")
#pragma comment(lib,"DbgHelp.lib")
#if _MSC_VER > 1500
#include <functional>
#include <atlstr.h>
#include <atltime.h>
#endif

static HRESULT CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException, MINIDUMP_TYPE DumpType)
{
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hDumpFile == INVALID_HANDLE_VALUE)
		return GetLastError();
	// Dump信息  

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// 写入Dump文件内容  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, DumpType, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);

	return S_OK;
}

#if _MSC_VER > 1500

class Guard
{
private:
	std::function<void()> __Prt;


	Guard(Guard const&);
	Guard& operator=(Guard const&);

public:
	Guard(std::function<void()>&& _Prt)
		:__Prt(move(_Prt))
	{
	}

	~Guard()
	{
		__Prt();
	}
};

//资源守卫者，负责执行完毕后自动回收资源,防止资源泄露
#define RunOnExit(callback) Guard __StrCat2(__OnEixtName,__LINE__)(callback)

//当程序崩溃时运行……
#define RunOnCrash(callback)\
SetUnhandledExceptionFilter([](EXCEPTION_POINTERS* excp)->long \
{\
	MINIDUMP_TYPE DumpType= callback();\
	if(DumpType!=-1)\
	{\
		CString Path; \
		Path.ReleaseBufferSetLength(GetModuleFileName(NULL, Path.GetBuffer(MAX_PATH), MAX_PATH+1)); \
		Path += CTime::GetTickCount().Format(L"%Y-%m-%d %H-%M-%S") + L".dmp"; \
		CreateDumpFile(Path, excp, DumpType); \
	}\
	return EXCEPTION_EXECUTE_HANDLER;\
})


#endif