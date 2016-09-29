
// PerformanceEvaluationSystemDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PerformanceEvaluationSystemDlg.h"
#include "afxdialogex.h"

//Matlab相关库文件（附加依赖项）
/*
#pragma comment(lib,"libmat.lib")
#pragma comment(lib,"libeng.lib")
#pragma comment(lib,"libmx.lib")
#pragma comment(lib,"mclmcrrt.lib")
#pragma comment(lib,"mclmcr.lib")
#pragma comment(lib,"mclbase.lib")
#pragma comment(lib,"mclcommain.lib")*/

//全局变量
CString strRunningInfo;				//程序运行信息
DWORD WINAPI DisplayData(LPVOID lpParam);

//线程相关函数
//UINT Page4ThreadRecv(LPVOID lpParam);
//DWORD WINAPI Page4ThreadRecv(LPVOID lpParam);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPerformanceEvaluationSystemDlg 对话框




CPerformanceEvaluationSystemDlg::CPerformanceEvaluationSystemDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPerformanceEvaluationSystemDlg::IDD, pParent),
	hDll(NULL),
	pGEDevice(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	position_file_plot = 0;
	CurrentX = 0;
	average = 0;
	count_average = 0;
	shock_average = 0;
	count_shock = 0;
}

CPerformanceEvaluationSystemDlg::~CPerformanceEvaluationSystemDlg()
{
	engClose(en);
};

void CPerformanceEvaluationSystemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPLOTX,m_iPlotX);
	DDX_Control(pDX, IDC_COMBO_WORKMODE, m_WorkMode);
	DDX_Control(pDX, IDC_COMBO_SIGNALTYPE, m_SignalType);
}

BEGIN_MESSAGE_MAP(CPerformanceEvaluationSystemDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BAND_SPECTRUM_SCANNING, &CPerformanceEvaluationSystemDlg::OnBnClickedBtnBandSpectrumScanning)
	ON_BN_CLICKED(IDC_BTN_BAND_CHANNELIZATION, &CPerformanceEvaluationSystemDlg::OnBnClickedBtnBandChannelization)
	ON_BN_CLICKED(IDC_BTN_PARAMETER_MEASUREMENT, &CPerformanceEvaluationSystemDlg::OnBnClickedBtnParameterMeasurement)
	ON_BN_CLICKED(IDC_BTN_CHANNELIZATION_DEMODULATION, &CPerformanceEvaluationSystemDlg::OnBnClickedBtnChannelizationDemodulation)
	ON_BN_CLICKED(IDC_BTN_REMOTE_SENSING_CONTROL, &CPerformanceEvaluationSystemDlg::OnBnClickedBtnRemoteSensingControl)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO_SIGNALTYPE, &CPerformanceEvaluationSystemDlg::OnCbnSelchangeComboSignaltype)
	ON_CBN_SELCHANGE(IDC_COMBO_WORKMODE, &CPerformanceEvaluationSystemDlg::OnCbnSelchangeComboWorkmode)
	ON_BN_CLICKED(IDC_BUTTON_TEST1, &CPerformanceEvaluationSystemDlg::OnBnClickedButtonTest1)
	ON_BN_CLICKED(IDC_BUTTON_TEST2, &CPerformanceEvaluationSystemDlg::OnBnClickedButtonTest2)
	ON_MESSAGE(WM_PLOTDATA, PlotData)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPerformanceEvaluationSystemDlg 消息处理程序

BOOL CPerformanceEvaluationSystemDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	//::SetWindowPos(GetSafeHwnd(),HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);		//使对话框处于窗口最顶层
	// TODO: 在此添加额外的初始化代码

	//创建千兆网设备对象
	if (pGEDevice==NULL){
		pGEDevice = new CGigabitEthernetDevice(m_hWnd);
	}

	InitialPages();				//初始化不同模式设置页面
	InitialControls();			//初始化界面控件
	InitialiPlotX();			//初始化绘图控件
	//InitialMatlabEngine();	//初始化Matlab引擎
	//InitialDLLs();			//初始化动态链接库
	
	SwitchPages(PAGE4);

	//屏蔽掉"服务器正在运行中"要选择"切换到..."或"重试"的对话框
	AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);
	AfxOleGetMessageFilter()->SetBusyReply(SERVERCALL_RETRYLATER);
	AfxOleGetMessageFilter()->EnableNotRespondingDialog(TRUE);
	AfxOleGetMessageFilter()->SetMessagePendingDelay(-1);

	pGEDevice->Initial();			//初始化千兆网接口
	/*RunInfo.open("C:\\Users\\Administrator\\Desktop\\RunInfo.txt",ios::out);*/
	if (!(en = engOpen(NULL)))
	{
		MessageBox(_T("打开Matlab引擎失败！"));
	}
	else
	{
		//打开成功则隐藏Matlab引擎
		engSetVisible(en,FALSE);
	}

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPerformanceEvaluationSystemDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPerformanceEvaluationSystemDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPerformanceEvaluationSystemDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//初始化控件
void CPerformanceEvaluationSystemDlg::InitialControls()
{
	//初始化工作模式
	m_WorkMode.Clear();
	m_WorkMode.AddString(_T("在线模式"));
	m_WorkMode.AddString(_T("离线模式"));
	m_WorkMode.SetCurSel(0);

	//初始化信号类型
	m_SignalType.Clear();
	m_SignalType.AddString(_T("窄带信号"));
	m_SignalType.AddString(_T("宽带信号"));
	m_SignalType.SetCurSel(0);
}

