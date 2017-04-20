#pragma once
#include "GigabitEthernetDevice.h"
#include <vector>

using namespace std;

// CPageBase 对话框

struct RectProportion
{
	double scale[4];
};

class CPageBase : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageBase)

public:
	CPageBase(UINT nDlgID,CWnd* pParent =NULL);   // 标准构造函数
	virtual ~CPageBase();

public:
	RECT DlgPosition;
	int cx;        //属性页宽度
	int cy;        //属性页长度
	vector<RectProportion> rectprop;      //各个控件占据属性页尺寸的比例
	static CGigabitEthernetDevice *pGEDevice;	//千兆网接口设备
	static void SetGEDevice(CGigabitEthernetDevice *pDevice);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
