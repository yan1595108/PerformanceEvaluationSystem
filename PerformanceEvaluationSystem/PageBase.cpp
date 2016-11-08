// PageBase.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PageBase.h"
#include "afxdialogex.h"


// CPageBase 对话框

IMPLEMENT_DYNAMIC(CPageBase, CDialogEx)

CPageBase::CPageBase(UINT nDlgID,CWnd* pParent)
	: CDialogEx(nDlgID, pParent)
{

}

CPageBase::~CPageBase()
{
}

void CPageBase::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageBase, CDialogEx)
END_MESSAGE_MAP()


// CPageBase 消息处理程序


BOOL CPageBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//CRect DialogBoundaryRect;
	//GetDlgItem(IDS_PAGES_BOUNDARY)->GetWindowRect(&DialogBoundaryRect);
	MoveWindow(DlgPosition.left,DlgPosition.top,DlgPosition.right-DlgPosition.left,DlgPosition.bottom-DlgPosition.top);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
