#pragma once
#include <atlstr.h>

class CBuffer :public CStringA
{
public:
	void Append(const void* pData, DWORD cbData)
	{
		auto NewLength = GetLength() + cbData;
		auto pBuffer = GetBuffer(NewLength);

		memcpy(pBuffer+ GetLength(), pData, cbData);

		ReleaseBufferSetLength(NewLength);
	}
};