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
	m_downsample1 = 0;
	m_downsample2 = 0;
	m_directpull = 0;
	bandwidth = 0;
}

CPage1::~CPage1()
{
}

void CPage1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CENTER_FREQ_UNIT, m_UnitCenterFreq);
	DDX_Control(pDX, IDC_BAND_WIDTH_UNIT, m_UnitBandWidth);
	DDX_Control(pDX, IDC_REFERENCE_LEVEL_UNIT, m_UnitReferenceLevel);
	DDX_Control(pDX, IDC_REFERENCE_LEVEL_DIV_UNIT, m_UnitReferenceLevelDiv);
	DDX_Text(pDX, IDE_CENTER_FREQ, m_CenterFreq);
	DDX_Text(pDX, IDE_BAND_WIDTH, m_BandWidth);
	DDX_Text(pDX, IDE_REFERENCE_LEVEL, m_ReferenceLevel);
	DDX_Text(pDX, IDE_REFERENCE_LEVEL_DIV, m_ReferenceLevelDiv);
	DDX_Text(pDX, IDE_FFT_POINTS, m_nNumFFTPoints);
	DDX_Control(pDX, IDC_BANDWIDTH, m_bandwidth);
	DDX_Text(pDX, IDE_DIRECTPULL, m_directpull);
	DDX_Text(pDX, IDE_DOWNSAMPLE1, m_downsample1);
	DDX_Text(pDX, IDE_DOWNSAMPLE2, m_downsample2);
}


BEGIN_MESSAGE_MAP(CPage1, CPageBase)
	ON_BN_CLICKED(IDB_SET, &CPage1::OnBnClickedButtonSet)
	ON_CBN_SELCHANGE(IDC_BANDWIDTH, &CPage1::OnCbnSelchangeComboBandwidth)
	ON_BN_CLICKED(IDB_SPECTRUM, &CPage1::OnBnClickedSpectrum)
	ON_BN_CLICKED(IDB_CHANNELIZE, &CPage1::OnBnClickedChannelize)
	ON_BN_CLICKED(IDB_UPLOAD, &CPage1::OnBnClickedUpload)
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

	m_bandwidth.AddString(_T("0.2M"));
	m_bandwidth.AddString(_T("1M"));
	m_bandwidth.AddString(_T("5M"));
	m_bandwidth.AddString(_T("30M"));

	return TRUE; 
}


void CPage1::UpdateControls(int index_bandwidth)
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


void CPage1::OnCbnSelchangeComboBandwidth()
{
	UpdateControls(m_bandwidth.GetCurSel());
}


void CPage1::OnBnClickedSpectrum()
{
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x82;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x7A;
	szDemodCmd_RecvData[8]=0xE1;         
	szDemodCmd_RecvData[9]=0x01;
	szDemodCmd_RecvData[10]=0x17;
	szDemodCmd_RecvData[11]=0x00;
	szDemodCmd_RecvData[12]=0x05;
	szDemodCmd_RecvData[13]=0x00;
	szDemodCmd_RecvData[14]=0x00;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage1::OnBnClickedChannelize()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x83;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=bandwidth >> 8;
	szDemodCmd_RecvData[8]=bandwidth;         
	szDemodCmd_RecvData[9]=m_directpull >> 8;
	szDemodCmd_RecvData[10]=m_directpull;
	szDemodCmd_RecvData[11]=m_downsample1 >> 8;
	szDemodCmd_RecvData[12]=m_downsample1;
	szDemodCmd_RecvData[13]=m_downsample2 >> 8;
	szDemodCmd_RecvData[14]=m_downsample2;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage1::OnBnClickedUpload()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x84;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=0x00;         
	szDemodCmd_RecvData[9]=0x00;
	szDemodCmd_RecvData[10]=0x01;
	szDemodCmd_RecvData[11]=0x00;
	szDemodCmd_RecvData[12]=0x00;
	szDemodCmd_RecvData[13]=0x00;
	szDemodCmd_RecvData[14]=0x00;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}