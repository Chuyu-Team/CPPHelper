#pragma once
#include "stdafx.h"


#define MSG_WM_ERASEBKGNDWithHWND(func) \
if (uMsg == WM_ERASEBKGND) \
{ \
SetMsgHandled(TRUE); \
lResult = (LRESULT)func(hWnd,(HDC)wParam); \
if (IsMsgHandled()) \
return TRUE; \
}

class CDialogCTL
{
public:

	BEGIN_MSG_MAP_EX(CDialogCTL)

		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)

		MSG_WM_ERASEBKGNDWithHWND(OnEraseBkgnd)
		MESSAGE_RANGE_HANDLER(WM_CTLCOLORMSGBOX, WM_CTLCOLORSTATIC, OnCtlColor)
		//MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		//MSG_WM_CTLCOLORDLG(OnCtlColorStatic)
		//MSG_WM_CTLCOLORBTN(OnCtlColorStatic)
		END_MSG_MAP()

	HRESULT OnCtlColor(int /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CDCHandle dc((HDC)wParam);
		dc.SetBkMode(TRANSPARENT);
		//dc.SetBkColor(REG());

		//dc.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));

		return  (HRESULT)GetSysColorBrush(/*COLOR_WINDOWFRAME*/COLOR_WINDOW);
	}

	BOOL OnEraseBkgnd(CWindow hWnd, CDCHandle dc)
	{
		RECT Rect;
		hWnd.GetClientRect(&Rect);


		dc.FillSolidRect(&Rect, RGB(255, 255, 255));


		return TRUE;
	}
};