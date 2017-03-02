#pragma once
#include <Windows.h>
#include <atlstr.h>


class CFileFindHelperIterator
{
public:
	WIN32_FIND_DATA FindData;

	HANDLE hFileFind;

	CFileFindHelperIterator()
		:hFileFind(INVALID_HANDLE_VALUE)
	{
	}

	CFileFindHelperIterator(CFileFindHelperIterator& Item)
	{
		hFileFind = Item.hFileFind;
		Item.hFileFind = INVALID_HANDLE_VALUE;


		FindData = Item.FindData;
	}

	~CFileFindHelperIterator()
	{
		if(hFileFind != INVALID_HANDLE_VALUE)
			FindClose(hFileFind);
	}

	CFileFindHelperIterator& operator++()
	{
		if (!FindNextFile(hFileFind, &FindData))
		{
			FindClose(hFileFind);
			hFileFind = INVALID_HANDLE_VALUE;
		}

		return *this;
	}

	WIN32_FIND_DATA& operator*()
	{	// return designated object
		return FindData;
	}


	bool operator!=(const CFileFindHelperIterator& Item)
	{
		return hFileFind != Item.hFileFind;
	}
};

class CFileFindHelper
{
	CString FindPath;
public:

	CFileFindHelper(CString _FindPath)
		:FindPath(_FindPath)
	{

	}

	CFileFindHelperIterator begin() const
	{
		CFileFindHelperIterator Temp;

		Temp.hFileFind = FindFirstFile(FindPath, &Temp.FindData);
		return Temp;
	}

	CFileFindHelperIterator end() const
	{
		return CFileFindHelperIterator();
	}
};