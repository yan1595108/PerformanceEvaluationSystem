#pragma once
#include "afxwin.h"


// CCOMM2 �Ի���

class CCOMM2 : public CPropertyPage
{
	DECLARE_DYNAMIC(CCOMM2)

public:
	CCOMM2();
	virtual ~CCOMM2();

// �Ի�������
	enum { IDD = IDD_COMM2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_bandwidth;
	short m_directpull;
	short m_downsample1;
	short m_downsample2;
	short bandwidth;
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboBandwidth();
	void UpdateControls(int index_bandwidth);
	afx_msg void OnBnClickedSpectrum();
	afx_msg void OnBnClickedChannelize();
	afx_msg void OnBnClickedUpload();
};
