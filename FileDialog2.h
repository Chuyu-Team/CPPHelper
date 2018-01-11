#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atldlgs.h>
#include <atlstr.h>
#include <vector>
using namespace std;

#include <atlcrack.h>
#include "BaseFunction.h"

#define ShellFileOperator (FOS_OVERWRITEPROMPT|FOS_NOCHANGEDIR|FOS_NOVALIDATE|FOS_ALLOWMULTISELECT|\
	FOS_PATHMUSTEXIST|FOS_FILEMUSTEXIST|FOS_CREATEPROMPT|FOS_SHAREAWARE|FOS_NOREADONLYRETURN|\
	FOS_NOTESTFILECREATE|FOS_NODEREFERENCELINKS|FOS_DONTADDTORECENT|FOS_FORCESHOWHIDDEN)

class CFileDialog2 : public CFileDialog
{
private:

	CString lpstrFilter;
	UINT FilterSpecCount;
	const COMDLG_FILTERSPEC* ArrFilterSpec;


	CString GetFilte(const COMDLG_FILTERSPEC* arrFilterSpec, UINT uFilterSpecCount)
	{

		CString lpstrFilter;

		if (uFilterSpecCount)
		{
			for (int i = 0; i != uFilterSpecCount; i++)
			{
				lpstrFilter += arrFilterSpec[i].pszName;


				lpstrFilter += L"(";
				lpstrFilter += arrFilterSpec[i].pszSpec;
				lpstrFilter += L")|";

				lpstrFilter += arrFilterSpec[i].pszSpec;
				lpstrFilter += L"|";
			}

			lpstrFilter.Replace(L'|', L'\0');

			//return lpstrFilter;
		}
		/*else
		{
		return L"";
		}*/

		return lpstrFilter;
	}




public:
	CFileDialog2(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = L"",
		LPCTSTR lpszFileName = L"",
		DWORD dwFlags = 0/*OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/,
		const COMDLG_FILTERSPEC* arrFilterSpec = NULL,
		UINT uFilterSpecCount = 0U)
		:ArrFilterSpec(arrFilterSpec)
		, FilterSpecCount(uFilterSpecCount)
		, CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags)
	{


	}


	CString FilePath;


	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		ComInitializeWrapper ComInitialize;

		FilePath.ReleaseBufferSetLength(0);

		//DWORD dwOptions = m_ofn.Flags&(OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_CREATEPROMPT | OFN_SHAREAWARE);
		INT_PTR ret = -1;

		if (m_bOpenFileDialog)
		{
			//OpenFile


			CShellFileOpenDialog OpenFile(m_szFileName, m_ofn.Flags&ShellFileOperator, m_ofn.lpstrDefExt, ArrFilterSpec, FilterSpecCount);

			if (OpenFile.m_spFileDlg)
			{
				OpenFile.m_spFileDlg->SetFileTypeIndex(m_ofn.nFilterIndex);

				ret = OpenFile.DoModal(hWndParent);
				if (ret != IDOK)
					return ret;


				OpenFile.m_spFileDlg->GetFileTypeIndex((UINT*)&m_ofn.nFilterIndex);

				CComPtr<IShellItemArray> pIShellItemArray;

				OpenFile.m_spFileDlg->GetResults(&pIShellItemArray);

				DWORD Count;
				pIShellItemArray->GetCount(&Count);
				IShellItem* pIShellItem;

				for (int i = 0; i != Count; i++)
				{
					pIShellItemArray->GetItemAt(i, &pIShellItem);

					CString Temp;
					OpenFile.GetFileNameFromShellItem(pIShellItem, SIGDN_FILESYSPATH, Temp);

					pIShellItem->Release();

					FilePath += Temp;

					FilePath.AppendChar(NULL);
				}

				return IDOK;
			}

		}
		else
		{
			//SaveFile

			CShellFileSaveDialog SaveFile(m_szFileName, m_ofn.Flags&ShellFileOperator, m_ofn.lpstrDefExt, ArrFilterSpec, FilterSpecCount);

			if (SaveFile.m_spFileDlg)
			{
				SaveFile.m_spFileDlg->SetFileTypeIndex(m_ofn.nFilterIndex);

				ret = SaveFile.DoModal(hWndParent);

				if (ret != IDOK)
					return ret;

				SaveFile.m_spFileDlg->GetFileTypeIndex((UINT*)&m_ofn.nFilterIndex);

				CString Temp;
				SaveFile.GetFilePath(Temp);

				FilePath += Temp;

				FilePath.AppendChar(NULL);

				return IDOK;
			}
		}

		auto rFilter = GetFilte(ArrFilterSpec, FilterSpecCount);
		m_ofn.lpstrFilter = rFilter;

		m_ofn.lpstrFile = FilePath.GetBuffer((MAX_PATH + 1) * 1000);
		m_ofn.nMaxFile = (MAX_PATH + 1) * 1000;

		return CFileDialog::DoModal(hWndParent);
	}
};


static void* Ptr;



class CFolderDialogEx : public CFileDialogImpl <CFolderDialogEx>
{



	LONG_PTR pWindProcOld;

	static LRESULT CALLBACK WindProcNew(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CFolderDialogEx* pThis = (CFolderDialogEx*)Ptr;

		LRESULT lResult = 1;

		pThis->ProcessWindowMessage(hWnd, uMsg, wParam, lParam, lResult, 1);

		if (lResult)
		{
			return CallWindowProcW((WNDPROC)pThis->pWindProcOld, hWnd, uMsg, wParam, lParam);
		}

		return 0;
	}

public:
	CString FolderPath;
	CFolderDialogEx(LPCWSTR lpstrTitle = NULL, UINT uFlags = OFN_HIDEREADONLY | OFN_FORCESHOWHIDDEN)
		: CFileDialogImpl<CFolderDialogEx>(TRUE, L"", L"", uFlags, L"1\0..")
	{
		m_ofn.lpstrTitle = lpstrTitle;
	}

	BEGIN_MSG_MAP_EX(CFolderDialogEx)
		//NOTIFY_CODE_HANDLER(CDN_SELCHANGE, _OnFolderChange)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CFileDialogImpl<CFolderDialogEx>)

		ALT_MSG_MAP(1)
		COMMAND_HANDLER(IDOK, BN_CLICKED, OnOk)
		break;
		END_MSG_MAP()

		void OnSelChange(LPOFNOTIFY lpon)
		{
			auto cch = GetFilePath(FolderPath.GetBuffer(1024), 1025) - 1;

			if (cch > 0)
			{
				FolderPath.ReleaseBufferSetLength(cch);

				GetParent().GetDlgItem(cmb13).SetWindowTextW(FolderPath);
			}
			return;
		}


		//LRESULT _OnFolderChange(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
		//{
		//	OnFolderChange((LPOFNOTIFY)pnmh);
		//	return 0;
		//}


		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
		{
			SetMsgHandled(0);


			CString FileTitle;

			if (m_ofn.lpstrFileTitle == NULL || (*m_ofn.lpstrFileTitle) == NULL)
			{
				LoadString_s(L"Comdlg32.dll", 439, FileTitle);

				/*if (FileTitle.GetLength() == 0)
				{
				FileTitle = L"选择文件夹";
				}*/
				GetParent().SetWindowTextW(FileTitle);

				GetParent().GetDlgItem(IDOK).SetWindowTextW(FileTitle);
			}

