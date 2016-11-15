#pragma once
#include "GigabitEthernetDevice.h"

// CPageBase 对话框

class CPageBase : public CDialogEx
{
	DECLARE_DYNAMIC(CPageBase)

public:
	CPageBase(UINT nDlgID,CWnd* pParent =NULL);   // 标准构造函数
	virtual ~CPageBase();

public:
	RECT DlgPosition;
	static CGigabitEthernetDevice *pGEDevice;	//千兆网接口设备

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
