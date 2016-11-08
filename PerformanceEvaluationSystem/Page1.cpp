// Page1.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "math.h"


// CPage1 对话框

IMPLEMENT_DYNAMIC(CPage1, CPageBase)

CPage1::CPage1(CWnd* pParent /*=NULL*/)
	: CPageBase(CPage1::IDD, pParent)
	,m_CenterFreq(70.0),m_BandWidth(5.0),m_ReferenceLevel(140),m_ReferenceLevelDiv(13),m_nNumFFTPoints(25*20*1024)
	,mPage1_dCenterFreq(70000000.0),mPage1_dBandWidth(5000000.0),mPage1_dReferenceLevel(140),mPage1_dReferenceLevelDiv(13),mPage1_nNumFFTPoints(25*20*1024)
{

}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_CENTER_FREQ_UNIT, m_UnitCenterFreq);
	DDX_Control(pDX, IDC_COMBO_BAND_WIDTH_UNIT, m_UnitBandWidth);
	DDX_Control(pDX, IDC_COMBO_REFERENCE_LEVEL_UNIT, m_UnitReferenceLevel);
	DDX_Control(pDX, IDC_COMBO_REFERENCE_LEVEL_DIV_UNIT, m_UnitReferenceLevelDiv);
	DDX_Text(pDX, IDE_CENTER_FREQ, m_CenterFreq);
	DDX_Text(pDX, IDE_BAND_WIDTH, m_BandWidth);
	DDX_Text(pDX, IDE_REFERENCE_LEVEL, m_ReferenceLevel);
	DDX_Text(pDX, IDE_REFERENCE_LEVEL_DIV, m_ReferenceLevelDiv);
	DDX_Text(pDX, IDE_FFT_POINTS, m_nNumFFTPoints);
}


BEGIN_MESSAGE_MAP(CPage1, CPageBase)
	ON_BN_CLICKED(IDB_SET, &CPage1::OnBnClickedButtonSet)
END_MESSAGE_MAP()


// CPage1 消息处理程序


void CPage1::OnBnClickedButtonSet()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	mPage1_dCenterFreq = m_CenterFreq*pow((double)10,3*(3-m_UnitCenterFreq.GetCurSel()));
	mPage1_dBandWidth = m_BandWidth*pow((double)10,3*(3-m_UnitBandWidth.GetCurSel()));
	mPage1_dReferenceLevel = m_ReferenceLevel;
	mPage1_dReferenceLevelDiv = m_ReferenceLevelDiv;
	mPage1_nNumFFTPoints = m_nNumFFTPoints;

	TRACE("mPage1_dCenterFreq:%lf\n",mPage1_dCenterFreq);
	TRACE("mPage1_dBandWidth:%lf\n",mPage1_dBandWidth);
	TRACE("mPage1_dReferenceLevel:%lf\n",mPage1_dReferenceLevel);
	TRACE("mPage1_dReferenceLevelDiv:%lf\n",mPage1_dReferenceLevelDiv);
	TRACE("mPage1_nNumFFTPoints:%d\n",mPage1_nNumFFTPoints);
}


BOOL CPage1::OnInitDialog()
{
	CPageBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_UnitCenterFreq.Clear();
	m_UnitCenterFreq.AddString(_T("GHz"));
	m_UnitCenterFreq.AddString(_T("MHz"));
	m_UnitCenterFreq.AddString(_T("KHz"));
	m_UnitCenterFreq.AddString(_T("Hz"));
	m_UnitCenterFreq.SetCurSel(1);

	m_UnitBandWidth.Clear();
	m_UnitBandWidth.AddString(_T("GHz"));
	m_UnitBandWidth.AddString(_T("MHz"));
	m_UnitBandWidth.AddString(_T("KHz"));
	m_UnitBandWidth.AddString(_T("Hz"));
	m_UnitBandWidth.SetCurSel(1);

	m_UnitReferenceLevel.Clear();
	m_UnitReferenceLevel.AddString(_T("dB"));
	m_UnitReferenceLevel.SetCurSel(0);

	m_UnitReferenceLevelDiv.Clear();
	m_UnitReferenceLevelDiv.AddString(_T("dB/Div"));
	m_UnitReferenceLevelDiv.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
