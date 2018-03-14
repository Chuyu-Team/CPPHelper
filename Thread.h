#pragma once
#include <Windows.h>
#include <xutility>
#include "CriticalSectionHelper.h"
#include <atlutil.h>

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
	Thread(_Ptr& _ptr, DWORD dwCreationFlags = 0)
	{
		hThread = (HANDLE)_beginthreadex(NULL, 0, [](void * UserData) ->unsigned
		{
			auto Ptr = (_Ptr*)UserData;


			(*Ptr)();


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


#define Task Thread

class CATLThreadPoolWorker
{
public:
	std::function<void()> Call;

	HANDLE hWorkEvent;

	volatile LONG Count;
	BOOL bCancelled;


	template<class _Ptr>
	//dwCreationFlags请参考CreateThread
	CATLThreadPoolWorker(_Ptr&& _ptr)
		: Call(std::move(_ptr))
		, Count(0)
		, bCancelled(FALSE)
		, hWorkEvent(CreateEvent(NULL, TRUE, TRUE, NULL))
	{
	}

	//开始执行任务
	void John()
	{
		if(!bCancelled)
			Call();

		Release();
	}

	//添加任务引用
	void AddRef()
	{
		if (InterlockedIncrement(&Count)==1)
		{
			ResetEvent(hWorkEvent);
		}
	}

	//减少任务引用
	void Release()
	{
		if (InterlockedDecrement(&Count) == 0)
		{
			SetEvent(hWorkEvent);
		}
	}

	DWORD __fastcall Wait(_In_ DWORD dwMilliseconds= INFINITE)
	{
		return WaitForSingleObject(hWorkEvent,dwMilliseconds);
	}
};

class CATLThreadPoolWorkerInternal
{
public:
	typedef CATLThreadPoolWorker* RequestType;

	static BOOL Initialize(void *pvParam)
	{
		return TRUE;
	}

	static void Terminate(void* pvParam)
	{
	}

	static void Execute(
		_In_ typename RequestType request,
		_In_ void *pvWorkerParam,
		_In_ OVERLAPPED *pOverlapped
		)
	{
		ATLASSERT(request != NULL);

		request->John();
	}

	static BOOL GetWorkerData(DWORD /*dwParam*/, void ** /*ppvData*/)
	{
		return FALSE;
	}
};


class CThreadPool2: public CThreadPool<CATLThreadPoolWorkerInternal>
{
public:

	BOOL QueueRequest(_In_ typename CATLThreadPoolWorkerInternal::RequestType request)
	{
		request->AddRef();
		
		if (CThreadPool<CATLThreadPoolWorkerInternal>::QueueRequest(request))
		{
			return TRUE;
		}
		else
		{
			//提交任务失败

			request->Release();
			return FALSE;
		}
	}

	//并行For，使用指针
	template<class Item, class _Ptr>
	void For(Item* pItems, LONG Count, _Ptr& _ptr)
	{
		volatile LONG Index = -1;

		CATLThreadPoolWorker Work([&Index, pItems,&_ptr]()
		{
			_ptr(pItems[InterlockedIncrement(&Index)]);
		});

		for (long i = 0; i != Count; ++i)
		{
			QueueRequest(&Work);
		}

		Work.Wait();
	}

	//并行For，使用迭代器
	template<class iterator, class _Ptr>
	void For(iterator Begin, iterator End, _Ptr& _ptr)
	{
		CCriticalSection Lock;
		iterator pItems = Begin;
		CATLThreadPoolWorker Work([&pItems,&Lock, &_ptr]()
		{
			Lock.Lock();

			auto& Item = *(pItems++);
			Lock.Unlock();

			_ptr(Item);
		});

		for (; Begin!= End; ++Begin)
		{
			QueueRequest(&Work);
		}

		Work.Wait();
	}
};