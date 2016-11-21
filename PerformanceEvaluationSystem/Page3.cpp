// Page3.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "Page3.h"
#include "afxdialogex.h"


// CPage3 对话框

IMPLEMENT_DYNAMIC(CPage3, CPageBase)

CPage3::CPage3(CWnd* pParent /*=NULL*/)
	: CPageBase(CPage3::IDD, pParent)
	, m_downsample1(0)
	, m_downsample2(0)
{
	bandwidth[0] = 200;
	bandwidth[1] = 300;
	bandwidth[2] = 400;
	bandwidth[3] = 500;
	bandwidth[4] = 600;
	bandwidth[5] = 700;
	bandwidth[6] = 800;
	bandwidth[7] = 900;
	bandwidth[8] = 1;
	bandwidth[9] = 2;
	bandwidth[10] = 3;
	bandwidth[11] = 4;
	bandwidth[12] = 5;
	bandwidth[13] = 6;
	bandwidth[14] = 7;
	bandwidth[15] = 8;
	bandwidth[16] = 9;
	bandwidth[17] = 10;
	bandwidth[18] = 20;
	bandwidth[19] = 50;
	bandwidth[20] = 80;
	bandwidth[21] = 100;
	bandwidth[22] = 200;
	bandwidth[23] = 300;
	bandwidth[24] = 400;
	bandwidth[25] = 500;
	bandwidth[26] = 600;
	bandwidth[27] = 700;
	bandwidth[28] = 800;
	bandwidth[29] = 900;
	bandwidth[30] = 1;
	bandwidth[31] = 1.1;
	bandwidth[32] = 1.2;
	bandwidth[33] = 1.3;
	bandwidth[34] = 1.4;
	bandwidth[35] = 1.5;
	bandwidth[36] = 1.6;
	bandwidth[37] = 1.7;
	bandwidth[38] = 1.8;
	bandwidth[39] = 1.9;
	bandwidth[40] = 2;
	bandwidth[41] = 2.1;
	bandwidth[42] = 2.2;
	bandwidth[43] = 2.3;
	bandwidth[44] = 2.4;
	bandwidth[45] = 2.5;
	bandwidth[46] = 2.6;
	bandwidth[47] = 2.7;
	bandwidth[48] = 2.8;
	bandwidth[49] = 2.9;
	bandwidth[50] = 3;

	directpull[0] = 170;
	directpull[1] = 100;
	directpull[2] = 170;
	directpull[3] = 170;
	directpull[4] = 100;
	directpull[5] = 45;
	directpull[6] = 170;
	directpull[7] = 35;
	directpull[8] = 170;
	directpull[9] = 170;
	directpull[10] = 100;
	directpull[11] = 170;
	directpull[12] = 170;
	directpull[13] = 100;
	directpull[14] = 45;
	directpull[15] = 170;
	directpull[16] = 35;
	directpull[17] = 170;
	directpull[18] = 170;
	directpull[19] = 68;
	directpull[20] = 170;
	directpull[21] = 68;
	directpull[22] = 34;
	directpull[23] = 10;
	directpull[24] = 10;
	directpull[25] = 6;
	directpull[26] = 10;
	directpull[27] = 9;
	directpull[28] = 4;
	directpull[29] = 2;
	directpull[30] = 3;
	directpull[31] = 2;
	directpull[32] = 5;
	directpull[33] = 5;
	directpull[34] = 5;
	directpull[35] = 2;
	directpull[36] = 4;
	directpull[37] = 2;
	directpull[38] = 2;
	directpull[39] = 3;
	directpull[40] = 3;
	directpull[41] = 2;
	directpull[42] = 2;
	directpull[43] = 2;
	directpull[44] = 3;
	directpull[45] = 5;
	directpull[46] = 5;
	directpull[47] = 5;
	directpull[48] = 5;
	directpull[49] = 1;
	directpull[50] = 1;

	downsample1[0] = 20;
	downsample1[1] = 20;
	downsample1[2] = 10;
	downsample1[3] = 10;
	downsample1[4] = 10;
	downsample1[5] = 20;
	downsample1[6] = 10;
	downsample1[7] = 20;
	downsample1[8] = 8;
	downsample1[9] = 5;
	downsample1[10] = 5;
	downsample1[11] = 4;
	downsample1[12] = 4;
	downsample1[13] = 4;
	downsample1[14] = 5;
	downsample1[15] = 10;
	downsample1[16] = 5;
	downsample1[17] = 8;
	downsample1[18] = 4;
	downsample1[19] = 4;
	downsample1[20] = 1;
	downsample1[21] = 2;
	downsample1[22] = 2;
	downsample1[23] = 4;
	downsample1[24] = 3;
	downsample1[25] = 4;
	downsample1[26] = 2;
	downsample1[27] = 2;
	downsample1[28] = 2;
	downsample1[29] = 8;
	downsample1[30] = 1;
	downsample1[31] = 6;
	downsample1[32] = 2;
	downsample1[33] = 2;
	downsample1[34] = 2;
	downsample1[35] = 4;
	downsample1[36] = 2;
	downsample1[37] = 4;
	downsample1[38] = 4;
	downsample1[39] = 2;
	downsample1[30] = 2;
	downsample1[41] = 3;
	downsample1[42] = 3;
	downsample1[43] = 3;
	downsample1[44] = 2;
	downsample1[45] = 1;
	downsample1[46] = 1;
	downsample1[47] = 1;
	downsample1[48] = 1;
	downsample1[49] = 4;
	downsample1[50] = 4;

	downsample2[0] = 20;
	downsample2[1] = 20;
	downsample2[2] = 20;
	downsample2[3] = 16;
	downsample2[4] = 20;
	downsample2[5] = 20;
	downsample2[6] = 10;
	downsample2[7] = 20;
	downsample2[8] = 10;
	downsample2[9] = 8;
	downsample2[10] = 8;
	downsample2[11] = 5;
	downsample2[12] = 4;
	downsample2[13] = 5;
	downsample2[14] = 8;
	downsample2[15] = 1;
	downsample2[16] = 8;
	downsample2[17] = 1;
	downsample2[18] = 1;
	downsample2[19] = 1;
	downsample2[20] = 1;
	downsample2[21] = 1;
	downsample2[22] = 1;
	downsample2[23] = 1;
	downsample2[24] = 1;
	downsample2[25] = 1;
	downsample2[26] = 1;
	downsample2[27] = 1;
	downsample2[28] = 1;
	downsample2[29] = 1;
	downsample2[30] = 1;
	downsample2[31] = 1;
	downsample2[32] = 1;
	downsample2[33] = 1;
	downsample2[34] = 1;
	downsample2[35] = 1;
	downsample2[36] = 1;
	downsample2[37] = 1;
	downsample2[38] = 1;
	downsample2[39] = 1;
	downsample2[40] = 1;
	downsample2[41] = 1;
	downsample2[42] = 1;
	downsample2[43] = 1;
	downsample2[44] = 1;
	downsample2[45] = 1;
	downsample2[46] = 1;
	downsample2[47] = 1;
	downsample2[48] = 1;
	downsample2[49] = 1;
	downsample2[50] = 1;

	analog_bandwidth[0] = 200000;
	analog_bandwidth[1] = 200000;
	analog_bandwidth[2] = 200000;
	analog_bandwidth[3] = 200000;
	analog_bandwidth[4] = 200000;
	analog_bandwidth[5] = 200000;
	analog_bandwidth[6] = 200000;
	analog_bandwidth[7] = 200000;
	analog_bandwidth[8] = 200000;
	analog_bandwidth[9] = 200000;
	analog_bandwidth[10] = 200000;
	analog_bandwidth[11] = 200000;
	analog_bandwidth[12] = 200000;
	analog_bandwidth[13] = 200000;
	analog_bandwidth[14] = 200000;
	analog_bandwidth[15] = 200000;
	analog_bandwidth[16] = 200000;
	analog_bandwidth[17] = 200000;
	analog_bandwidth[18] = 200000;
	analog_bandwidth[19] = 200000;
	analog_bandwidth[20] = 200000;
	analog_bandwidth[21] = 200000;
	analog_bandwidth[22] = 1000000;
	analog_bandwidth[23] = 1000000;
	analog_bandwidth[24] = 1000000;
	analog_bandwidth[25] = 1000000;
	analog_bandwidth[26] = 1000000;
	analog_bandwidth[27] = 1000000;
	analog_bandwidth[28] = 1000000;
	analog_bandwidth[29] = 1000000;
	analog_bandwidth[30] = 5000000;
	analog_bandwidth[31] = 5000000;
	analog_bandwidth[32] = 5000000;
	analog_bandwidth[33] = 5000000;
	analog_bandwidth[34] = 5000000;
	analog_bandwidth[35] = 5000000;
	analog_bandwidth[36] = 5000000;
	analog_bandwidth[37] = 5000000;
	analog_bandwidth[38] = 5000000;
	analog_bandwidth[39] = 5000000;
	analog_bandwidth[40] = 5000000;
	analog_bandwidth[41] = 5000000;
	analog_bandwidth[42] = 5000000;
	analog_bandwidth[43] = 5000000;
	analog_bandwidth[44] = 5000000;
	analog_bandwidth[45] = 5000000;
	analog_bandwidth[46] = 5000000;
	analog_bandwidth[47] = 5000000;
	analog_bandwidth[48] = 5000000;
	analog_bandwidth[49] = 5000000;
	analog_bandwidth[50] = 5000000;
}

