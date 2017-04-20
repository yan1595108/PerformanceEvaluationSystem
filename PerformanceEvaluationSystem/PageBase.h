#pragma once
#include "GigabitEthernetDevice.h"
#include <vector>

using namespace std;

// CPageBase �Ի���

struct RectProportion
{
	double scale[4];
};

class CPageBase : public CPropertyPage
{
	DECLARE_DYNAMIC(CPageBase)

public:
	CPageBase(UINT nDlgID,CWnd* pParent =NULL);   // ��׼���캯��
	virtual ~CPageBase();

public:
	RECT DlgPosition;
	int cx;        //����ҳ���
	int cy;        //����ҳ����
	vector<RectProportion> rectprop;      //�����ؼ�ռ������ҳ�ߴ�ı���
	static CGigabitEthernetDevice *pGEDevice;	//ǧ�����ӿ��豸
	static void SetGEDevice(CGigabitEthernetDevice *pDevice);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
