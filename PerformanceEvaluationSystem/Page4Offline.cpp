// Page4Offline.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "Page4Offline.h"
#include "afxdialogex.h"


// CPage4Offline 对话框

IMPLEMENT_DYNAMIC(CPage4Offline, CPropertyPage)

CPage4Offline::CPage4Offline(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CPage4Offline::IDD)
{

}

CPage4Offline::~CPage4Offline()
{
}

void CPage4Offline::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPage4Offline, CPropertyPage)
END_MESSAGE_MAP()


// CPage4Offline 消息处理程序
