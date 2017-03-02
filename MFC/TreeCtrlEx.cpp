// TreeCtrlEx.cpp : 实现文件
//

#include "stdafx.h"
#include "DismMgr.h"
#include "TreeCtrlEx.h"


// CTreeCtrlEx
Travel NowTravelParent, NowTravelChild;
IMPLEMENT_DYNAMIC(CTreeCtrlEx, CTreeCtrl)

CTreeCtrlEx::CTreeCtrlEx()

{
	
}

CTreeCtrlEx::~CTreeCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTreeCtrlEx, CTreeCtrl)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CTreeCtrlEx 消息处理程序
//nState: 0->无选择钮 1->没有选择 2->部分选择 3->全部选择 
//BOOL CTreeCtrlEx::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask)
//{
//
//}
void CTreeCtrlEx::SetCheck(HTREEITEM hItem)
{
	SetItemState(hItem, 2<<12, TVIS_STATEIMAGEMASK);
	(this->*NowTravelChild)(hItem, 2 << 12);
	//*(CTreeCtrlEx::NowTravelChild)(hItem, temp);

	//在向上反馈状态
	(this->*NowTravelParent)(hItem, 2 << 12);
}
void CTreeCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	//this->OnCreate
	UINT temp;
	HTREEITEM hItem = HitTest(point,&temp);

	if (temp&TVHT_ONITEMSTATEICON)
	{
		
		temp = ((GetItemState(hItem, TVIS_STATEIMAGEMASK)>>12)%Flage+1)<<12;
		SetItemState(hItem, temp, TVIS_STATEIMAGEMASK);

		//////////////////////////////
		//将状态向下延伸
		//TravelChild(hItem, temp);
		(this->*NowTravelChild)(hItem,temp);
		//*(CTreeCtrlEx::NowTravelChild)(hItem, temp);
		
		//在向上反馈状态
		(this->*NowTravelParent)(hItem, temp);
		
	}
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CTreeCtrlEx::OnInitDialog(int s)
{
	//Flage = s;
	m_imgList.Create(14, 14, ILC_COLOR, 4, 4);
	m_imgList.SetBkColor(RGB(255, 255, 255));
	CBitmap Temp;
	Temp.LoadBitmapW(IDB_BITMAP_NotChecked);
	m_imgList.Add(&Temp, RGB(0, 0, 0));
	m_imgList.Add(&Temp, RGB(0, 0, 0));
	Temp.DeleteObject();
	

	

	Temp.LoadBitmapW(IDB_BITMAP_Checked);
	m_imgList.Add(&Temp, RGB(0, 0, 0));
	Temp.DeleteObject();
	
	Temp.LoadBitmapW(IDB_BITMAP_PartChecked);
	m_imgList.Add(&Temp, RGB(0, 0, 0));
	Temp.DeleteObject();

	Temp.LoadBitmapW(IDB_BITMAP_Delete);
	m_imgList.Add(&Temp, RGB(0, 0, 0));
	Temp.DeleteObject();

	SetImageList(&m_imgList, TVSIL_STATE);

	if (s == 1)
	{
		NowTravelParent = &CTreeCtrlEx::TravelParent1;
		NowTravelChild = &CTreeCtrlEx::TravelChild1;
		Flage = 2;
	}
	else if (s == 0)
	{
		NowTravelParent = &CTreeCtrlEx::TravelParent0;
		NowTravelChild = &CTreeCtrlEx::TravelChild0;
		Flage = 2;
	}
	else if (s == 2)
	{
		NowTravelParent = &CTreeCtrlEx::TravelParent2;
		NowTravelChild = &CTreeCtrlEx::TravelChild2;
		Flage = 4;
	}

	return 1;
}
HTREEITEM CTreeCtrlEx::InsertItem(_In_z_ LPCTSTR lpszItem,  _In_ HTREEITEM hParent,int Check)
{
	auto hItem = CTreeCtrl::InsertItem(lpszItem, hParent);

	SetItemState(hItem, INDEXTOSTATEIMAGEMASK(Check), TVIS_STATEIMAGEMASK);

	return hItem;
}

