#pragma once

#include <Windows.h>
#include <atlstr.h>


static CString LCIDToName(
	_In_ DWORD Lcid
	)
{
	CString LanguagName;

	LanguagName.ReleaseBuffer(LCIDToLocaleName(Lcid, LanguagName.GetBuffer(LOCALE_NAME_MAX_LENGTH), LOCALE_NAME_MAX_LENGTH, 0) - 1);

	return LanguagName;
}