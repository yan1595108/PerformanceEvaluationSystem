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
	bool dynamicbutton;        //指示按钮是否是动态创建的
	CButton **m_buttons;       //所有的按钮
	int buttonnum_online;             //在线按钮的数量
	int buttonnum_offline;            //离线按钮的数量
	CRect *rect_button;//存放几个按钮的位置

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