//初始化iPlotX控件
void CPerformanceEvaluationSystemDlg::InitialiPlotX()
{
	//////////////////////////////////////////////////////////////////////////
	//组件布局
	m_iPlotX.DisableLayoutManager();
	//添加标签控件
	lIndexLabel_CenterFreq = m_iPlotX.AddLabel();
	lIndexLabel_CenterFreq_Val = m_iPlotX.AddLabel();
	lIndexLabel_Span = m_iPlotX.AddLabel();
	lIndexLabel_Span_Val = m_iPlotX.AddLabel();
	lIndexLabel_Ref = m_iPlotX.AddLabel();
	lIndexLabel_Ref_Val = m_iPlotX.AddLabel();
	lIndexLabel_Marker = m_iPlotX.AddLabel();
	lIndexLabel_Marker_Val1 = m_iPlotX.AddLabel();
	lIndexLabel_Marker_Val2 = m_iPlotX.AddLabel();
	//纵向布局-自下向上
	//第一层-CenterFreq
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetZOrder(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetStartPercent(6);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetStopPercent(16);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetZOrder(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetStartPercent(18);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetStopPercent(27);
	//第一层-Span
	m_iPlotX.GetLabels(lIndexLabel_Span).SetZOrder(0);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetStartPercent(86);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetStopPercent(89);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetZOrder(0);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetStartPercent(90);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetStopPercent(100);
	//第二层-DataView
	m_iPlotX.SetDataViewZHorz(1);
	//第三层-Ref
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetZOrder(2);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetStartPercent(9);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetStopPercent(11);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetZOrder(2);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetStartPercent(11);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetStopPercent(18);
	//第三层-Marker
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetZOrder(2);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetStartPercent(72);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetStopPercent(80);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetZOrder(2);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetStartPercent(80);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetStopPercent(90);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetZOrder(2);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetStartPercent(90);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetStopPercent(100);
	//第四层-ToolBar
	m_iPlotX.GetToolBar(0).SetZOrder(3);
	//横线布局-从左往右
	//第一列
	m_iPlotX.GetYAxis(0).SetZOrder(0);
	//第二列
	m_iPlotX.SetDataViewZVert(1);
	//设置边框距
	m_iPlotX.SetOuterMarginTop(0);
	m_iPlotX.SetOuterMarginBottom(0);
	m_iPlotX.SetOuterMarginLeft(0);
	m_iPlotX.SetOuterMarginRight(10);
	m_iPlotX.EnableLayoutManager();
	//////////////////////////////////////////////////////////////////////////
	//工具栏设置
	//m_iPlotX.GetToolBar(0).SetVisible(FALSE);

	//标题设置
	m_iPlotX.SetTitleVisible(FALSE);

	//通道栏设置
	m_iPlotX.GetLegend(0).SetVisible(FALSE);

	//网格设置
	m_iPlotX.GetDataView(0).SetGridShow(FALSE);
	m_iPlotX.GetDataView(0).SetGridLineColor(GRAY);
	m_iPlotX.GetDataView(0).SetEnabled(TRUE);
	m_iPlotX.GetDataView(0).SetGridLineShowXMajors(TRUE);//主要的X轴网格线可见
	m_iPlotX.GetDataView(0).SetGridLineShowYMajors(TRUE);
	m_iPlotX.GetDataView(0).SetGridLineShowTop(FALSE);   //边界网格线不可见
	m_iPlotX.GetDataView(0).SetGridLineShowBottom(FALSE);
	m_iPlotX.GetDataView(0).SetGridLineShowLeft(FALSE);
	m_iPlotX.GetDataView(0).SetGridLineShowRight(FALSE);
	m_iPlotX.GetDataView(0).SetPopupEnabled(FALSE);

	//坐标轴设置
	m_iPlotX.GetXAxis(0).SetEnabled(TRUE);
	m_iPlotX.GetYAxis(0).SetEnabled(TRUE);
	//m_iPlotX.GetXAxis(0).SetVisible(FALSE);
	/*m_iPlotX.GetXAxis(0).SetMin(pPage1->mPage1_dCenterFreq-pPage1->mPage1_dBandWidth/2);
	m_iPlotX.GetXAxis(0).SetSpan(pPage1->mPage1_dBandWidth);
	m_iPlotX.GetYAxis(0).SetMin(pPage1->mPage1_dReferenceLevel - 10*pPage1->mPage1_dReferenceLevelDiv);
	m_iPlotX.GetYAxis(0).SetSpan(10*pPage1->mPage1_dReferenceLevelDiv);*/
	//UpdateAxis();
	m_iPlotX.GetYAxis(0).SetMinorCount(0);//设置每格小的刻度个数
	m_iPlotX.GetYAxis(0).SetMajorLength(5);//设置刻度线长度

	//字体设置
	COleFont OldLableFont = m_iPlotX.GetLabels(lIndexLabel_Span).GetFont();
	COleFont NewLableFont = OldLableFont;
	//NewLableFont.SetBold(FALSE);
	//NewLableFont.SetName("Times New Roman");
	CY NewLabelFontSize;
	NewLabelFontSize.int64 = (LONGLONG)(OldLableFont.GetSize().int64/1.5);
	NewLableFont.SetSize(NewLabelFontSize);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetFont(NewLableFont);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetFont(NewLableFont);

	//第二层-Center Freq
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetCaption(_T("Center Freq"));
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetAlignment(AlignhLeft); //1左对齐
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetMarginTop(0.1);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetMarginBottom(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetCaption(_T("160.00MHz"));
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetAlignment(AlignhRight);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetMarginTop(0.1);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetMarginBottom(0);
	//第二层-Span
	m_iPlotX.GetLabels(lIndexLabel_Span).SetCaption(_T("Span"));
	m_iPlotX.GetLabels(lIndexLabel_Span).SetAlignment(AlignhLeft); //1左对齐
	m_iPlotX.GetLabels(lIndexLabel_Span).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetMarginTop(0.1);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Span).SetMarginBottom(0);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetCaption(_T("320.00MHz"));
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetAlignment(AlignhRight);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetMarginTop(0.1);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetMarginBottom(0);
	//第四层-Ref
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetCaption(_T("Ref"));
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetAlignment(AlignhLeft); //1左对齐
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetMarginTop(0.2);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetMarginBottom(0.1);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetCaption(_T("10.00dBm"));
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetAlignment(AlignhRight);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetMarginTop(0.2);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetMarginBottom(0.1);
	
	//第四层-Marker
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetCaption(_T("Marker1"));
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetAlignment(AlignhLeft); //1左对齐
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetMarginTop(0.2);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetMarginBottom(0.1);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetCaption(_T("320.00MHz"));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetAlignment(AlignhRight);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetMarginTop(0.2);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetMarginBottom(0.1);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetCaption(_T("-44.91dBm"));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetAlignment(AlignhRight);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetMarginLeft(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetMarginTop(0.2);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetMarginRight(0);
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetMarginBottom(0.1);

	UpdateLabels();

	//添加网格
	//for (int i=0;i<11;i++)
	//{
	//	long lIndexLimitX = m_iPlotX.AddLimit();
	//	m_iPlotX.GetLimit(lIndexLimitX).SetStyle(0);//0:Y 1:X
	//	m_iPlotX.GetLimit(lIndexLimitX).SetColor(GRAY);
	//}
	//for (int i=0;i<11;i++)
	//{
	//	long lIndexLimitX = m_iPlotX.AddLimit();
	//	//TRACE("%d\n",lIndexLimitX);
	//	m_iPlotX.GetLimit(lIndexLimitX).SetStyle(1);//0:Y 1:X
	//	m_iPlotX.GetLimit(lIndexLimitX).SetColor(GRAY);
	//}
	//UpdateGrid();
	//设置iPlotX控件的环形Buffer大小
	int nRingBufferSize = 1000000;
	m_iPlotX.GetChannel(0).SetRingBufferSize(nRingBufferSize);
}

void CPerformanceEvaluationSystemDlg::InitialiPlotX_Page1()
{
	m_Page1.UpdateData(TRUE);	//更新控件变量对应的值

	//设置坐标轴标题不可见
	m_iPlotX.GetXAxis(0).SetTitleShow(FALSE);
	m_iPlotX.GetYAxis(0).SetTitleShow(FALSE);

	//设置标签页
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetCaption(_T("Center Freq"));
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetCaption(Data2Unit(m_Page1.mPage1_dCenterFreq,UNIT_FREQUENCY));
	m_iPlotX.GetLabels(lIndexLabel_Span).SetCaption(_T("Span"));
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetCaption(Data2Unit(m_Page1.mPage1_dBandWidth,UNIT_FREQUENCY));
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetCaption(_T("Ref"));
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetCaption(Data2Unit(m_Page1.mPage1_dReferenceLevel,UNIT_AMPLITUDE));
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetCaption(_T("Marker1"));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetCaption(_T("0.0MHz"));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetCaption(_T("0.0dBm"));
	
	//显示网格线
	/*for (int i=0;i<11;i++)
	{
		m_iPlotX.GetLimit(i).SetVisible(TRUE);
		m_iPlotX.GetLimit(i+11).SetVisible(TRUE);
	}*/
}

void CPerformanceEvaluationSystemDlg::InitialiPlotX_Page4(int nInitMode)
{
	//清空标签页的内容
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Span).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Ref).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Marker).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetCaption(_T(""));
	m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetCaption(_T(""));

	//设置坐标轴标题
	COleFont m_font;
	m_font = m_iPlotX.GetXAxis(0).GetLabelsFont();
	m_iPlotX.GetXAxis(0).SetTitleShow(TRUE);
	m_iPlotX.GetXAxis(0).SetTitleMargin(0);
	m_iPlotX.GetXAxis(0).SetTitleFont(m_font);
	m_iPlotX.GetXAxis(0).SetTitleFontColor(GREEN);
	m_iPlotX.GetYAxis(0).SetTitleShow(TRUE);
	m_iPlotX.GetYAxis(0).SetTitleMargin(0);
	m_iPlotX.GetYAxis(0).SetTitleFont(m_font);
	m_iPlotX.GetYAxis(0).SetTitleFontColor(GREEN);

	//隐藏网格线
	/*for (int i=0;i<11;i++)
	{
		m_iPlotX.GetLimit(i).SetVisible(FALSE);
		m_iPlotX.GetLimit(i+11).SetVisible(FALSE);
	}*/

	switch(nInitMode)
	{
	case WaveDispMode_DemoduData:
		m_iPlotX.GetXAxis(0).SetTitle(_T("Time(ms)"));
		m_iPlotX.GetYAxis(0).SetTitle(_T("Amplitude"));
		break;
	case WaveDispMode_PLLAccumEr:
		m_iPlotX.GetXAxis(0).SetTitle(_T("Time(ms)"));
		m_iPlotX.GetYAxis(0).SetTitle(_T("Phase(rad)"));
		break;
	case WaveDispMode_TimLoopEr:
		m_iPlotX.GetXAxis(0).SetTitle(_T("Time(ms)"));
		m_iPlotX.GetYAxis(0).SetTitle(_T("Phase(rad)"));
		break;
	case WaveDispMode_Variance:
		m_iPlotX.GetXAxis(0).SetTitle(_T("Time(ms)"));
		m_iPlotX.GetYAxis(0).SetTitle(_T("Amplitude"));
		break;
	default:

		break;
	}

}


//将数值根据不同类别转换成带单位的CString类型
CString CPerformanceEvaluationSystemDlg::Data2Unit(double dDataVal,int nUnitType)
{
	CString strTemp;
	if (UNIT_FREQUENCY == nUnitType)//频率单位Hz
	{
		if (abs(dDataVal)>=pow((double)10,9))
		{
			strTemp.Format(_T("%.2lfGHz"),dDataVal/pow((double)10,9));
		}
		else if (abs(dDataVal)>=pow((double)10,6))
		{
			strTemp.Format(_T("%.2lfMHz"),dDataVal/pow((double)10,6));
		}
		else if (abs(dDataVal)>=pow((double)10,3))
		{
			strTemp.Format(_T("%.2lfKHz"),dDataVal/pow((double)10,3));
		}
		else if (abs(dDataVal)>=0)
		{
			strTemp.Format(_T("%.2lfHz"),dDataVal);
		}
	}
	else if (UNIT_AMPLITUDE == nUnitType)//幅度单位dB
	{
		if (abs(dDataVal)>=pow((double)10,9))
		{
			strTemp.Format(_T("%.2lfGdB"),dDataVal/pow((double)10,9));
		}
		else if (abs(dDataVal)>=pow((double)10,6))
		{
			strTemp.Format(_T("%.2lfMdB"),dDataVal/pow((double)10,6));
		}
		else if (abs(dDataVal)>=pow((double)10,3))
		{
			strTemp.Format(_T("%.2lfKdB"),dDataVal/pow((double)10,3));
		}
		else if (abs(dDataVal)>=0)
		{
			strTemp.Format(_T("%.2lfdB"),dDataVal);
		}
	}
	else if (UNIT_TIME == nUnitType)//时间单位us
	{
		if (abs(dDataVal)>=pow((double)10,6))
		{
			strTemp.Format(_T("%.2lfs"),dDataVal/pow((double)10,6));
		}
		else if (abs(dDataVal)>=pow((double)10,3))
		{
			strTemp.Format(_T("%.2lfms"),dDataVal/pow((double)10,3));
		}
		else if (abs(dDataVal)>=0)
		{
			strTemp.Format(_T("%.2lfus"),dDataVal);
		}
	}
	return strTemp;
}

