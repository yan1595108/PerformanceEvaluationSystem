#pragma once


// CCOMM1 �Ի���

class CCOMM1 : public CPropertyPage
{
	DECLARE_DYNAMIC(CCOMM1)

public:
	CCOMM1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCOMM1();

// �Ի�������
	enum { IDD = IDD_COMM1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	double m_offsetvalue;
	double m_c1delta;
	double m_c2delta;
	afx_msg void OnBnClickedBeginoffset();
	afx_msg void OnBnClickedEndoffset();
	afx_msg void OnBnClickedBegintrack();
	afx_msg void OnBnClickedEndtrack();
};
