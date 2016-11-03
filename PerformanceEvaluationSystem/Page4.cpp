// Page4.cpp : 实现文件
//

#include "stdafx.h"
#include "PerformanceEvaluationSystem.h"
#include "PerformanceEvaluationSystemDlg.h"
#include "Page4.h"
#include "afxdialogex.h"

DWORD WINAPI ThreadRecv(LPVOID lpParam);
DWORD WINAPI ThreadStartPlot(LPVOID lpParam);

extern CString strRunningInfo;

// CPage4 对话框

IMPLEMENT_DYNAMIC(CPage4, CPageBase)

CPage4::CPage4(CWnd* pParent /*=NULL*/)
	: CPageBase(CPage4::IDD, pParent)
	, pTempMainDlg(NULL),pGEDevice(NULL)
	, bThreadStopRecv(TRUE),bThreadStopPlot(TRUE)
	, hThreadRecv(INVALID_HANDLE_VALUE),hThreadPlot(INVALID_HANDLE_VALUE)
	, dwThreadRecvID(0),dwThreadPlotID(0)
	, m_nRecvSize(1000000),m_nFramesPerPackage(255)
	, m_Carrier(70.0),m_SymbolRateVal(100),m_C1(12),m_C2(20)
	, m_c1delta(0)
	, m_c2delta(0)
	, m_offsetvalue(0)
	, m_modulatedeep(0)
{

}

CPage4::~CPage4()
{

}

void CPage4::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RECVSIZE, m_nRecvSize);
	DDX_Text(pDX, IDC_EDIT_PACKAGESIZE, m_nFramesPerPackage);
	DDX_Text(pDX,IDC_EDIT_CARRIER,m_Carrier);
	DDX_Control(pDX,IDC_COMBO_MODULATE_MODE,m_ModulateMode);
	DDX_Text(pDX,IDC_EDIT_SYMBOL_RATE_VAL,m_SymbolRateVal);
	DDX_Control(pDX,IDC_COMBO_SYMBOL_RATE_UNIT,m_SymbolRateUnit);
	DDX_Control(pDX,IDC_COMBO_UPLOAD_DATA_TYPE,m_UploadDataType);
	DDX_Text(pDX,IDC_EDIT_C1,m_C1);
	DDX_Text(pDX,IDC_EDIT_C2,m_C2);
	DDV_MinMaxInt(pDX, m_nFramesPerPackage, 1, 255);
	DDX_Text(pDX, IDC_EDIT_C1DELTA, m_c1delta);
	DDX_Text(pDX, IDC_EDIT_C2DELTA, m_c2delta);
	DDX_Text(pDX, IDC_OFFSETVALUE, m_offsetvalue);
	DDX_Text(pDX, IDC_MODULATEDEEP, m_modulatedeep);
}


BEGIN_MESSAGE_MAP(CPage4, CPageBase)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_CONFIGURE, &CPage4::OnBnClickedButtonConfigure)
	ON_BN_CLICKED(IDC_BUTTON_RECVDATA, &CPage4::OnBnClickedButtonRecvdata)
	ON_BN_CLICKED(IDC_BUTTON_COMPAR, &CPage4::OnBnClickedButtonCompar)
	ON_BN_CLICKED(IDC_BUTTON_STOP_RECV, &CPage4::OnBnClickedButtonStopRecv)
	ON_BN_CLICKED(IDC_BUTTON_MODULATOR, &CPage4::OnBnClickedButtonModulator)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CPage4::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_CALLSIMULINK, &CPage4::OnBnClickedCallsimulink)
	ON_EN_CHANGE(IDC_EDIT_RECVSIZE, &CPage4::OnEnChangeEditRecvsize)
	ON_BN_CLICKED(IDC_BEGINTRACK, &CPage4::OnBnClickedBegintrack)
	ON_BN_CLICKED(IDC_ENDTRACK, &CPage4::OnBnClickedEndtrack)
	ON_BN_CLICKED(IDC_BEGINOFFSET, &CPage4::OnBnClickedBeginoffset)
	ON_BN_CLICKED(IDC_ENDOFFSET, &CPage4::OnBnClickedEndoffset)
	ON_MESSAGE(WM_CHANGEBUTTON, OnButtonChanged)
END_MESSAGE_MAP()


// CPage4 消息处理程序


BOOL CPage4::Create(UINT nIDTemplate, CWnd* pParentWnd)
{
	// TODO: 在此添加专用代码和/或调用基类
	pTempMainDlg = pParentWnd;
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);
	pGEDevice = pMainDlg->pGEDevice;
	return CPageBase::Create(nIDTemplate, pParentWnd);
}

