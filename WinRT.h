#pragma once

#include <atlcomcli.h>

//C++ CX接口支持

#include <wrl\client.h>
#include <wrl\wrappers\corewrappers.h>

using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#pragma comment(lib,"runtimeobject.lib")



#include <windows.management.deployment.h>
#include <AppxPackaging.h>

#include <Windows.ApplicationModel.resources.core.h>

using namespace ABI::Windows::Management::Deployment;

using namespace ABI::Windows::ApplicationModel;


using namespace ABI::Windows::Foundation::Collections;

using namespace ABI::Windows::Foundation::Internal;

using namespace ABI::Windows::ApplicationModel::Resources::Core;


HRESULT WINAPI DllGetActivationFactory(HSTRING activatableClassId, IActivationFactory ** factory);

static HRESULT ActivateInstance(
	_In_z_       LPCWSTR ActivatableClassId,
	_In_         REFIID  riid,
	_COM_Outptr_ void**  instance
	)
{
	*instance = NULL;
	CComPtr<IInspectable> pInspectable;

	HString activatableClassId;

	activatableClassId.Set(ActivatableClassId);


	HRESULT hr = RoActivateInstance(activatableClassId.Get(), &pInspectable);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = pInspectable->QueryInterface(riid, instance);

	return FAILED(hr) ? hr : S_OK;
}

template<class Q>
static HRESULT ActivateInstance(
	_In_z_       LPCWSTR ActivatableClassId,
	_COM_Outptr_ Q**     instance
	)
{
	return ActivateInstance(ActivatableClassId, __uuidof(Q), (void**)instance);
}

MIDL_INTERFACE("7d9da47a-8bc7-49d3-97aa-f7db06049172")
IResourceManagerFactory : public IInspectable
{
public:
	virtual HRESULT WINAPI GetResourceManagerForSystemProfile(
		_COM_Outptr_ IResourceManager**
		) = 0;

	virtual HRESULT WINAPI GetCurrentResourceManagerForSystemProfile(
		_COM_Outptr_ IResourceManager**
		) = 0;
};

template<class T>
static HRESULT GetActivationFactory(
	_In_z_       LPCWSTR ActivatableClassId,
	_COM_Outptr_ T**     ppFactory
	)
{
	HString _ActivatableClassId;
	_ActivatableClassId.Set(ActivatableClassId);
	return RoGetActivationFactory(_ActivatableClassId.Get(), __uuidof(T), (void**)ppFactory);
}




////////////////////////////////////////////////////////////
//IIterable迭代器支持，便于IIterable支持for遍历
template<typename Q>
class IterableIterator
{
	bool get_HasCurrent() const
	{
		if (pfirst)
		{
			boolean fHasCurrent = false;
			pfirst->get_HasCurrent(&fHasCurrent);

			return fHasCurrent;
		}
		else
		{
			return false;
		}
	}

public:
	typename IIterator<typename GetLogicalType<typename Q::T_complex>::type>* pfirst;
	typedef typename GetAbiType<typename Q::T_complex>::type type;

	void* v1;

	IterableIterator(typename Q* pCollection)
		: pfirst(NULL)
		, v1(NULL)
	{
		if (pCollection)
		{
			pCollection->First(&pfirst);
		}
	}


	~IterableIterator()
	{
		if (pfirst)
			pfirst->Release();

		if (v1)
			((type)v1)->Release();
	}


	void operator++()
	{
		if (v1)
		{
			((type)v1)->Release();

			v1 = NULL;
		}

		assert(get_HasCurrent());
		
		boolean fHasCurrent = false;

		
		pfirst->MoveNext(&fHasCurrent);

	}


	bool operator!=(const IterableIterator& Item) const
	{
		if (get_HasCurrent())
		{

			return Item.get_HasCurrent()==false||pfirst != Item.pfirst;
			
		}
		else
		{
			return Item.get_HasCurrent();
		}
	}

	type operator*() const
	{
		assert(get_HasCurrent());
		
		//type v1 = NULL;
		if (v1 == NULL)
		{
			pfirst->get_Current((type*)&v1);

		}
		return (type)v1;
	}
};


//IIterable的安全封装，自动释放空间
template <typename Q>
class IIterable2
{
public:
	typedef typename IterableIterator<typename Q> MyIterator;

	CComPtr<typename Q> pCollection;

	

	MyIterator end() const
	{

		return MyIterator(NULL);
	}

	MyIterator begin() const
	{
		return MyIterator(pCollection.p);
	}


	typename Q** operator&()
	{
		return &pCollection.p;
	}

	operator typename Q*() const
	{
		return pCollection.p;
	}

	typename Q* operator->() const
	{
		return pCollection.p;
	}
};