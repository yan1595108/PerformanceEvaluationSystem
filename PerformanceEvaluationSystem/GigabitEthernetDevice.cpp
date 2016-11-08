#pragma once
#include "StdAfx.h"
#include "GigabitEthernetDevice.h"
#include "PerformanceEvaluationSystem.h"
#include "PerformanceEvaluationSystemDlg.h"

//int nNumFramesInOnePackage = 255;
const int nLenCmd = 2;
const int nLenFrame = 1026;
CString str_SystemMes("");

//ȫ�ֺ�������
//void PutOutSystemMes(CInterfaceClassDlg *MainDlg_GEIUDP,CString str_Mes);

///////////////////////////////////������////////////////////////////////////
//����ƽ����������
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
		AfxMessageBox(_T("���󣺳�ʼʱ��ֵΪ0��"));
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

///////////////////////////////ǧ��������ӿ���///////////////////////////////
//ǧ�����޲����
CGigabitEthernetDevice::CGigabitEthernetDevice(HWND hDlg = NULL)
	//:hMainDlg(hpMainDlg),
	//:pMainDlgTemp(pMainDlg),
	//:m_hMainDlg(hWnd),
	:m_hMainDlg(hDlg),
	bStopTransfer(FALSE),
	nPackageIndexLastStore(0),nNumPackets2Store(1),llPackagesRecv(0),
	strFileStorePath("F:\\Recv\\")
{
	//��ʼ���׽��ֵ�ַ�ṹ��
	ZeroMemory(&saFPGA,sizeof(SOCKADDR_IN));
	ZeroMemory(&saPC,sizeof(SOCKADDR_IN));

	//�ص��˿ڳ�ʼ��
	ZeroMemory(&overlapSend,sizeof(overlapSend));
	ZeroMemory(&overlapSend,sizeof(overlapSend));
	overlapSend.hEvent = INVALID_HANDLE_VALUE;
	overlapRecv.hEvent = INVALID_HANDLE_VALUE;
	m_socket = INVALID_SOCKET;

	//����ָ���ʼ��
	cmd_query[0]	= 0xe8;	cmd_query[1]	= 0x01;	cmd_query[2]	= 0xff;
	cmd_success[0]	= 0xe8;	cmd_success[1]	= 0x02;	cmd_success[2]	= 0x01;
	cmd_failure[0]	= 0xe8;	cmd_failure[1]	= 0x03; cmd_failure[2]	= 0x01;
	cmd_timeout[0]	= 0xe8;	cmd_timeout[1]	= 0x04;	cmd_timeout[2]	= 0x01;
	cmd_quit[0]		= 0xe8;	cmd_quit[1]		= 0x05;	cmd_quit[2]		= 0x00;

	//���ջ���ռ��ʼ��
	nLenBufRecv = nLenFrame;
	pBufRecv = new char[nLenBufRecv];
	ZeroMemory(pBufRecv,nLenBufRecv);
	wsaBufRecv.buf = pBufRecv;
	wsaBufRecv.len = nLenBufRecv;
	//���ͻ���ռ��ʼ��
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
	//�����׽��ֿ�
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2,2);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err!=0)
	{
		AfxMessageBox(_T("�����׽���ʧ�ܣ�"));
		return FALSE;
	}
	if (LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion)!=2)
	{
		WSACleanup();
		AfxMessageBox(_T("�޷����ؼ����׽���2.2���ð汾��֧�֣�"));
		return FALSE;
	}

	//����PC��ַ��Ϣ
	SetPCAddress("192.168.1.100",8000);
	//SetPCAddress("192.168.1.100",7000);

	//����FPGA��ַ��Ϣ
	SetFPGAAddress("192.168.1.150",8080);

	//�����ļ�����·��
	//SetStorePath("C:\\");

	nPackageIndexLastStore = 0;

	//ZeroMemory(&overlappedWrite,sizeof(OVERLAPPED));
	//����һ��WSA_FLAG_OVERLAPPED�׽���
	//WSASocket��������һ���첽IO�׽���
	m_socket = WSASocket(AF_INET,SOCK_DGRAM,IPPROTO_UDP,NULL,0,WSA_FLAG_OVERLAPPED);
	if (m_socket == INVALID_SOCKET)
	{
		TRACE("WSASocket Error:%d",WSAGetLastError());
		return FALSE;
	}

	//�׽��ֽ��ա����ͻ�������
	int nBufs_Recv,nBufs_Send;
	nBufs_Recv = 1024*1024*32; //2MB
	//nBufs_Send = 1024;//1M;
	nBufs_Send = 0;//1M;
	int retval11 = setsockopt(m_socket,SOL_SOCKET,SO_RCVBUF,(char *)&nBufs_Recv,sizeof(int)); //���ý��ջ�������С
	int retval12 = setsockopt(m_socket,SOL_SOCKET,SO_SNDBUF,(char *)&nBufs_Send,sizeof(int)); //���ý��ջ�������С
	/*BOOL bDontLinger = FALSE;
	int retval13 = setsockopt(m_socket,SOL_SOCKET,SO_DONTLINGER,(const char*)&bDontLinger,sizeof(BOOL));
	BOOL bReuseaddr = TRUE;
	int retval14 = setsockopt(m_socket,SOL_SOCKET,SO_REUSEADDR,(const char*)&bReuseaddr,sizeof(BOOL));*/
	if (retval11!=0 && retval12!=0)
	{
		AfxMessageBox(_T("���ò���1ʧ��"));
	}
	int nTimeOut = 100;
	int nRetval = setsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nTimeOut,sizeof(int));
	int nRealTimeOut = 0;
	int nlen_nRealTimeOut = sizeof(int);
	getsockopt(m_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&nRealTimeOut,&nlen_nRealTimeOut);
	TRACE(_T("��ʱʱ�䣺%d\n"),nRealTimeOut);

	//����bind�ѱ����׽��ֵ�ַ�󶨵������׽���
	if (SOCKET_ERROR == bind(m_socket,(sockaddr*)&saPC,sizeof(sockaddr_in)))
	{
		TRACE("Bind Error %d\n",WSAGetLastError());
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
		return FALSE;
	}

	//�����׽����¼�
	overlapSend.hEvent = WSACreateEvent();
	overlapRecv.hEvent = WSACreateEvent();

	if (overlapSend.hEvent==INVALID_HANDLE_VALUE || overlapRecv.hEvent==INVALID_HANDLE_VALUE)
	{
		TRACE("WSACreateEvent Error!\n");
	}
}

