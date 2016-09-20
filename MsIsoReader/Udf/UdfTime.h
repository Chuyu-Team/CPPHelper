#pragma once
#include <atltime.h>


class UdfTime :public CTime
{
public:
	void Parse(int start, byte* buffer)
	{
		buffer += start;

		*(CTime*)this = CTime(*(WORD*)(buffer +2), buffer[4], buffer[5], buffer[6], buffer[7], buffer[8]);


		auto t = (*(WORD*)buffer) & 0xFFF;
		if ((t >> 11) != 0)
			t -= (1 << 12);
		if ((t > (60 * 24) || t < -(60 * 24)))
		{
			//? 0 : t;
			*(UINT64*)this += t*60;
		}
		
	}
};