// COMM2.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "COMM2.h"
#include "afxdialogex.h"
#include "Page4.h"


// CCOMM2 对话框

IMPLEMENT_DYNAMIC(CCOMM2, CPropertyPage)

CCOMM2::CCOMM2()
	: CPropertyPage(CCOMM2::IDD)
	, m_downsample1(0)
	, m_downsample2(0)
	, m_directpull(0)
{
	bandwidth = 0;
}

CCOMM2::~CCOMM2()
{
}

void CCOMM2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_BANDWIDTH, m_bandwidth);
	DDX_Text(pDX, IDE_DIRECTPULL, m_directpull);
	DDX_Text(pDX, IDE_DOWNSAMPLE1, m_downsample1);
	DDX_Text(pDX, IDE_DOWNSAMPLE2, m_downsample2);
}


BEGIN_MESSAGE_MAP(CCOMM2, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_COMBO_BANDWIDTH, &CCOMM2::OnCbnSelchangeComboBandwidth)
	ON_BN_CLICKED(IDB_SPECTRUM, &CCOMM2::OnBnClickedSpectrum)
	ON_BN_CLICKED(IDB_CHANNELIZE, &CCOMM2::OnBnClickedChannelize)
	ON_BN_CLICKED(IDB_UPLOAD, &CCOMM2::OnBnClickedUpload)
END_MESSAGE_MAP()


// CCOMM2 消息处理程序


BOOL CCOMM2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ÔÚ´ËÌí¼Ó¶îÍâµÄ³õÊ¼»¯
	m_bandwidth.AddString(_T("0.2M"));
	m_bandwidth.AddString(_T("1M"));
	m_bandwidth.AddString(_T("5M"));
	m_bandwidth.AddString(_T("30M"));
	
	switch (bandwidth)
	{
	case 0:
		UpdateControls(0);
		m_bandwidth.SetCurSel(0);
		break;
	case 1:
		UpdateControls(1);
		m_bandwidth.SetCurSel(1);
		break;
	case 5:
		UpdateControls(2);
		m_bandwidth.SetCurSel(2);
		break;
	case 30:
		UpdateControls(3);
		m_bandwidth.SetCurSel(3);
		break;
	default:
		break;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// Òì³£: OCX ÊôÐÔÒ³Ó¦·µ»Ø FALSE
}


void CCOMM2::OnCbnSelchangeComboBandwidth()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateControls(m_bandwidth.GetCurSel());
}


void CCOMM2::UpdateControls(int index_bandwidth)
{
	switch (index_bandwidth)
	{
	case 0:
		bandwidth = 0;
		m_directpull = 30;
		m_downsample1 = 5;
		m_downsample2 = 5;
		break;
	case 1:
		bandwidth = 1;
		m_directpull = 15;
		m_downsample1 = 2;
		m_downsample2 = 5;
		break;
	case 2:
		bandwidth = 5;
		m_directpull = 3;
		m_downsample1 = 2;
		m_downsample2 = 5;
		break;
	case 3:
		bandwidth = 30;
		m_directpull = 1;
		m_downsample1 = 1;
		m_downsample2 = 3;
		break;
	default:
		break;
	}
	UpdateData(FALSE);
}


void CCOMM2::OnBnClickedSpectrum()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->SpectrumAnalyse();
}


void CCOMM2::OnBnClickedChannelize()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->bandwidth = bandwidth;
	pParent->directpull = m_directpull;
	pParent->downsample1 = m_downsample1;
	pParent->downsample2 = m_downsample2;
	pParent->Channelize();
}


void CCOMM2::OnBnClickedUpload()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	CPage4 *pParent = (CPage4 *)GetParent();
	pParent->DataUpload();
}