//信道化解调模式-配置
void CPage4::OnBnClickedButtonConfigure()
{
	// TODO: 在此添加控件通知处理程序代码
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);

	UpdateData(TRUE);
	CString info;         //用于显示相关信息
	strRunningInfo.Empty();       //清空相关信息的显示
	int Mode=0;//调制方式
	Mode =m_ModulateMode.GetCurSel();
	char modulatestyle[10] = "BPSK";
	switch (Mode)
	{
	case 0:
		strcpy_s(modulatestyle, "BPSK");
		break;
	case 1:
		strcpy_s(modulatestyle, "QPSK");
		break;
	case 2:
		strcpy_s(modulatestyle, "16QAM");
		break;
	case 3:
		strcpy_s(modulatestyle, "32QAM");
		break;
	case 4:
		strcpy_s(modulatestyle, "Sin");
		break;
	case 5:
		strcpy_s(modulatestyle, "OQPSK");
		break;
	case 6:
		strcpy_s(modulatestyle, "GMSK");
		break;
	case 7:
		strcpy_s(modulatestyle, "2FSK");
		break;
	case 8:
		strcpy_s(modulatestyle, "FM");
		break;
	case 9:
		strcpy_s(modulatestyle, "AM");
		break;
	default:
		break;
	}
	TRACE("ModulateMode = %s\n",modulatestyle);

	int Hz=0;
	double symbolrate=0.0;				//符号速率
	double fo = m_Carrier*1000000;	//中心频率
	TRACE("中心频率fo = %f\n",fo);
	int cic=1;						//cic 下抽因子
	double fc=0;					//下抽前的中心频率
	double fsample=0;				// 数字采样率
	double fsymbol_32 = 0;          //符号速率的32倍
	double adsample=0;				// AD采样率
	double sample_flimit=100*1000000;//AD DA的最低采样时钟
	double analog_filter=0;
	int up_s=1;
	Hz = m_SymbolRateUnit.GetCurSel();
	if(Hz==0)
	{
		symbolrate=m_SymbolRateVal*1000000.0;
	}
	else if (Hz==1)
	{
		symbolrate=m_SymbolRateVal*1000.0;
	}
	else
	{
		symbolrate=m_SymbolRateVal*1.0;
	}

	if(symbolrate>400*1000)
	{
		analog_filter=3000000;//3MHz
	}
	else if (symbolrate>50*1000)
	{
		analog_filter=1000000;//1MHz
	}
	else
	{
		analog_filter=200000;//200KHz
	}
	TRACE("符号速率symbolrate = %d\n", symbolrate);
	//***************依次上抽 使得信号在规定模拟带宽内 只保留主瓣******************************************
	int struct_coe;
	switch (Mode)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
		struct_coe = 16;
		break;
	case 6:
	case 7:
		struct_coe = 32;
		break;
	default:
		struct_coe = 0;
		break;
	}

	while(1)
	{
		fsample=symbolrate*struct_coe*cic;
		
		fc=fo-int((fo/fsample))*fsample;
		
		if((fc-analog_filter/2>0&&fc+analog_filter/2<fsample/2)||(fc-analog_filter/2>fsample/2&&fc+analog_filter/2<fsample))
		{
			break;
			if(cic>10000)
			{
				break;
				AfxMessageBox(_T("error"));
			}
		}
		else
			cic++;
	}
	
	int HB[3]={1,1,1};
	if(cic%2==0)
	{
		cic=cic/2;
		HB[0]=2;
	}
	else if(cic>1)
	{
		cic=cic+1;
		cic=cic/2;
		HB[0]=2;
	}
	if(cic%2==0)
	{
		cic=cic/2;
		HB[1]=2;
	}
	else if(cic>1)
	{
		cic=cic+1;
		cic=cic/2;
		HB[1]=2;
	}
	if(cic%2==0)
	{
		cic=cic/2;
		HB[2]=2;
	}
	else if(cic>1)
	{
		cic=cic+1;
		cic=cic/2;
		HB[2]=2;
	}

	while (cic > 20)
	{
		cic = cic / 2;
	}
	fsample=symbolrate*struct_coe*cic*HB[0]*HB[1]*HB[2];
	fsymbol_32 = symbolrate*struct_coe;//GMSK和2FSK的struct_coe都是32
	TRACE("符号速率的32倍 = %d\n", fsymbol_32);
	TRACE("直抽后采样率 = %f\n", fsample);
	TRACE("欠采样等效载波 = %f\n", fc);
	TRACE("下抽系数cic = %d\n", cic);
	TRACE("HB[0] = %d, HB[1] = %d, HB[2] = %d\n", HB[0], HB[1], HB[2]);

	//************************************确定NCO 频率控制字*****************************************************************
	unsigned long pinlvkongzhizi_down=0;//第一级数字下变频
	pinlvkongzhizi_down=(fo)*4294967296.0/fsample;
	TRACE("pinlvkongzhizi_down = %u\n", pinlvkongzhizi_down);
	unsigned long pinlvkongzhizi_up=0;//数字上变频
	pinlvkongzhizi_up=(1.0/5)*4294967296.0; //因为使用平方环，为保证频率字应当小于1/2
	TRACE("pinlvkongzhizi_up = %ld\n", pinlvkongzhizi_up);

	unsigned long pinlvkongzhizi1=0;//载波恢复频率字   乘以2是因为平方环
	unsigned long pinlvkongzhizi2=0;
	switch (Mode)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
		pinlvkongzhizi1 = fo * 4294967296.0 / fsample;
		pinlvkongzhizi2=(fsample*1.0/5-symbolrate/2)*2*4294967296.0/fsample;
		break;
	case 6:
		pinlvkongzhizi1 = (fsymbol_32 * 1.0 / 5 + symbolrate / 2) * 2 * 4294967296.0 / fsymbol_32;
		pinlvkongzhizi2 = (fsymbol_32 * 1.0 / 5 - symbolrate / 2) * 2 * 4294967296.0 / fsymbol_32;
		break;
	case 7:
		pinlvkongzhizi1 = (fsymbol_32 * 1.0 / 5 + m_modulatedeep * symbolrate / 2) * 4294967296.0 / fsymbol_32;
		pinlvkongzhizi2 = (fsymbol_32 * 1.0 / 5 - m_modulatedeep * symbolrate / 2) * 4294967296.0 / fsymbol_32;
		break;
	default:
		pinlvkongzhizi1 = 0;
		pinlvkongzhizi2=(fsample*1.0/5-symbolrate/2)*2*4294967296.0/fsample;
		break;
	}
	TRACE("pinlvkongzhizi1=%ld\n", pinlvkongzhizi1);
	info.Format("pinlvkongzhizi1=%ld", pinlvkongzhizi1);
	pMainDlg->DisplayRunningInfo(info);
	TRACE("pinlvkongzhizi2=%ld\n", pinlvkongzhizi2);
	info.Format("pinlvkongzhizi2=%ld", pinlvkongzhizi2);
	pMainDlg->DisplayRunningInfo(info);
	//***********************************************************************************************************************
	for (up_s=2;up_s<1000;up_s++)
	{
		adsample=fsample*up_s;		//up_s从2开始是保证直抽因子必须大于等于2，FPGA使用的是双通道滤波器
		if(adsample>=sample_flimit)
			break;
	}
	TRACE("adsample=%f\n", adsample);
	//************************************确定ADF4351 因子*****************************************************************
	int R_4351=0;
	int INT_4351=0;
	int FRAC_4351=0;
	double adf4351_out=0;
	adf4351_out=adsample/1000000;
	while(adf4351_out<2200.0)
	{
		adf4351_out=adf4351_out*2;
		R_4351++;
	}
	adf4351_out=adf4351_out*10.0;
	INT_4351 =adf4351_out;
	FRAC_4351 =(adf4351_out-INT_4351)*1000.0;
	TRACE("R_4351=%d\n",R_4351);
	TRACE("INT_4351=%d\n",INT_4351);
	TRACE("FRAC_4351=%d\n",FRAC_4351);

	//************************************发送命令*****************************************************************
	//配置预处理板
	int nDemodulateCmdSize =20;
	char *szDemodulateCmd=new char[nDemodulateCmdSize];
	szDemodulateCmd[0]=0x17;
	szDemodulateCmd[1]=0x57;
	szDemodulateCmd[2]=0x90;
	szDemodulateCmd[3]=0xeb;
	szDemodulateCmd[4]=0x82;//配置及ADF4351
	szDemodulateCmd[5]=0x00;
	szDemodulateCmd[6]=0x0f;
	szDemodulateCmd[7]=INT_4351>>8;
	szDemodulateCmd[8]=INT_4351;
	szDemodulateCmd[9]=FRAC_4351>>8;
	szDemodulateCmd[10]=FRAC_4351;
	szDemodulateCmd[11]=R_4351>>8;
	szDemodulateCmd[12]=R_4351;
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}

	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);
	//************************************************************************************************//
	//数字解调板 FPGA 配置
	szDemodulateCmd[4]=0x01;//模式配置
	szDemodulateCmd[7]=0x00;
	szDemodulateCmd[8]=Mode;
	szDemodulateCmd[9]=up_s>>8;
	szDemodulateCmd[10]=up_s;
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=0x00;
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x02;//滤波器
	szDemodulateCmd[7]=cic>>8;
	szDemodulateCmd[8]=cic;
	szDemodulateCmd[9]=0x00;
	szDemodulateCmd[10]=HB[0];
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=HB[1];
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=HB[2];
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x03;//C1 C2
	szDemodulateCmd[7]=0x00;
	szDemodulateCmd[8]=m_C1;
	szDemodulateCmd[9]=0x00;
	szDemodulateCmd[10]=m_C2;
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=0x00;
	szDemodulateCmd[13]=0x00;  
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x04;//解调环路复位
	szDemodulateCmd[7]=0x00;
	szDemodulateCmd[8]=0x01;
	szDemodulateCmd[9]=0x00;
	szDemodulateCmd[10]=0x00;
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=0x00;
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x05;//解调载波1
	szDemodulateCmd[7]=pinlvkongzhizi1>>24;
	szDemodulateCmd[8]=pinlvkongzhizi1>>16;
	szDemodulateCmd[9]=pinlvkongzhizi1>>8;
	szDemodulateCmd[10]=pinlvkongzhizi1;
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=0x00;
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x07;//GMSK下/上变频
	szDemodulateCmd[7]=pinlvkongzhizi_down>>24;
	szDemodulateCmd[8]=pinlvkongzhizi_down>>16;
	szDemodulateCmd[9]=pinlvkongzhizi_down>>8;
	szDemodulateCmd[10]=pinlvkongzhizi_down;
	szDemodulateCmd[11]=pinlvkongzhizi_up>>24;
	szDemodulateCmd[12]=pinlvkongzhizi_up>>16;
	szDemodulateCmd[13]=pinlvkongzhizi_up>>8;
	szDemodulateCmd[14]=pinlvkongzhizi_up;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	szDemodulateCmd[4]=0x08;//解调载波2
	szDemodulateCmd[7]=pinlvkongzhizi2>>24;
	szDemodulateCmd[8]=pinlvkongzhizi2>>16;
	szDemodulateCmd[9]=pinlvkongzhizi2>>8;
	szDemodulateCmd[10]=pinlvkongzhizi2;
	szDemodulateCmd[11]=0x00;
	szDemodulateCmd[12]=0x00;
	szDemodulateCmd[13]=0x00;
	szDemodulateCmd[14]=0x00;
	szDemodulateCmd[15]=0x00;
	szDemodulateCmd[16]=0x00;
	szDemodulateCmd[17]=0x00;
	szDemodulateCmd[18]=0x01;
	szDemodulateCmd[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodulateCmd[19]=szDemodulateCmd[19]+szDemodulateCmd[i];
	}
	pGEDevice->SendTo(szDemodulateCmd,nDemodulateCmdSize);

	delete [] szDemodulateCmd;
	CString dpy;
	dpy.Format(_T(" fo:%3f MHz\r\n adsample:%3f MHz \r\n up_s:%d\r\n HB:%d %d %d\r\n cic:%d\r\n R_4351:%d INT_4351:%d FRAC_4351:%d"),fsample*1.0/5/1000000,adsample/1000000,up_s,HB[0],HB[1],HB[2],cic,R_4351,INT_4351,FRAC_4351);
	pMainDlg->DisplayRunningInfo(dpy);
}

