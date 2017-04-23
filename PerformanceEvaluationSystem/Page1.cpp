// Page1.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PerformanceEvaluationSystemDlg.h"
#include "Page1.h"
#include "afxdialogex.h"
#include "math.h"

extern const int nLenFrame;

DWORD WINAPI ThreadAnalysis(LPVOID lpParam);

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
	buttonnum_online = 4;
	buttonnum_offline = 1;
	dynamicbutton = false;
}

CPage1::~CPage1()
{
	if (dynamicbutton)
	{
		for (int i = 0; i < buttonnum_online; i++)
		{
			if (m_buttons[i] != nullptr)
			{
				delete m_buttons[i];
			}
		}
	}
	delete[] m_buttons;
	delete[] rect_button;
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
	ON_MESSAGE(WM_CHANGEBUTTON, OnButtonChanged)
	ON_BN_CLICKED(IDB_ANALYSIS, &CPage1::OnBnClickedAnalysis)
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

	rect_button = new CRect[buttonnum_online];
	GetDlgItem(IDB_SET)->GetWindowRect(rect_button[0]);
	GetDlgItem(IDB_CHANNELIZE)->GetWindowRect(rect_button[1]);
	GetDlgItem(IDB_SPECTRUM)->GetWindowRect(rect_button[2]);
	GetDlgItem(IDB_UPLOAD)->GetWindowRect(rect_button[3]);
	ScreenToClient(rect_button[0]);
	ScreenToClient(rect_button[1]);
	ScreenToClient(rect_button[2]);
	ScreenToClient(rect_button[3]);
	m_buttons = new CButton *[buttonnum_online];
	m_buttons[0] = static_cast<CButton *>(GetDlgItem(IDB_SET));
	m_buttons[1] = static_cast<CButton *>(GetDlgItem(IDB_CHANNELIZE));
	m_buttons[2] = static_cast<CButton *>(GetDlgItem(IDB_SPECTRUM));
	m_buttons[3] = static_cast<CButton *>(GetDlgItem(IDB_UPLOAD));

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
	delete[] szDemodCmd_RecvData;
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
	delete[] szDemodCmd_RecvData;
}


void CPage1::OnBnClickedUpload()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	int nDemodCmdSize_SendData =20;
	char *szDemodCmd_SendData=new char[nDemodCmdSize_SendData];
	szDemodCmd_SendData[0]=0x17;
	szDemodCmd_SendData[1]=0x57;
	szDemodCmd_SendData[2]=0x90;
	szDemodCmd_SendData[3]=0xeb;
	szDemodCmd_SendData[4]=0x84;
	szDemodCmd_SendData[5]=0x00;
	szDemodCmd_SendData[6]=0x0f;
	szDemodCmd_SendData[7]=0x00;
	szDemodCmd_SendData[8]=0x00;         
	szDemodCmd_SendData[9]=0x00;
	szDemodCmd_SendData[10]=0x01;
	szDemodCmd_SendData[11]=0x00;
	szDemodCmd_SendData[12]=0x00;
	szDemodCmd_SendData[13]=0x00;
	szDemodCmd_SendData[14]=0x00;
	szDemodCmd_SendData[15]=0x00;
	szDemodCmd_SendData[16]=0x00;
	szDemodCmd_SendData[17]=0x00;
	szDemodCmd_SendData[18]=0x01;
	szDemodCmd_SendData[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_SendData[19]=szDemodCmd_SendData[19]+szDemodCmd_SendData[i];
	}
	pGEDevice->SendTo(szDemodCmd_SendData,nDemodCmdSize_SendData);
	pGEDevice->Open();							//打开千兆网设备
	pGEDevice->SetStorePath(_T("C:\\Recv\\"));
	float *pBufReceive = new float[320 * 1024];
	pGEDevice->RecvFrom((char *)pBufReceive, 320 * 1024 * 4, 10000);//接收320K数据
	double *pfftdata = new double[200 * 1024];
	for (int j = 0; j < 10; j++)   //提取出正确的fft数据
	{
		for (int i = 0; i < 10240; i++)
		{
			pfftdata[j * 20480 + i] = pBufReceive[j * 32768 + 16383 - i];
			pfftdata[j * 20480 + 10240 + i] = pBufReceive[j * 32768 + 32767 - i];
		}
	}
	CPerformanceEvaluationSystemDlg *pParent = (CPerformanceEvaluationSystemDlg *)GetParent();
	double carrier_frequency, signal_bandwith;
	int cursel = m_bandwidth.GetCurSel();
	double bandwidth;
	switch (cursel)
	{
	case 0:
		bandwidth = 0.2;
		break;
	case 1:
		bandwidth = 1;
		break;
	case 2:
		bandwidth = 5;
		break;
	case 3:
		bandwidth = 30;
		break;
	default:
		break;
	}
	pParent->GetCarrierFreqAndBW(pfftdata, 50 * 1024, bandwidth, carrier_frequency, signal_bandwith);
	pParent->m_Page3.fcw0 = carrier_frequency; 
	delete[] szDemodCmd_SendData;
	delete[] pBufReceive;
	delete[] pfftdata;
}