void CPerformanceEvaluationSystemDlg::UpdateLabels()
{
	//m_iPlotX.GetLabels(lIndexLabel_RBW_Val).SetCaption("");
	//m_iPlotX.GetLabels(lIndexLabel_VBW_Val).SetCaption("30.000KHz");
	//m_iPlotX.GetLabels(lIndexLabel_SWT_Val).SetCaption("30.000KHz");
	m_iPlotX.GetLabels(lIndexLabel_CenterFreq_Val).SetCaption(Data2Unit(m_Page1.mPage1_dCenterFreq,UNIT_FREQUENCY));
	m_iPlotX.GetLabels(lIndexLabel_Span_Val).SetCaption(Data2Unit(m_Page1.mPage1_dBandWidth,UNIT_FREQUENCY));
	m_iPlotX.GetLabels(lIndexLabel_Ref_Val).SetCaption(Data2Unit(m_Page1.mPage1_dReferenceLevel,UNIT_AMPLITUDE));
	//m_iPlotX.GetLabels(lIndexLabel_Att_Val).SetCaption("20dB");
	//m_iPlotX.GetLabels(lIndexLabel_Marker_Val1).SetCaption("30.000KHz");
	//m_iPlotX.GetLabels(lIndexLabel_Marker_Val2).SetCaption("30.000KHz");
}

void CPerformanceEvaluationSystemDlg::UpdateAxis()
{

	m_iPlotX.GetXAxis(0).SetMin(m_Page1.mPage1_dCenterFreq/pow((double)10,6)-m_Page1.mPage1_dBandWidth/pow((double)10,6)/2);
	m_iPlotX.GetXAxis(0).SetSpan(m_Page1.mPage1_dBandWidth/pow((double)10,6));
	//TRACE("Xmin:%lf Xspan:%lf\n",m_iPlotX.GetXAxis(0).GetMin(),m_iPlotX.GetXAxis(0).GetSpan());
	m_iPlotX.GetYAxis(0).SetMin(m_Page1.mPage1_dReferenceLevel - 10*m_Page1.mPage1_dReferenceLevelDiv);
	m_iPlotX.GetYAxis(0).SetSpan(10*m_Page1.mPage1_dReferenceLevelDiv);
}

void CPerformanceEvaluationSystemDlg::UpdateGrid()
{
	/*m_iPlotX.GetXAxis(0).SetMin(pPage1->mPage1_dCenterFreq - pPage1->mPage1_dBandWidth/2);
	m_iPlotX.GetXAxis(0).SetSpan(pPage1->mPage1_dBandWidth);
	m_iPlotX.GetYAxis(0).SetMin(pPage1->mPage1_dReferenceLevel-10*pPage1->mPage1_dReferenceLevelDiv);
	m_iPlotX.GetYAxis(0).SetSpan(10*pPage1->mPage1_dReferenceLevelDiv);*/
	
	for (int i=0;i<11;i++)
	{
		m_iPlotX.GetLimit(i).SetLine1Position(i*(m_iPlotX.GetXAxis(0).GetSpan())/10+m_iPlotX.GetXAxis(0).GetMin());
		m_iPlotX.GetLimit(i+11).SetLine1Position(i*(m_iPlotX.GetYAxis(0).GetSpan())/10+m_iPlotX.GetYAxis(0).GetMin());
	}
}

//初始化Matlab引擎
void CPerformanceEvaluationSystemDlg::InitialMatlabEngine()
{
	//打开Matlab引擎 engOpen参数可以为NULL也可以为"\0"即engOpen("\0")
	/*if (!(ep = engOpen(NULL)))
	{
		MessageBox("打开Matlab引擎失败！");
		return;
	}
	else
	{
		//打开成功则隐藏Matlab引擎
		engSetVisible(ep,FALSE);
		//engGetVisible（ep,retval）可以获取引擎显示、隐藏状态在BOOL型变量retval中
	}*/
}

void CPerformanceEvaluationSystemDlg::InitialDLLs()
{
	/*hDll = LoadLibrary(_T(".\\Cfft.dll"));
	if (NULL == hDll)
	{
		AfxMessageBox(_T("DLL加载失败"));
		return;
	}
	FFT_Init = (lpFun_CfftInitialize)GetProcAddress(hDll,MAKEINTRESOURCE(2));
	if (NULL == FFT_Init)
	{
		AfxMessageBox(_T("DLL中函数寻找失败:CfftInitialize!"));
		return;
	}
	FFT = (lpFun_Cfft)GetProcAddress(hDll,MAKEINTRESOURCE(1));
	if (NULL == FFT)
	{
		AfxMessageBox(_T("DLL中函数寻找失败:Cfft"));
		return;
	}
	if (FFT_Init())
	{
		TRACE("Initialize Success!\n");
	}
	else
	{
		TRACE("Initialize Failure!\n");
	}*/
}

void MatlabDemo()
{
	/*double time[] = {0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0};

	//创建一个矩阵，并命名、赋值
	mxArray *T = NULL;
	T = mxCreateDoubleMatrix(1,10,mxREAL);
	TRACE("The size of time is:%d\n",sizeof(time));
	//将数组time赋值给矩阵mxArray
	memcpy((void*)mxGetPr(T),(void*)time,sizeof(time));

	//将新矩阵送到Matlab的工作区
	engPutVariable(ep,"T",T);

	//在Matlab中计算D = .5.*(-9.8).*T.^2 
	CString str_cmd1;
	str_cmd1 = "D = .5.*(-9.8).*T.^2";
	engEvalString(ep,str_cmd1);
	//engEvalString(ep,"D = .5.*(-9.8).*T.^2");
	//函数返回值为0表示执行成功 返回1表示执行失败
	engEvalString(ep,"plot(T,D);");
	engEvalString(ep,"title('Position vs. Time for a falling object');");
	engEvalString(ep,"xlabel('Time (seconds)')");
	engEvalString(ep,"ylabel('Position(meters)');");

	//删除数组T
	mxDestroyArray(T);*/
}

//////////////////////////////////////////////////////////////////////////
//页面控制
void CPerformanceEvaluationSystemDlg::InitialPages()
{
	RECT DialogBoundaryRect;
	GetDlgItem(IDC_STATIC_PAGES_BOUNDARY)->GetWindowRect(&DialogBoundaryRect);
	ScreenToClient(&DialogBoundaryRect);
	DialogBoundaryRect.left += 1;
	DialogBoundaryRect.right -= 1;
	DialogBoundaryRect.top += 1;
	DialogBoundaryRect.bottom -= 1;

	m_Page1.DlgPosition = DialogBoundaryRect;
	m_Page2.DlgPosition = DialogBoundaryRect;
	m_Page3.DlgPosition = DialogBoundaryRect;
	m_Page4.DlgPosition = DialogBoundaryRect;
	m_Page5.DlgPosition = DialogBoundaryRect;

	m_Page1.Create(IDD_PAGE1,this);
	m_Page2.Create(IDD_PAGE2,this);
	m_Page3.Create(IDD_PAGE3,this);
	m_Page4.Create(IDD_PAGE4,this);
	m_Page5.Create(IDD_PAGE5,this);

	m_Page1.ShowWindow(SW_SHOW);
	m_Page2.ShowWindow(SW_HIDE);
	m_Page3.ShowWindow(SW_HIDE);
	m_Page4.ShowWindow(SW_HIDE);
	m_Page5.ShowWindow(SW_HIDE);
}

void CPerformanceEvaluationSystemDlg::SwitchPages(int nPageIndex)
{
	if (nPageIndex == PAGE1)
	{
		InitialiPlotX_Page1();
		m_Page1.ShowWindow(SW_SHOW);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page4.ShowWindow(SW_HIDE);
		m_Page5.ShowWindow(SW_HIDE);
	}
	else if (nPageIndex == PAGE2)
	{
		m_Page1.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_SHOW);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page4.ShowWindow(SW_HIDE);
		m_Page5.ShowWindow(SW_HIDE);
	}
	else if (nPageIndex == PAGE3)
	{
		m_Page1.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_SHOW);
		m_Page4.ShowWindow(SW_HIDE);
		m_Page5.ShowWindow(SW_HIDE);
	}
	else if (nPageIndex == PAGE4)
	{
		//int nCurSel = 
		InitialiPlotX_Page4(1);
		m_Page1.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page4.ShowWindow(SW_SHOW);
		m_Page5.ShowWindow(SW_HIDE);
	}
	else if (nPageIndex == PAGE5)
	{
		m_Page1.ShowWindow(SW_HIDE);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page4.ShowWindow(SW_HIDE);
		m_Page5.ShowWindow(SW_SHOW);
	}
	else
	{
		m_Page1.ShowWindow(SW_SHOW);
		m_Page2.ShowWindow(SW_HIDE);
		m_Page3.ShowWindow(SW_HIDE);
		m_Page4.ShowWindow(SW_HIDE);
		m_Page5.ShowWindow(SW_HIDE);
	}
}