//信道化解调模式-接收数据
void CPage4::OnBnClickedButtonRecvdata()
{
	// TODO: 在此添加控件通知处理程序代码
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);
	pMainDlg->SetTimer(1, 200, NULL);
	pMainDlg->m_iPlotX.GetXAxis(0).SetMin(0);
	pMainDlg->m_iPlotX.GetXAxis(0).SetSpan(10);
	pMainDlg->m_iPlotX.GetChannel(0).Clear();

	strRunningInfo.Empty();
	//清除套接字缓存
	int nClearBuf = pGEDevice->ClearSocketBuffer();
	UpdateData(TRUE);

	//发送千兆网查询指令
	char cmd_query[3] = {0xe8,0x01,(char)m_nFramesPerPackage};
	//TRACE("每包包含帧数：%d\n",m_nFramesPerPackage);
	pGEDevice->SetFramesInOnePackage(m_nFramesPerPackage);
	pGEDevice->SetRecvSize(m_nRecvSize);
	//TRACE("接收总包数：%d\n",m_nRecvSize);
	pGEDevice->SendTo(cmd_query,3);

	//////////////////////////////////////////////////////////////////////////
	//发送接收数据指令
	UpdateData(TRUE);
	int upmode=0;//上传数据类型
	upmode =m_UploadDataType.GetCurSel();
	TRACE("Mode=%d\n",upmode);

	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	//预处理板
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x83;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=upmode;
	szDemodCmd_RecvData[9]=0x00;
	szDemodCmd_RecvData[10]=0x01;  //开始上传
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

	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x06;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=upmode;
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
	delete [] szDemodCmd_RecvData;

	//////////////////////////////////////////////////////////////////////////
	//开始接收数据
	pMainDlg->ClearFiles("C:\\Recv\\*.dat");	//清除缓存文件
	pGEDevice->Open();							//打开千兆网设备
	pGEDevice->SetStorePath(_T("C:\\Recv\\"));

	//按钮禁用设置
	GetDlgItem(IDC_BUTTON_RECVDATA)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP_RECV)->EnableWindow(TRUE);
	
	TRACE("开启线程！\n");

	//避免出现线程尚未退出又继续创建线程
	if (bThreadStopRecv){
		bThreadStopRecv = FALSE;
	}
	else{
		return;
	}
	if (hThreadRecv!=INVALID_HANDLE_VALUE)
	{
		TRACE("线程未退出，创建线程失败！\n");
		return;
	}
	//创建接收线程
	ThreadRecvParam.pPage4Dlg = this;
	hThreadRecv = CreateThread(NULL,0,ThreadRecv,&ThreadRecvParam,0,&dwThreadRecvID);
	CloseHandle(hThreadRecv);
	hThreadRecv = INVALID_HANDLE_VALUE;

	//避免出现线程尚未退出又继续创建线程
	if (bThreadStopPlot){
		bThreadStopPlot = FALSE;
	}
	else{
		return;
	}
	if (hThreadPlot!=INVALID_HANDLE_VALUE)
	{
		TRACE("线程未退出，创建线程失败！\n");
		return;
	}
	//创建线程
	ThreadRecvParam.pPage4Dlg = this;
	hThreadPlot = CreateThread(NULL,0,ThreadStartPlot,&ThreadRecvParam,0,&dwThreadPlotID);
	CloseHandle(hThreadPlot);
	hThreadPlot = INVALID_HANDLE_VALUE;
}


