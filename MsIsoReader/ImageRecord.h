#pragma once
#include <vector>
#include "Udf\UdfTime.h"
class ImageRecord
{
public:
	ImageRecord* pParent;

	std::vector<ImageRecord*> _SubItems;
	ImageRecord()
		:pParent(NULL)
	{

	}
	virtual ~ImageRecord()
	{
		Clear();
	}

	UdfTime _dateTime;
	CString _name;
	CString _path;
	UINT64 _size;
	long _location;
	bool _isDirectory;
	bool _isSystem;


	virtual UINT64 get_Size()
	{
		if (IsDirectory())
		{
			if (_size == 0)
				_size = GetSize(this);
			return _size;
		}
		else
			return _size;
	
	}

	virtual long get_Location()
	{
		return _location;
	}

	virtual bool IsDirectory()
	{
		return _isDirectory;
	}

	virtual bool IsSystemItem()
	{
		return _isSystem;
	}

	virtual bool IsUdf()
	{
		return false;
	}

	virtual CString get_Name()
	{
		return _name;
	}

	virtual CString get_Path()
	{
		if (_path.IsEmpty())
			_path = GetPath();
		return _path;
	}
	

	virtual void Clear()
	{
		for (auto Item : _SubItems)
			delete Item;

		_SubItems.clear();
		*(UINT64*)&_dateTime = 0;
		pParent = NULL;
		_name.Empty();
		_path.Empty();
		_location = 0;
		_size = 0;
		_isDirectory = false;
	}

	static UINT64 GetSize(ImageRecord* record)
	{
		UINT64 Size = 0;
		for (auto& Item : record->_SubItems)
		{
			Size += Item->get_Size();
		}
		return Size;
	}

	static CString GetPath(ImageRecord* pItem)
	{
		CString Temp;

		if (pItem)
		{
			Temp += GetPath(pItem->pParent);

			Temp += pItem->get_Name();
			Temp += L'\\';
		}
		return Temp;
	}

	CString GetPath()
	{
		return GetPath(this);
	}
};