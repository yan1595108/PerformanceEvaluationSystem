// PageBase.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PageBase.h"
#include "afxdialogex.h"


// CPageBase �Ի���

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


// CPageBase ��Ϣ�������


BOOL CPageBase::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//CRect DialogBoundaryRect;
	//GetDlgItem(IDS_PAGES_BOUNDARY)->GetWindowRect(&DialogBoundaryRect);
	MoveWindow(DlgPosition.left,DlgPosition.top,DlgPosition.right-DlgPosition.left,DlgPosition.bottom-DlgPosition.top);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
