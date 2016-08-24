#pragma once
// PerformanceEvaluationSystemDlg.h : 头文件
//
#include "iplotx.h"
#include "engine.h"
//#include "mclmcrrt.h"
//#include "mclcppclass.h"

#include "math.h"
#include "FFT_DSP.h"
#include "CommonFunction.h"

//iPlotX控件相关头文件
#include "iPlotChannelX.h"
#include "iPlotAxisX.h"
#include "iplotlegendx.h"
#include "iplottoolbarx.h"
#include "iplotdataviewx.h"
#include "iplotannotationx.h"
#include "iplotlabelx.h"
#include "iplotlimitx.h"
#include "iplotdatacursorx.h"
#include "font.h"

#include "Page1.h"
#include "Page2.h"
#include "Page3.h"
#include "Page4.h"
#include "Page5.h"

#include "GigabitEthernetDevice.h"

#include "afxwin.h"
#pragma comment(lib, "libmat.lib")
#pragma comment(lib, "libmx.lib")
#pragma comment(lib, "libmex.lib")
#pragma comment(lib, "libeng.lib")

#define WM_PLOTDATA (WM_USER + 1)
#define WM_CHANGEBUTTON (WM_USER + 2)

typedef enum COLOR{
	BLACK = 0,

	LIGHT_RED = 16711935,
	RED = 255,
	DARK_RED = 192,
	
	LIGHT_YELLOW = 32896,
	YELLOW = 65535,
	DARK_YELLOW = 49344,

	LIGHT_BLUE = 12582912,
	BLUE = 16711680,
	DARK_BLUE = 8388608,

	LIGHT_GREEN = 49152,
	GREEN = 65280,
	DARK_GREEN = 32768,

	GRAY = 8421504,

	WHITE = 16777215,
};

typedef enum AlignmentHorizontal{
	AlignCenter = 0,
	AlignhLeft = 1,
	AlignhRight = 2,
};

//Page4-信道化解调模式
typedef enum Page4_WaveDisplayMode{
	WaveDispMode_DemoduData	= 0,	//解调数据
	WaveDispMode_PLLAccumEr	= 1,	//锁相环累计误差
	WaveDispMode_TimLoopEr	= 2,	//定时环路误差
	WaveDispMode_Variance	= 3,	//方差
};

typedef enum PAGES
{
	PAGE1 = 0,
	PAGE2 = 1,
	PAGE3 = 2,
	PAGE4 = 3,
	PAGE5 = 4,
};

typedef enum UNIT
{
	UNIT_FREQUENCY = 0,
	UNIT_AMPLITUDE = 1,
	UNIT_TIME = 2,
};

typedef struct _ThreadSpectrumDisplayParam
{
	LPVOID pMainDlg;
	LPVOID pGEDevice;
	LPVOID piPlotX;
	//结构构造函数，用户变量初始化
	_ThreadSpectrumDisplayParam()
	{
		pMainDlg = NULL;
		pGEDevice = NULL;
		piPlotX = NULL;
	};
}ThreadSpectrumDisplayParam,*pThreadSpectrumDisplayParam;

typedef struct _PAGE4_PARAM{
	int		nRecvSize;					//接收包数
	int		nFramesPerPackage;			//每包包含的帧数
	double	dCarrier;					//载波
	int		nModulateMode;				//调制方式
	double	dSymbolRateVal;				//符号速率
	int		nSymbolRateUnit;			//符号速率单位
	int		nUploadDataType;			//上传数据类型
	int		m_C1;						//滤波系数C1
	int		m_C2;						//滤波系数C2
}PAGE4_PARAM,*PPAGE4_PARAM;


// CPerformanceEvaluationSystemDlg 对话框
class CPerformanceEvaluationSystemDlg : public CDialogEx
{
// 构造
public:
	CPerformanceEvaluationSystemDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CPerformanceEvaluationSystemDlg();

// 对话框数据
	enum { IDD = IDD_PERFORMANCEEVALUATIONSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CComboBox m_WorkMode;				//工作模式复合框
	CComboBox m_SignalType;				//信号类型复合框
	Engine *en;							//Matlab引擎
	CiPlotX	m_iPlotX;					//绘图控件
	long FilePositionPlot;				//绘图函数从文件中获取数据的文件位置
	double CurrentX;					//绘图函数绘图使用的X轴坐标