//////////////////////////////////////////////////////////////////////////

void CPerformanceEvaluationSystemDlg::OnBnClickedBtnBandSpectrumScanning()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchPages(PAGE1);
	return;

	ThreadSpectrumDisplayParam SpectrumDisplayParam;
	SpectrumDisplayParam.pMainDlg = this;
	SpectrumDisplayParam.piPlotX = &m_iPlotX;

	m_pThreadSpectrumDisplay = AfxBeginThread(Thread_SpectrumDisplay,&SpectrumDisplayParam);

	return;
	/*//////////////////////////////////////////////////////////////////////////
	//读Matlab处理过的数据
	engEvalString(ep,"cd E:\\MainProject\\5-4\\多通道转发性能评估系统\\PerformanceEvaluationSystem\\MatlabFiles\\频段频谱扫描模式;");
	//清除Matlab工作区间的变量
	engEvalString(ep,"clear all;");
	engEvalString(ep,"clc;");
	//调用.m程序
	engEvalString(ep,"SpectrumDisplay;");
	mxArray *SpectrumData;
	//获取Matlab工作空间的变量SpectrumData(SpectrumData由运行“SpectrumDisplay”生成)
	SpectrumData = engGetVariable(ep,"y_ampl");
	//获取矩阵QPSK的行数和列数
	int M,N;
	M = N = 0;
	if (SpectrumData!=NULL)
	{
		M = mxGetM(SpectrumData);
		N = mxGetN(SpectrumData);
	}
	else
	{
		TRACE("SpectrumData为空\n");
	}
	TRACE("*******************************************************************\n");
	TRACE("M:%d  N:%d\n",M,N);
	TRACE("*******************************************************************\n");
	//求一维向量的长度
	LONGLONG llSizeSpectrumData = 0;
	if (M>N)
	{
		llSizeSpectrumData = M;
	}
	else
	{
		llSizeSpectrumData = N;
	}
	double *pSpectrumData = new double[llSizeSpectrumData];
	memcpy((void *)pSpectrumData,(void *)(mxGetPr(SpectrumData)),llSizeSpectrumData*sizeof(double));

	double dFrequencySampling = 32;
	double dPointReadIn_X = -dFrequencySampling/2;
	double dPointReadIn_Y = 0.0;
	double dDetFrequency = (double)dFrequencySampling/llSizeSpectrumData;
	m_iPlotX.GetChannel(0).SetTraceVisible(TRUE);
	m_iPlotX.GetChannel(0).SetTraceLineStyle(0);
	m_iPlotX.GetChannel(0).SetColor(YELLOW);

	CiPlotChannelX ChannelTemp = m_iPlotX.GetChannel(0);

	for (int i=0;i<1;i++)
	{
		m_iPlotX.ClearAllData();
		for (int i=0;i<llSizeSpectrumData;i++)
		{
			//TRACE("x:%.10lf y:%lf\n",dPointReadIn_X,pSpectrumData[i]);
			//ChannelTemp.AddXY(dPointReadIn_X,pSpectrumData[i]*(1+rand()/(double)RAND_MAX));
			ChannelTemp.AddXY(dPointReadIn_X,-rand()%90);
			dPointReadIn_X += dDetFrequency;
		}
		ChannelTemp.AddXNull(dPointReadIn_X);
		UpdateGrid();
	}
	
	//m_iPlotX.GetXAxis(0).SetMin(-dFrequencySampling/2*1.2);
	//m_iPlotX.GetXAxis(0).SetSpan(dFrequencySampling*1.2);
	//m_iPlotX.GetXAxis(0).SetMin(-dFrequencySampling/2);
	//m_iPlotX.GetXAxis(0).SetSpan(dFrequencySampling);
	
	//m_iPlotX.ClearAllData();
	//m_iPlotX.RepaintAll();
	
	//获取输入一组数据的最大值、均值、最小值
	double dYMax = m_iPlotX.GetChannel(0).GetYMax();
	double dYMean = m_iPlotX.GetChannel(0).GetYMean();
	double dYMin = m_iPlotX.GetChannel(0).GetYMin();
	TRACE("dYMax:%lf dYMean:%lf dYMin:%f\n",dYMax,dYMean,dYMin);

	UpdateGrid();

	double dTestVal = m_iPlotX.GetChannel(0).GetDataX(16);
	TRACE("dTestVal:%d\n",dTestVal);

	delete[] pSpectrumData;
	/ * //读入数据
	FILE *pFileReadIn =NULL;
	CString strFileReadInDir("E:\\MainProject\\5-4\\C程序\\频谱显示\\绘图控件测试\\TEST\\Spectrum.txt");
	pFileReadIn = fopen(strFileReadInDir,"r");
	if (pFileReadIn == NULL)
	{
		TRACE("Open File Filure!\n");
		return;
	}
	double dPointReadIn_X = 0.0;
	double dPointReadIn_Y = 0.0;
	double dDetTime = (double)1/32/1024/1024;
	m_iPlotX.GetChannel(0).SetTraceVisible(TRUE);
	m_iPlotX.GetChannel(0).SetTraceLineStyle(0);
	//m_iPlotX.GetChannel(0).SetColor(100000);
	m_iPlotX.GetChannel(0).SetColor(110000);
	while(fscanf(pFileReadIn,"%lf",&dPointReadIn_Y)!=EOF)
	{
		double dPointReadIn_YTemp = 0.0;
		dPointReadIn_YTemp = (double)20*log10(abs(dPointReadIn_Y));
		//TRACE("x:%.10lf y:%lf\n",dPointReadIn_X,dPointReadIn_Y);
		if (dPointReadIn_YTemp>=0 && dPointReadIn_YTemp<=100)
		{
			m_iPlotX.GetChannel(0).AddXY(dPointReadIn_X,dPointReadIn_YTemp);
		}
		else
		{
		}
		dPointReadIn_X += dDetTime;
	}
	fclose(pFileReadIn);
	m_iPlotX.GetXAxis(0).SetSpan(dPointReadIn_X);* /*/
}