DWORD WINAPI ThreadRecv(LPVOID lpParam)
{
	// TODO: 在此添加控件通知处理程序代码
	THREADPARAM* pThreadParam = (THREADPARAM*)lpParam;
	CPage4* pPage4Dlg = dynamic_cast<CPage4*>(pThreadParam->pPage4Dlg);
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pPage4Dlg->pTempMainDlg);

	char *pBufRecvPackage = new char[255*nLenFrame];

	while(1)
	{
		if (pPage4Dlg->bThreadStopRecv)
		{
			char cmd_quit[2] = {0xe8,0x05};
			pPage4Dlg->pGEDevice->SendTo(cmd_quit,2);
			TRACE("发送停止指令\n");
			break;
		}
		int RetRecv = pPage4Dlg->pGEDevice->RecvFrom(pBufRecvPackage,255*nLenFrame,10000);
		if (0x54 == RetRecv)
		{
			TRACE("超时次数超过3次，接收程序被迫终止！");
			pPage4Dlg->bThreadStopRecv = TRUE;
			pPage4Dlg->bThreadStopPlot = TRUE;
			Beep(1000,600);
			break;
		}
		else if (0x53 == RetRecv)
		{
			TRACE("丢包次数超过3次，接收程序被迫终止！");
			pPage4Dlg->bThreadStopRecv = TRUE;
			pPage4Dlg->bThreadStopPlot = TRUE;
			Beep(1000,600);
			break;
		}
		else if (0x55 == RetRecv)
		{
			/*TRACE("接收完成！");
			pPage4Dlg->bThreadStopRecv = TRUE;
			pPage4Dlg->bThreadStopPlot = TRUE;
			pMainDlg->DisplayRunningInfo("接收完成！");
			pPage4Dlg->OnBnClickedButtonStopRecv();
			break;*/
			if (pPage4Dlg->pGEDevice->GetReceivedPackage() == pPage4Dlg->m_nRecvSize)
			{
				TRACE("接收完成！");
				pPage4Dlg->bThreadStopRecv = TRUE;
				pPage4Dlg->bThreadStopPlot = TRUE;
				break;
			}
		}
	}
	delete[] pBufRecvPackage;
	TRACE("Page4退出线程-ThreadRecv！\n");
	return 0x55;
}