BOOL CGigabitEthernetDevice::Destroy()
{
	TRACE("��ʼ������Դ��\n");
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
	TRACE("������Դ�ɹ���\n\n");
	return TRUE;
}

BOOL CGigabitEthernetDevice::Open()
{
	bStopTransfer = FALSE;
	nPackageIndexLastStore = 0;
	nNumPackets2Store = 1;

	//�������ิλ ����ͳ�ƴ�������
	llPackagesRecv = 0;
	TransmissionRate.Reset();

	return TRUE;
}

BOOL CGigabitEthernetDevice::Close()
{
	bStopTransfer = TRUE;
	if (overlapSend.hEvent == INVALID_HANDLE_VALUE)
	{
		TRACE("�����¼���������Ч��\n");
	}
	if (overlapRecv.hEvent == INVALID_HANDLE_VALUE)
	{
		TRACE("�����¼���������Ч��\n");
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
	//·�����������Զ�����
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
	//���Ͳ���������ʽ������ʱ��̶��Ҳ����ڷ��Ͳ���ȥ�������
	sendto(m_socket,pBufSend,nSizeBufSend,0,(SOCKADDR*)&saFPGA,sizeof(SOCKADDR));
	return 0x00;

	//��������ʽ
	int nTimeWait = 10000;
	wsaBufSend.buf = pBufSend;
	wsaBufSend.len = nSizeBufSend;
	DWORD flags = 0;
	int nLenSOCKADDR = sizeof(SOCKADDR);

	if (m_socket==INVALID_SOCKET)
	{
		TRACE("����ʧ�ܣ���Ч�׽��֣�\n");
		return INVALID_SOCKET;
	}
	else if (overlapSend.hEvent==INVALID_HANDLE_VALUE)
	{
		TRACE("����ʧ�ܣ���Ч�ص��˿��¼������\n");
		return -1;
	}

	int nRet = WSASendTo(m_socket,&wsaBufSend,1,NULL,flags,(SOCKADDR*)&saFPGA,nLenSOCKADDR,&overlapSend,NULL);
	DWORD dw_Send = WSAWaitForMultipleEvents(1,&overlapSend.hEvent,TRUE,nTimeWait,FALSE);
	if (bStopTransfer){
		return -2;
	}
	if (dw_Send == WSA_WAIT_TIMEOUT)
	{
		AfxMessageBox(_T("SendTo�������ݳ�ʱ��"));
		//RunInfo<<"Semd Cmd:Send cmd failure-TimeOut!\n";
		//TRACE("Send cmd Failure-Timeout��\n");
		return -3;
	}
	WSAResetEvent(overlapSend.hEvent);
	if (dw_Send == WSA_WAIT_EVENT_0)
	{
		//RunInfo<<"Send Cmd:Send cmd success!\n";
		//TRACE("Send cmd success��\n");
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

	//TRACE("��ʼ���գ�\n");
	//CString strMes = "��ʼ�������ݣ�";
	//pMainDlg->DisplayRunningInfo(strMes);
	//pMainDlg->Test();

	int nRet = 0;
	int receivebytes = 0;
	//����Դ�˵�ַ��Ϣ
	int nLenSOCKADDR = sizeof(SOCKADDR);
	DWORD flags = 0;

	//���յ�һ֡���ݵĳ�ʱʱ�䣨��һ�ν�����ɺ������Զ��޸ĳ�ʱʱ�䣩
	DWORD dwWaitTime = nTimeWait;

	int nFrameIndexCount = 0;		//֡����
	int nFrameIndexReal = 0;		//�ӽ��������л�ȡ��֡���
	int nFrameIndexLast = 0;		//��һ֡���ݵ�֡���

	int nNumLost = 0;				//������������
	int nNumTimeOut = 0;			//��ʱ��������
	int nPackageIndexTimeOut = 0;	//�����˳�ʱ�ش�ָ��Ͳ���Ҫ�ٴη��Ͷ�ʧ�ش�ָ��
	int nPackageIndexReal = 0;		//ʵ�ʰ����
	SOCKADDR_IN saTemp;

	while(1)
	{
		//��⵽ֹͣ�����������˳�
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
			//�����ʱ��Ϣ
			TRACE("��ʱ��\n");
			CTime m_Time1;
			m_Time1 = CTime::GetCurrentTime();
			CString strMes1;
			strMes1.Format(_T("%s:TimeOut Retry!,nIndexCount:%d,nIndexReal:%d,nPackageIndexReal:%d"),m_Time1.Format(_T("%H:%M:%S")),nFrameIndexCount,nFrameIndexReal,nPackageIndexReal);
			//RunInfo<<strMes1;
			//RunInfo<<", set nIndexCount = 0\n";

			pMainDlg->DisplayRunningInfo(strMes1);

			//���յ����ݳ�ʱ
			if (nFrameIndexCount>0){
				nPackageIndexTimeOut = nPackageIndexReal;
				//RunInfo<<"TimeOut:nPackageIndexTimeOut = "<<nPackageIndexTimeOut<<endl;
			}
			//û�н��յ����ݳ�ʱ
			else{
			}

			//����nFrameIndexCount��nFrameIndexLast
			nFrameIndexCount = 0;
			nFrameIndexLast = 0;

			//����������ʱ����
			nNumTimeOut++;
			if (nNumTimeOut==2)
			{
				//RunInfo<<"TimeOut:nNumTimeOut>2";
				AfxMessageBox(_T("��ʱ�����������ޣ���������ʧ�ܣ�"));
				return 0x54;
			}

			//���ͳ�ʱ�ش�ָ��
			cmd_timeout[2] = nPackageIndexReal;
			wsaBufSend.buf = cmd_timeout;
			wsaBufSend.len = sizeof(cmd_timeout);
			SendTo(cmd_query,3);
			TRACE("���ͳ�ʱ�ش�ָ�� nIndexCount:%d,nIndexReal:%d,nPackageIndexReal:%d\n",nFrameIndexCount,nFrameIndexReal,nPackageIndexReal);
			//��ʱ֮��ȴ�9�� �ȴ�Ӳ����λ
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
				//���ӱ�����ر�
				if (lastErr == WSAECONNRESET){
					TRACE("Connection was reset.\n");
					AfxMessageBox(_T("Connection was reset!"));
				}
				TRACE("Break From here!\n");
				break;
			}
			TRACE("bytes = %d\n", bytes);
			//�������ó�ʱʱ��
			//dwWaitTime = 10000;

			//��ȡʵ�ʵ�֡���
			nPackageIndexReal = (int)((unsigned char)pBufRecv[0]);
			nFrameIndexReal = (int)((unsigned char)pBufRecv[1]);
			TRACE("���յ���֡�İ���ţ�%d ֡��ţ�%d\n",nPackageIndexReal,nFrameIndexReal);
			
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

			//�����յ������ݱ��浽������
			memcpy(pBufRecvPackage+(nFrameIndexReal-1)*(nLenBufRecv-2),pBufRecv+2,nLenBufRecv-2);
			receivebytes += (nLenBufRecv-2);
			/*memcpy(pBufRecvPackage+(nFrameIndexReal-1)*(nLenBufRecv),pBufRecv,nLenBufRecv);
			receivebytes += (nLenBufRecv);*/

			if (nPackageIndexReal==0x00&&nFrameIndexReal==0x00)
			{
				TRACE("�յ�0֡\n");
				continue;
			}

			//�ж�֡����Ƿ��쳣
			if ((nFrameIndexReal<1) || (nFrameIndexReal>nNumFramesInOnePackage))
			{
				CString strErrorInfo;
				strErrorInfo.Format(_T("��֡ͷ��ȡ��֡����쳣nFrameIndexReal:%d nFrameIndexLast:%d"),nFrameIndexReal,nFrameIndexLast);
				TRACE("��֡ͷ��ȡ��֡����쳣nFrameIndexReal:%d nFrameIndexLast:%d",nFrameIndexReal,nFrameIndexLast);
				AfxMessageBox(strErrorInfo);
				break;
			}
			//֡���Ϊ1������nFrameIndexCount��nFrameIndexLast
			if (nFrameIndexReal == 1)
			{
				nFrameIndexCount = 0;
				nFrameIndexLast = 0;
				receivebytes = 0;
				//RunInfo<<"Recv: nIndexReal == 1,set nIndexCount = 0, nIndexLast:"<<nFrameIndexLast<<endl;
			}

			//���ǰ��֡��Ų�ֵΪ1 ֡������1
			if (nFrameIndexReal - nFrameIndexLast == 1){
				nFrameIndexCount++;
			}
			else{
				nFrameIndexCount = 0;
				TRACE("��֡��\n");
			}
			//��¼��ǰ֡��֡���
			nFrameIndexLast = nFrameIndexReal;

			//���֡�����Ƿ��쳣
			if (nFrameIndexCount > nNumFramesInOnePackage){
				CString strErrorInfo;
				strErrorInfo.Format(_T("֡�����쳣nFrameIndexCount:%d!"),nFrameIndexCount);
				AfxMessageBox(strErrorInfo);
				break;
			}

			//��⵽֡���Ϊ���һ֡
			if (nNumFramesInOnePackage == nFrameIndexReal)
			{
				//RunInfo<<"Recv: nIndexReal==255,set nIndexCount = 0\n";
				//���֡����ͬ���һ֡֡���һ�� ������޶���
				if (nFrameIndexCount == nNumFramesInOnePackage)
				{
					//CTime m_Time1;
					//m_Time1 = CTime::GetCurrentTime();
					//CString strMes1;
					//strMes1.Format("%s:Recv Package! nIndexCount��%d\n",m_Time1.Format("%H:%M:%S"),nFrameIndexCount);
					//RunInfo<<strMes1;

					//��������Ƿ���ͬ �����ͬ���ʾ�յ��ظ���
					if (nPackageIndexLastStore == nPackageIndexReal)
					{
						//RunInfo<<"�ظ����ݰ�-"<<nPackageIndexReal<<endl;
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
								AfxMessageBox("Send cmd_failure timeout��");
							}
							WSAResetEvent(overlapSend.hEvent);
							if (dw_SendCmd_Failure == WSA_WAIT_EVENT_0)
							{
								RunInfo<<"Recv Lost:Send cmd success!\n\n";
							}
							break;
						}*/
						//��������
						//�ֶ���ļ��洢
						/*strFileStoreName.Format("Package%d.dat",nNumPackets2Store++);
						strFileStoreDir = strFileStorePath + strFileStoreName;
						//RunInfo<<"Store File - "<<strFileStoreName<<endl;
						//RunInfo<<"nPackageIndex = "<<nPackageIndexReal<<endl;
						FileStore.Open(strFileStoreDir,CFile::modeCreate | CFile::modeWrite);
						FileStore.Write(pBufRecvPackage,nLenFrame*nNumFramesInOnePackage);
						FileStore.Close();*/

						//����һ���ļ�
						//RunInfo<<"Store File - "<<strFileStoreName<<endl;
						//RunInfo<<"nPackageIndex = "<<nPackageIndexReal<<endl;
						FileStore.Open(strFileStoreDir,CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate | CFile::shareDenyNone);
						FileStore.SeekToEnd();
						//FileStore.Write(pBufRecvPackage,nLenFrame * nNumFramesInOnePackage);
						FileStore.Write(pBufRecvPackage,(nLenFrame-2)*nNumFramesInOnePackage);
						FileStore.Close();

						llPackagesRecv++;

						nPackageIndexLastStore = nPackageIndexReal;

						//����ʵʱ����
						double dTransRate = TransmissionRate.GetTramsmissionRate(llPackagesRecv,nNumFramesInOnePackage);
						CString strTransRate;
						strTransRate.Format(_T("%.2fMbps"),dTransRate);

						pMainDlg->SetDlgItemText(IDS_TRANSFER_RATE,strTransRate);

						if (nNumPackets2Store == 10001)
						{
							nNumPackets2Store = 1;
						}

						//����������ʱ����
						nNumTimeOut = 0;
						nNumLost = 0;

						//Send cmd_success ֻ���յ����ظ�����֪ͨFPGA����ż�һ
						cmd_success[2] = nPackageIndexReal;
						wsaBufSend.buf = cmd_success;
						wsaBufSend.len = sizeof(cmd_success);
						if (bStopTransfer)
						{
							TRACE("Transfer Stop, forbidden send cmd_success!\n");
							break;
						}
						SendTo(cmd_success,3);
						
						//����������������˳�
						//if (llPackagesRecv == nRecvSize)
						//{
						TRACE("������һ������,nPackageIndexReal:%d\n", nPackageIndexReal);
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
					TRACE("��ʧ��%d֡\n", nNumFramesInOnePackage - nFrameIndexCount);

					//��֡Ҳ���ͳɹ���������
					/*cmd_success[2] = nPackageIndexReal;
					wsaBufSend.buf = cmd_success;
					wsaBufSend.len = sizeof(cmd_success);
					if (bStopTransfer)
					{
						TRACE("Transfer Stop, forbidden send cmd_success!\n");
						break;
					}
					SendTo(cmd_success,3);*/
					//�ж��� ����ʧ�ܣ�
					//�������ͬһ�������Ѿ������˳�ʱ�ش�ָ�� ���ٷ��Ͷ����ش�ָ��
					if (nPackageIndexTimeOut == nPackageIndexReal)
					{
						CString strMes1;
						strMes1.Format(_T("����ͬһ�����ݳ�ʱ�ش�֮��Ĳ����ж����ش���"));
						pMainDlg->DisplayRunningInfo(strMes1);
					} 
					else
					{
						CTime m_Time1;
						m_Time1 = CTime::GetCurrentTime();
						CString strMes1;
						strMes1.Format(_T("%s:Lost Frames! nIndexCount��%d\n"),m_Time1.Format("%H:%M:%S"),nFrameIndexCount);
						pMainDlg->DisplayRunningInfo(strMes1);

						//���¶�������
						nNumLost++;
						if (nNumLost==2)
						{
							//RunInfo<<"Lost:nNumLost>2";
							AfxMessageBox(_T("�����ش������������ޣ���������ʧ�ܣ�"));
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

						//����֮��ȴ�9�� �ȴ�Ӳ����λ
						//dwWaitTime = 9000;
					}					
				}
				nFrameIndexCount = 0;
				nFrameIndexLast = 0;
			}//���Ϊ���һ֡
		}//Event0
		else
		{
			nFrameIndexCount = 0;
			TRACE("Recv: Unkown Error,set nIndexCount = 0\n");
		}
	}
	//FileTest.Close();
	TRACE("�˳�RecvFrom����!\n");
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
	//�ӽ��ջ����ж�ȡ������
	int nRetVal = 0;
	//��ʱ�ȴ�ʱ��0.2��
	DWORD dwWaitTime = 100;

	//����Դ�˵�ַ��Ϣ
	int nLenSOCKADDR = sizeof(SOCKADDR);
	DWORD flags = 0;

	//////////////////////////////////////////////////////////////////////////
	//ע�⣺
	//		����File��Close��while֮�� ��������û��Close
	//		���ٴ�ʹ��Open���ļ����в������ַ��ʳ�ͻ
	//////////////////////////////////////////////////////////////////////////
	//��Ч���ݴ��λ��
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
			AfxMessageBox(_T("Recv Failure��"));
			return nRetVal;
		}
		else if (dw == WSA_WAIT_TIMEOUT)
		{
			//�����ʱ��Ϣ
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
				//���ӱ�����ر�
				if (lastErr == WSAECONNRESET){
					TRACE("Connection was reset.\n");
					AfxMessageBox(_T("Connection was reset!"));
				}
			}
			//��������
			//FileStore.Write(pBufRecv,bytes);
			nRetVal += bytes;
		}
	}
	//FileStore.Close();
	delete[] pBufTemp;
	TRACE("��ջ��������%dB������\n", nRetVal);
	TRACE("�˳�ClearSocketBuffer����\n");
	return nRetVal;
}

//�ص��˿ڽ����ļ�IO����
/*strFileStoreName.Format("Package%d.dat",nPacketsCount++);
						strFileStoreDir = strFileStorePath + strFileStoreName;
						hFileStore = ::CreateFileA(strFileStoreDir,
							GENERIC_ALL,
							FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL,//��ȫ����
							OPEN_ALWAYS,//�����������
							FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_SYSTEM | FILE_FLAG_OVERLAPPED,
							NULL);
						if (hFileStore==INVALID_HANDLE_VALUE)
						{
							AfxMessageBox("�ļ�����ʧ�ܣ�");
						}

						DWORD dwLenRealWrite = 0;
						BOOL nRet = WriteFile(hFileStore,pBufRecvPackage,nLenFrame*nNumFramesInOnePackage,&dwLenRealWrite,&overlappedWrite);
						if (nRet)
						{
							//д�������
							TRACE("1д������ɣ�\n");
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
								TRACE("I/O����\n");
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