UINT Thread_SpectrumDisplay(LPVOID lpParam)
{
	//获取传入线程的参数
	pThreadSpectrumDisplayParam pThreadParam = (pThreadSpectrumDisplayParam)lpParam;
	CPerformanceEvaluationSystemDlg* pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pThreadParam->pMainDlg);
	CiPlotX* iPlotX = static_cast<CiPlotX*>(pThreadParam->piPlotX);

	//将经常需要访问的组件存放到一个临时变量中，可减少访问组件所需要的时间
	CiPlotChannelX ChannelTemp = iPlotX->GetChannel(0);
	CiPlotLabelX LabelTemp_CenterFreq = iPlotX->GetLabels(pMainDlg->lIndexLabel_CenterFreq_Val);
	CiPlotLabelX LabelTemp_Span = iPlotX->GetLabels(pMainDlg->lIndexLabel_Span_Val);
	CiPlotLabelX LabelTemp_Ref = iPlotX->GetLabels(pMainDlg->lIndexLabel_Ref_Val);
	CiPlotLabelX LabelTemp_MarkerVal1 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val1);
	CiPlotLabelX LabelTemp_MarkerVal2 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val2);

	ChannelTemp.SetTraceVisible(TRUE);
	ChannelTemp.SetTraceLineStyle(0);
	ChannelTemp.SetColor(YELLOW);

	CFile FileSubChannels;
	CString strFileSubChaDir,strFileSubChaPath,strFileSubChaName;
	strFileSubChaPath = "E:\\MainProject\\5-4\\多通道转发性能评估系统\\频谱分析\\子信道化数据\\MAT2DAT\\Data_dat\\";
	int nNumSubChannels = 25;				//子信道数
	int nSubChanNFFT = 32*1024;				//子信道FFT点数
	int nSubChanIntercepNFFT = 20*1024;		//子信道FFT截取长度 fs/Pdown/nSubChanNFFT*nSubChanIntercepNFFT = dBand/nNumSubChannels
	double dCenterFreq = 70*pow(10.0,6);		//中心频率
	double dAnalogBand = 5*pow(10.0,6);				//带宽
	double dDefFreq = dAnalogBand/(nSubChanIntercepNFFT*nNumSubChannels);					//频率分辨率
	double dAxisXFreq = dCenterFreq - dAnalogBand/2;										//X轴起始频率
	short *pDataRead = new short[2*nSubChanNFFT];                                   //存放子信道化数据
	double *dpSubChannelsFFT_Complex = new double[nNumSubChannels*nSubChanNFFT*2];	//子信道IQ两路FFT-复数形式
	double *dpSubChannelsFFT_Abs = new double[nNumSubChannels*nSubChanNFFT];		//子信道IQ两路取模-实数形式
	double *dpAllChannelsFFT = new double[nNumSubChannels*nSubChanIntercepNFFT];	//子信道FFT截取点数
	double *dpFFT_Wn = new double[nSubChanNFFT];									//FFT旋转因子
	LONGLONG llPosFileSubChan = 0;													//子信道数据文件位置
	BOOL bIsFirstTimeAddData = TRUE;
	double dYMaxVal = 0.0;
	double dXMaxVal = 0.0;
	CString strYMaxVal,strXMaxVal;

	pMainDlg->m_Page1.mPage1_dCenterFreq = 70*pow(10.0,6);
	pMainDlg->m_Page1.mPage1_dBandWidth = 5*pow(10.0,6);
	pMainDlg->m_Page1.mPage1_dReferenceLevel = 2100000;
	pMainDlg->m_Page1.mPage1_dReferenceLevelDiv = 210000;
	pMainDlg->m_Page1.mPage1_nNumFFTPoints = nSubChanIntercepNFFT*nNumSubChannels;

	pMainDlg->UpdateAxis();
	pMainDlg->UpdateGrid();

	//旋转因子初始化
	gen_w_r2(dpFFT_Wn,nSubChanNFFT);
	bit_rev(dpFFT_Wn,nSubChanNFFT>>1);

	//while(1)
	{
		for (int i=0;i<nNumSubChannels;i++)
		{
			strFileSubChaName.Format(_T("SubChannel%d.dat"),i+1);
			strFileSubChaDir = strFileSubChaPath + strFileSubChaName;
			FileSubChannels.Open(strFileSubChaDir,CFile::modeRead | CFile::shareDenyNone);
			while((llPosFileSubChan+2*nSubChanNFFT*sizeof(short))>=FileSubChannels.GetLength())
			{
				Sleep(2);
			}
			
			FileSubChannels.Read(pDataRead,2*nSubChanNFFT*sizeof(short));           //读取子信道化数据
			if (nNumSubChannels-1 == i)
			{
				llPosFileSubChan += 2*nSubChanNFFT*sizeof(short);
			}
			FileSubChannels.Close();
			for (int j=0;j<2*nSubChanNFFT;j++)
			{
				dpSubChannelsFFT_Complex[j] = (double)((short)pDataRead[j]);
			}
			/*for (int m=0;m<10;m++)
			{
				TRACE("%lf  ",dpSubChannelsFFT_Complex[m]);
			}
			TRACE("\n");
			for (int m=0;m<10;m++)
			{
				TRACE("%lf  ",dpSubChannelsFFT_Complex[2*nSubChanNFFT-10+m]);
			}*/
			DSPF_sp_cfftr2_dit(dpSubChannelsFFT_Complex,dpFFT_Wn,nSubChanNFFT);
			bit_rev(dpSubChannelsFFT_Complex,nSubChanNFFT);
			double dTemp1;
			/*for (int s=0;s<10;s++)
			{
				dTemp1 = dpSubChannelsFFT_Complex[s];
				TRACE("%lf  ",dTemp1);
			}
			TRACE("\n");
			for(int t=0;t<10;t++)
			{
				dTemp1 = dpSubChannelsFFT_Complex[2*nSubChanNFFT-10+t];
				TRACE("%lf  ",dTemp1);
			}
			TRACE("\n\n");*/
			complex2real(dpSubChannelsFFT_Complex,dpSubChannelsFFT_Abs,nSubChanNFFT);

			double dTemp2;
			/*for (int s=0;s<10;s++)
			{
				dTemp2 = dpSubChannelsFFT_Abs[s];
				TRACE("%lf  ",dTemp2);
			}
			for (int s=0;s<10;s++)
			{
				dTemp2 = dpSubChannelsFFT_Abs[nSubChanNFFT-10+s];
				TRACE("%lf  ",dTemp2);
			}*/

			for (int j=0;j<nSubChanNFFT;j++)
			{
				//dpSubChannelsFFT_Abs[j] = 20*log10(dpSubChannelsFFT_Abs[j]);
				dpSubChannelsFFT_Abs[j] = dpSubChannelsFFT_Abs[j];
			}
			/*for (int s=0;s<10;s++)
			{
				dTemp2 = dpSubChannelsFFT_Abs[s];
				TRACE("%lf  ",dTemp2);
			}*/

			//将子信道化数据放在一起
			memcpy(dpAllChannelsFFT+i*nSubChanIntercepNFFT,dpSubChannelsFFT_Abs+nSubChanNFFT-nSubChanIntercepNFFT/2,nSubChanIntercepNFFT/2*sizeof(double));
			//TRACE("%d  %d\n",(i*nSubChanIntercepNFFT+nSubChanIntercepNFFT/2)*sizeof(double),nSubChanIntercepNFFT/2);
			memcpy(dpAllChannelsFFT+i*nSubChanIntercepNFFT+nSubChanIntercepNFFT/2,dpSubChannelsFFT_Abs,nSubChanIntercepNFFT/2*sizeof(double));
			//TRACE("%lf  \n",dpSubChannelsFFT_Abs[0]);
			//TRACE("%lf \n",dpAllChannelsFFT[10240]);
			//TRACE("0x%x  0x%x\n",dpAllChannelsFFT+(i*nSubChanIntercepNFFT+nSubChanIntercepNFFT/2),&dpAllChannelsFFT[10240]);
			/*for (int s=0;s<10;s++)
			{
				dTemp2 = dpAllChannelsFFT[s+nSubChanIntercepNFFT];
				TRACE("%lf  ",dTemp2);
			}
			TRACE("\n");
			for (int s=0;s<10;s++)
			{
				dTemp2 = dpAllChannelsFFT[nSubChanIntercepNFFT/2+nSubChanIntercepNFFT+s];
				TRACE("%lf  ",dTemp2);
			}*/
			//TRACE("\n\n");

		}
		/*double dTemp2;
		for (int s=0;s<10;s++)
		{
			dTemp2 = dpAllChannelsFFT[(nNumSubChannels-1)*nSubChanIntercepNFFT+s];
			TRACE("%lf  ",dTemp2);
		}
		TRACE("\n");
		for (int s=0;s<10;s++)
		{
			dTemp2 = dpAllChannelsFFT[(nNumSubChannels-1)*nSubChanIntercepNFFT+nSubChanIntercepNFFT/2+s];
			TRACE("%lf  ",dTemp2);
		}*/

		if (bIsFirstTimeAddData)
		{
			bIsFirstTimeAddData = FALSE;
			for (int i=0;i<nSubChanIntercepNFFT*nNumSubChannels;i++)
			{
				ChannelTemp.AddXY(dAxisXFreq/pow(10.0,6),dpAllChannelsFFT[i]);
				dAxisXFreq += dDefFreq;
				//TRACE("%d：%lf \n",i,dpAllChannelsFFT[i]);
			}
		}
		else
		{
			dYMaxVal = dpAllChannelsFFT[0];
			dXMaxVal = 0;
			for (int i=0;i<nSubChanIntercepNFFT*nNumSubChannels;i++)
			{
				ChannelTemp.SetDataY(i,dpAllChannelsFFT[i]);
				if (dpAllChannelsFFT[i]>dYMaxVal)
				{
					dYMaxVal = dpAllChannelsFFT[i];
					dXMaxVal = i;
				}
			}
			strXMaxVal.Format(_T("%.2lfMHz"),dXMaxVal*dDefFreq + dCenterFreq-dAnalogBand/2);
			strYMaxVal.Format(_T("%.2lfdBm"),dYMaxVal);
			LabelTemp_MarkerVal1.SetCaption(strXMaxVal);
			LabelTemp_MarkerVal2.SetCaption(strYMaxVal);
		}
		pMainDlg->UpdateGrid();
		double dCarrierFrq = 0.0,dSignalBW = 0.0,dSNR;
		pMainDlg->GetCarrierFreqAndBW(dpAllChannelsFFT,nNumSubChannels*nSubChanIntercepNFFT,dAnalogBand,dCarrierFrq,dSignalBW);
		TRACE(_T("载波频率差：%lf\n信号带宽：%lf\n"),dCarrierFrq,dSignalBW);
		pMainDlg->GetSNR(dpAllChannelsFFT,nNumSubChannels*nSubChanIntercepNFFT,dCarrierFrq+70*pow(10.0,6),0.32*pow(10.0,6),dAnalogBand,dSignalBW,dSNR);
		//CarryAndBWCalculate();
	}
	delete[] pDataRead;
	delete[] dpSubChannelsFFT_Complex;
	delete[] dpSubChannelsFFT_Abs;
	delete[] dpAllChannelsFFT;
	delete[] dpFFT_Wn;

	DWORD t1 = GetTickCount();
	DWORD t2 = GetTickCount();
	CString strTimeInfo;
	strTimeInfo.Format(_T("共耗时：%dms"),t2-t1);
	AfxMessageBox(strTimeInfo);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//用动态链接库
