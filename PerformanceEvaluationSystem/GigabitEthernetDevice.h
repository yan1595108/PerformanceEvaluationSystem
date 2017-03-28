#pragma once

#include <fstream>

using namespace std;

/*
struct _PACKAGE{
	char Package[1024*255];
}PACKAGE;*/

///////////////////////////////////测试类////////////////////////////////////
//计算平均传输速率
class CTestTramsmissionRateCalculate{
private:
	BOOL bIsStarted;
	DWORD dwBeginTime;
	DWORD dwEndTime;
	double dAverageTramsmissionRate;

public:
	CTestTramsmissionRateCalculate(void);
	void	Reset();
	DWORD	GetBeginTime();
	double	GetTramsmissionRate(LONGLONG llPackagesRecv,int nNumFramesInOnePackage);
};

class CGigabitEthernetDevice
{
public:
	//deque<PACKAGE> DequeRecvData;

private:
	//CDialogEx *pMainDlgTemp;

	HWND m_hMainDlg;

	SOCKET m_socket;
	SOCKADDR_IN saFPGA;
	SOCKADDR_IN saPC;

	WSAOVERLAPPED overlapSend;
	WSAOVERLAPPED overlapRecv;

	WSABUF wsaBufSend;
	WSABUF wsaBufRecv;
	char *pBufRecv;
	int nLenBufRecv;
	char *pBufSend;
	int nLenBufSend;

	char cmd_query[3];
	char cmd_success[3];
	char cmd_failure[3];
	char cmd_timeout[3];
	char cmd_quit[3];

	int nPackageIndexLastStore;

	//
	int nNumFramesInOnePackage;	//每包包含的帧数
	int nRecvSize;			//接收包数量

	//数据存储
	LONGLONG llPackagesRecv;
	int nNumPackets2Store; 
	CFile FileStore;
	//HANDLE hFileStore;
	//OVERLAPPED overlappedWrite;
	CString strFileStoreDir;
	CString strFileStorePath;
	CString strFileStoreName;

	//测试类变量
	CTestTramsmissionRateCalculate TransmissionRate;


public:
	CGigabitEthernetDevice(HWND m_hWnd);
	//CGigabitEthernetDevice(CDialogEx *pDlg);
	~CGigabitEthernetDevice(void);

public:
	volatile BOOL bStopTransfer;
	
public:
	BOOL	Initial();
	BOOL	Open();
	int		SendTo(char *pBufSend,int nSizeBufSend);
	int		RecvFrom(char *pBufRecvPackage,int nSizeBufRecv,int nTimeWait);
	BOOL	Close();
	BOOL	Destroy();

	BOOL	SetFPGAAddress(char *addr,short nPort);
	BOOL	SetPCAddress(char *addr,short nPort);
	BOOL	SetStorePath(CString strPath);
	BOOL	SetFramesInOnePackage(int nNumFrames);
	BOOL	SetRecvSize(int size);   //此函数无意义
	int		ClearSocketBuffer();
	int GetReceivedPackage(void);
};

