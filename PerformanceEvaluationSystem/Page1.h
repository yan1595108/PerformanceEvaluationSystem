#pragma once

#include "PageBase.h"
#include "afxwin.h"

typedef struct _Page1Param
{
	CPageBase *pPage1;
	_Page1Param(){
		pPage1 = NULL;
	}
}Page1Param,*pPage1Param;

// CPage1 对话框
class CPage1 : public CPageBase
{
	DECLARE_DYNAMIC(CPage1)

public:
	CPage1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage1();

//线程相关变量
private:
	CWinThread *m_pThread1;
	Page1Param m_PageParam;

private:
	double	m_CenterFreq;
	double	m_BandWidth;
	double	m_ReferenceLevel;
	double	m_ReferenceLevelDiv;
	int		m_nNumFFTPoints;
	CComboBox m_UnitCenterFreq;
	CComboBox m_UnitBandWidth;
	CComboBox m_UnitReferenceLevel;
	CComboBox m_UnitReferenceLevelDiv;

public:
	double	mPage1_dCenterFreq;
	double	mPage1_dBandWidth;
	double	mPage1_dReferenceLevel;
	double	mPage1_dReferenceLevelDiv;
	int		mPage1_nNumFFTPoints;

// 对话框数据
	enum { IDD = IDD_PAGE1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonSet();
	virtual BOOL OnInitDialog();
};
