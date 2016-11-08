// COMM1.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "COMM1.h"
#include "afxdialogex.h"
#include "Page4.h"


// CCOMM1 对话框

IMPLEMENT_DYNAMIC(CCOMM1, CPropertyPage)

CCOMM1::CCOMM1(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CCOMM1::IDD)
	, m_offsetvalue(0)
	, m_c1delta(0)
	, m_c2delta(0)
{

}

CCOMM1::~CCOMM1()
{
}

void CCOMM1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDE_OFFSETVALUE, m_offsetvalue);
	DDX_Text(pDX, IDE_C1DELTA, m_c1delta);
	DDX_Text(pDX, IDE_C2DELTA, m_c2delta);
}


BEGIN_MESSAGE_MAP(CCOMM1, CPropertyPage)
	ON_BN_CLICKED(IDB_BEGINOFFSET, &CCOMM1::OnBnClickedBeginoffset)
	ON_BN_CLICKED(IDB_ENDOFFSET, &CCOMM1::OnBnClickedEndoffset)
	ON_BN_CLICKED(IDB_BEGINTRACK, &CCOMM1::OnBnClickedBegintrack)
	ON_BN_CLICKED(IDB_ENDTRACK, &CCOMM1::OnBnClickedEndtrack)
END_MESSAGE_MAP()


// CCOMM1 消息处理程序


void CCOMM1::OnBnClickedBeginoffset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->offsetvalue = m_offsetvalue;
	pParent->Beginoffset();
}


void CCOMM1::OnBnClickedEndoffset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->offsetvalue = m_offsetvalue;
	pParent->Endoffset();
}


void CCOMM1::OnBnClickedBegintrack()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->c1delta = m_c1delta;
	pParent->c2delta = m_c2delta;
	pParent->Begintrack();
}


void CCOMM1::OnBnClickedEndtrack()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->c1delta = m_c1delta;
	pParent->c2delta = m_c2delta;
	pParent->Endtrack();
}
