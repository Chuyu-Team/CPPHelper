#pragma once


//#include <atlbase.h>
//#include <atlwin.h>
//#include <atlapp.h>
//#include <atldlgs.h>
//#include <atltypes.h>
//#include <atlctrls.h>
//
//#define NM_GetTipToolInfo NM_FIRST-50
//using namespace WTL;
//
//struct TipTooInfo
//{
//	NMHDR hr;
//	HTREEITEM Item;
//	LPCWSTR String;
//};

//
//class CTreeViewCtrl2 :public  CWindowImpl<CTreeViewCtrl2, CTreeViewCtrl>//, public CDoubleBufferImpl<CTreeViewCtrl2>
//{
//	CToolTipCtrl m_pToolTip;
//
//	
//	TipTooInfo TipInfo;
//	//HTREEITEM LastItem;
//	//模式0 
//	void TravelParent0(HTREEITEM hItem, int nState)
//	{
//		HTREEITEM Parent = GetParentItem(hItem), Child;
//		if (Parent)
//		{
//			int Count[5] = { 0 };
//
//			for (Child = GetChildItem(Parent); Child; Child = GetNextSiblingItem(Child))
//			{
//				Count[GetItemState(Child, TVIS_STATEIMAGEMASK) >> 12]++;
//			}
//
//			if (Count[3] || Count[2])
//			{
//				//while (Parent)
//				do
//				{
//					SetItemState(Parent, 0x3000, TVIS_STATEIMAGEMASK);
//				} while (Parent = GetParentItem(Parent));
//			}
//			else
//			{
//				SetItemState(Parent, 0x1000, TVIS_STATEIMAGEMASK);
//				TravelParent0(Parent, 0);
//			}
//		}
//	}
//	void TravelChild0(HTREEITEM hItem, int nState)
//	{
//		hItem = GetChildItem(hItem);
//
//		while (hItem)
//		{
//			SetItemState(hItem, nState, TVIS_STATEIMAGEMASK);
//			TravelChild0(hItem, nState);
//			hItem = GetNextSiblingItem(hItem);
//		}
//	}
//
//
//	//模式1  自然模式
//	void TravelParent1(HTREEITEM hItem, int nState)
//	{
//		HTREEITEM Parent = GetParentItem(hItem), Child;
//		int TState;
//
//
//		if (Parent&&GetItemState(Parent, TVIS_STATEIMAGEMASK) > 0xFFF)
//		{
//			Child = GetChildItem(Parent);
//
//			while (Child)
//			{
//				if (Child != hItem)
//				{
//					TState = GetItemState(Child, TVIS_STATEIMAGEMASK)&TVIS_STATEIMAGEMASK;
//					if (TState > 0xFFF && TState != nState)
//					{
//						//nState = INDEXTOSTATEIMAGEMASK(2);
//
//						do
//						{
//							SetItemState(Parent, 0x3000, TVIS_STATEIMAGEMASK);
//							Parent = GetParentItem(Parent);
//
//						} while (Parent&&GetItemState(Parent, TVIS_STATEIMAGEMASK) > 0xFFF);
//
//						return;
//					}
//				}
//				Child = GetNextSiblingItem(Child);
//			}
//
//			SetItemState(Parent, nState, TVIS_STATEIMAGEMASK);
//			TravelParent1(Parent, nState);
//		}
//	}
//#define TravelChild1 TravelChild0
//	//void TravelChild(HTREEITEM hItem, int nState);
//
//	//模式2
//	void TravelParent2(HTREEITEM hItem, int nState)
//	{
//		HTREEITEM Parent = GetParentItem(hItem), Child;
//		if (Parent)
//		{
//			int Count[5] = { 0 };
//
//			for (Child = GetChildItem(Parent); Child; Child = GetNextSiblingItem(Child))
//			{
//				Count[GetItemState(Child, TVIS_STATEIMAGEMASK) >> 12]++;
//			}
//
//			if (Count[1] == 0 && Count[4] == 0 && Count[3] == 0)
//			{
//				SetItemState(Parent, 2 << 12, TVIS_STATEIMAGEMASK);
//			}
//			else
//			{
//				if ((GetItemState(Parent, TVIS_STATEIMAGEMASK) >> 12) == 2 || Count[2] || Count[3])
//				{
//					SetItemState(Parent, 3 << 12, TVIS_STATEIMAGEMASK);
//				}
//				else if ((GetItemState(Parent, TVIS_STATEIMAGEMASK) >> 12) != 3)
//				{
//					SetItemState(Parent, 1 << 12, TVIS_STATEIMAGEMASK);
//				}
//			}
//
//			TravelParent2(Parent, nState);
//		}
//	}
//	void TravelChild2(HTREEITEM hItem, int nState)
//	{
//		switch (nState >> 12)
//		{
//		case 4:
//		case 2:
//		case 1:
//			TravelChild0(hItem, nState); break;
//		case 3:
//		{
//			if (GetChildItem(hItem))
//				TravelChild0(hItem, 1 << 12);
//			else
//			{
//				SetItemState(hItem, (3 % Flage + 1) << 12, TVIS_STATEIMAGEMASK);
//			}
//			break;
//		}
//		default:
//			break;
//		}
//	}
//
//	typedef void (CTreeViewCtrl2::*Travel)(HTREEITEM, int);
//
//	Travel NowTravelParent, NowTravelChild;
//
//public:
//	int Flage;
//
//
//	void SubclassWindow(HWND hWnd, int s = 2)
//	{
//		//LastItem = 0;
//		if (hWnd)
//		{
//			CWindowImpl<CTreeViewCtrl2, CTreeViewCtrl>::SubclassWindow(hWnd);
//			//this->SetBkColor(GetSysColor(COLOR_WINDOWFRAME));
//
//			static CImageList m_imgList;
//			if (m_imgList.m_hImageList == NULL)
//			{
//				m_imgList.Create(14, 14, ILC_COLOR, 4, 4);
//				m_imgList.SetBkColor(RGB(255, 255, 255));
//				CBitmap Temp;
//				Temp.LoadBitmapW(IDB_BITMAP_NotChecked);
//				m_imgList.Add(Temp, RGB(0, 0, 0));
//				m_imgList.Add(Temp, RGB(0, 0, 0));
//				Temp.DeleteObject();
//
//				Temp.LoadBitmapW(IDB_BITMAP_Checked);
//				m_imgList.Add(Temp, RGB(0, 0, 0));
//				Temp.DeleteObject();
//
//				Temp.LoadBitmapW(IDB_BITMAP_PartChecked);
//				m_imgList.Add(Temp, RGB(0, 0, 0));
//				Temp.DeleteObject();
//
//				Temp.LoadBitmapW(IDB_BITMAP_Delete);
//				m_imgList.Add(Temp, RGB(0, 0, 0));
//				Temp.DeleteObject();
//			}
//
//			SetImageList(m_imgList, TVSIL_STATE);
//			TipInfo.hr.code = NM_GetTipToolInfo;
//			TipInfo.hr.hwndFrom = m_hWnd;
//			TipInfo.hr.idFrom = CWindow(m_hWnd).GetDlgCtrlID();
//
//			TipInfo.Item = NULL;
//			TipInfo.String = NULL;
//
//			if (s == 1)
//			{
//				NowTravelParent = &CTreeViewCtrl2::TravelParent1;
//				NowTravelChild = &CTreeViewCtrl2::TravelChild1;
//				Flage = 2;
//			}
//			else if (s == 0)
//			{
//				NowTravelParent = &CTreeViewCtrl2::TravelParent0;
//				NowTravelChild = &CTreeViewCtrl2::TravelChild0;
//				Flage = 2;
//			}
//			else if (s == 2)
//			{
//				NowTravelParent = &CTreeViewCtrl2::TravelParent2;
//				NowTravelChild = &CTreeViewCtrl2::TravelChild2;
//				Flage = 4;
//			}
//
//			m_pToolTip.Create(m_hWnd, 0, NULL, TTS_ALWAYSTIP);
//
//			m_pToolTip.AddTool(m_hWnd, LPSTR_TEXTCALLBACK);
//			this->SetToolTips(m_pToolTip);
//
//			m_pToolTip.Activate(TRUE);
//			
//			
//
//			m_pToolTip.SetDelayTime(TTDT_AUTOPOP, 10000);
//			m_pToolTip.SetMaxTipWidth(300);
//
//			//m_pToolTip.SetTipBkColor(GetSysColor(COLOR_WINDOW));
//			//m_pToolTip.SetTipTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
//
//			//CDCHandle cd(m_pToolTip.GetDC());
//			//cd.SetBkColor(GetSysColor(COLOR_WINDOW));
//			//cd.SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
//
//		}
//	}
//
//	BEGIN_MSG_MAP_EX(CTreeViewCtrl2)
//		MSG_WM_LBUTTONDOWN(OnLButtonDown)
//		//MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
//		MSG_WM_RBUTTONDOWN(OnRButtonDown)
//		MSG_WM_MOUSEMOVE(OnMouseMove)
//		//CHAIN_MSG_MAP(CTreeViewCtrlWithWindow)
//		DEFAULT_REFLECTION_HANDLER()
//	END_MSG_MAP()
//
//	//BOOL PreTranslateMessage(MSG* pMsg)
//	//{
//	//	m_pToolTip.Activate(TRUE);
//	//	m_pToolTip.RelayEvent(pMsg);
//
//	//	return FALSE;
//	//}
//
//	void OnMouseMove(UINT nFlags, CPoint point)
//	{
//		SetMsgHandled(FALSE);
//
//		UINT temp;
//		HTREEITEM hItem = HitTest(point, &temp);
//		if (temp&TVHT_ONITEM)
//		{
//			if (TipInfo.Item != hItem)
//			{
//				TipInfo.Item = hItem;
//				//m_pToolTip.SetWindowTextW(L"哈哈");
//				//m_pToolTip.Activate(1);
//				//
//				//m_pToolTip.ShowWindow(1);
//				TipInfo.String = NULL;
//
//
//				::SendMessage(GetParent(), WM_NOTIFY, 0, (LPARAM)&TipInfo);
//
//				if (TipInfo.String)
//				{
//					
//					//m_pToolTip.SetToolInfo()
//					m_pToolTip.Activate(1);
//					
//					m_pToolTip.UpdateTipText(TipInfo.String, m_hWnd);
//					
//					//m_pToolTip.ShowWindow(1);
//					
//				}
//				else
//				{
//					//m_pToolTip.ShowWindow(0);
//					//m_pToolTip.Pop();
//					m_pToolTip.Activate(0);
//				}
//			}
//			
//			return;
//			
//		}
//		else
//		{
//			TipInfo.Item = NULL;
//		}
//		m_pToolTip.Activate(0);
//		//m_pToolTip.Pop();
//		//m_pToolTip.ShowWindow(0);
//		//m_pToolTip.Activate(0);
//
//		//SetMsgHandled(FALSE);
//	}
//	
//	void OnLButtonDown(UINT nFlags, CPoint point)
//	{
//		//this->P
//		//m_pToolTip.RelayEvent()
//		UINT temp;
//		HTREEITEM hItem = HitTest(point, &temp);
//
//		if (temp&TVHT_ONITEMSTATEICON)
//		{
//
//			temp = ((GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12) % Flage + 1) << 12;
//			SetItemState(hItem, temp, TVIS_STATEIMAGEMASK);
//
//			//////////////////////////////
//			//将状态向下延伸
//			//TravelChild(hItem, temp);
//			(this->*NowTravelChild)(hItem, temp);
//			//*(CTreeCtrlEx::NowTravelChild)(hItem, temp);
//
//			//在向上反馈状态
//			(this->*NowTravelParent)(hItem, temp);
//
//		}
//
//		SetMsgHandled(FALSE);
//	}
//
//	//LRESULT OnRButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
//	void OnRButtonDown(UINT nFlags, CPoint point)
//	{
//		SetMsgHandled(FALSE);
//		//bHandled=FALSE;
//		UINT temp;
//		HTREEITEM hItem = HitTest(point, &temp);
//
//		if (temp&TVHT_ONITEM)
//		{
//			auto T = GetNextSelectedItem();
//
//			if (T)
//			{
//				SetItemState(T, 0, TVIS_SELECTED);
//			}
//
//			SetItemState(hItem, TVIS_SELECTED, TVIS_SELECTED);
//		}
//		//SendMessage(WM_LBUTTONDOWN, wParam, lParam);
//
//		//return 0;
//	}
//
//
//	void SetCheck(HTREEITEM hItem)
//	{
//		SetItemState(hItem, 2 << 12, TVIS_STATEIMAGEMASK);
//		(this->*NowTravelChild)(hItem, 2 << 12);
//		//*(CTreeCtrlEx::NowTravelChild)(hItem, temp);
//
//		//在向上反馈状态
//		(this->*NowTravelParent)(hItem, 2 << 12);
//	}
//};