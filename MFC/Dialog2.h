#pragma once
#include "afxwin.h"
#include "stdafx.h"
#include <BaseFunction.h>
#include <map>

using namespace std;

/////////////////////////////////
//       MoveFlage

#define _自己处理 -1

#define _水平操作 0xFFFF
#define _水平不变 0
#define _水平平移 1
#define _水平拉伸 2

#define _垂直操作 0xFFFF0000
#define _垂直不变 0
#define _垂直平移 1<<16
#define _垂直拉伸 2<<16



//////////////////////////////////////////////////////////
//MFC 自定义消息映射
#define BEGIN_MESSAGE_MAP2(theClass, baseClass) \
	protected: \
	PTM_WARNING_DISABLE \
	virtual const AFX_MSGMAP* theClass::GetMessageMap() const \
		{ return GetThisMessageMap(); } \
	static const AFX_MSGMAP* PASCAL theClass::GetThisMessageMap() \
	{ \
		typedef theClass ThisClass;						   \
		typedef baseClass TheBaseClass;					   \
		static const AFX_MSGMAP_ENTRY _messageEntries[] =  \
		{

#define END_MESSAGE_MAP2() \
			{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 } \
		}; \
		static const AFX_MSGMAP messageMap = \
		{ &TheBaseClass::GetThisMessageMap, &_messageEntries[0] }; \
		return &messageMap; \
	}								  \
	PTM_WARNING_RESTORE


//
//////////////////////////////////

typedef void(*MoveItemCallback)(CWnd* Item, RECT& Rect, int x, int y);


class CDialog2 :
	public CDialog
{
	//Accelerator支持
	HACCEL m_hAcce;

public:
	int MinWide, MinLen;
	int _cx, _cy;
private:
	
	std::map<CWnd*, pair<DWORD, MoveItemCallback>> AddUserFlags;

	void MoveItem(CWnd* Item, RECT& Rect, DWORD MoveFlage, int x, int y)
	{
		if (MoveFlage&&(x||y))
		{
			switch (MoveFlage&_水平操作)
			{
			case _水平平移:
				Rect.left += x;
			case _水平拉伸:
				Rect.right += x;
				break;
			}

			switch (MoveFlage&_垂直操作)
			{
			case _垂直平移:
				Rect.top += y;
			case _垂直拉伸:
				Rect.bottom += y;
				break;
			}

			Item->MoveWindow(&Rect);
		}
	}

	void _Initialize()
	{
		_cx = _cy = MinLen = MinWide = -1;
		m_hAcce = NULL;
	}
	

	//DECLARE_MESSAGE_MAP()
	
	BEGIN_MESSAGE_MAP2(CDialog2, CDialog)
		ON_WM_GETMINMAXINFO()
		ON_WM_SIZE()
	END_MESSAGE_MAP2()

public:
	CDialog2(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL)
		:CDialog(lpszTemplateName, pParentWnd)
	{
		_Initialize();
	}

	CDialog2(UINT nIDTemplate, CWnd* pParentWnd = NULL)
		:CDialog(nIDTemplate, pParentWnd)
	{
		_Initialize();
	}

	BOOL LoadAcceleratorsW(int ScrId)
	{
		m_hAcce = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(ScrId));
		return m_hAcce!=NULL;
	}

	virtual BOOL OnInitDialog()
	{
		//_cx = _cy = -1;
		CDialog::OnInitDialog();
		// TODO:  在此添加额外的初始化
		SetWindowPos(this, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		CRect Rect;
		GetWindowRect(Rect);
		MinWide = Rect.Width();
		MinLen = Rect.Height();

		return TRUE;  // return TRUE unless you set the focus to a control
		// 异常:  OCX 属性页应返回 FALSE
	}

	BOOL AddUserMoveFlage(CWnd* Item, DWORD MoveFlage, MoveItemCallback CallBack = NULL)
	{
		if (Item==NULL||MoveFlage == _自己处理&&CallBack == NULL)
		{
			return FALSE;
		}

		AddUserFlags[Item] = pair<DWORD, MoveItemCallback>(MoveFlage, CallBack);

		return TRUE;
	}

	BOOL RemoveUserMoveFlage(CWnd* Item)
	{
		auto T = AddUserFlags.find(Item);

		if (T != AddUserFlags.end())
		{
			AddUserFlags.erase(T);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI)
	{
		// TODO:  在此添加消息处理程序代码和/或调用默认值
		lpMMI->ptMinTrackSize.x = MinWide;
		lpMMI->ptMinTrackSize.y = MinLen;

		CDialog::OnGetMinMaxInfo(lpMMI);
	}

	afx_msg void OnSize(UINT nType, int cx, int cy)
	{
		CDialog::OnSize(nType, cx, cy);
		
		// TODO:  在此处添加消息处理程序代码
		if (nType != SIZE_MINIMIZED)
		{
			int x = cx - _cx;
			int y = cy - _cy;
			TCHAR TClassName[512];
			RECT Rect;

			//SetRedraw(0);
			for (CWnd* pWnd = GetWindow(GW_CHILD); pWnd; pWnd = pWnd->GetWindow(GW_HWNDNEXT))
			{
				pWnd->GetWindowRect(&Rect);
				ScreenToClient(&Rect);
				
				auto T = AddUserFlags.find(pWnd);

				if (T == AddUserFlags.end())
				{
					::GetClassName(pWnd->m_hWnd, TClassName, 512);

					//F = ((Rect.left + Rect.right)>_cx) | (((Rect.bottom + Rect.top)>_cy)<<1);

					if (StrCmp(TClassName, _T("SysListView32")) == 0 || StrCmp(TClassName, _T("SysTreeView32")) == 0 || StrCmp(TClassName, _T("SysTabControl32")) == 0)
					{
						MoveItem(pWnd, Rect, _水平拉伸 | _垂直拉伸, x, y);
					}
					else if (StrCmp(TClassName, _T("msctls_progress32")) == 0 || StrCmp(TClassName, _T("msctls_progress32")) == 0)
					{
						MoveItem(pWnd, Rect, _水平拉伸 | _垂直平移, x, y);
					}
					else if (StrCmp(TClassName, _T("Edit")) == 0)
					{
						MoveItem(pWnd, Rect, _水平拉伸 | (((Rect.bottom + Rect.top) > _cy) ? _垂直平移 : 0), x, y);
					}
					else
					{
						MoveItem(pWnd, Rect, (((Rect.left + Rect.right) > _cx) ? _水平平移 : 0) | (((Rect.bottom + Rect.top) > _cy) ? _垂直平移 : 0), x, y);
					}
				}
				else if (T->second.first != _自己处理)
				{
					MoveItem(pWnd, Rect, T->second.first, x, y);
				}
				else
				{
					T->second.second(pWnd, Rect, x, y);
				}
			}

			_cx = cx;
			_cy = cy;
			//SetRedraw(1);
			InvalidateRect(NULL, TRUE);

		}

	}
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		// TODO:  在此添加专用代码和/或调用基类
		if (m_hAcce)
			::TranslateAccelerator(m_hWnd, m_hAcce, pMsg);
		//{
		//	return TRUE;
		//}
		//else
		//{
		return CDialog::PreTranslateMessage(pMsg);
		//}
	}
};

#ifdef _MessageMap

/////////////////////////////将以下代码添加到
BEGIN_MESSAGE_MAP(CDialog2, CDialog)
ON_WM_GETMINMAXINFO()
ON_WM_SIZE()
END_MESSAGE_MAP()

#endif

