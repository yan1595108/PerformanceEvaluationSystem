#pragma once
#include "StdAfx.h"
#include "GigabitEthernetDevice.h"
#include "PerformanceEvaluationSystem.h"
#include "PerformanceEvaluationSystemDlg.h"

//int nNumFramesInOnePackage = 255;
const int nLenCmd = 2;
const int nLenFrame = 1026;
CString str_SystemMes("");

//全局函数声明
//void PutOutSystemMes(CInterfaceClassDlg *MainDlg_GEIUDP,CString str_Mes);

///////////////////////////////////测试类////////////////////////////////////
//计算平均传输速率
CTestTramsmissionRateCalculate::CTestTramsmissionRateCalculate(void)
	:bIsStarted(FALSE),dwBeginTime(0),dwEndTime(0),dAverageTramsmissionRate(0.0)
{

}

void CTestTramsmissionRateCalculate::Reset()
{
	bIsStarted = FALSE;
	dwBeginTime = 0;
	dwEndTime = 0;
	dAverageTramsmissionRate = 0.0;
}

DWORD CTestTramsmissionRateCalculate::GetBeginTime()
{
	if (!bIsStarted)
	{
		bIsStarted = TRUE;
		dwBeginTime = GetTickCount();
	}
	else
	{
	}
	return dwBeginTime;
}

double CTestTramsmissionRateCalculate::GetTramsmissionRate(LONGLONG llPackagesRecv,int nNumFramesInOnePackage)
{
	if (0 == dwBeginTime)
	{
		AfxMessageBox(_T("错误：初始时刻值为0！"));
	}
	dwEndTime = GetTickCount();
	if((dwEndTime-dwBeginTime)==0)
	{
		dAverageTramsmissionRate = 0.0;
	}
	else
	{
		dAverageTramsmissionRate = ((double)llPackagesRecv/(dwEndTime-dwBeginTime))*nNumFramesInOnePackage*nLenFrame*8/1000;
	}
	return dAverageTramsmissionRate;
}

///////////////////////////////千兆网传输接口类///////////////////////////////
//千兆网无差错传输
CGigabitEthernetDevice::CGigabitEthernetDevice(HWND hDlg = NULL)
	//:hMainDlg(hpMainDlg),
	//:pMainDlgTemp(pMainDlg),
	//:m_hMainDlg(hWnd),
	:m_hMainDlg(hDlg),
	bStopTransfer(FALSE),
	nPackageIndexLastStore(0),nNumPackets2Store(1),llPackagesRecv(0),
	strFileStorePath("F:\\Recv\\")
{
	//初始化套接字地址结构体
	ZeroMemory(&saFPGA,sizeof(SOCKADDR_IN));
	ZeroMemory(&saPC,sizeof(SOCKADDR_IN));

	//重叠端口初始化
	ZeroMemory(&overlapSend,sizeof(overlapSend));
	ZeroMemory(&overlapSend,sizeof(overlapSend));
	overlapSend.hEvent = INVALID_HANDLE_VALUE;
	overlapRecv.hEvent = INVALID_HANDLE_VALUE;
	m_socket = INVALID_SOCKET;

	//传输指令初始化
	cmd_query[0]	= 0xe8;	cmd_query[1]	= 0x01;	cmd_query[2]	= 0xff;
	cmd_success[0]	= 0xe8;	cmd_success[1]	= 0x02;	cmd_success[2]	= 0x01;
	cmd_failure[0]	= 0xe8;	cmd_failure[1]	= 0x03; cmd_failure[2]	= 0x01;
	cmd_timeout[0]	= 0xe8;	cmd_timeout[1]	= 0x04;	cmd_timeout[2]	= 0x01;
	cmd_quit[0]		= 0xe8;	cmd_quit[1]		= 0x05;	cmd_quit[2]		= 0x00;

	//接收缓存空间初始化
	nLenBufRecv = nLenFrame;
	pBufRecv = new char[nLenBufRecv];
	ZeroMemory(pBufRecv,nLenBufRecv);
	wsaBufRecv.buf = pBufRecv;
	wsaBufRecv.len = nLenBufRecv;
	//发送缓存空间初始化
	nLenBufSend = nLenCmd;
	pBufSend = new char[nLenBufSend];
	ZeroMemory(pBufRecv,nLenBufRecv);
	wsaBufSend.buf = pBufSend;
	wsaBufSend.len = nLenBufSend;
}

