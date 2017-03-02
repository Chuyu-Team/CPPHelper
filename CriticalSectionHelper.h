#pragma once
#include <Windows.h>


//自动进行初始化。自动释放临界区
class CCriticalSection : private CRITICAL_SECTION
{
public:
	BOOL IsInitialize;
	CCriticalSection()
		:IsInitialize(InitializeCriticalSectionEx(this, 0, CRITICAL_SECTION_NO_DEBUG_INFO))
	{
	}

	//禁用复制构造
	CCriticalSection(const CCriticalSection&) = delete;

	//禁用赋值
	CCriticalSection& operator=(const CCriticalSection&) = delete;


	~CCriticalSection()
	{
		if(IsInitialize)
			DeleteCriticalSection(this);
	}


	void Lock()
	{
		if(IsInitialize)
			EnterCriticalSection(this);
	}

	void Unlock()
	{
		if (IsInitialize)
			LeaveCriticalSection(this);
	}
};

template<class T>
class CLockHelper
{
	
public:
	T* pLock;

	CLockHelper(T* _pLock)
		:pLock(_pLock)
	{
		pLock->Lock();
	}

	~CLockHelper()
	{
		pLock->Unlock();
	}
};

typedef CLockHelper<CCriticalSection> CCriticalLockHelper;


//读写锁
class CSRWLock : SRWLOCK
{
public:
	CSRWLock()
	{
		InitializeSRWLock(this);
	}

	~CSRWLock()
	{
	}


	void ReadLock()
	{
		AcquireSRWLockShared(this);
	}

	void ReadUnlock()
	{
		ReleaseSRWLockShared(this);
	}

	void WriteLock()
	{
		AcquireSRWLockExclusive(this);
	}

	void WriteUnLock()
	{
		ReleaseSRWLockExclusive(this);
	}
};