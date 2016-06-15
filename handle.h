#pragma once

//#include <Wimgapi.h>

#define DefHandle(ClassName,type,hInvalidHandle,TCloseHandle)\
class ClassName\
{\
	public:\
	type hHandle;\
	ClassName(type _hHandle = hInvalidHandle):hHandle(_hHandle){}\
	~ClassName(){ Close(); }\
	type* operator &(){	return &hHandle;  } \
	type operator=(type hHandle){ Close(); return this->hHandle = hHandle; }\
	operator type() {return hHandle;}\
	BOOL IsInvalid(){return hHandle == hInvalidHandle;	}\
	type Detach(){ type temp=hHandle; hHandle=hInvalidHandle;return temp; }\
	HRESULT Close(){ if(IsInvalid()) return ERROR_INVALID_HANDLE; HRESULT hr=##TCloseHandle; if(!hr) hHandle=hInvalidHandle; return hr;	}\
};

//关闭句柄形式1，直接返回 HRESULT
#define TCloseHandle1(_TCloseHandle) _TCloseHandle(hHandle)

//关闭句柄形式2，函数返回BOOL值，并通过GetLastError返回错误代码
#define TCloseHandle2(_TCloseHandle) _TCloseHandle(hHandle) ? S_OK : GetLastError()


//用于关闭RegOpenKey打开的句柄
DefHandle(CHKEY, HKEY, NULL, TCloseHandle1(RegCloseKey))

//用于关闭CreateFile打开的句柄
DefHandle(CHFile, HANDLE, INVALID_HANDLE_VALUE, TCloseHandle2(CloseHandle))

//用于关闭FindFirstFile打开的句柄
DefHandle(CHFileFind, HANDLE, INVALID_HANDLE_VALUE, TCloseHandle2(FindClose))

//自动关闭LoadLibrary的句柄
DefHandle(CHModule, HMODULE, NULL, TCloseHandle2(FreeLibrary))

//自动关闭互斥量的句柄
DefHandle(CHMutex, HANDLE, NULL, TCloseHandle2(CloseHandle))


typedef CHMutex CHEvent;

typedef CHMutex CHProcess;

#ifdef _WIMGAPI_H_
DefHandle(CHWim, HANDLE, NULL, TCloseHandle2(WIMCloseHandle))
#endif