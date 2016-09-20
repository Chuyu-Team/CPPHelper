#include "XMLHelper.h"
#include "StringHelper.h"
#include "FileHelper.h"

rapidxml::XMLNote* XMLOpenNote(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath)
{
	if (pRootNote == NULL)
	{
		//SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if (StrEmpty(SubPath))
		return pRootNote;

	if (auto NextPathName = StrChrW(SubPath, L'/'))
	{
		return XMLOpenNote(pRootNote->first_node(SubPath, NextPathName - SubPath), NextPathName + 1);
	}
	else
	{
		return pRootNote->first_node(SubPath);
	}
}

rapidxml::xml_node<char>* XMLOpenNote(rapidxml::xml_node<char>* pRootNote, const char* SubPath)
{
	if (pRootNote == NULL)
	{
		//SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	if (auto NextPathName = StrChrA(SubPath, '/'))
	{
		return XMLOpenNote(pRootNote->first_node(SubPath, NextPathName - SubPath), NextPathName + 1);
	}
	else
	{
		return pRootNote->first_node(SubPath);
	}
}

rapidxml::XMLNote* XMLCreateNote(rapidxml::XMLNote* pRootNote, LPCWSTR NoteName, DWORD CreateType)
{
	if (pRootNote == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	auto NextPathName = StrChr(NoteName, L'/');

	int chSubPath = NextPathName ? NextPathName - NoteName : StrLen(NoteName);

	rapidxml::XMLNote* pChildNote = NULL;

	switch (CreateType)
	{
	case OPEN_EXISTING:
	case OPEN_ALWAYS:
		pChildNote = pRootNote->first_node(NoteName, chSubPath);

		if (CreateType == OPEN_EXISTING || pChildNote)
		{
			break;
		}
	case CREATE_ALWAYS:
		pChildNote = pRootNote->pDocument->allocate_node(rapidxml::node_element, pRootNote->pDocument->allocate_string(NoteName, chSubPath + 1), NULL, chSubPath);
		pRootNote->append_node(pChildNote);
		break;
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
		break;
	}

	if (NextPathName == NULL || pChildNote == NULL)
	{
		return pChildNote;
	}
	else
	{
		NextPathName++;
		return XMLCreateNote(pChildNote, NextPathName, CreateType);
	}
}

rapidxml::XMLNote* XMLSetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR NoteName, LPCWSTR NoteValue)
{
	auto pNote = XMLCreateNote(RootNote, NoteName, OPEN_ALWAYS);

	if (pNote == NULL)
		return NULL;

	auto cbNoteValue = StrLen(NoteValue);

	pNote->value(pNote->pDocument->allocate_string(NoteValue, cbNoteValue + 1), cbNoteValue);

	if (auto pChild = pNote->first_node())
	{
		if (pChild->next_sibling() == 0 && pChild->type() == rapidxml::node_data)
		{
			pChild->value(pNote->value(), cbNoteValue);
		}
	}



	return pNote;
}



rapidxml::XMLNote* XMLSetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR NoteName, FILETIME NoteValue)
{
	auto pNote = XMLCreateNote(RootNote, NoteName, OPEN_ALWAYS);

	if (pNote == NULL)
		return NULL;


	CString TimeStr;
	TimeStr.Format(L"0x%.8X", NoteValue.dwHighDateTime);

	XMLSetNoteValue(pNote, L"HIGHPART", TimeStr.GetBuffer());

	TimeStr.Format(L"0x%.8X", NoteValue.dwLowDateTime);
	XMLSetNoteValue(pNote, L"LOWPART", TimeStr.GetBuffer());


	return pNote;
}


HRESULT XMLCreateXMLDocumentByString(CString Str, rapidxml::XMLDocument* pDocument)
{
	return pDocument->Load(Str);
}

HRESULT XMLCreateXMLDocumentByData(const byte* pData, DWORD cbData, rapidxml::XMLDocument* pDocument, int CodePage)
{
	if (CodePage == -1)
	{
		//自动检测编码标志
		if (cbData >= sizeof(BomUTF16) && memcmp(BomUTF16, pData, sizeof(BomUTF16)) == 0)
		{
			//UTF16
			CodePage = CP_UTF8 + 1;
		}
		else if (cbData >= sizeof(BomUTF8) && memcmp(BomUTF8, pData, sizeof(BomUTF8)) == 0)
		{
			CodePage = CP_UTF8;
		}
		else if (cbData >= 2 && pData[1])
		{
			CodePage = CP_UTF8;
		}
		else
		{
			CodePage = CP_UTF8 + 1;
		}
	}

	switch (CodePage)
	{
	case CP_UTF8:
		return XMLCreateXMLDocumentByString(UTF8ToUnicode((char*)pData, cbData), pDocument);
		break;
	case CP_UTF8 + 1:
		return XMLCreateXMLDocumentByString(CString((LPCWSTR)pData, cbData >> 1), pDocument);
		break;
	case CP_ACP:
		return XMLCreateXMLDocumentByString(CString((char*)pData, cbData), pDocument);
	default:
		return 87;
		break;
	}
}

HRESULT XMLCreateXMLDocumentByFile(LPCWSTR FilePath, rapidxml::XMLDocument* pDocument)
{
	CHFile hFile = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if (hFile.IsInvalid())
		return GetLastError();

	CHFile hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMap.IsInvalid())
		return GetLastError();

	auto pBase = (byte*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if(!pBase)
		return GetLastError();

	auto ret = XMLCreateXMLDocumentByData(pBase, GetFileSize(hFile, NULL), pDocument, -1);

	UnmapViewOfFile(pBase);

	return ret;
}






HRESULT XMLOpenMultiNotes(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath, std::vector<rapidxml::XMLNote*>& Notes)
{
	auto NextPathName = StrChr(SubPath, L'/');

	int chSubPath = NextPathName ? NextPathName - SubPath : StrLen(SubPath);

	rapidxml::xml_base<wchar_t> Temp(SubPath, chSubPath);

	if (NextPathName)
	{
		NextPathName++;

		if (chSubPath == 1 && *SubPath == L'*')
		{
			chSubPath = 0;
			SubPath = NULL;
		}

		for (auto pItem = pRootNote->first_node(SubPath, chSubPath); pItem; pItem = pItem->next_sibling(SubPath, chSubPath))
		{
			XMLOpenMultiNotes(pItem, NextPathName, Notes);
		}
	}
	else
	{

		for (auto pItem = pRootNote->first_node(SubPath, chSubPath); pItem; pItem = pItem->next_sibling(SubPath, chSubPath))
		{
			Notes.push_back(pItem);
		}
	}

	return S_OK;
}

std::vector<rapidxml::XMLNote*> XMLOpenMultiNotes(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath)
{
	std::vector<rapidxml::XMLNote*> Notes;

	Notes.reserve(100);

	XMLOpenMultiNotes(pRootNote, SubPath, Notes);

	return Notes;
}


rapidxml::xml_attribute<WCHAR>* XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, int chAttributeName)
{
	return Note->first_attribute(AttributeName, chAttributeName);

}

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, LPBSTR pAttributeValue)
{
	auto pAttribute = XMLGetNoteAttribute(Note, AttributeName, StrLen(AttributeName));

	if (pAttribute == NULL)
		return E_FAIL;

	*pAttributeValue = SysAllocStringLen(pAttribute->value(), pAttribute->value_size());

	return S_OK;
}




HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, CString& AttributeValue)
{
	auto pAttribute = XMLGetNoteAttribute(Note, AttributeName, StrLen(AttributeName));

	if (pAttribute == NULL)
		return E_FAIL;


	AttributeValue = L"";
	AttributeValue.Append(pAttribute->value(), pAttribute->value_size());

	//*pAttributeValue = SysAllocStringLen(, );

	return S_OK;
}

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* RootNote, LPCWSTR Path, LPCWSTR AttributeName, LPBSTR pAttributeValue)
{
	RootNote = XMLOpenNote(RootNote, Path);
	if (RootNote == NULL)
		return GetLastError();

	return XMLGetNoteAttribute(RootNote, AttributeName, pAttributeValue);
}


HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* RootNote, LPCWSTR Path, LPCWSTR AttributeName, CString& AttributeValue)
{
	RootNote = XMLOpenNote(RootNote, Path);
	if (RootNote == NULL)
		return GetLastError();

	return XMLGetNoteAttribute(RootNote, AttributeName, AttributeValue);
}

HRESULT XMLGetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR Path, BSTR* Value)
{
	RootNote = XMLOpenNote(RootNote, Path);
	if (RootNote == NULL)
		return GetLastError();

	*Value = SysAllocStringLen(RootNote->value(), RootNote->value_size());
	return S_OK;
}

HRESULT XMLGetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR Path, CString& Value)
{
	RootNote = XMLOpenNote(RootNote, Path);
	if (RootNote == NULL)
		return GetLastError();

	Value = L"";
	Value.Append(RootNote->value(), RootNote->value_size());
	return S_OK;
}