CGigabitEthernetDevice::~CGigabitEthernetDevice(void)
{
	if (pBufRecv!=NULL)
	{
		delete[] pBufRecv;
	}
	if (pBufSend!=NULL)
	{
		delete[] pBufSend;
	}
}

BOOL CGigabitEthernetDevice::Initial()
{
	//加载套接字库
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2,2);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err!=0)
	{
		AfxMessageBox(_T("加载套接字失败！"));
		return FALSE;
	}
	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		WSACleanup();
		AfxMessageBox(_T("无法加载加载套接字2.2，该版本不支持！"));
		return FALSE;
	}

	//设置PC地址信息
	SetPCAddress("192.168.1.100",8000);
	//SetPCAddress("192.168.1.100",7000);

	//设置FPGA地址信息
	SetFPGAAddress("192.168.1.150",8080);

	//设置文件保存路径
	//SetStorePath("C:\\");

	nPackageIndexLastStore = 0;

	//ZeroMemory(&overlappedWrite,sizeof(OVERLAPPED));
	//创建一个WSA_FLAG_OVERLAPPED套接字
	//WSASocket创建的是一个异步IO套接字
	m_socket = WSASocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		TRACE("WSASocket Error:%d",WSAGetLastError());
		return FALSE;
	}

	//套接字接收、发送缓存设置
	int nBufs_Recv,nBufs_Send;
	nBufs_Recv = 1024*1024*32; //2MB
	//nBufs_Send = 1024;//1M;
	nBufs_Send = 0;//1M;
	int retval11 = setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&nBufs_Recv,sizeof(int)); //设置接收缓冲区大小
	int retval12 = setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&nBufs_Send,sizeof(int)); //设置接收缓冲区大小
	/*BOOL bDontLinger = FALSE;
	int retval13 = setsockopt(m_socket,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));
	BOOL bReuseaddr = TRUE;
	int retval14 = setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL));*/
	if (retval11!=0 && retval12!=0)
	{
		AfxMessageBox(_T("设置参数1失败"));
	}
	int nTimeOut = 100;
	int nRetval = setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nTimeOut,sizeof(int));
	int nRealTimeOut = 0;
	int nlen_nRealTimeOut = sizeof(int);
	getsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nRealTimeOut,&nlen_nRealTimeOut);
	TRACE(_T("超时时间：%d\n"),nRealTimeOut);

	//调用bind把本地套接字地址绑定到监听套接字
	if (SOCKET_ERROR == bind(m_socket,(sockaddr*)&saPC,sizeof(sockaddr_in)))
	{
		TRACE("Bind Error %d\n",WSAGetLastError());
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	//创建套接字事件
	overlapSend.hEvent = WSACreateEvent();
	overlapRecv.hEvent = WSACreateEvent();

	if (overlapSend.hEvent==INVALID_HANDLE_VALUE || overlapRecv.hEvent==INVALID_HANDLE_VALUE)
	{
		TRACE("WSACreateEvent Error!\n");
	}
}