/*
UINT Thread_SpectrumDisplay(LPVOID lpParam)
{
	//获取传入线程的参数
	pThreadSpectrumDisplayParam pThreadParam = (pThreadSpectrumDisplayParam)lpParam;
	CPerformanceEvaluationSystemDlg* pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pThreadParam->pMainDlg);
	CiPlotX* iPlotX = static_cast<CiPlotX*>(pThreadParam->piPlotX);

	//将经常需要访问的组件存放到一个临时变量中，可减少访问组件所需要的时间
	CiPlotChannelX ChannelTemp = iPlotX->GetChannel(0);
	CiPlotLabelX LabelTemp_RBW = iPlotX->GetLabels(pMainDlg->lIndexLabel_RBW_Val);
	CiPlotLabelX LabelTemp_VBW = iPlotX->GetLabels(pMainDlg->lIndexLabel_VBW_Val);
	CiPlotLabelX LabelTemp_SWT = iPlotX->GetLabels(pMainDlg->lIndexLabel_SWT_Val);
	CiPlotLabelX LabelTemp_CenterFreq = iPlotX->GetLabels(pMainDlg->lIndexLabel_CenterFreq_Val);
	CiPlotLabelX LabelTemp_Span = iPlotX->GetLabels(pMainDlg->lIndexLabel_Span_Val);
	CiPlotLabelX LabelTemp_Ref = iPlotX->GetLabels(pMainDlg->lIndexLabel_Ref_Val);
	CiPlotLabelX LabelTemp_Att = iPlotX->GetLabels(pMainDlg->lIndexLabel_Att_Val);
	CiPlotLabelX LabelTemp_MarkerVal1 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val1);
	CiPlotLabelX LabelTemp_MarkerVal2 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val2);

	ChannelTemp.SetTraceVisible(TRUE);
	ChannelTemp.SetTraceLineStyle(0);
	ChannelTemp.SetColor(YELLOW);

	pMainDlg->UpdateAxis();
	pMainDlg->UpdateGrid();

	const int nNumFFTPoint = pMainDlg->pPage1->mPage1_nNumFFTPoints;
	TRACE("FFT点数：%d\n",nNumFFTPoint);
	double dFrequencySampling = pMainDlg->pPage1->mPage1_dBandWidth/pow((double)10,6);
	double dPointReadIn_X = pMainDlg->pPage1->mPage1_dCenterFreq/pow((double)10,6) - dFrequencySampling/2;
	TRACE("Xmin:%lf Xspan:%lf\n",dPointReadIn_X,dFrequencySampling);
	double dDetFrequency = (double)dFrequencySampling/nNumFFTPoint;

	mwArray mwATimeDomain(1,nNumFFTPoint,mxDOUBLE_CLASS);
	mwArray mwAFrequencyDomain(1,nNumFFTPoint,mxDOUBLE_CLASS);
	mwArray mwAAarrySize(1,1,mxDOUBLE_CLASS);
	double *pdFrequencyDomain = new double[nNumFFTPoint];
	double dAarrySize = nNumFFTPoint;

	CFile FileReadIn;
	CString strFileReadInDir;
	strFileReadInDir = "E:\\MainProject\\5-4\\多通道转发性能评估系统\\频谱分析\\FFT\\显示加载\\FFTVSCode\\FFTVSCode\\QPSKSourceFile\\QPSKSource.dat";
	FileReadIn.Open(strFileReadInDir,CFile::modeRead);
	LONGLONG llLenFileReadIn = FileReadIn.GetLength();
	TRACE("%d\n",llLenFileReadIn);
	LONGLONG llReadPos = 0;
	short *psTimeDomain = new short[nNumFFTPoint];
	double *pdTimeDomain = new double[nNumFFTPoint];

	DWORD t1 = GetTickCount();
	BOOL bIsFirstTimeAddData = TRUE;

	CString strYMaxVal,strXMaxVal;
	double dYMaxVal = 0.0;
	double dXMaxVal = 0.0;
	int nCountTest = 0;
	while((llLenFileReadIn-llReadPos)>=nNumFFTPoint)
	{
		nCountTest++;
		if (nCountTest==80)
		{
			pMainDlg->OnBnClickedBtnParameterMeasurement();
		}

		FileReadIn.Read(psTimeDomain,2*nNumFFTPoint);
		llReadPos += 2*nNumFFTPoint;
		//short转double型
		for (int i=0;i<nNumFFTPoint;i++)
		{
			pdTimeDomain[i] = (double)((short)psTimeDomain[i]);
		}

		mwATimeDomain.SetData(pdTimeDomain,nNumFFTPoint);
		mwAAarrySize.SetData(&dAarrySize,1);

		pMainDlg->FFT(1,mwAFrequencyDomain,mwATimeDomain,mwAAarrySize);
		mwAFrequencyDomain.GetData(pdFrequencyDomain,dAarrySize);

		if (bIsFirstTimeAddData)
		{
			bIsFirstTimeAddData = FALSE;
			for (int i=0;i<nNumFFTPoint;i++)
			{
				ChannelTemp.AddXY(dPointReadIn_X,pdFrequencyDomain[i]);
				dPointReadIn_X += dDetFrequency;
			}
		}
		else
		{
			dYMaxVal = pdFrequencyDomain[0];
			dXMaxVal = 0;
			for (int i=0;i<nNumFFTPoint;i++)
			{
				ChannelTemp.SetDataY(i,pdFrequencyDomain[i]);
				if (pdFrequencyDomain[i]>dYMaxVal)
				{
					dYMaxVal = pdFrequencyDomain[i];
					dXMaxVal = i;
				}
			}
			strXMaxVal.Format("%.2lfMHz",dXMaxVal*dDetFrequency - dFrequencySampling/2);
			strYMaxVal.Format("%.2lfdBm",dYMaxVal);
			LabelTemp_MarkerVal1.SetCaption(strXMaxVal);
			LabelTemp_MarkerVal2.SetCaption(strYMaxVal);
		}
		pMainDlg->UpdateGrid();
	}
	TRACE("nCountTest:%d\n",nCountTest);
	FileReadIn.Close();
	DWORD t2 = GetTickCount();
	CString strTimeInfo;
	strTimeInfo.Format("共耗时：%dms",t2-t1);
	AfxMessageBox(strTimeInfo);

	delete[] psTimeDomain;
	delete[] pdTimeDomain;
	delete[] pdFrequencyDomain;

	return TRUE;
}*/


void CPerformanceEvaluationSystemDlg::OnBnClickedBtnBandChannelization()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchPages(PAGE2);
	return;
}

void CPerformanceEvaluationSystemDlg::OnBnClickedBtnParameterMeasurement()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchPages(PAGE3);
	return;
	UpdateAxis();
	UpdateGrid();
}


void CPerformanceEvaluationSystemDlg::OnBnClickedBtnChannelizationDemodulation()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchPages(PAGE4);
	return;
	SwitchPages(PAGE1);

	ThreadSpectrumDisplayParam SpectrumDisplayParam;
	SpectrumDisplayParam.pMainDlg = this;
	SpectrumDisplayParam.piPlotX = &m_iPlotX;

	m_pThreadDemodulation = AfxBeginThread(Thread_Demodulation,&SpectrumDisplayParam);

}


