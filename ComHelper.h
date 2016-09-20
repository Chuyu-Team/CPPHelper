#pragma once

#include <comdef.h>
#include <atlcom.h>
#include <vector>

///////////////////////////////////////////////////
//
//          用户改进Com接口编写的便利性  
//
///////////////////////////////////////////////////


#define _QueryMapBegin(T) virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject){\
if(((T*)this)->QueryMap(riid,ppvObject)==S_OK) return S_OK;


//#define _QueryMapEnd(T) return T::QueryInterface(riid,ppvObject);}
#define _QueryMapEnd return E_NOINTERFACE;}

#define __QueryInterface(T) \
	if(__uuidof(T)==riid)\
	{\
		*ppvObject = (T*)this;\
		AddRef();\
		return S_OK;\
	}



template <class Base,class T>
class IUnknownT : public T
{
private:
	ULONG RefCount;
//protected:
//	virtual HRESULT MyQueryInterface(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
//	{
//		_QueryInterface(IUnknown);
//		_QueryInterface(T);
//
//		return E_NOINTERFACE;
//	}

public:
	IUnknownT()
		:RefCount(1)
	{
	}

	~IUnknownT()
	{
	}

	HRESULT QueryMap(REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)
	{
		return E_NOINTERFACE;
	}

	_QueryMapBegin(Base)
		__QueryInterface(IUnknown);
		__QueryInterface(T);
	_QueryMapEnd

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return ++RefCount;
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		if (--RefCount)
		{
			return RefCount;
		}
		else
		{
			delete (Base*)this;
			return 0;
		}
	}
};

template <class Base,class T, class ItemType>
class ICollectionT :public IUnknownT<Base,T>
{
	
public:
	std::vector<IUnknown*> m_Collection;

	~ICollectionT()
	{
		Cleanup();
	}
	void Cleanup()
	{
		for (auto Item : m_Collection)
		{
			Item->Release();
		}

		m_Collection.clear();
	}

	void PushItem(IUnknown* Item)
	{
		m_Collection.push_back(Item);
	}

	virtual HRESULT WINAPI get_Count(_Out_ long* pcount)
	{
		*pcount = m_Collection.size();
		return S_OK;
	}

	virtual HRESULT WINAPI get_Item(_In_ long Index, _Out_ ItemType** pvar)
	{
		if (pvar == NULL)
			return E_POINTER;

		if (Index < 0 || Index >= m_Collection.size())
			return E_INVALIDARG;

		return m_Collection[Index]->QueryInterface(pvar);
	}

	virtual HRESULT WINAPI get__NewEnum(_Out_ IUnknown** ppUnk)
	{
		return E_NOINTERFACE;
	}
};

typedef HRESULT(_stdcall*TDllGetClassObject) (REFCLSID, REFIID, LPVOID*);

template<class T, class CLSID>
class DllGetClassObjectHelper
{
private:
	HMODULE hModule;
	
public:
	T* p;

	DllGetClassObjectHelper()
		:hModule(NULL)
		, p(NULL)
	{
	}

	~DllGetClassObjectHelper()
	{
		Close();
	}

	void Close()
	{
		if (hModule)
		{
			if (p)
			{
				p->Release();

				p = NULL;
			}

			FreeLibrary(hModule);

			hModule = NULL;
		}
	}

	T* operator->()
	{
		return p;
	}


	HRESULT LoadDll(LPCWSTR DllName)
	{
		Close();

		hModule = LoadLibrary(DllName);

		if (hModule == NULL)
			return GetLastError();

		
		TDllGetClassObject _DllGetClassObject = (TDllGetClassObject)GetProcAddress(hModule, "DllGetClassObject");

		HRESULT hr = S_OK;

		if (_DllGetClassObject)
		{
			IClassFactory* pIClassFactory;

			hr= _DllGetClassObject(__uuidof(CLSID), IID_IClassFactory, (LPVOID*)&pIClassFactory);

			if (SUCCEEDED(hr))
			{
				hr=pIClassFactory->CreateInstance(NULL, __uuidof(T), (LPVOID *)&p);

				pIClassFactory->Release();

				if (SUCCEEDED(hr))
				{
					return S_OK;
				}
			}
		}
		else
		{
			hr = GetLastError();
		}

		
		Close();

		return hr;
	}
};

class ComInitializeWrapper
{
public:
	HRESULT hr;
	ComInitializeWrapper(DWORD dwCoInit)
		:hr(CoInitializeEx(NULL, dwCoInit))
	{
		
	}

	ComInitializeWrapper()
		:hr(CoInitialize(NULL))
	{

	}

	~ComInitializeWrapper()
	{
		if (SUCCEEDED(hr))
			CoUninitialize();
	}
};