DWORD WINAPI ThreadStartPlot(LPVOID lpParam)
{                   //给PerformanceEvaluationSystem发送画图消息，启动它的画图定时器
	THREADPARAM* pThreadParam = (THREADPARAM*)lpParam;
	CPage4* pPage4Dlg = dynamic_cast<CPage4*>(pThreadParam->pPage4Dlg);
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pPage4Dlg->pTempMainDlg);
	::SendMessage(pMainDlg->GetSafeHwnd(), WM_PLOTDATA, 0, 0);
	return 0;
	//CiPlotChannelX ChannelTemp = pMainDlg->m_iPlotX.GetChannel(0);
	//CiPlotAxisX AxisX = pMainDlg->m_iPlotX.GetXAxis(0);
	//double dAxisX =0.0;
	//double dAxisXDelt = 0.001;

	//int uploadtype = pPage4Dlg->m_UploadDataType.GetCurSel();         //上传数据类型
	//CFile FileData;
	//CString strFileDataDir = _T("C:\\Recv\\RecvData.dat");
	//FileData.Open(strFileDataDir,CFile::modeRead | CFile::modeCreate | CFile::modeNoTruncate | CFile::shareDenyNone);
	//LONGLONG llPosFileData = 0;
	//while(1)
	//{
	//	if (pPage4Dlg->bThreadStopPlot)
	//	{
	//		break;
	//	}
	//	int nLenDelt = FileData.GetLength()-llPosFileData;

	//	if (nLenDelt>=10240)
	//	{
	//		char *pBufFileReadIn = new char[nLenDelt];
	//		FileData.Read(pBufFileReadIn,nLenDelt);
	//		if (uploadtype == 5)           //中频数据为2个字节，数据长度应除以2
	//		{
	//			nLenDelt = nLenDelt / 2;
	//		}
	//		else if (uploadtype == 1)      //锁相环累计误差数据为4个字节，数据长度应除以4
	//		{
	//			nLenDelt = nLenDelt / 4;
	//		}
	//		llPosFileData = FileData.GetPosition();
	//		for (int i=0;i<nLenDelt;i++)
	//		{
	//			if (uploadtype == 5)
	//			{
	//				ChannelTemp.AddXY(dAxisX,*(pBufFileReadIn + 2 * i) * 256 + *(pBufFileReadIn + 2 * i));
	//			}
	//			else if (uploadtype == 1)
	//			{
	//				long temp = *(long *)(pBufFileReadIn + 4 * i);
	//				ChannelTemp.AddXY(dAxisX, *(long *)(pBufFileReadIn + 4 * i));
	//			}
	//			else
	//			{
	//				ChannelTemp.AddXY(dAxisX,(double)*(pBufFileReadIn + i));
	//			}
	//			dAxisX += dAxisXDelt;
	//			if (pPage4Dlg->bThreadStopPlot)
	//			{
	//				break;
	//			}
	//		}
	//		delete[] pBufFileReadIn;
	//		Sleep(1000);
	//	}
	//	else
	//	{
	//		Sleep(50);
	//	}
	//}
	//FileData.Close();

	//TRACE("Page4退出线程-ThreadStartPlot！\n");
	//return 0x55;
}