BOOL CGigabitEthernetDevice::Destroy()
{
	TRACE("开始回收资源！\n");
	if (overlapSend.hEvent != INVALID_HANDLE_VALUE)
	{
		WSACloseEvent(overlapSend.hEvent);
		overlapSend.hEvent = INVALID_HANDLE_VALUE;
	}
	if (overlapRecv.hEvent != INVALID_HANDLE_VALUE)
	{
		WSACloseEvent(overlapRecv.hEvent);
		overlapRecv.hEvent = INVALID_HANDLE_VALUE;
	}
	if (m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	WSACleanup();
	delete[] pBufRecv;
	delete[] pBufSend;
	pBufRecv = NULL;
	pBufSend = NULL;
	TRACE("回收资源成功！\n\n");
	return TRUE;
}

BOOL CGigabitEthernetDevice::Open()
{
	bStopTransfer = FALSE;
	nPackageIndexLastStore = 0;
	nNumPackets2Store = 1;

	//测速率类复位 重新统计传输速率
	llPackagesRecv = 0;
	TransmissionRate.Reset();

	return TRUE;
}

BOOL CGigabitEthernetDevice::Close()
{
	bStopTransfer = TRUE;
	if (overlapSend.hEvent == INVALID_HANDLE_VALUE)
	{
		TRACE("发送事件对象句柄无效！\n");
	}
	if (overlapRecv.hEvent == INVALID_HANDLE_VALUE)
	{
		TRACE("接收事件对象句柄无效！\n");
	}
	TRACE("error code %d\n", GetLastError());
	if (!CancelIo((HANDLE)m_socket))
	{
		TRACE("Cancel SnedIO Operation error %d\n", GetLastError());
	}
	/*if (!CancelIo(overlapRecv.hEvent))
	{
	TRACE("Cancel RecvIO Operation error %d\n", GetLastError());
	}*/
	Sleep(20);
	return TRUE;
}

BOOL CGigabitEthernetDevice::SetPCAddress(char *addr = "192.168.1.100",short nPort = 8000)
{
	saPC.sin_family = AF_INET;
	//saPC.sin_addr.s_addr = htonl(INADDR_ANY);
	saPC.sin_addr.S_un.S_addr = inet_addr(addr);
	saPC.sin_port = htons(nPort);
	return TRUE;
}

BOOL CGigabitEthernetDevice::SetFPGAAddress(char *addr = "192.168.1.150",short nPort = 8080)
{
	saFPGA.sin_family = AF_INET;
	saFPGA.sin_addr.S_un.S_addr = inet_addr(addr);
	saFPGA.sin_port = htons(nPort);
	return TRUE;
}

BOOL CGigabitEthernetDevice::SetStorePath(CString strPath)
{
	strFileStorePath = strPath;
	//路径不存在则自动创建
	int pos = strPath.Find('\\');
	while(pos>0)
	{
		pos = strPath.Find('\\',pos+1);
		if (pos>0)
		{
			CString strSub = strPath.Left(pos);
			CreateDirectory(strSub,NULL);
		}
	}
	strFileStoreName.Format(_T("RecvData.dat"));
	strFileStoreDir = strFileStorePath + strFileStoreName;
	return TRUE;
}

BOOL CGigabitEthernetDevice::SetFramesInOnePackage(int nNumFrames)
{
	nNumFramesInOnePackage = nNumFrames;
	return TRUE;
}

BOOL CGigabitEthernetDevice::SetRecvSize(int nSize)
{
	nRecvSize = nSize;
	return TRUE;
}

//PC->FPGA:send_cmd+recv_ack
int CGigabitEthernetDevice::SendTo(char *pBufSend,int nSizeBufSend)
{
	//发送采用阻塞方式（发送时间短而且不存在发送不出去的情况）
	sendto(m_socket,pBufSend,nSizeBufSend,0,(SOCKADDR*)&saFPGA,sizeof(SOCKADDR));
	return 0x00;

	//非阻塞方式
	int nTimeWait = 10000;
	wsaBufSend.buf = pBufSend;
	wsaBufSend.len = nSizeBufSend;
	DWORD flags = 0;
	int nLenSOCKADDR = sizeof(SOCKADDR);

	if (m_socket==INVALID_SOCKET)
	{
		TRACE("发送失败，无效套接字！\n");
		return INVALID_SOCKET;
	}
	else if (overlapSend.hEvent==INVALID_HANDLE_VALUE)
	{
		TRACE("发送失败，无效重叠端口事件句柄！\n");
		return -1;
	}

	int nRet = WSASendTo(m_socket,&wsaBufSend,1,NULL,flags,(SOCKADDR*)&saFPGA,nLenSOCKADDR,&overlapSend,NULL);
	DWORD dw_Send = WSAWaitForMultipleEvents(1,&overlapSend.hEvent,TRUE,nTimeWait,FALSE);
	if (bStopTransfer){
		return -2;
	}
	if (dw_Send == WSA_WAIT_TIMEOUT)
	{
		AfxMessageBox(_T("SendTo发送数据超时！"));
		//RunInfo<<"Semd Cmd:Send cmd failure-TimeOut!\n";
		//TRACE("Send cmd Failure-Timeout！\n");
		return -3;
	}
	WSAResetEvent(overlapSend.hEvent);
	if (dw_Send == WSA_WAIT_EVENT_0)
	{
		//RunInfo<<"Send Cmd:Send cmd success!\n";
		//TRACE("Send cmd success！\n");
		return 0;
	}
}

//PC->FPGA:recv_data+send_ack
int CGigabitEthernetDevice::RecvFrom(char *pBufRecvPackage,int nSizeBufRecv,int nTimeWait = 20000)
{
	CWnd* pWnd = CWnd::FromHandle(m_hMainDlg);
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pWnd);
	//CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pMainDlgTemp);

	ZeroMemory(pBufRecvPackage,nSizeBufRecv);

	//TRACE("开始接收！\n");
	//CString strMes = "开始接收数据！";
	//pMainDlg->DisplayRunningInfo(strMes);
	//pMainDlg->Test();

	int nRet = 0;
	int receivebytes = 0;
	//接收源端地址信息
	int nLenSOCKADDR = sizeof(SOCKADDR);
	DWORD flags = 0;

	//接收第一帧数据的超时时间（第一次接收完成后程序会自动修改超时时间）
	DWORD dwWaitTime = nTimeWait;

	int nFrameIndexCount = 0;		//帧计数
	int nFrameIndexReal = 0;		//从接收数据中获取的帧编号
	int nFrameIndexLast = 0;		//上一帧数据的帧编号

	int nNumLost = 0;				//丢包次数计数
	int nNumTimeOut = 0;			//超时次数计数
	int nPackageIndexTimeOut = 0;	//发送了超时重传指令就不需要再次发送丢失重传指令
	int nPackageIndexReal = 0;		//实际包编号
	SOCKADDR_IN saTemp;

	while(1)
	{
		//检测到停止接收命令则退出
		if (bStopTransfer){
			TRACE("Transfer stop Break1\n");
			break;
		}
		wsaBufRecv.buf = pBufRecv;
		wsaBufRecv.len = nLenBufRecv;
		int nRet = WSARecvFrom(m_socket,&wsaBufRecv,1,NULL,&flags,(SOCKADDR*)&saTemp,&nLenSOCKADDR,&overlapRecv,NULL);
		int lastErr = WSAGetLastError();
		if (nRet == SOCKET_ERROR && WSA_IO_PENDING!= lastErr){
			TRACE("WSARecvFrom error\n");
			return nRet;
		}

		DWORD dw_Recv = WSAWaitForMultipleEvents(1,&overlapRecv.hEvent,TRUE,dwWaitTime,FALSE);
		WSAResetEvent(overlapRecv.hEvent);
		if (dw_Recv == WSA_WAIT_TIMEOUT)
		{
			if (bStopTransfer){
				TRACE("Transfer Stop, exit from TimeOut!\n");
				break;
			}
			//输出超时信息
			TRACE("超时了\n");
			CTime m_Time1;
			m_Time1 = CTime::GetCurrentTime();
			CString strMes1;
			strMes1.Format(_T("%s:TimeOut Retry!,nIndexCount:%d,nIndexReal:%d,nPackageIndexReal:%d"),m_Time1.Format(_T("%H:%M:%S")),nFrameIndexCount,nFrameIndexReal,nPackageIndexReal);
			//RunInfo<<strMes1;
			//RunInfo<<", set nIndexCount = 0\n";

			pMainDlg->DisplayRunningInfo(strMes1);

			//接收到数据超时
			if (nFrameIndexCount>0){
				nPackageIndexTimeOut = nPackageIndexReal;
				//RunInfo<<"TimeOut:nPackageIndexTimeOut = "<<nPackageIndexTimeOut<<endl;
			}
			//没有接收到数据超时
			else{
			}

			//重置nFrameIndexCount、nFrameIndexLast
			nFrameIndexCount = 0;
			nFrameIndexLast = 0;

			//更新连续超时计数
			nNumTimeOut++;
			if (nNumTimeOut==2)
			{
				//RunInfo<<"TimeOut:nNumTimeOut>2";
				AfxMessageBox(_T("超时次数超过上限，接收数据失败！"));
				return 0x54;
			}

			//发送超时重传指令
			cmd_timeout[2] = nPackageIndexReal;
			wsaBufSend.buf = cmd_timeout;
			wsaBufSend.len = sizeof(cmd_timeout);
			SendTo(cmd_query,3);
			TRACE("发送超时重传指令 nIndexCount:%d,nIndexReal:%d,nPackageIndexReal:%d\n",nFrameIndexCount,nFrameIndexReal,nPackageIndexReal);
			//超时之后等待9秒 等待硬件复位
			//dwWaitTime = 10000;
		}
		else if (dw_Recv == WSA_WAIT_EVENT_0)
		{
			if (bStopTransfer)
			{
				TRACE("Transfer Stop, Break form Event0\n");
				break;
			}

			TransmissionRate.GetBeginTime();

			/*if (pMainDlg->bTimerBegin == FALSE)
			{
				pMainDlg->bTimerBegin = TRUE;
				CTime m_Time1;
				m_Time1 = CTime::GetCurrentTime();
				CString strMes1;
				strMes1.Format("%s\n",m_Time1.Format("%Y-%m-%d %H:%M:%S"));
				pMainDlg->SetDlgItemTextA(IDC_STATIC_MES1,strMes1);
			}*/
			DWORD bytes = 0;
			if (!WSAGetOverlappedResult(m_socket,&overlapRecv,&bytes,FALSE,&flags)){
				int lastErr = WSAGetLastError();
				TRACE("WSAGetOverlappedResult error %d\n",lastErr);
				//连接被意外关闭
				if (lastErr == WSAECONNRESET){
					TRACE("Connection was reset.\n");
					AfxMessageBox(_T("Connection was reset!"));
				}
				TRACE("Break From here!\n");
				break;
			}
			TRACE("bytes = %d\n", bytes);
			//重新设置超时时间
			//dwWaitTime = 10000;

			//获取实际的帧编号
			nPackageIndexReal = (int)((unsigned char)pBufRecv[0]);
			nFrameIndexReal = (int)((unsigned char)pBufRecv[1]);
			TRACE("接收到的帧的包编号：%d 帧编号：%d\n",nPackageIndexReal,nFrameIndexReal);
			
			//if (nFrameIndexReal == 0 || nFrameIndexReal == 1)
			//{
			//	for (int i = 0; i < 1000; i++)
			//	{
			//		TRACE("%d ", (int)(unsigned char)pBufRecv[i]);
			//		if (i % 16 == 15)
			//		{
			//			TRACE("\n");
			//		}
			//	}
			//}

			//将接收到的数据保存到缓存中
			memcpy(pBufRecvPackage+(nFrameIndexReal-1)*(nLenBufRecv-2),pBufRecv+2,nLenBufRecv-2);
			receivebytes += (nLenBufRecv-2);
			/*memcpy(pBufRecvPackage+(nFrameIndexReal-1)*(nLenBufRecv),pBufRecv,nLenBufRecv);
			receivebytes += (nLenBufRecv);*/

			if (nPackageIndexReal==0x00&&nFrameIndexReal==0x00)
			{
				TRACE("收到0帧\n");
				continue;
			}

			//判断帧编号是否异常
			if ((nFrameIndexReal<1) || (nFrameIndexReal>nNumFramesInOnePackage))
			{
				CString strErrorInfo;
				strErrorInfo.Format(_T("从帧头获取的帧编号异常nFrameIndexReal:%d nFrameIndexLast:%d"),nFrameIndexReal,nFrameIndexLast);
				TRACE("从帧头获取的帧编号异常nFrameIndexReal:%d nFrameIndexLast:%d",nFrameIndexReal,nFrameIndexLast);
				AfxMessageBox(strErrorInfo);
				break;
			}
			//帧编号为1则重置nFrameIndexCount、nFrameIndexLast
			if (nFrameIndexReal == 1)
			{
				nFrameIndexCount = 0;
				nFrameIndexLast = 0;
				receivebytes = 0;
				//RunInfo<<"Recv: nIndexReal == 1,set nIndexCount = 0, nIndexLast:"<<nFrameIndexLast<<endl;
			}

			//如果前后帧编号差值为1 帧计数加1
			if (nFrameIndexReal - nFrameIndexLast == 1){
				nFrameIndexCount++;
			}
			else{
				nFrameIndexCount = 0;
				TRACE("丢帧了\n");
			}
			//记录当前帧的帧编号
			nFrameIndexLast = nFrameIndexReal;

			//检查帧计数是否异常
			if (nFrameIndexCount > nNumFramesInOnePackage){
				CString strErrorInfo;
				strErrorInfo.Format(_T("帧计数异常nFrameIndexCount:%d!"),nFrameIndexCount);
				AfxMessageBox(strErrorInfo);
				break;
			}

			//检测到帧编号为最后一帧
			if (nNumFramesInOnePackage == nFrameIndexReal)
			{
				//RunInfo<<"Recv: nIndexReal==255,set nIndexCount = 0\n";
				//如果帧计数同最后一帧帧编号一致 则接收无丢包
				if (nFrameIndexCount == nNumFramesInOnePackage)
				{
					//CTime m_Time1;
					//m_Time1 = CTime::GetCurrentTime();
					//CString strMes1;
					//strMes1.Format("%s:Recv Package! nIndexCount：%d\n",m_Time1.Format("%H:%M:%S"),nFrameIndexCount);
					//RunInfo<<strMes1;

					//检查包编号是否相同 如果相同则表示收到重复包
					if (nPackageIndexLastStore == nPackageIndexReal)
					{
						//RunInfo<<"重复数据包-"<<nPackageIndexReal<<endl;
					}
					else
					{
						/*if(nPackageIndexLastStore == 200)
						{
							cmd_failure[1] = nPackageIndexReal;
							wsaBufSend.buf = cmd_failure;
							wsaBufSend.len = sizeof(cmd_failure);
							if (bStopTransfer)
							{
								TRACE("Transfer Stop, forbidden send cmd_failure!\n");
								break;
							}
							WSASendTo(m_socket,&wsaBufSend,1,NULL,flags,(SOCKADDR*)&saFPGA,nLenSOCKADDR,&overlapSend,NULL);
							DWORD dw_SendCmd_Failure = WSAWaitForMultipleEvents(1,&overlapSend.hEvent,TRUE,20,FALSE);
							if (bStopTransfer){
								TRACE("Transfer Stop, exit from send cmd_failure!\n");
								break;
							}
							if (dw_SendCmd_Failure == WSA_WAIT_FAILED || dw_SendCmd_Failure == WSA_WAIT_TIMEOUT)
							{
								AfxMessageBox("Send cmd_failure timeout！");
							}
							WSAResetEvent(overlapSend.hEvent);
							if (dw_SendCmd_Failure == WSA_WAIT_EVENT_0)
							{
								RunInfo<<"Recv Lost:Send cmd success!\n\n";
							}
							break;
						}*/
						//保存数据
						//分多个文件存储
						/*strFileStoreName.Format("Package%d.dat",nNumPackets2Store++);
						strFileStoreDir = strFileStorePath + strFileStoreName;
						//RunInfo<<"Store File - "<<strFileStoreName<<endl;
						//RunInfo<<"nPackageIndex = "<<nPackageIndexReal<<endl;
						FileStore.Open(strFileStoreDir,CFile::modeCreate | CFile::modeWrite);
						FileStore.Write(pBufRecvPackage,nLenFrame*nNumFramesInOnePackage);
						FileStore.Close();*/

						//存入一个文件
						//RunInfo<<"Store File - "<<strFileStoreName<<endl;
						//RunInfo<<"nPackageIndex = "<<nPackageIndexReal<<endl;
						FileStore.Open(strFileStoreDir,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone);
						FileStore.SeekToEnd();
						//FileStore.Write(pBufRecvPackage,nLenFrame * nNumFramesInOnePackage);
						FileStore.Write(pBufRecvPackage,(nLenFrame-2)*nNumFramesInOnePackage);
						FileStore.Close();

						llPackagesRecv++;

						nPackageIndexLastStore = nPackageIndexReal;

						//计算实时速率
						double dTransRate = TransmissionRate.GetTramsmissionRate(llPackagesRecv,nNumFramesInOnePackage);
						CString strTransRate;
						strTransRate.Format(_T("%.2fMbps"),dTransRate);

						pMainDlg->SetDlgItemText(IDS_TRANSFER_RATE,strTransRate);

						if (nNumPackets2Store == 10001)
						{
							nNumPackets2Store = 1;
						}

						//重置连续超时计数
						nNumTimeOut = 0;
						nNumLost = 0;

						//Send cmd_success 只有收到非重复包才通知FPGA包编号加一
						cmd_success[2] = nPackageIndexReal;
						wsaBufSend.buf = cmd_success;
						wsaBufSend.len = sizeof(cmd_success);
						if (bStopTransfer)
						{
							TRACE("Transfer Stop, forbidden send cmd_success!\n");
							break;
						}
						SendTo(cmd_success,3);
						
						//如果接收满数据则退出
						//if (llPackagesRecv == nRecvSize)
						//{
						TRACE("接收完一包数据,nPackageIndexReal:%d\n", nPackageIndexReal);
							return 0x55;
						//}
					}
				}
				else
				{
					FileStore.Open(strFileStoreDir,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone);
					FileStore.SeekToEnd();
					FileStore.Write(pBufRecvPackage,nLenFrame * nNumFramesInOnePackage);
					//FileStore.Write(pBufRecvPackage,(nLenFrame-2)*nNumFramesInOnePackage);
					FileStore.Close();
					TRACE("丢失了%d帧\n", nNumFramesInOnePackage - nFrameIndexCount);

					//丢帧也发送成功，测试用
					/*cmd_success[2] = nPackageIndexReal;
					wsaBufSend.buf = cmd_success;
					wsaBufSend.len = sizeof(cmd_success);
					if (bStopTransfer)
					{
						TRACE("Transfer Stop, forbidden send cmd_success!\n");
						break;
					}
					SendTo(cmd_success,3);*/
					//有丢包 接收失败！
					//如果对于同一包数据已经发送了超时重传指令 则不再发送丢包重传指令
					if (nPackageIndexTimeOut == nPackageIndexReal)
					{
						CString strMes1;
						strMes1.Format(_T("对于同一包数据超时重传之后的不进行丢包重传！"));
						pMainDlg->DisplayRunningInfo(strMes1);
					} 
					else
					{
						CTime m_Time1;
						m_Time1 = CTime::GetCurrentTime();
						CString strMes1;
						strMes1.Format(_T("%s:Lost Frames! nIndexCount：%d\n"),m_Time1.Format("%H:%M:%S"),nFrameIndexCount);
						pMainDlg->DisplayRunningInfo(strMes1);

						//更新丢包次数
						nNumLost++;
						if (nNumLost==2)
						{
							//RunInfo<<"Lost:nNumLost>2";
							AfxMessageBox(_T("丢包重传次数超过上限，接收数据失败！"));
							return 0x53;
						}

						cmd_failure[2] = nPackageIndexReal;
						wsaBufSend.buf = cmd_failure;
						wsaBufSend.len = sizeof(cmd_failure);
						if (bStopTransfer)
						{
							TRACE("Transfer Stop, forbidden send cmd_failure!\n");
							break;
						}
						SendTo(cmd_failure,3);

						//丢包之后等待9秒 等待硬件复位
						//dwWaitTime = 9000;
					}					
				}
				nFrameIndexCount = 0;
				nFrameIndexLast = 0;
			}//编号为最后一帧
		}//Event0
		else
		{
			nFrameIndexCount = 0;
			TRACE("Recv: Unkown Error,set nIndexCount = 0\n");
		}
	}
	//FileTest.Close();
	TRACE("退出RecvFrom函数!\n");
	return nRet;
}

