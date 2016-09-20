#pragma once
//#include <msxml.h>
#include <Windows.h>

//#include <rapidxml/rapidxml.hpp>
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
//#include <comdef.h>
//#include <atlcomcli.h>
//#include <atlstr.h>
//#include <BaseFunction.h>
//#include <StringHelper.h>


namespace rapidxml
{
	typedef rapidxml::xml_document<wchar_t> XMLDocument;
	typedef rapidxml::xml_node<wchar_t> XMLNote;

	inline bool operator<(const xml_base<char>& Item1, const xml_base<char>& Item2)
	{
		/*if (Item1.m_name_size == Item2.m_name_size)
		return StrCmpN(Item1.m_name, Item2.m_name, Item1.m_name_size) < 0;
		else */
		if (Item1.m_name_size < Item2.m_name_size)
			return StrCmpNA(Item1.m_name, Item2.m_name, Item1.m_name_size) <= 0;
		else
			return StrCmpNA(Item1.m_name, Item2.m_name, Item2.m_name_size) < 0;
	}

	inline bool operator<(const xml_base<wchar_t>& Item1, const xml_base<wchar_t>& Item2)
	{
		/*if (Item1.m_name_size == Item2.m_name_size)
		return StrCmpN(Item1.m_name, Item2.m_name, Item1.m_name_size) < 0;
		else */
		if (Item1.m_name_size < Item2.m_name_size)
			return StrCmpNW(Item1.m_name, Item2.m_name, Item1.m_name_size) <= 0;
		else
			return StrCmpNW(Item1.m_name, Item2.m_name, Item2.m_name_size) < 0;
	}

	inline bool operator==(const xml_base<char>& Item1, const xml_base<char>& Item2)
	{
		return Item1.m_name_size == Item2.m_name_size&&StrCmpNA(Item1.m_name, Item2.m_name, Item1.m_name_size) == 0;
	}

	inline bool operator==(const xml_base<wchar_t>& Item1, const xml_base<wchar_t>& Item2)
	{
		return Item1.m_name_size == Item2.m_name_size&&StrCmpNW(Item1.m_name, Item2.m_name, Item1.m_name_size) == 0;
	}
};


rapidxml::XMLNote* XMLOpenNote(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath);

rapidxml::xml_node<char>* XMLOpenNote(rapidxml::xml_node<char>* pRootNote, const char* SubPath);

rapidxml::XMLNote* XMLCreateNote(rapidxml::XMLNote* pRootNote, LPCWSTR NoteName, DWORD CreateType);

rapidxml::XMLNote* XMLSetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR NoteName, LPCWSTR NoteValue);

rapidxml::XMLNote* XMLSetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR NoteName, FILETIME NoteValue);

HRESULT XMLCreateXMLDocumentByString(CString Str, rapidxml::XMLDocument* pDocument);

HRESULT XMLCreateXMLDocumentByFile(LPCWSTR FilePath, rapidxml::XMLDocument* pDocument);

HRESULT XMLCreateXMLDocumentByData(const byte* pData, DWORD cbData, rapidxml::XMLDocument* pDocument, int CodePage=-1);

HRESULT XMLOpenMultiNotes(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath, std::vector<rapidxml::XMLNote*>& Notes);

std::vector<rapidxml::XMLNote*> XMLOpenMultiNotes(rapidxml::XMLNote* pRootNote, LPCWSTR SubPath);

rapidxml::xml_attribute<WCHAR>* XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, int chAttributeName);

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, LPBSTR pAttributeValue);

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* Note, LPCWSTR AttributeName, CString& AttributeValue);

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* RootNote, LPCWSTR Path, LPCWSTR AttributeName, LPBSTR pAttributeValue);

HRESULT XMLGetNoteAttribute(rapidxml::XMLNote* RootNote, LPCWSTR Path, LPCWSTR AttributeName, CString& AttributeValue);

HRESULT XMLGetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR Path, BSTR* Value);

HRESULT XMLGetNoteValue(rapidxml::XMLNote* RootNote, LPCWSTR Path, CString& Value);