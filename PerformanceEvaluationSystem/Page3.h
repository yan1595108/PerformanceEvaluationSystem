#pragma once

#include "PageBase.h"


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
};