/*
void PutOutSystemMes(CInterfaceClassDlg *MainDlg_GEIUDP,CString str_Mes)
{
	if (str_SystemMes=="")
	{
		str_SystemMes = str_SystemMes + str_Mes;
	}
	else
	{
		str_SystemMes = str_SystemMes + "\r\n" + str_Mes;
	}
	MainDlg_GEIUDP->SetDlgItemText(IDC_EDIT_SYSMES,str_SystemMes);
}*/


int CGigabitEthernetDevice::ClearSocketBuffer()
{
	CWnd* pWnd = CWnd::FromHandle(m_hMainDlg);
	CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pWnd);
	//CPerformanceEvaluationSystemDlg *pMainDlg = static_cast<CPerformanceEvaluationSystemDlg*>(pMainDlgTemp);
	//从接收缓存中读取的数据
	int nRetVal = 0;
	//超时等待时间0.2秒
	DWORD dwWaitTime = 100;

	//接收源端地址信息
	int nLenSOCKADDR = sizeof(SOCKADDR);
	DWORD flags = 0;

	//////////////////////////////////////////////////////////////////////////
	//注意：
	//		由于File的Close在while之后 可能由于没有Close
	//		而再次使用Open对文件进行操作出现访问冲突
	//////////////////////////////////////////////////////////////////////////
	//无效数据存放位置
	//CString strFileClearBuf;
	//strFileClearBuf = "F:\\InvalideData.dat";
	//FileStore.Open(strFileClearBuf,CFile::modeCreate | CFile::modeWrite);
	
	SOCKADDR_IN saTemp;
	int nBufTempSize = 32*1024*1024;
	char *pBufTemp = new char[nBufTempSize];
	WSABUF wsaBufTemp;
	wsaBufTemp.buf = pBufTemp;
	wsaBufTemp.len = nBufTempSize;

	while(1)
	{
		int nRet = WSARecvFrom(m_socket,&wsaBufTemp,1,NULL,&flags,(SOCKADDR*)&saTemp,&nLenSOCKADDR,&overlapRecv,NULL);
		int lastErr = WSAGetLastError();
		if (nRet == SOCKET_ERROR && WSA_IO_PENDING!= lastErr){
			TRACE("WSARecvFrom error\n");
			return nRetVal;
		}

		DWORD dw = WSAWaitForMultipleEvents(1,&overlapRecv.hEvent,FALSE,dwWaitTime,FALSE);
		WSAResetEvent(overlapRecv.hEvent);
		if (dw == WSA_WAIT_FAILED){
			AfxMessageBox(_T("Recv Failure！"));
			return nRetVal;
		}
		else if (dw == WSA_WAIT_TIMEOUT)
		{
			//输出超时信息
			CTime m_Time1;
			m_Time1 = CTime::GetCurrentTime();
			CString strMes1;
			strMes1.Format(_T("%s:Clear FIFO - DataSize:%dB!"),
				m_Time1.Format("%H:%M:%S"),nRetVal);
			pMainDlg->DisplayRunningInfo(strMes1);
			CancelIoEx((HANDLE)m_socket, &overlapRecv);
			break;
		}
		else if (dw == WSA_WAIT_EVENT_0)
		{
			DWORD bytes = 0;
			if (!WSAGetOverlappedResult(m_socket,&overlapRecv,&bytes,FALSE,&flags)){
				int lastErr = WSAGetLastError();
				TRACE("WSAGetOverlappedResult error %d\n",lastErr);
				//连接被意外关闭
				if (lastErr == WSAECONNRESET){
					TRACE("Connection was reset.\n");
					AfxMessageBox(_T("Connection was reset!"));
				}
			}
			//保存数据
			//FileStore.Write(pBufRecv,bytes);
			nRetVal += bytes;
		}
	}
	//FileStore.Close();
	delete[] pBufTemp;
	TRACE("清空缓存清空了%dB的数据\n", nRetVal);
	TRACE("退出ClearSocketBuffer函数\n");
	return nRetVal;
}

