#pragma once
#include "StreamEx.h"
#include "ComHelper.h"
#include "km.h"

#pragma warning(push)
#pragma warning(disable: 28251)

//内存流
class CMemStream : public IUnknownT<CMemStream, IReadStream>
{
public:
	UINT64 Size;
	const BYTE* pData;
	UINT64 Point;

	CMemStream(const void* _pData, UINT64 _Size)
		: pData((byte*)_pData)
		, Size(_Size)
		, Point(0)
	{
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		UINT64* pByteOffset)
	{
		auto TempPoint = pByteOffset ? *pByteOffset : Point;

		if(TempPoint>= Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);


		ULONG cbRead = min(cb, Size - TempPoint);
		
		memcpy(pv, pData + TempPoint, cbRead);

		if(!pByteOffset)
			Point += cbRead;

		if (pcbRead)
			*pcbRead = cbRead;

		return cbRead == cb ? S_OK : S_FALSE;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition)
	{
		switch (dwOrigin)
		{
		case FILE_CURRENT:
			dlibMove += Point;
			break;
		case FILE_BEGIN:
			break;
		case FILE_END:
			dlibMove += Size;
			break;
		default:
			return __HRESULT_FROM_WIN32(87);
			break;
		}

		if(dlibMove<0)
			return __HRESULT_FROM_WIN32(87);

		if(dlibMove>Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		Point = dlibMove;

		if (plibNewPosition)
			*plibNewPosition = Point;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize)
	{
		*pSize = Size;
		return S_OK;
	}
};

IReadStream * StreamCreate(const void * _pData, UINT64 _Size)
{
	return (IReadStream*)new CMemStream(_pData, _Size);
}

//局部流
class CPartStream : public IUnknownT<CPartStream, IReadStream>
{
public:
	UINT64 Size;
	UINT64 Start;
	IReadStream* pStream;
	UINT64 Point;

	CPartStream(IReadStream* _pStream, UINT64 _Start, UINT64 _Size)
		: pStream(_pStream)
		, Start(_Start)
		, Size(_Size)
		, Point(0)
	{
		pStream->AddRef();
	}

	~CPartStream()
	{
		pStream->Release();
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		_In_ UINT64* pByteOffset)
	{
		UINT64 _Start = pByteOffset ? *pByteOffset : Point;

		if (_Start >= Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);


		ULONG cbRead = min(cb, Size - _Start);

		_Start += Start;

		auto hr = pStream->Read(pv, cbRead, &cbRead, &_Start);

		if (FAILED(hr))
			return hr;

		//更新文件指针
		if (!pByteOffset)
			Point += cbRead;

		if (pcbRead)
			*pcbRead = cbRead;

		return cbRead == cb ? S_OK : S_FALSE;
	}

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize)
	{
		*pSize = Size;
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition)
	{
		switch (dwOrigin)
		{
		case FILE_CURRENT:
			dlibMove += Point;
			break;
		case FILE_BEGIN:
			break;
		case FILE_END:
			dlibMove += Size;
			break;
		default:
			return __HRESULT_FROM_WIN32(87);
			break;
		}

		if (dlibMove<0)
			return __HRESULT_FROM_WIN32(87);

		if (dlibMove>Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);

		Point = dlibMove;

		if (plibNewPosition)
		{
			*plibNewPosition = Point;
		}

		return S_OK;
	}
};


IReadStream* StreamCreate(IReadStream* pStream, UINT64 Start, UINT64 Size)
{
	if (pStream == NULL || Size == 0)
	{
		SetLastError(87);

		return NULL;
	}

	UINT64 TSize;

	auto hr = pStream->get_Size(&TSize);

	if (hr!=S_OK)
	{
		SetLastError(hr);

		return NULL;
	}
	//参数错误
	if (TSize < Start + Size)
	{
		SetLastError(87);

		return NULL;
	}

	return (IReadStream*)new CPartStream(pStream, Start, Size);
}

//文件流
class CFileStream : public IUnknownT<CFileStream, IReadWriteStream>
{
public:
	HANDLE hFile;

