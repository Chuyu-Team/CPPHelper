#pragma once
//#include <atltypes.h>
//#include <atlcrack.h>


#define MSG_WM_PAINT_Window(func) \
	if (uMsg == WM_PAINT) \
	{ \
		SetMsgHandled(TRUE); \
		func(hWnd,(HDC)wParam); \
		lResult = 0; \
		if(IsMsgHandled()) \
			return TRUE; \
	}

class CDoubleBuffer
{
public:

	BEGIN_MSG_MAP_EX(CDoubleBuffer)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT_Window(OnPaint)
	END_MSG_MAP()

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		return TRUE;
	}

	void OnPaint(CWindow hWnd, HDC dc)
	{
		if (dc)
		{
			RECT rect;
			hWnd.GetClientRect(&rect);

			CMemoryDC dcMem(dc, rect);

			dcMem.FillSolidRect(&rect, GetSysColor(COLOR_WINDOW));

			DefWindowProc(hWnd, WM_PAINT, (WPARAM)dcMem.m_hDC, 0);
		}
		else
		{
			CPaintDC dc(hWnd);
			CMemoryDC dcMem(dc.m_hDC, dc.m_ps.rcPaint);

			dcMem.FillSolidRect(&dc.m_ps.rcPaint, GetSysColor(COLOR_WINDOW));

			DefWindowProc(hWnd, WM_PAINT, (WPARAM)dcMem.m_hDC, 0);
		}
	}

};