//信道化解调模式-停止接收数据
void CPage4::OnBnClickedButtonStopRecv()
{
	// TODO: 在此添加控件通知处理程序代码
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);
	pMainDlg->KillTimer(1);
	pMainDlg->position_file_plot = 0;
	pMainDlg->CurrentX = 0;
	pMainDlg->average = 0;
	pMainDlg->count_average = 0;
	pMainDlg->shock_average = 0;
	pMainDlg->count_shock = 0;

	//发送指令 将解调FPGA的上传数据停止
	int nDemodCmdSize_StopRecv =20;
	char *szDemodCmd_StopRecv=new char[nDemodCmdSize_StopRecv];
	szDemodCmd_StopRecv[0]=0x17;
	szDemodCmd_StopRecv[1]=0x57;
	szDemodCmd_StopRecv[2]=0x90;
	szDemodCmd_StopRecv[3]=0xeb;
	szDemodCmd_StopRecv[4]=0x06;
	szDemodCmd_StopRecv[5]=0x00;
	szDemodCmd_StopRecv[6]=0x0f;
	szDemodCmd_StopRecv[7]=0x00;
	szDemodCmd_StopRecv[8]=0x00;
	szDemodCmd_StopRecv[9]=0x00;
	szDemodCmd_StopRecv[10]=0x00;
	szDemodCmd_StopRecv[11]=0x00;
	szDemodCmd_StopRecv[12]=0x00;
	szDemodCmd_StopRecv[13]=0x00;
	szDemodCmd_StopRecv[14]=0x00;
	szDemodCmd_StopRecv[15]=0x00;
	szDemodCmd_StopRecv[16]=0x00;
	szDemodCmd_StopRecv[17]=0x00;
	szDemodCmd_StopRecv[18]=0x01;
	szDemodCmd_StopRecv[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_StopRecv[19]=szDemodCmd_StopRecv[19]+szDemodCmd_StopRecv[i];
	}
	pGEDevice->SendTo(szDemodCmd_StopRecv,nDemodCmdSize_StopRecv);
	//将预处理FPGA的上传数据停止
	szDemodCmd_StopRecv[0]=0x17;
	szDemodCmd_StopRecv[1]=0x57;
	szDemodCmd_StopRecv[2]=0x90;
	szDemodCmd_StopRecv[3]=0xeb;
	szDemodCmd_StopRecv[4]=0x83;
	szDemodCmd_StopRecv[5]=0x00;
	szDemodCmd_StopRecv[6]=0x0f;
	szDemodCmd_StopRecv[7]=0x00;
	szDemodCmd_StopRecv[8]=0x00;  //停止上传
	szDemodCmd_StopRecv[9]=0x00;
	szDemodCmd_StopRecv[10]=0x00;
	szDemodCmd_StopRecv[11]=0x00;
	szDemodCmd_StopRecv[12]=0x00;
	szDemodCmd_StopRecv[13]=0x00;
	szDemodCmd_StopRecv[14]=0x00;
	szDemodCmd_StopRecv[15]=0x00;
	szDemodCmd_StopRecv[16]=0x00;
	szDemodCmd_StopRecv[17]=0x00;
	szDemodCmd_StopRecv[18]=0x01;
	szDemodCmd_StopRecv[19]=0x00;
	for(int i=0;i<19;i++)
	{
		szDemodCmd_StopRecv[19]=szDemodCmd_StopRecv[19]+szDemodCmd_StopRecv[i];
	}
	pGEDevice->SendTo(szDemodCmd_StopRecv,nDemodCmdSize_StopRecv);
	delete [] szDemodCmd_StopRecv;

	//////////////////////////////////////////////////////////////////////////
	//发送千兆网复位指令

	char *szResetCmd = new char[2];
	szResetCmd[0] = 0xe8;
	szResetCmd[1] = 0x05;
	pGEDevice->SendTo(szResetCmd,2);
	delete[] szResetCmd;

	bThreadStopRecv = TRUE;
	bThreadStopPlot = TRUE;
	pGEDevice->Close();
	
	GetDlgItem(IDC_BUTTON_RECVDATA)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_RECV)->EnableWindow(FALSE);
}

//信道化解调模式-数据比对
void CPage4::OnBnClickedButtonCompar()
{
	// TODO: 在此添加控件通知处理程序代码
	//CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);

	CString strExeFilepath;
	strExeFilepath="F:\\504\\上位机\\BERdetection\\Debug\\BERdetection.exe";
	ShellExecute(NULL,_T("open"),strExeFilepath,NULL,NULL,SW_SHOWNORMAL);
}

//信道化解调模式-千兆网复位
void CPage4::OnBnClickedButtonReset()
{
	// TODO: 在此添加控件通知处理程序代码
	//发送千兆网复位指令
	char *szResetCmd = new char[2];
	szResetCmd[0] = 0xe8;
	szResetCmd[1] = 0x05;
	pGEDevice->SendTo(szResetCmd,2);
	delete [] szResetCmd;
}

//信道化解调模式-调制器
void CPage4::OnBnClickedButtonModulator()
{
	// TODO: 在此添加控件通知处理程序代码
	//CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);

	CString strExeFilepath;
	strExeFilepath="F:\\504\\上位机\\调制源\\Debug\\USBTest.exe";
	ShellExecute(NULL,"open",strExeFilepath,NULL,NULL,SW_SHOWNORMAL);
}