//重叠端口进行文件IO操作
/*strFileStoreName.Format("Package%d.dat",nPacketsCount++);
						strFileStoreDir = strFileStorePath + strFileStoreName;
						hFileStore = ::CreateFileA(strFileStoreDir,
							GENERIC_ALL,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,//安全属性
							OPEN_ALWAYS,//创建或打开属性
							FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED,
							NULL);
						if (hFileStore==INVALID_HANDLE_VALUE)
						{
							AfxMessageBox("文件创建失败！");
						}

						DWORD dwLenRealWrite = 0;
						BOOL nRet = WriteFile(hFileStore,pBufRecvPackage,nLenFrame*nNumFramesInOnePackage,&dwLenRealWrite,&overlappedWrite);
						if (nRet)
						{
							//写数据完成
							TRACE("1写数据完成！\n");
						}
						else
						{
							if (GetLastError()==ERROR_IO_PENDING)
							{
								//TRACE("IO_PENDING\n");
								WaitForSingleObject(hFileStore,INFINITE);
								DWORD dwNumWrite = 0;
								BOOL nOverlapRet = GetOverlappedResult(hFileStore,&overlappedWrite,&dwNumWrite,FALSE);
								//TRACE("dwNumWrite:%d\n",dwNumWrite);
							}
							else
							{
								TRACE("I/O出错！\n");
							}
						}
						//::FlushFileBuffers(hFileStore);
						
						if (hFileStore!=INVALID_HANDLE_VALUE)
						{
							CloseHandle(hFileStore);
							}*/


int CGigabitEthernetDevice::GetReceivedPackage(void)
{
	return llPackagesRecv;
}
