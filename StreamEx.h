#pragma once
#include <comdef.h>


//用于支持多线程快速随机存取
MIDL_INTERFACE("AD0E66E8-5F08-4763-BBA0-F4873EC0DCFB")
IReadStream : public IUnknown
{
public:
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		_In_opt_ UINT64* pByteOffset) = 0;

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition) = 0;

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize) = 0;
};

//支持写入
MIDL_INTERFACE("7C0A26D6-BC9A-4D80-8B54-B0C35824DAAC")
IReadWriteStream : public IReadStream
{
public:
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
		/* [annotation] */
		_In_reads_bytes_(cb)  const void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbWritten,
		_In_opt_ UINT64* pByteOffset) = 0;

	virtual HRESULT STDMETHODCALLTYPE put_Size(_In_ UINT64 Size) = 0;
};



//创建只读IStreamEx流
IReadStream* StreamCreate(const void* pData, UINT64 Size);

//根据现有流，创建一个局部流
IReadStream* StreamCreate(IReadStream* pStream, UINT64 Start, UINT64 Size);

//HRESULT StreamCreate(LPCWSTR FilePath, IReadStream** ppStream);

IReadWriteStream* StreamCreate(LPCWSTR FilePath,
	_In_ DWORD dwDesiredAccess=GENERIC_READ,
	_In_ DWORD dwShareMode = FILE_SHARE_DELETE|FILE_SHARE_READ,
	_In_ DWORD dwCreationDisposition = OPEN_EXISTING,
	_In_ DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS );

//从一个只读流虚拟出一个可写入的流
IReadWriteStream* StreamCreateVirtualWrite(IReadStream* pStream);