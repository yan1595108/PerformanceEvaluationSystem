#pragma once

#include "PageBase.h"


// CPage3 �Ի���

class CPage3 : public CPageBase
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage3();

// �Ի�������
	enum { IDD = IDD_PAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