	CFileStream(HANDLE _hFile)
		:hFile(_hFile)
	{
	}

	~CFileStream()
	{
		CloseHandle(hFile);
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		_Inout_opt_ UINT64* pByteOffset)
	{
		IO_STATUS_BLOCK  IoStatusBlock;
		
		auto hr=NtReadFile(hFile, NULL, NULL, NULL, &IoStatusBlock, pv, cb,(PLARGE_INTEGER)pByteOffset, NULL);

		if (hr)
		{
			return HRESULT_FROM_WIN32(RtlNtStatusToDosError(hr));
		}

		if (pcbRead)
		{
			*pcbRead = IoStatusBlock.Information;
		}

		return cb == IoStatusBlock.Information ? S_OK : S_FALSE;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition)
	{
		if (SetFilePointerEx(hFile, *(LARGE_INTEGER*)&dlibMove, (PLARGE_INTEGER)plibNewPosition, dwOrigin))
		{
			return S_OK;
		}
		else
		{
			return HresultFromBool();
		}
	}

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize)
	{
		return GetFileSizeEx(hFile, (PLARGE_INTEGER)pSize) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
		/* [annotation] */
		_In_reads_bytes_(cb)  const void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbWritten,
		_Inout_opt_ UINT64* pByteOffset)
	{
		IO_STATUS_BLOCK  IoStatusBlock;

		auto Status =NtWriteFile(hFile, NULL, NULL, NULL, &IoStatusBlock, (void*)pv, cb, (PLARGE_INTEGER)pByteOffset, NULL);

		if (Status<0)
		{
			return HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
		}

		if (pcbWritten)
		{
			*pcbWritten = IoStatusBlock.Information;
		}

		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE put_Size(_In_ UINT64 Size)
	{
		IO_STATUS_BLOCK IoStatusBlock;

		auto Status = NtSetInformationFile(hFile, &IoStatusBlock, &Size, sizeof(FILE_END_OF_FILE_INFORMATION), FileEndOfFileInformation);

		if (Status < 0)
			return HRESULT_FROM_WIN32(RtlNtStatusToDosError(Status));
		else
			return S_OK;
	}
};


IReadWriteStream* StreamCreate(LPCWSTR FilePath,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes )
{
	auto hFile = CreateFile(FilePath, dwDesiredAccess, dwShareMode,NULL, dwCreationDisposition, dwFlagsAndAttributes| FILE_FLAG_BACKUP_SEMANTICS,0);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;


	return (IReadWriteStream*)new CFileStream(hFile);
}

struct VirtualWriteBuffer
{
	BYTE Data[1024 * 1024];
};

class CVirtualWriteStream : public IUnknownT<CVirtualWriteStream, IReadWriteStream>
{
	//HANDLE hFile;
	std::map<UINT64, CStringA> Ceche;

	UINT64 Size;
	IReadStream* pStream;
	UINT64 Point;
public:
	CVirtualWriteStream(IReadStream* _pStream)
		:pStream(_pStream)
		, Point(0)
	{
		pStream->AddRef();
		pStream->get_Size(&Size);
	}

	~CVirtualWriteStream()
	{
		pStream->Release();
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
		/* [annotation] */
		_Out_writes_bytes_to_(cb, *pcbRead)  void *pv,
		/* [annotation][in] */
		_In_  ULONG cb,
		/* [annotation] */
		_Out_opt_  ULONG *pcbRead,
		_In_opt_ UINT64* pByteOffset)
	{
		auto Offset = pByteOffset ? *pByteOffset : Point;

		if (Offset >= Size)
			return __HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);


		ULONG cbRead = min(cb, Size - Offset);


		auto hr=pStream->Read(pv, cbRead, &cbRead,&Offset);

		if (FAILED(hr))
			return hr;

		if (!pByteOffset)
		{
			Point += cbRead;
		}

		if (pcbRead)
			*pcbRead = cbRead;

		auto End = Offset + cbRead;

