#pragma once


// CPage4Offline �Ի���

class CPage4Offline : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage4Offline)

public:
	CPage4Offline(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPage4Offline();

// �Ի�������
	enum { IDD = IDD_PAGE4_OFFLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
