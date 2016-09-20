#pragma once
#include <Windows.h>
#include <atlstr.h>

class UdfString
{
public:
	CStringA Data;
	//byte type;

	void Parse(int start, byte* buffer, int size)
	{
		buffer += start;

		//type = *buffer;
		//Data = new byte[size];
		//Data = UdfHelper.Readbytes(start, buffer, size);

		Data.SetString((char*)buffer, size);
	}

	CString GetString() const
	{
		return ParseString((byte*)Data.GetString(), Data.GetLength());
	}

	static CString ParseString(byte* data, int size)
	{
		CString Temp;

		if(size==0)
			return Temp;

		/*if (size > 0 && data != null)
		{
			var sb = new StringBuilder();
			if (type == 8) {
				for (int i = 1; i < size; i++) {
					char c = (char)data[i];
					if (c == 0)
						break;
					sb.Append(c);
				}
			}
			else if (type == 16) {
				for (int i = 1; i + 2 <= size; i += 2) {
					char c = (char)((data[i + 1]) | data[i] << 8);
					sb.Append(c);
				}
			}
			return sb.ToString().TrimEnd();
		}
		return string.Empty;*/

		switch (*data)
		{
		case 8:
			Temp.GetBuffer(size);
			for (int i = 1;i != size&&data[i];++i)
			{
				Temp.AppendChar(data[i]);
			}
			
			break;
		
		case 16:
			--size;
			for (int i = 1;i < size;i+=2)
			{
				Temp.AppendChar(((wchar_t)data[i]<<8)| data[i+1]);
			}
			
			break;
		}

		return Temp;
	}
};

class UdfString128 :public UdfString
{
public:
	void Parse(int start, byte* buffer)
	{
		UdfString::Parse(start, buffer,128);
		//buffer += start;
		//Data.SetString((char*)buffer, 128);

		//Data = UdfHelper.Readbytes(start, buffer, 128);
		//type = Data[0];
	}
};