		for (auto& Item : Ceche)
		{
			if (End <= Item.first)
				break;

			if (Offset >= Item.first + Item.second.GetLength())
				continue;

			INT64 NewOffect = Offset - Item.first;

			if (NewOffect <= 0)
			{
				memcpy((byte*)pv - NewOffect, Item.second.GetString(), min(Item.second.GetLength(), cbRead + NewOffect));
			}
			else
			{
				memcpy(pv, Item.second.GetString() + NewOffect, min(Item.second.GetLength() - NewOffect, cbRead));
			}
		}
		return S_OK;
	}

	//virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read(
	//	/* [annotation] */
	//	_Out_writes_bytes_to_(cb, *pcbRead)  void *pBuffer,
	//	/* [annotation][in] */
	//	_In_  ULONG nNumberOfBytesToRead,
	//	/* [annotation] */
	//	_Out_opt_  ULONG *pNumberOfBytesRead)
	//{
	//	LARGE_INTEGER Offset = *(LARGE_INTEGER*)&Overlapped.Offset;

	//	//if (!SetFilePointerEx(hFile, LARGE_INTEGER{}, &Offset, FILE_CURRENT))
	//	//{
	//	//	goto Error;
	//	//}

	//	auto Ret = ReadFile(hFile, pBuffer, nNumberOfBytesToRead, pNumberOfBytesRead, &Overlapped);
	//	Overlapped = OVERLAPPED{};
	//	*(LARGE_INTEGER*)&Overlapped.Offset = Offset;

	//	if (!Ret)
	//		goto Error;


	//	DWORD NumberOfBytesRead = *pNumberOfBytesRead;
	//	auto End = Offset.QuadPart + NumberOfBytesRead;

	//	for (auto& Item : Ceche)
	//	{
	//		if (End <= Item.first)
	//			break;

	//		if (Offset.QuadPart >= Item.first + Item.second.GetLength())
	//			continue;

	//		INT64 NewOffect = Offset.QuadPart - Item.first;

	//		if (NewOffect <= 0)
	//		{
	//			memcpy((byte*)pBuffer - NewOffect, Item.second.GetString(), min(Item.second.GetLength(), NumberOfBytesRead + NewOffect));
	//		}
	//		else
	//		{
	//			memcpy(pBuffer, Item.second.GetString() + NewOffect, min(Item.second.GetLength() - NewOffect, NumberOfBytesRead));
	//		}
	//	}
	//	return S_OK;

	//Error:
	//	return HRESULT_FROM_WIN32(GetLastError());

	//}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write(
			/* [annotation] */
			_In_reads_bytes_(cb)  const void *pv,
			/* [annotation][in] */
			_In_  ULONG cb,
			/* [annotation] */
			_Out_opt_  ULONG *pcbWritten,
			_In_opt_ UINT64* pByteOffset)
	{
		auto TempPoint = pByteOffset ? *pByteOffset : Point;

		Ceche[TempPoint].SetString((char*)pv, cb);

		if (pcbWritten)
			*pcbWritten = cb;

		if (!pByteOffset)
			Point += cb;

		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ INT64 dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  UINT64 *plibNewPosition)
	{
		switch (dwOrigin)
		{
		case FILE_BEGIN:
			//*(UINT64*)&Overlapped.Offset = dlibMove.QuadPart;
			break;
		case FILE_END:
			dlibMove += Size;
			break;
		case FILE_CURRENT:
			dlibMove = Point + dlibMove;
			break;
		default:
			return E_NOTIMPL;
			break;
		}

		if (dlibMove <= Size)
		{
			Point = dlibMove;

			if (plibNewPosition)
				*plibNewPosition = Point;

			return S_OK;
		}
		else
		{
			return E_NOTIMPL;
		}
	}

	virtual HRESULT STDMETHODCALLTYPE put_Size(_In_ UINT64 NewSize)
	{
		if (NewSize <= Size)
		{
			Size = NewSize;
			return S_OK;
		}
		else
			return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE get_Size(_Out_ UINT64* pSize)
	{
		*pSize = Size;
		return S_OK;
	}
};

IReadWriteStream* StreamCreateVirtualWrite(IReadStream* pStream)
{
	return new CVirtualWriteStream(pStream);
}


#pragma warning(pop)
