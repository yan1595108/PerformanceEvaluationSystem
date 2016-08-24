#pragma once
// PerformanceEvaluationSystemDlg.h : ͷ�ļ�
//
#include "iplotx.h"
#include "engine.h"
//#include "mclmcrrt.h"
//#include "mclcppclass.h"

#include "math.h"
#include "FFT_DSP.h"
#include "CommonFunction.h"

//iPlotX�ؼ����ͷ�ļ�
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

//Page4-�ŵ������ģʽ
typedef enum Page4_WaveDisplayMode{
	WaveDispMode_DemoduData	= 0,	//�������
	WaveDispMode_PLLAccumEr	= 1,	//���໷�ۼ����
	WaveDispMode_TimLoopEr	= 2,	//��ʱ��·���
	WaveDispMode_Variance	= 3,	//����
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
	//�ṹ���캯�����û�������ʼ��
	_ThreadSpectrumDisplayParam()
	{
		pMainDlg = NULL;
		pGEDevice = NULL;
		piPlotX = NULL;
	};
}ThreadSpectrumDisplayParam,*pThreadSpectrumDisplayParam;

typedef struct _PAGE4_PARAM{
	int		nRecvSize;					//���հ���
	int		nFramesPerPackage;			//ÿ��������֡��
	double	dCarrier;					//�ز�
	int		nModulateMode;				//���Ʒ�ʽ
	double	dSymbolRateVal;				//��������
	int		nSymbolRateUnit;			//�������ʵ�λ
	int		nUploadDataType;			//�ϴ���������
	int		m_C1;						//�˲�ϵ��C1
	int		m_C2;						//�˲�ϵ��C2
}PAGE4_PARAM,*PPAGE4_PARAM;


// CPerformanceEvaluationSystemDlg �Ի���
class CPerformanceEvaluationSystemDlg : public CDialogEx
{
// ����
public:
	CPerformanceEvaluationSystemDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CPerformanceEvaluationSystemDlg();

// �Ի�������
	enum { IDD = IDD_PERFORMANCEEVALUATIONSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CComboBox m_WorkMode;				//����ģʽ���Ͽ�
	CComboBox m_SignalType;				//�ź����͸��Ͽ�
	Engine *en;							//Matlab����
	CiPlotX	m_iPlotX;					//��ͼ�ؼ�
	long FilePositionPlot;				//��ͼ�������ļ��л�ȡ���ݵ��ļ�λ��
	double CurrentX;					//��ͼ������ͼʹ�õ�X������

    //ǧ������ر���
	//fstream RunInfo;					//ǧ�����ӿ�����״̬��¼�ļ�
	CGigabitEthernetDevice* pGEDevice;	//ǧ�����ӿ��豸

//�ؼ���ر���
public:
	//�ؼ�label���
	long lIndexLabel_CenterFreq;		//�ؼ���ǩ-����Ƶ��Name
	long lIndexLabel_CenterFreq_Val;	//�ؼ���ǩ-����Ƶ��Value
	long lIndexLabel_Span;				//�ؼ���ǩ-Ƶ�ʷ�ΧName
	long lIndexLabel_Span_Val;			//�ؼ���ǩ-Ƶ�ʷ�ΧValue
	long lIndexLabel_Ref;				//�ؼ���ǩ-�ο���ƽName
	long lIndexLabel_Ref_Val;			//�ؼ���ǩ-�ο���ƽValue
	long lIndexLabel_Marker;			//�ؼ���ǩ-���Name
	long lIndexLabel_Marker_Val1;		//�ؼ���ǩ-���Value1
	long lIndexLabel_Marker_Val2;		//�ؼ���ǩ-���Value2

public:
	//��ͬģʽ������ҳ��
	CPage1 m_Page1;						//Ƶ��Ƶ��ɨ��ģʽ
	CPage2 m_Page2;						//Ƶ���ŵ���ģʽ
	CPage3 m_Page3;						//��������ģʽ
	CPage4 m_Page4;						//�ŵ������ģʽ
	CPage5 m_Page5;						//ң��ң��

private:
	CWinThread *m_pThreadSpectrumDisplay;
	CWinThread *m_pThreadDemodulation;
	//Engine *en;

//��̬���ӿ����
private:
	HINSTANCE hDll;


//��ʼ������
public:
	void InitialControls();				//������ؼ���ʼ��
	void InitialiPlotX();				//��ͼ�ؼ���ʼ��
	void InitialMatlabEngine();			//Matlab�����ʼ��
	void InitialDLLs();					//��̬���ʼ��

//��ͼ�ؼ����ƺ���
public:
	void UpdateAxis();					//����������
	void UpdateGrid();					//��������
	void UpdateLabels();				//���±�ǩҳ
	
//��ͬģʽ����ҳ����ƺ���
public:
	void InitialPages();
	void SwitchPages(int nPageIndex);

//Ƶ�׷���
public:
	void GetCarrierFreqAndBW(double pdSpectrumData[],int nLenSpectrumData,double dBand,double &dCarrierFrq,double &dSignalBW);
	void GetSNR(double pdSpectrumData[],int nLenSpectrumData,double dFc,double dFs,double dAnalogBW,double dSymbolRate,double &dSNR);

//����ļ�����ClearFiles��ʹ�ã�֮������Ϊ���Ա����Ϊ��ΪClearFiles�����е���ʱ����ż���ᵼ��dbgheap.c�ļ��е�CheckBytes��������ԭ���꣩
	CFileFind find1;
	

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	//Page1��غ���
	void InitialiPlotX_Page1();

	//////////////////////////////////////////////////////////////////////////
	//Page4��غ���
	void InitialiPlotX_Page4(int nInitMode);
	afx_msg void OnBnClickedButtonTest2();

	//////////////////////////////////////////////////////////////////////////
	//���湦�ܺ���
	void	CreateDir(CString str);							//����·��
	CString	GetExePath();									//��ȡexe����·��
	CString	Data2Unit(double dDataVal,int nUnitType);		//�������ֳ���ת���ɲ�ͬ��λ����ֵ
	void	ClearFiles(CString Path_DatFile);				//����ļ�
	void	DisplayRunningInfo(CString strInfo);			//���ϵͳ������Ϣ
	void	Test();											//���԰�ť
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};


UINT Thread_SpectrumDisplay(LPVOID lpParam);
UINT Thread_Demodulation(LPVOID lpParam);