CPage3::~CPage3()
{
}

void CPage3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SIGNALBANDWIDTH, m_bandwidth);
	DDX_Text(pDX, IDE_DOWNSAMPLE3, m_downsample1);
	DDX_Text(pDX, IDE_DOWNSAMPLE4, m_downsample2);
	DDX_Text(pDX, IDE_DIRECTPULL2, m_directpull);
}


BEGIN_MESSAGE_MAP(CPage3, CPageBase)
	ON_CBN_SELCHANGE(IDC_SIGNALBANDWIDTH, &CPage3::OnCbnSelchangeSignalbandwidth)
	ON_BN_CLICKED(IDB_SPECTRUM2, &CPage3::OnBnClickedSpectrum2)
	ON_BN_CLICKED(IDB_PREPROCESSPARA, &CPage3::OnBnClickedPreprocesspara)
END_MESSAGE_MAP()

BOOL CPage3::OnInitDialog()
{
	CPageBase::OnInitDialog();

	// TODO:  ÔÚ´ËÌí¼Ó¶îÍâµÄ³õÊ¼»¯
	CString bandwidth_item;
	for (int i = 0; i < 51; i++)
	{
		bandwidth_item.Format(_T("%0.1f"), bandwidth[i]);
		if (i >= 30)
		{
			bandwidth_item += _T("M");
		}
		else if (i >= 8)
		{
			bandwidth_item += _T("K");
		}
		m_bandwidth.AddString(bandwidth_item);
	}
	m_bandwidth.SetCurSel(0);
	m_directpull = directpull[0];
	m_downsample1 = downsample1[0];
	m_downsample2 = downsample2[0];
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}


