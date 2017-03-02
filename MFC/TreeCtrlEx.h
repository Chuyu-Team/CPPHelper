#pragma once
#include "stdafx.h"

class CTreeCtrlEx : public CTreeCtrl
{
	DECLARE_DYNAMIC(CTreeCtrlEx)


	//模式0 
	void TravelParent0(HTREEITEM hItem, int nState);
	void TravelChild0(HTREEITEM hItem, int nState);


	//模式1  自然模式
	void TravelParent1(HTREEITEM hItem, int nState);
#define TravelChild1 TravelChild0
	//void TravelChild(HTREEITEM hItem, int nState);

	//模式2
	void TravelParent2(HTREEITEM hItem, int nState);
	void TravelChild2(HTREEITEM hItem, int nState);




public:
	//UINT m_uFlags;
	CTreeCtrlEx();
	virtual ~CTreeCtrlEx();
	//BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
	BOOL OnInitDialog(int s = 1);
	HTREEITEM InsertItem(_In_z_ LPCTSTR lpszItem, _In_ HTREEITEM hParent = TVI_ROOT, int Check = 1);
	int Flage;

	void SetCheck(HTREEITEM hItem);
protected:
	CImageList m_imgList;
	DECLARE_MESSAGE_MAP()

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnStateIconClick(NMHDR* pNMHDR, LRESULT* pResult);

};