afx_msg LRESULT CPage1::OnButtonChanged(WPARAM wParam, LPARAM lParam)
{
	switch ((int)wParam)
	{
	case 0:               //在线模式
		{
			for (int i = 0; i < buttonnum_offline; i++)          //先销毁离线模式的按钮
			{
				m_buttons[i]->DestroyWindow();
				delete m_buttons[i];
				m_buttons[i] = nullptr;
			}

			for (int i = 0; i < buttonnum_online; i++)           //再新建在线模式的按钮
			{
				m_buttons[i] = new CButton();
			}
			m_buttons[0]->Create(_T("设置"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[0], this, IDB_SET);
			m_buttons[1]->Create(_T("信道化参数"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[1], this, IDB_CHANNELIZE);
			m_buttons[2]->Create(_T("预处理参数"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[2], this, IDB_SPECTRUM);
			m_buttons[3]->Create(_T("上传数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[3], this, IDB_UPLOAD);
			m_buttons[0]->SetFont(GetFont());
			m_buttons[1]->SetFont(GetFont());
			m_buttons[2]->SetFont(GetFont());
			m_buttons[3]->SetFont(GetFont());
			break;
		}
	case 1:               //离线模式
		{
			for (int i = 0; i < buttonnum_online; i++)
			{
				m_buttons[i]->DestroyWindow();
				if (dynamicbutton)
				{
					delete m_buttons[i];
				}
				m_buttons[i] = nullptr;
			}
			dynamicbutton = true;

			for (int i = 0; i < buttonnum_offline; i++)
			{
				m_buttons[i] = new CButton();
			}
			m_buttons[0]->Create(_T("频谱分析"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[0], this, IDB_ANALYSIS);
			m_buttons[0]->SetFont(GetFont());
			break;
		}

	default:
		break;
	}
	return 0;
}


void CPage1::OnBnClickedAnalysis()
{
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg *>(GetParent());
	HANDLE hThread = CreateThread(NULL, 0, ThreadAnalysis, pMainDlg, 0, NULL);
	CloseHandle(hThread);
}


DWORD WINAPI ThreadAnalysis(LPVOID lpParam)
{
	CPerformanceEvaluationSystemDlg *pMainDlg = (CPerformanceEvaluationSystemDlg *)lpParam;
	TCHAR path_execution[100];
	GetModuleFileName(NULL, path_execution, 100);
	CString path_simulink(path_execution);
	path_simulink = path_simulink.Left(path_simulink.ReverseFind('\\'));
	path_simulink = path_simulink.Left(path_simulink.ReverseFind('\\'));
	path_simulink.Append(_T("\\MatlabFiles\\pinpu_analyse"));        //获取频谱分析simulink文件的路径
	if (GetFileAttributes(path_simulink.GetBuffer()) == INVALID_FILE_ATTRIBUTES)
	{
		TRACE(_T("路径pinpu_analyse不存在"));
		return 0;
	}

	Engine *en;
	if (!(en = engOpen(NULL)))
	{
		TRACE(_T("打开Matlab引擎失败！"));
		return 0;
		//MessageBox(_T("打开Matlab引擎失败！"));
	}
	else
	{
		//打开成功则隐藏Matlab引擎
		engSetVisible(en,FALSE);
	}

	mxArray *mxPath = nullptr;
	mxPath = mxCreateString(path_simulink.GetBuffer());
	engPutVariable(en, _T("simulinkpath"), mxPath);
	engEvalString(en, _T("cd(simulinkpath)"));
	pMainDlg->DisplayRunningInfo(_T("开始频谱分析"));
	engEvalString(en, _T("channelfft"));               //进行子信道频谱分析
	pMainDlg->DisplayRunningInfo(_T("频谱分析结束，开始画图"));
	mxArray *mxfftdata = NULL;
	if ((mxfftdata = engGetVariable(en, _T("fftresult"))) == NULL)
	{
		TRACE(_T("获取fftresult失败\r\n"));
		return 0;
	}
	int row = 0;
	if ((row = mxGetM(mxfftdata)) == 0)
	{
		TRACE(_T("row = 0\r\n"));
		return 0;
	}
	double *fftdata = new double[row];
	memcpy((void *)fftdata, (void *)(mxGetPr(mxfftdata)), row * sizeof(double));             //获取频谱分析结果
	engClose(en);
	const int spectrumlen = 200 * 1024;
	double *spectrumjoint = new double[spectrumlen];
	for (int j = 0; j < 10; j++)   //进行频谱拼接
	{
		for (int i = 0; i < 10240; i++)
		{
			spectrumjoint[j * 20480 + i] = fftdata[j * 32768 + 16383 - i];
			spectrumjoint[j * 20480 + 10240 + i] = fftdata[j * 32768 + 32767 - i];
		}
	}
	delete[] fftdata;
	::SendMessage(pMainDlg->GetSafeHwnd(), WM_PLOTDATA, spectrumlen, (LPARAM)spectrumjoint);
	delete[] spectrumjoint;
}