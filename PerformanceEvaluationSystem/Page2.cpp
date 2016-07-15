// Page2.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "Page2.h"
#include "afxdialogex.h"


// CPage2 对话框

IMPLEMENT_DYNAMIC(CPage2, CDialogEx)

CPage2::CPage2(CWnd* pParent /*=NULL*/)
	: CPageBase(CPage2::IDD, pParent)
{

}

CPage2::~CPage2()
{
}

void CPage2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPage2, CDialogEx)
END_MESSAGE_MAP()
