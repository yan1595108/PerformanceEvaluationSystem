// PageBase.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PageBase.h"
#include "afxdialogex.h"


// CPageBase �Ի���

IMPLEMENT_DYNAMIC(CPageBase, CPropertyPage)

CPageBase::CPageBase(UINT nDlgID,CWnd* pParent)
	: CPropertyPage(nDlgID)
{

}

CPageBase::~CPageBase()
{
}

CGigabitEthernetDevice * CPageBase::pGEDevice(NULL);

void CPageBase::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPageBase, CPropertyPage)
END_MESSAGE_MAP()


// CPageBase ��Ϣ�������


BOOL CPageBase::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//CRect DialogBoundaryRect;
	//GetDlgItem(IDS_PAGES_BOUNDARY)->GetWindowRect(&DialogBoundaryRect);
	MoveWindow(DlgPosition.left,DlgPosition.top,DlgPosition.right-DlgPosition.left,DlgPosition.bottom-DlgPosition.top);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPageBase::SetGEDevice(CGigabitEthernetDevice *pDevice)
{
	pGEDevice = pDevice;
}