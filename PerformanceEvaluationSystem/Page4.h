#pragma once
#include "PageBase.h"
#include "afxwin.h"

extern const int nLenFrame;

// CPage4 �Ի���

typedef struct _THREADPARAM
{
	CPageBase* pPage4Dlg;
	_THREADPARAM()
	{
		pPage4Dlg = NULL;
	};

}THREADPARAM,*PTHREADPARAM;

class CPage4 : public CPageBase
{
	DECLARE_DYNAMIC(CPage4)

public:
	CPage4(CWnd* pParent = NULL);		// ��׼���캯��
	virtual ~CPage4();

public:
	LPVOID pTempMainDlg;				//���Խ���ָ��

public:
	volatile BOOL bThreadStopRecv;		//ֹͣ���ձ�־
	HANDLE hThreadRecv;					//�����߳̾��
	DWORD dwThreadRecvID;				//�����߳�ID
	volatile BOOL bThreadStopPlot;		//ֹͣ��ͼ��־
	HANDLE hThreadPlot;					//��ͼ�߳�
	DWORD dwThreadPlotID;				//��ͼ�߳�ID
	THREADPARAM ThreadRecvParam;		//�̲߳���

//�ؼ���ر���
public:
	int m_nRecvSize;					//���հ���
	int m_nFramesPerPackage;			//ÿ��������֡��
	double m_Carrier;					//�ز�
	CComboBox m_ModulateMode;			//���Ʒ�ʽ
	double m_SymbolRateVal;				//��������
	CComboBox m_SymbolRateUnit;			//�������ʵ�λ
	CComboBox m_UploadDataType;			//�ϴ���������
	int m_C1;							//�˲�ϵ��C1
	int m_C2;							//�˲�ϵ��C2
	double m_modulatedeep;
	short bandwidth;
	short directpull;						//ֱ�鱶��
	short downsample1;					//��һ��CIC�³鱶��
	short downsample2;					//�ڶ���CIC�³鱶��
	double m_offsetvalue;
	double m_c1delta;
	double m_c2delta;
	unsigned char m_ifdelay;

// �Ի�������
	enum { IDD = IDD_PAGE4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonConfigure();
	afx_msg void OnBnClickedButtonRecvdata();
	afx_msg void OnBnClickedButtonCompar();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonStopRecv();
	afx_msg void OnBnClickedButtonModulator();
	afx_msg LRESULT OnButtonChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedCallsimulink();
	afx_msg void OnBnClickedBeginoffset();
	afx_msg void OnBnClickedEndoffset();
	afx_msg void OnBnClickedBegintrack();
	afx_msg void OnBnClickedEndtrack();
	afx_msg void OnBnClickedGmskifdelay();
	afx_msg void OnBnClickedFlash1();
	afx_msg void OnBnClickedFlash2();
};
