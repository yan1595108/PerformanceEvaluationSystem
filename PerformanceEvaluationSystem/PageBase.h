#pragma once
#include "GigabitEthernetDevice.h"

// CPageBase �Ի���

class CPageBase : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageBase)

public:
	CPageBase(UINT nDlgID,CWnd* pParent =NULL);   // ��׼���캯��
	virtual ~CPageBase();

public:
	RECT DlgPosition;
	static CGigabitEthernetDevice *pGEDevice;	//ǧ�����ӿ��豸
	static void SetGEDevice(CGigabitEthernetDevice *pDevice);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
