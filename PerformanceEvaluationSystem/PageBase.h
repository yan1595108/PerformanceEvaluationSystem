#pragma once
#include "GigabitEthernetDevice.h"

// CPageBase �Ի���

class CPageBase : public CDialogEx
{
	DECLARE_DYNAMIC(CPageBase)

public:
	CPageBase(UINT nDlgID,CWnd* pParent =NULL);   // ��׼���캯��
	virtual ~CPageBase();

public:
	RECT DlgPosition;
	static CGigabitEthernetDevice *pGEDevice;	//ǧ�����ӿ��豸
	bool dynamicbutton;        //ָʾ��ť�Ƿ��Ƕ�̬������
	CButton **m_buttons;       //���еİ�ť
	int buttonnum_online;             //���߰�ť������
	int buttonnum_offline;            //���߰�ť������
	CRect *rect_button;//��ż�����ť��λ��

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
