#pragma once
#include "PageBase.h"

// CPage5 对话框

class CPage5 : public CPageBase
{
	DECLARE_DYNAMIC(CPage5)

public:
	CPage5(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage5();

// 对话框数据
	enum { IDD = IDD_PAGE5 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
