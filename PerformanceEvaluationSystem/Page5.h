#pragma once
#include "PageBase.h"

// CPage5 �Ի���

class CPage5 : public CPageBase
{
	DECLARE_DYNAMIC(CPage5)

public:
	CPage5(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage5();

// �Ի�������
	enum { IDD = IDD_PAGE5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
