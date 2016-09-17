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

class ThreadPoolWork
{
public:
	PTP_WORK m_Work;
	ThreadPoolWork(PTP_WORK pWork)
		:m_Work(pWork)
	{
	}


	ThreadPoolWork(ThreadPoolWork const&) = delete;
	ThreadPoolWork(ThreadPoolWork&& tmp)
	{
		m_Work = tmp.m_Work;
		tmp.m_Work = NULL;
	}

	ThreadPoolWork& operator=(ThreadPoolWork const&) = delete;

	~ThreadPoolWork()
	{
		CloseThreadpoolWork(m_Work);
	}

	void Wait()
	{
		Wait(FALSE);
	}

	void Wait(_In_ BOOL fCancelPendingCallbacks)
	{
		WaitForThreadpoolWorkCallbacks(m_Work, fCancelPendingCallbacks);
	}

	//加入工作队列
	void Submit()
	{
		SubmitThreadpoolWork(m_Work);
	}
};

class ThreadPool
{
public:
	PTP_POOL pPool;
	TP_CALLBACK_ENVIRON pcbe;


	ThreadPool(PTP_POOL _pPool = CreateThreadpool(NULL))
		:pPool(_pPool)
	{
		InitializeThreadpoolEnvironment(&pcbe);

		SetThreadpoolCallbackPool(&pcbe, pPool);
	}

	~ThreadPool()
	{
		DestroyThreadpoolEnvironment(&pcbe);
		//关闭线程池
		CloseThreadpool(pPool);
	}

	ThreadPool(ThreadPool const&) = delete;
	ThreadPool& operator=(ThreadPool const&) = delete;

	BOOL SetThreadMinimum(DWORD cthrdMic)
	{
		return SetThreadpoolThreadMinimum(pPool, cthrdMic);
	}

	void SetThreadMaximum(DWORD cthrdMost)
	{
		SetThreadpoolThreadMaximum(pPool, cthrdMost);
	}


	//提交任务
	template<class _Ptr>
	BOOL TrySubmit(_Ptr&& _ptr)
	{
		return TrySubmitThreadpoolCallback([](PTP_CALLBACK_INSTANCE Instance, PVOID Context) ->void
		{
			auto Ptr = (_Ptr*)Context;

			(*Ptr)();

			delete Ptr;

		}, (void*)new _Ptr(std::move(_ptr)), &pcbe);
	}

	template<class _Ptr>
	ThreadPoolWork CreateWork(_Ptr&& _ptr)
	{
		return ThreadPoolWork(CreateThreadpoolWork([](PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WORK Work)->void
		{

			auto Ptr = (_Ptr*)Context;

			(*Ptr)();
			//auto Ptr = (_Ptr*)Context;

			//(*Ptr)();

			//delete Ptr;

		}, (void*)&_ptr, &pcbe));
	}

	//并行for
	template<class Items, class _Ptr>
	void For(Items& m_Items, _Ptr&& _ptr)
	{
		volatile auto b = m_Items.begin();
		CComAutoCriticalSection Section;

		auto&& Work = CreateWork([&b, &Section, &_ptr]()
		{
			Section.Lock();

			auto& Item = *(b++);

			Section.Unlock();

			_ptr(Item);
		});

		for (auto& Item : m_Items)
		{
			Work.Submit();
		}

		Work.Wait();

	}

	//并行for
	template<class Item, class _Ptr>
	void For(Item* pItems, LONG Count, _Ptr&& _ptr)
	{
		volatile LONG Index = -1;

		auto&& Work = CreateWork([&Index, &_ptr, pItems]()
		{
			_ptr(pItems[InterlockedIncrement(&Index)]);
		});

		for (long i = 0;i != Count;++i)
		{
			Work.Submit();
		}

		Work.Wait();
	}
};

//通过系统内置线程池创建异步任务
template<class _Ptr>
static BOOL __fastcall Task(_Ptr&& _ptr)
{
	return TrySubmitThreadpoolCallback([](PTP_CALLBACK_INSTANCE Instance, PVOID Context) ->void
	{
		auto Ptr = (_Ptr*)Context;

		(*Ptr)();

		delete Ptr;

	}, (void*)new _Ptr(std::move(_ptr)), NULL);
}