BOOL CPage4::OnInitDialog()
{
	CPageBase::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pTempMainDlg);
	pMainDlg->ClearFiles(_T("F:\\Recv\\*.dat"));

	//////////////////////////////////////////////////////////////////////////
	//调试用
	GetDlgItem(IDC_BUTTON_CONFIGURE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RECVDATA)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_COMPAR)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_MODULATOR)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP_RECV)->EnableWindow(FALSE);
	GetDlgItem(IDC_ENDTRACK)->EnableWindow(FALSE);
	GetDlgItem(IDC_ENDOFFSET)->EnableWindow(FALSE);


	/*pGEDevice = new CGigabitEthernetDevice(m_hWnd);
	pGEDevice->Initial();
	pGEDevice->Open();*/

	//解调参数相关初始化
	m_ModulateMode.AddString(_T("BPSK"));
	m_ModulateMode.AddString(_T("QPSK"));
	m_ModulateMode.AddString(_T("16QAM"));
	m_ModulateMode.AddString(_T("32QAM"));
	m_ModulateMode.AddString(_T("Sin"));
	m_ModulateMode.AddString(_T("OQPSK"));
	m_ModulateMode.AddString(_T("GMSK"));
	m_ModulateMode.AddString(_T("2FSK"));
	m_ModulateMode.AddString(_T("FM"));
	m_ModulateMode.AddString(_T("AM"));
	m_ModulateMode.SetCurSel(2);

	m_SymbolRateUnit.AddString(_T("MHz"));
	m_SymbolRateUnit.AddString(_T("KHz"));
	m_SymbolRateUnit.AddString(_T("Hz"));
	m_SymbolRateUnit.SetCurSel(1);

	m_UploadDataType.AddString(_T("解调数据"));
	m_UploadDataType.AddString(_T("锁相环累计误差"));
	m_UploadDataType.AddString(_T("定时环路误差"));
	m_UploadDataType.AddString(_T("累加数据"));
	m_UploadDataType.AddString(_T("星座点数据"));
	m_UploadDataType.AddString(_T("中频数据"));
	m_UploadDataType.SetCurSel(3);

	GetDlgItem(IDC_BUTTON_CONFIGURE)->GetWindowRect(rect_button[0]);
	GetDlgItem(IDC_BUTTON_RECVDATA)->GetWindowRect(rect_button[1]);
	GetDlgItem(IDC_BUTTON_COMPAR)->GetWindowRect(rect_button[2]);
	GetDlgItem(IDC_BUTTON_RESET)->GetWindowRect(rect_button[3]);
	GetDlgItem(IDC_BUTTON_STOP_RECV)->GetWindowRect(rect_button[4]);
	GetDlgItem(IDC_BUTTON_MODULATOR)->GetWindowRect(rect_button[5]);
	ScreenToClient(rect_button[0]);
	ScreenToClient(rect_button[1]);
	ScreenToClient(rect_button[2]);
	ScreenToClient(rect_button[3]);
	ScreenToClient(rect_button[4]);
	ScreenToClient(rect_button[5]);

	m_buttons[0] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_CONFIGURE));
	m_buttons[1] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_RECVDATA));
	m_buttons[2] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_COMPAR));
	m_buttons[3] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_RESET));
	m_buttons[4] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_STOP_RECV));
	m_buttons[5] = static_cast<CButton *>(GetDlgItem(IDC_BUTTON_MODULATOR));
	TRACE("Page4初始化完成！\n");
	return TRUE;  
	// return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CPage4::OnEnChangeEditRecvsize()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CPageBase::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CPage4::OnBnClickedBegintrack()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	GetDlgItem(IDC_ENDTRACK)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGINTRACK)->EnableWindow(FALSE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x09;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=m_c1delta;
	szDemodCmd_RecvData[9]=0x00;
	szDemodCmd_RecvData[10]=m_c2delta;
	szDemodCmd_RecvData[11]=0x00;
	szDemodCmd_RecvData[12]=0x01;
	szDemodCmd_RecvData[13]=0x00;
	szDemodCmd_RecvData[14]=0x00;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	TRACE("m_c1delta = %u\n", m_c1delta);
	TRACE("m_c2delta = %u\n", m_c2delta);
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage4::OnBnClickedEndtrack()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	GetDlgItem(IDC_ENDTRACK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEGINTRACK)->EnableWindow(TRUE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x09;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=m_c1delta;
	szDemodCmd_RecvData[9]=0x00;
	szDemodCmd_RecvData[10]=m_c2delta;
	szDemodCmd_RecvData[11]=0x00;
	szDemodCmd_RecvData[12]=0x00;
	szDemodCmd_RecvData[13]=0x00;
	szDemodCmd_RecvData[14]=0x00;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	TRACE("m_c1delta = %u\n", m_c1delta);
	TRACE("m_c2delta = %u\n", m_c2delta);
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage4::OnBnClickedBeginoffset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	UpdateData(TRUE);
	GetDlgItem(IDC_BEGINOFFSET)->EnableWindow(FALSE);
	GetDlgItem(IDC_ENDOFFSET)->EnableWindow(TRUE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x10;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=0x01;            //开始去偏置标志
	szDemodCmd_RecvData[9]=(short)m_offsetvalue >> 8;
	szDemodCmd_RecvData[10]=(short)m_offsetvalue;
	szDemodCmd_RecvData[11]=0x00;
	szDemodCmd_RecvData[12]=0x00;
	szDemodCmd_RecvData[13]=0x00;
	szDemodCmd_RecvData[14]=0x00;
	szDemodCmd_RecvData[15]=0x00;
	szDemodCmd_RecvData[16]=0x00;
	szDemodCmd_RecvData[17]=0x00;
	szDemodCmd_RecvData[18]=0x01;
	szDemodCmd_RecvData[19]=0x00;
	TRACE("m_offsetvalue_H = %d\n",szDemodCmd_RecvData[9]);
	TRACE("m_offsetvalue_L = %d\n", szDemodCmd_RecvData[10]);
	for(int i=0;i<19;i++)
	{
		szDemodCmd_RecvData[19]=szDemodCmd_RecvData[19]+szDemodCmd_RecvData[i];
	}
	pGEDevice->SendTo(szDemodCmd_RecvData,nDemodCmdSize_RecvData);
}


void CPage4::OnBnClickedEndoffset()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	GetDlgItem(IDC_BEGINOFFSET)->EnableWindow(TRUE);
	GetDlgItem(IDC_ENDOFFSET)->EnableWindow(FALSE);
	int nDemodCmdSize_RecvData =20;
	char *szDemodCmd_RecvData=new char[nDemodCmdSize_RecvData];
	szDemodCmd_RecvData[0]=0x17;
	szDemodCmd_RecvData[1]=0x57;
	szDemodCmd_RecvData[2]=0x90;
	szDemodCmd_RecvData[3]=0xeb;
	szDemodCmd_RecvData[4]=0x10;
	szDemodCmd_RecvData[5]=0x00;
	szDemodCmd_RecvData[6]=0x0f;
	szDemodCmd_RecvData[7]=0x00;
	szDemodCmd_RecvData[8]=0x00;            //停止去偏置标志
	szDemodCmd_RecvData[9]=0x00;
	szDemodCmd_RecvData[10]=0x00;
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


afx_msg LRESULT CPage4::OnButtonChanged(WPARAM wParam, LPARAM lParam)
{
	switch ((int)wParam)
	{
	case 0:               //在线模式
		{
			for (int i = 0; i < BUTTON_NUM_OFFLINE; i++)          //先销毁离线模式的按钮
			{
				m_buttons[i]->DestroyWindow();
				m_buttons[i] = nullptr;
			}

			for (int i = 0; i < BUTTON_NUM_ONLINE; i++)           //再新建在线模式的按钮
			{
				m_buttons[i] = new CButton();
			}
			m_buttons[0]->Create(_T("配置"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[0], this, IDC_BUTTON_CONFIGURE);
			m_buttons[1]->Create(_T("接收数据"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[1], this, IDC_BUTTON_RECVDATA);
			m_buttons[2]->Create(_T("比对"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[2], this, IDC_BUTTON_COMPAR);
			m_buttons[3]->Create(_T("千兆网复位"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[3], this, IDC_BUTTON_RESET);
			m_buttons[4]->Create(_T("停止接收"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[4], this, IDC_BUTTON_STOP_RECV);
			m_buttons[5]->Create(_T("调制器"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[5], this, IDC_BUTTON_MODULATOR);
			m_buttons[0]->SetFont(GetFont());
			m_buttons[1]->SetFont(GetFont());
			m_buttons[2]->SetFont(GetFont());
			m_buttons[3]->SetFont(GetFont());
			m_buttons[4]->SetFont(GetFont());
			m_buttons[5]->SetFont(GetFont());
			break;
		}
	case 1:               //离线模式
	{
		for (int i = 0; i < BUTTON_NUM_ONLINE; i++)
		{
			m_buttons[i]->DestroyWindow();
			m_buttons[i] = nullptr;
		}

		for (int i = 0; i < BUTTON_NUM_OFFLINE; i++)
		{
			m_buttons[i] = new CButton();
		}
		m_buttons[0]->Create(_T("调用simulink"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rect_button[0], this, IDC_CALLSIMULINK);
		m_buttons[0]->SetFont(GetFont());
		break;
	}

	default:
		break;
	}
	return 0;
}


void CPage4::OnBnClickedCallsimulink()
{
	// TODO: ÔÚ´ËÌí¼Ó¿Ø¼þÍ¨Öª´¦Àí³ÌÐò´úÂë
	TCHAR path_execution[100];
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg *>(GetParent());
	GetModuleFileName(NULL, path_execution, 100);
	CString path_simulink(path_execution);
	path_simulink = path_simulink.Left(path_simulink.ReverseFind('\\'));
	path_simulink = path_simulink.Left(path_simulink.ReverseFind('\\'));
	path_simulink.Append(_T("\\MatlabFiles\\Demodulation"));
	mxArray *mPath = nullptr;
	mPath = mxCreateString(path_simulink.GetBuffer());
	engPutVariable(pMainDlg->en, _T("simulinkpath"), mPath);
	engEvalString(pMainDlg->en, _T("cd(simulinkpath)"));
	//engEvalString(pMainDlg->en, "callsimulink");
	engEvalString(pMainDlg->en, _T("load_system('QAM16_100K_I2Q2');"));
	engEvalString(pMainDlg->en, _T("sim('QAM16_100K_I2Q2')"));
	engEvalString(pMainDlg->en, _T("close_system('QAM16_100K_I2Q2')"));
	engEvalString(pMainDlg->en, _T("save result"));
	engEvalString(pMainDlg->en, _T("subplot(2,1,1);"));
	engEvalString(pMainDlg->en, _T("plot(result.time, result.signals(1).values);"));
	engEvalString(pMainDlg->en, _T("subplot(2,1,2);"));
	engEvalString(pMainDlg->en, _T("plot(result.time, result.signals(2).values);"));
}