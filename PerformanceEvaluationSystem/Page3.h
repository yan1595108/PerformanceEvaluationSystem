#pragma once

#include "PageBase.h"
#include "afxwin.h"

// CPage3 对话框

class CPage3 : public CPageBase
{
	DECLARE_DYNAMIC(CPage3)

public:
	CPage3(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage3();

// 对话框数据
	enum { IDD = IDD_PAGE3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_bandwidth;
	short m_directpull;
	short m_downsample1;
	short m_downsample2;
	double bandwidth[51];
	short directpull[51];
	short downsample1[51];
	short downsample2[51];
	int analog_bandwidth[51];
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeSignalbandwidth();
	afx_msg void OnBnClickedSpectrum2();
	afx_msg void OnBnClickedPreprocesspara();
};