			LoadString_s(L"Comdlg32.dll", 438, FileTitle);

			/*if (FileTitle.GetLength() == 0)
			FileTitle = L"文件夹";*/

			GetParent().GetDlgItem(stc3).SetWindowTextW(FileTitle);

			//GetParent().GetDlgItem(ctl1).ShowWindow(0);
			GetParent().GetDlgItem(cmb1).ShowWindow(0);
			//GetParent().GetDlgItem(cmb13).ShowWindow(0);
			GetParent().GetDlgItem(stc2).ShowWindow(0);
			//GetParent().GetDlgItem(stc3).ShowWindow(0);

			Ptr = this;
			pWindProcOld = GetParent().SetWindowLongPtr(GWLP_WNDPROC, (LONG_PTR)(&WindProcNew));

			return TRUE;
		}


		LRESULT OnOk(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{

			//GetDlgItem(edt1).EnableWindow(0);

			GetParent().GetDlgItem(cmb13).GetWindowTextW(FolderPath);

			/*CString FilePath;

			GetParent().GetDlgItem(cmb13).GetWindowTextW(FilePath);
			if (FilePath.GetLength())
			{
			if (FolderPath[FolderPath.GetLength() - 1] != L'\\')
			FolderPath += L'\\';
			FolderPath += FilePath;
			}*/

			if (GetFileType(FolderPath)==PathIsDir)
			{

				::EndDialog(GetParent(), IDOK);

				return 0;
			}
			else
			{
				return 0;
			}
		}
};



class CFolderDialog2 :public CFolderDialogEx
{
public:
	CFolderDialog2(LPCWSTR lpstrTitle = NULL, UINT uFlags = OFN_HIDEREADONLY | OFN_FORCESHOWHIDDEN)
		:CFolderDialogEx(lpstrTitle, uFlags)
	{
	}


	INT_PTR DoModal(HWND hWndParent = ::GetActiveWindow())
	{
		ComInitializeWrapper ComInitialize;

		CShellFileOpenDialog OpenFile(L"", FOS_PICKFOLDERS | (m_ofn.Flags&ShellFileOperator), L"");

		if (OpenFile.m_spFileDlg == NULL)
		{
			return CFolderDialogEx::DoModal(hWndParent);
		}
		else
		{
			OpenFile.m_spFileDlg->SetTitle(m_ofn.lpstrTitle);

			if (OpenFile.DoModal(hWndParent) == IDOK)
			{
				OpenFile.GetFilePath(FolderPath);

				if(FolderPath.IsEmpty())
					return IDCANCEL;

				return IDOK;
			}
			else
			{
				return IDCANCEL;
			}
		}
	}

};