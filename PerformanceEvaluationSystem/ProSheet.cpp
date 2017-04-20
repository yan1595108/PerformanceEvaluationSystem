// ProSheet.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "ProSheet.h"
#include "Page4.h"
#include "Page4Offline.h"


// CProSheet

IMPLEMENT_DYNAMIC(CProSheet, CPropertySheet)

CProSheet::CProSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}

CProSheet::CProSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{

}

CProSheet::~CProSheet()
{
}


BEGIN_MESSAGE_MAP(CProSheet, CPropertySheet)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CProSheet 消息处理程序


void CProSheet::OnSize(UINT nType, int cx, int cy)
{
	CPropertySheet::OnSize(nType, cx, cy);
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
	CPage4 *page4 = (CPage4 *)GetPage(0);
	HWND page4hwnd = page4->GetSafeHwnd();
	if (page4hwnd != NULL)
	{
		CRect rect1;
		rect1.top = rect.top + 35;
		rect1.bottom = rect.bottom;
		rect1.left = rect.left + 8;
		rect1.right = rect.right;
		page4->cx = rect1.Width();
		page4->cy = rect1.Height();
		page4->MoveWindow(&rect1);
		CPage4Offline *page4offline = (CPage4Offline *)GetPage(1);
		page4offline->cx = rect1.Width();
		page4offline->cy = rect1.Height();
	}

	// TODO: 在此处添加消息处理程序代码
}