UINT Thread_Demodulation(LPVOID lpParam)
{
	//获取传入线程的参数
	pThreadSpectrumDisplayParam pThreadParam = (pThreadSpectrumDisplayParam)lpParam;
	CPerformanceEvaluationSystemDlg* pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pThreadParam->pMainDlg);
	CiPlotX* iPlotX = static_cast<CiPlotX*>(pThreadParam->piPlotX);

	//将经常需要访问的组件存放到一个临时变量中，可减少访问组件所需要的时间
	CiPlotChannelX ChannelTemp = iPlotX->GetChannel(0);
	CiPlotLabelX LabelTemp_CenterFreq = iPlotX->GetLabels(pMainDlg->lIndexLabel_CenterFreq_Val);
	CiPlotLabelX LabelTemp_Span = iPlotX->GetLabels(pMainDlg->lIndexLabel_Span_Val);
	CiPlotLabelX LabelTemp_Ref = iPlotX->GetLabels(pMainDlg->lIndexLabel_Ref_Val);
	CiPlotLabelX LabelTemp_MarkerVal1 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val1);
	CiPlotLabelX LabelTemp_MarkerVal2 = iPlotX->GetLabels(pMainDlg->lIndexLabel_Marker_Val2);

	ChannelTemp.SetTraceVisible(TRUE);
	ChannelTemp.SetTraceLineStyle(0);
	ChannelTemp.SetColor(YELLOW);

	pMainDlg->UpdateAxis();
	pMainDlg->UpdateGrid();

	const int nNumFFTPoint = pMainDlg->m_Page1.mPage1_nNumFFTPoints;
	TRACE("FFT点数：%d\n",nNumFFTPoint);
	double dFrequencySampling = pMainDlg->m_Page1.mPage1_dBandWidth/pow((double)10,6);
	double dPointReadIn_X = pMainDlg->m_Page1.mPage1_dCenterFreq/pow((double)10,6) - dFrequencySampling/2;
	TRACE("Xmin:%lf Xspan:%lf\n",dPointReadIn_X,dFrequencySampling);
	double dDetFrequency = (double)dFrequencySampling/nNumFFTPoint;

	DWORD t1 = GetTickCount();
	BOOL bIsFirstTimeAddData = TRUE;

	CString strYMaxVal,strXMaxVal;
	double dYMaxVal = 0.0;
	double dXMaxVal = 0.0;
	int nCountTest = 0;
	while(1)
	{
		nCountTest++;
		if (nCountTest==80)
		{
			pMainDlg->OnBnClickedBtnParameterMeasurement();
		}
		if (nCountTest==200)
		{
			break;
		}

		if (bIsFirstTimeAddData)
		{
			bIsFirstTimeAddData = FALSE;
			for (int i=0;i<nNumFFTPoint;i++)
			{
				ChannelTemp.AddXY(dPointReadIn_X,(double)rand()/RAND_MAX*(-80)-10);
				dPointReadIn_X += dDetFrequency;
			}
		}
		else
		{
			//dYMaxVal = rand()/RAND_MAX*(-80)-10;
			//dXMaxVal = 0;
			for (int i=0;i<nNumFFTPoint;i++)
			{
				ChannelTemp.SetDataY(i,(double)rand()/RAND_MAX*(-80)-10);
				//if ((rand()/RAND_MAX*(-80)-10)>dYMaxVal)
				//{
				//	dYMaxVal = rand()/RAND_MAX*(-80)-10;
				//	dXMaxVal = i;
				//}
			}
			//strXMaxVal.Format("%.2lfMHz",dXMaxVal*dDetFrequency - dFrequencySampling/2);
			//strYMaxVal.Format("%.2lfdBm",dYMaxVal);
			//LabelTemp_MarkerVal1.SetCaption(strXMaxVal);
			//LabelTemp_MarkerVal2.SetCaption(strYMaxVal);
		}
		//pMainDlg->UpdateGrid();
	}
	TRACE(_T("nCountTest:%d\n"),nCountTest);

	DWORD t2 = GetTickCount();
	CString strTimeInfo;
	strTimeInfo.Format(_T("共耗时：%dms"),t2-t1);
	AfxMessageBox(strTimeInfo);

	return TRUE;
}


void CPerformanceEvaluationSystemDlg::OnBnClickedBtnRemoteSensingControl()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchPages(PAGE5);
	return;
}

void CPerformanceEvaluationSystemDlg::GetCarrierFreqAndBW(double pdSpectrumData[],int nLenSpectrumData,double dBand,double &dDetCarrierFrq,double &dSignalBW)
{
	double *pdSpectrumDataTemp = new double[nLenSpectrumData];
	//ZeroMemory(pdSpectrumDataTemp,nLenSpectrumData*sizeof(double));
	int nLenMeanFliter = 1201; //均值滤波长度
	memcpy(pdSpectrumDataTemp,pdSpectrumData,nLenMeanFliter/2*sizeof(double));
	memcpy(pdSpectrumDataTemp+nLenSpectrumData-nLenMeanFliter/2,pdSpectrumData+nLenSpectrumData-nLenMeanFliter/2,nLenMeanFliter/2*sizeof(double));
	/*for (int i=0;i<10;i++)
	{
		TRACE("%lf  ",pdSpectrumDataTemp[i]);
	}
	TRACE("\n");
	for (int i=0;i<10;i++)
	{
		TRACE("%lf  ",pdSpectrumDataTemp[nLenSpectrumData-i-1]);
	}*/
	for (int i=nLenMeanFliter/2;i<nLenSpectrumData-nLenMeanFliter/2;i++)
	{
		double sum = 0.0;
		int nj=0;
		for (int j=i-nLenMeanFliter/2;j<i+nLenMeanFliter/2+1;j++)
		{
			//TRACE("%d :%.6lf\n",j+1,pdSpectrumData[j]);
			sum += pdSpectrumData[j];
			nj = j;
			
		}
		pdSpectrumDataTemp[i] = sum/nLenMeanFliter;
		//TRACE("%d:%.15lf\n",i+1,pdSpectrumDataTemp[i]);
	}
	double dSpectrumMaxVal = 0.0;
	int nSpectrumMaxValIndex = 0;
	Max(pdSpectrumDataTemp,nLenSpectrumData,dSpectrumMaxVal,nSpectrumMaxValIndex);
	double dFreqLeft1 = 0.0,dFreqRight1 = 0.0;
	for (int i=nLenMeanFliter/2;i<nLenSpectrumData;i++)
	{
		if (pdSpectrumDataTemp[i]>=dSpectrumMaxVal/10)
		{
			dFreqLeft1 = (double)((i+1)-nLenSpectrumData/2)/nLenSpectrumData*dBand;
			//TRACE("i:%d\n",i);
			//TRACE("%.15lf\n",dFreqLeft1);
			break;
		}
	}
	for (int i=nLenMeanFliter/2;i<nLenSpectrumData;i++)
	{
		if (pdSpectrumDataTemp[nLenSpectrumData-1-i]>=dSpectrumMaxVal/10)
		{
			dFreqRight1 = (double)(nLenSpectrumData-1-i+1-nLenSpectrumData/2)/nLenSpectrumData*dBand;
			//TRACE("i:%d\n",i);
			//TRACE("%.15lf\n",dFreqRight1);
			break;
		}
	}
	dDetCarrierFrq = (dFreqLeft1+dFreqRight1)/2;
	double dFreqLeft2 = 0.0,dFreqRight2 = 0.0;
	for (int i=nLenMeanFliter/2;i<nLenSpectrumData;i++)
	{
		if (pdSpectrumDataTemp[i]>=dSpectrumMaxVal/20)
		{
			dFreqLeft2 = (double)(i+1-nLenSpectrumData/2)/nLenSpectrumData*dBand;
			break;
		}
	}
	for (int i=nLenMeanFliter/2;i<nLenSpectrumData;i++)
	{
		if (pdSpectrumDataTemp[nLenSpectrumData-1-i]>=dSpectrumMaxVal/20)
		{
			dFreqRight2 = (double)(nLenSpectrumData-1-i+1-nLenSpectrumData/2)/nLenSpectrumData*dBand;
			break;
		}
	}
	dSignalBW = abs(dFreqRight2 - dFreqLeft2);
	delete[] pdSpectrumDataTemp;
}

//计算信噪比
void CPerformanceEvaluationSystemDlg::GetSNR(double pdSpectrumData[],int nLenSpectrumData,double dFc,double dFs,double dAnalogBW,double dSymbolRate,double &dSNR)
{
	double M = 2;

	double *pdSpectrumDataTemp = new double[nLenSpectrumData];
	memcpy(pdSpectrumDataTemp,pdSpectrumData,nLenSpectrumData*sizeof(double));
	for (int i=0;i<nLenSpectrumData;i++)
	{
		pdSpectrumDataTemp[i] = pdSpectrumData[i]*pdSpectrumData[i];
	}
	int n1 = 0;	 //模拟带宽左结点
	int n2 = 0;	 //模拟带宽右结点
	int n3 = 0;	 //信号左结点
	int n4 = 0;	 //信号右结点
	dFc = dFc - (long long)(dFc/dFs)*dFs;
	n1 = (int)((dFc-dAnalogBW/2)/dFs*nLenSpectrumData+1);
	n2 = (int)((dFc-dSymbolRate)/dFs*nLenSpectrumData+1);
	n3 = (int)((dFc+dSymbolRate)/dFs*nLenSpectrumData+1);
	n4 = (int)((dFc+dAnalogBW/2)/dFs*nLenSpectrumData+1);

	double dPowerNoise = 0.0,dPowerSignal = 0.0;
	for (int i=n1;i<=n2;i++)
	{
		dPowerNoise += pdSpectrumDataTemp[i-1];
	}
	for (int i=n3;i<=n4;i++)
	{
		dPowerNoise += pdSpectrumDataTemp[i-1];
	}
	dPowerNoise = dPowerNoise/(n4-n3+n2-n1)*(n3-n2);

	for (int i=n2;i<=n3;i++)
	{
		dPowerSignal += pdSpectrumDataTemp[i-1];
	}
	dPowerSignal = dPowerSignal - dPowerNoise;
	double EsNo = 10*log10(dPowerSignal/dPowerNoise*2);
	double EbNo = EsNo - 10*log10(M);
	double NoiseDensity = dPowerNoise/(n3-n2);

}

void CPerformanceEvaluationSystemDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	pGEDevice->Destroy();
	if (pGEDevice!=NULL){
		delete pGEDevice;
		pGEDevice = NULL;
	}
	/*if (NULL != hDll)
	{
		FreeLibrary(hDll);
	}*/

	//关闭Matlab引擎
	//engEvalString(ep,"close;");
	//engClose(ep);
}

