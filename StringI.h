#pragma once

#include <atlstr.h>
#include <utility>

class CStringI :
	public CString
{
public:
	CStringI()
		:CString()
	{

	}

	CStringI(LPCWSTR Str)
		:CString(Str)
	{
	}
	CStringI(LPCWSTR Str,int cchStr)
		:CString(Str, cchStr)
	{
	}

	/*CStringI(CStringI& Str)
		:CString(Str)
	{
	}*/
	CStringI(CString& Str)
		:CString(Str)
	{
	}
	CStringI(const CString& Str)
		:CString(Str)
	{
	}
	/*CStringI(const CStringI& Str)
		:CString(Str)
	{
	}*/

	CStringI MakeLowerI() const
	{
		CStringI Temp = *this;

		Temp.MakeLower();

		return Temp;
	}

	/*operator PCWSTR() const
	{
		return GetString();
	}*/

	friend bool operator!=(const CStringI& a, const CString& b)
	{
		return StrCmpI(a, b) != 0;
	}

	friend bool operator==(const CStringI& a, const CStringI& b)
	{
		return StrCmpI(a, b) == 0;
	}

	friend bool operator<(const CStringI& a, const CStringI& b)
	{
		return StrCmpI(a, b)<0;
	}
	friend bool operator<=(const CStringI& a, const CStringI& b)
	{
		return StrCmpI(a, b) <= 0;
	}

	friend bool operator>(const CStringI& a, const CStringI& b)
	{
		return  StrCmpI(a, b)>0;
	}
	friend bool operator>=(const CStringI& a, const CStringI& b)
	{
		return StrCmpI(a, b) >= 0;
	}
};