void CTreeCtrlEx::TravelChild0(HTREEITEM hItem, int nState)
{
	hItem = GetChildItem(hItem);

	while (hItem)
	{
		SetItemState(hItem, nState, TVIS_STATEIMAGEMASK);
		TravelChild0(hItem, nState);
		hItem = GetNextSiblingItem(hItem);
	}
}

void CTreeCtrlEx::TravelParent0(HTREEITEM hItem, int nState)
{
	HTREEITEM Parent = GetParentItem(hItem), Child;
	if (Parent)
	{
		int Count[5] = { 0 };

		for (Child = GetChildItem(Parent); Child; Child = GetNextSiblingItem(Child))
		{
			Count[GetItemState(Child, TVIS_STATEIMAGEMASK) >> 12]++;
		}

		if (Count[3]||Count[2])
		{
			//while (Parent)
			do
			{
				SetItemState(Parent, 0x3000, TVIS_STATEIMAGEMASK);
			} while (Parent = GetParentItem(Parent));
		}
		else
		{
			SetItemState(Parent, 0x1000, TVIS_STATEIMAGEMASK);
			TravelParent0(Parent, 0);
		}
	}
}


void CTreeCtrlEx::TravelParent1(HTREEITEM hItem, int nState)
{
	HTREEITEM Parent = GetParentItem(hItem), Child;
	int TState;
	
		
	if (Parent&&GetItemState(Parent, TVIS_STATEIMAGEMASK) > 0xFFF)
	{
		Child = GetChildItem(Parent);

		while (Child)
		{
			if (Child != hItem)
			{
				TState = GetItemState(Child, TVIS_STATEIMAGEMASK)&TVIS_STATEIMAGEMASK;
				if (TState > 0xFFF && TState != nState)
				{
					//nState = INDEXTOSTATEIMAGEMASK(2);

					do
					{
						SetItemState(Parent, 0x3000, TVIS_STATEIMAGEMASK);
						Parent = GetParentItem(Parent);

					} while (Parent&&GetItemState(Parent, TVIS_STATEIMAGEMASK) > 0xFFF);

					return;
				}
			}
			Child = GetNextSiblingItem(Child);
		}

		SetItemState(Parent, nState, TVIS_STATEIMAGEMASK);
		TravelParent1(Parent, nState);
	}

}


void CTreeCtrlEx::TravelChild2(HTREEITEM hItem, int nState)
{
	switch (nState >> 12)
	{
	case 4:
	case 2:
	case 1:
		TravelChild0(hItem, nState); break;
	case 3:
	{
			  if (GetChildItem(hItem))
				  TravelChild0(hItem, 1<<12);
			  else
			  {
				  SetItemState(hItem, (3 % Flage + 1) << 12, TVIS_STATEIMAGEMASK);
			  }
			  break;
	}
	default:
		break;
	}
}

void CTreeCtrlEx::TravelParent2(HTREEITEM hItem, int nState)
{
	HTREEITEM Parent = GetParentItem(hItem), Child;
	if (Parent)
	{
		int Count[5] = {0};
		
		for (Child = GetChildItem(Parent); Child; Child = GetNextSiblingItem(Child))
		{
			Count[GetItemState(Child, TVIS_STATEIMAGEMASK) >> 12]++;
		}

		if (Count[1] == 0 && Count[4] == 0 && Count[3] == 0)
		{
			SetItemState(Parent, 2 << 12, TVIS_STATEIMAGEMASK);
		}
		else
		{
			if ((GetItemState(Parent, TVIS_STATEIMAGEMASK) >> 12) == 2||Count[2]||Count[3])
			{
				SetItemState(Parent, 3 << 12, TVIS_STATEIMAGEMASK);
			}
			else if ((GetItemState(Parent, TVIS_STATEIMAGEMASK) >> 12) != 3)
			{
				SetItemState(Parent, 1 << 12, TVIS_STATEIMAGEMASK);
			}
		}

		TravelParent2(Parent, nState);
	}

}