    //千兆网相关变量
	//fstream RunInfo;					//千兆网接口运行状态记录文件
	CGigabitEthernetDevice* pGEDevice;	//千兆网接口设备

//控件相关变量
public:
	//控件label相关
	long lIndexLabel_CenterFreq;		//控件标签-中心频率Name
	long lIndexLabel_CenterFreq_Val;	//控件标签-中心频率Value
	long lIndexLabel_Span;				//控件标签-频率范围Name
	long lIndexLabel_Span_Val;			//控件标签-频率范围Value
	long lIndexLabel_Ref;				//控件标签-参考电平Name
	long lIndexLabel_Ref_Val;			//控件标签-参考电平Value
	long lIndexLabel_Marker;			//控件标签-标记Name
	long lIndexLabel_Marker_Val1;		//控件标签-标记Value1
	long lIndexLabel_Marker_Val2;		//控件标签-标记Value2

public:
	//不同模式的设置页面
	CPage1 m_Page1;						//频段频谱扫描模式
	CPage2 m_Page2;						//频段信道化模式
	CPage3 m_Page3;						//参数测量模式
	CPage4 m_Page4;						//信道化解调模式
	CPage5 m_Page5;						//遥测遥控

private:
	CWinThread *m_pThreadSpectrumDisplay;
	CWinThread *m_pThreadDemodulation;
	//Engine *en;

//动态链接库相关
private:
	HINSTANCE hDll;


//初始化函数
public:
	void InitialControls();				//主界面控件初始化
	void InitialiPlotX();				//绘图控件初始化
	void InitialMatlabEngine();			//Matlab引擎初始化
	void InitialDLLs();					//动态库初始化

//绘图控件控制函数
public:
	void UpdateAxis();					//更新坐标轴
	void UpdateGrid();					//更新网格
	void UpdateLabels();				//更新标签页
	
//不同模式设置页面控制函数
public:
	void InitialPages();
	void SwitchPages(int nPageIndex);

//频谱分析
public:
	void GetCarrierFreqAndBW(double pdSpectrumData[],int nLenSpectrumData,double dBand,double &dCarrierFrq,double &dSignalBW);
	void GetSNR(double pdSpectrumData[],int nLenSpectrumData,double dFc,double dFs,double dAnalogBW,double dSymbolRate,double &dSNR);

//清除文件函数ClearFiles中使用（之所以作为类成员是因为作为ClearFiles函数中的临时变量偶尔会导致dbgheap.c文件中的CheckBytes函数出错，原因不详）
	CFileFind find1;
	

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBandSpectrumScanning();
	afx_msg void OnBnClickedBtnBandChannelization();
	afx_msg void OnBnClickedBtnParameterMeasurement();
	afx_msg void OnBnClickedBtnChannelizationDemodulation();
	afx_msg void OnBnClickedBtnRemoteSensingControl();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeComboSignaltype();
	afx_msg void OnCbnSelchangeComboWorkmode();
	afx_msg void OnBnClickedButtonTest1();

	//////////////////////////////////////////////////////////////////////////
	//Page1相关函数
	void InitialiPlotX_Page1();

	//////////////////////////////////////////////////////////////////////////
	//Page4相关函数
	void InitialiPlotX_Page4(int nInitMode);
	afx_msg void OnBnClickedButtonTest2();

	//////////////////////////////////////////////////////////////////////////
	//常规功能函数
	void	CreateDir(CString str);							//创建路径
	CString	GetExePath();									//获取exe所在路径
	CString	Data2Unit(double dDataVal,int nUnitType);		//根据数字长度转换成不同单位的数值
	void	ClearFiles(CString Path_DatFile);				//清除文件
	void	DisplayRunningInfo(CString strInfo);			//输出系统运行信息
	void	Test();											//测试按钮
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


UINT Thread_SpectrumDisplay(LPVOID lpParam);
UINT Thread_Demodulation(LPVOID lpParam);