void CPerformanceEvaluationSystemDlg::OnCbnSelchangeComboWorkmode()
{
	// TODO: 在此添加控件通知处理程序代码
	::SendMessage(m_Page4.GetSafeHwnd(), WM_CHANGEBUTTON, (WPARAM)m_WorkMode.GetCurSel(), 0);
}

void CPerformanceEvaluationSystemDlg::OnCbnSelchangeComboSignaltype()
{
	// TODO: 在此添加控件通知处理程序代码
	if (0 == m_SignalType.GetCurSel())
	{
		GetDlgItem(IDC_BTN_BAND_SPECTRUM_SCANNING)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_BAND_CHANNELIZATION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_PARAMETER_MEASUREMENT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CHANNELIZATION_DEMODULATION)->EnableWindow(TRUE);
	} 
	else
	{
		GetDlgItem(IDC_BTN_BAND_SPECTRUM_SCANNING)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_BAND_CHANNELIZATION)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_PARAMETER_MEASUREMENT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_CHANNELIZATION_DEMODULATION)->EnableWindow(FALSE);
	}
}

void CPerformanceEvaluationSystemDlg::OnBnClickedButtonTest1()
{
	// TODO: 在此添加控件通知处理程序代码
	/*unsigned short Sum_temp = 0;
	unsigned char *pData = new unsigned char[1024];
	ZeroMemory(pData,1024);
	for (int i=0;i<1023;i++)
	{
		pData[i] = i*3;
	}
	pData[1023] = GetChecksum(pData,1023);
	TRACE("CheckSum:0x%x\n",pData[1023]);
	pData[0] = 157;
	TRACE("Result:0x%x\n",GetChecksum(pData,1024));
	delete[] pData;*/

	//InitialiPlotX_Page1();
	CString strPath = _T("C:\\接收文件\\文件\\");
	CreateDir(strPath);
}


void CPerformanceEvaluationSystemDlg::OnBnClickedButtonTest2()
{
	// TODO: 在此添加控件通知处理程序代码
	//InitialiPlotX_Page4(2);
	//void CreateDir(CString str);							//创建路径
	/*CString strPath = "C:\\文件1\\文件2\\文件3\\";
	CreateDir(strPath);*/
	//CString GetExePath();									//获取exe所在路径
	/*CString strExePath = GetExePath();
	TRACE("%s\n",strExePath);*/
	//CString Data2Unit(double dDataVal,int nUnitType);		//根据数字长度转换成不同单位的数值
	//void ClearFiles(CString Path_DatFile);				//清除文件
	/*CString strFilesPath = "C:\\文件1\\*.dat";
	ClearFiles(strFilesPath);*/
	//void DisplayRunningInfo(CString strInfo);				//输出系统运行信息
	/*CString str = "Hello world";
	DisplayRunningInfo(str);
	str = "nice day!";
	DisplayRunningInfo(str);*/
}









//////////////////////////////////////////////////////////////////////////
//常用功能函数
//自动创建路径
void CPerformanceEvaluationSystemDlg::CreateDir(CString str)
{
	int pos = str.Find('\\');
	while(pos>0)
	{
		pos = str.Find('\\',pos+1);
		if (pos>0)
		{
			CString strSub = str.Left(pos);
			CreateDirectory(strSub,NULL);
		}
	}
}

//获取exe文件所在路径
CString CPerformanceEvaluationSystemDlg::GetExePath()
{
	char cExeDir[MAX_PATH];
	GetModuleFileNameA(NULL,cExeDir,MAX_PATH);
	CString strExeDir;
	strExeDir.Format(_T("%s"),cExeDir);
	TRACE(_T("%s\n"),strExeDir);
	CString strTopFileDir = strExeDir.Left(strExeDir.ReverseFind('\\')+1);
	return strTopFileDir;
}

//清除文件
void CPerformanceEvaluationSystemDlg::ClearFiles(CString Path_DatFile)
{
	//CFileFind find1;

	BOOL ret=find1.FindFile(Path_DatFile);
	while (ret)
	{
		CString filename(_T(""));
		filename.Empty();
		ret=find1.FindNextFile();
		filename=find1.GetFileName();
		if (!find1.IsDots() && !find1.IsDirectory())
		{
			CString Path_tp;
			Path_tp=find1.GetFilePath();
			DeleteFile(Path_tp);
			//TRACE("找到文件：%s\n",Path_tp);
		}
		else if (find1.IsDots())
		{
			//TRACE("找到文件：%s\n",filename);
			//continue;
		}
		else if (find1.IsDirectory())
		{
			//TRACE("是文件夹（目录）%s\n",filename);
		}
		else
		{
			//TRACE("未知情况\n");
		}
	}
}

void CPerformanceEvaluationSystemDlg::DisplayRunningInfo(CString strInfo)
{
	CString strTemp = strRunningInfo + strInfo + _T("\r\n");
	strRunningInfo = strTemp;
	GetDlgItem(IDC_EDIT_SYS_RUN_INFO)->SetWindowText(strRunningInfo);
}

void CPerformanceEvaluationSystemDlg::Test()
{
	MessageBox(_T("Test"));
}


LRESULT CPerformanceEvaluationSystemDlg::PlotData(WPARAM wParam, LPARAM lParam)
{
	//m_iPlotX.GetXAxis(0).SetMin(0);
	//m_iPlotX.GetXAxis(0).SetSpan(10);
	SetTimer(1, 100, NULL);
	return 0;
}

void CPerformanceEvaluationSystemDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ÔÚ´ËÌí¼ÓÏûÏ¢´¦Àí³ÌÐò´úÂëºÍ/»òµ÷ÓÃÄ¬ÈÏÖµ
	int len_per_display_byte = 10000;              //一次显示的数据的字节数
	int len_per_display;                          //一次显示的数据个数
	CFile FileData;
	double sum = 0;

	CString strFileDataDir = _T("C:\\Recv\\RecvData.dat");
	FileData.Open(strFileDataDir,CFile::modeRead | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone);
	if (FileData.GetLength() <= position_file_plot + len_per_display_byte)
	{
		return;
	}
	else
	{
		FileData.Seek(position_file_plot, CFile::begin);
		char *pBufFileReadIn = new char[len_per_display_byte];
		FileData.Read(pBufFileReadIn,len_per_display_byte);
		int uploadtype = m_Page4.m_UploadDataType.GetCurSel();
		int maxdata = 0, mindata = 0;
		if (uploadtype == 5)           //中频数据为2个字节，数据长度应除以2
		{
			len_per_display = len_per_display_byte / 2;
		}
		else if (uploadtype == 1)      //锁相环累计误差数据为4个字节，数据长度应除以4
		{
			len_per_display = len_per_display_byte / 4;
		}
		else
		{
			len_per_display = len_per_display_byte;
		}

		double dAxisXDelt = 0.001;
		CiPlotChannelX ChannelX = m_iPlotX.GetChannel(0);
		CiPlotAxisX XAxis = m_iPlotX.GetXAxis(0);
		long origindata;
		for (int i=0;i< len_per_display;i++)
		{
			if (uploadtype == 5)
			{
				origindata = *(pBufFileReadIn + 2 * i) * 256 + *(pBufFileReadIn + 2 * i);
				ChannelX.AddXY(CurrentX, origindata);
			}
			else if (uploadtype == 1)
			{
				origindata = *(long *)(pBufFileReadIn + 4 * i);
				ChannelX.AddXY(CurrentX, origindata);
			}
			else
			{
				origindata = (double)*(pBufFileReadIn + i);
				ChannelX.AddXY(CurrentX, origindata);
			}
			CurrentX += dAxisXDelt;
			sum += origindata;
			if (origindata > maxdata)
			{
				maxdata = origindata;
			}
			if (origindata < mindata)
			{
				mindata = origindata;
			}
		}
		delete[] pBufFileReadIn;
		position_file_plot += len_per_display_byte;
		double cur_average = (double)sum / len_per_display;                     //本次显示的数据的平均值
		average = average * count_average / (count_average + len_per_display) + cur_average * len_per_display / (count_average + len_per_display);            //由之前总的平均值和本次平均值算出新的总的平均值
		count_average += len_per_display;
		
		int differ = maxdata - mindata;
		shock_average = shock_average * count_shock / (count_shock + 1) + differ / (count_shock + 1);
		count_shock++;

		CString str_average;
		str_average.Format(_T("%f"), average);
		m_Page4.GetDlgItem(IDC_AVERAGE)->SetWindowText(str_average);
		str_average.Format(_T("%f"), shock_average);
		m_Page4.GetDlgItem(IDC_SHOCKAVERAGE)->SetWindowText(str_average);

		//m_iPlotX.GetYAxis(0).SetMin(m_iPlotX.GetChannel(0).GetYMin());                //每次更新显示数据后，设置坐标轴的范围，貌似不用不用设置，空间可以自己调整
		//m_iPlotX.GetYAxis(0).SetSpan(m_iPlotX.GetChannel(0).GetYMax() - m_iPlotX.GetChannel(0).GetYMin());
	}
	FileData.Close();

	CDialogEx::OnTimer(nIDEvent);
}