void CPage3::OnCbnSelchangeSignalbandwidth()
{
	int cursel = m_bandwidth.GetCurSel();
	m_directpull = directpull[cursel];
	m_downsample1 = downsample1[cursel];
	m_downsample2 = downsample2[cursel];
	UpdateData(FALSE);
}


void CPage3::OnBnClickedSpectrum2()
{
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	int cur_analog_bandwidth = analog_bandwidth[m_bandwidth.GetCurSel()];
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
	szDemodCmd_RecvData[13]=cur_analog_bandwidth >> 24;
	szDemodCmd_RecvData[14]=cur_analog_bandwidth >> 16;
	szDemodCmd_RecvData[15]=cur_analog_bandwidth >> 8;
	szDemodCmd_RecvData[16]=cur_analog_bandwidth;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage3::OnBnClickedPreprocesspara()
{
	UpdateData(TRUE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	int cur_analog_bandwidth = analog_bandwidth[m_bandwidth.GetCurSel()];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x81;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=m_directpull >> 8;
	szDemodCmd_RecvData[8]=m_directpull;         
	szDemodCmd_RecvData[9]=m_downsample1 >> 8;
	szDemodCmd_RecvData[10]=m_downsample1;
	szDemodCmd_RecvData[11]=m_downsample2 >> 8;
	szDemodCmd_RecvData[12]=m_downsample2;
	szDemodCmd_RecvData[13]=0;
	szDemodCmd_RecvData[14]=0;
	szDemodCmd_RecvData[15]=0;
	szDemodCmd_RecvData[16]=0;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}
