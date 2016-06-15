#pragma once
#include <Windows.h>
#include <xutility>


///////////////////////////////////////////////////////////////////////////////////////////////
//
//                  自制的一个轻量级线程封装类
//
//////////////////////////////////////////////////////////////////////////////////////////////



class Thread
{
public:
	HANDLE hThread;

	template<class _Ptr>
	//dwCreationFlags请参考CreateThread
	Thread(_Ptr&& _ptr, DWORD dwCreationFlags = 0)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, [](void * UserData) ->unsigned
		{
			auto Ptr = (_Ptr*)UserData;

			auto hr=CoInitialize(NULL);

			(*Ptr)();

			if (SUCCEEDED(hr))
				CoUninitialize();

			delete Ptr;

			return 0;

		}, (void*)new _Ptr(std::move(_ptr)), dwCreationFlags, NULL);
	}

	template<class _Ptr>
	Thread(COINIT ,_Ptr&& _ptr,  DWORD dwCreationFlags = 0)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, [](void * UserData) ->unsigned
		{
			auto Ptr = (_Ptr*)UserData;

			auto hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);

			
			(*Ptr)();

			if (SUCCEEDED(hr))
				CoUninitialize();

			delete Ptr;

			return 0;

		}, (void*)new _Ptr(std::move(_ptr)), dwCreationFlags, NULL);
	}

	//抑制复制构造等函数
	Thread(Thread const&);
	Thread& operator=(Thread const&);

	Thread()
		:hThread(NULL)
	{

	}
	Thread(Thread &&Item)
		:hThread(Item.Detach())
	{
	}

	~Thread()
	{
		if (hThread)
		{
			CloseHandle(hThread);
		}
	}

	HANDLE Detach()
	{
		HANDLE TempThread = hThread;
		hThread = NULL;
		return TempThread;
	}

	//恢复线程运行
	DWORD ResumeThread()
	{
		return ::ResumeThread(hThread);
	}

	//挂起线程
	DWORD SuspendThread()
	{
		return ::SuspendThread(hThread);
	}

	//等待线程，dwMilliseconds等同于WaitForSingleObject
	DWORD WaitThread(DWORD dwMilliseconds = INFINITE)
	{
		return WaitForSingleObject(hThread, dwMilliseconds);
	}
};