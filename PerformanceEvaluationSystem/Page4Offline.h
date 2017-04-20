#pragma once
#include "PageBase.h"

// CPage4Offline 对话框

class CPage4Offline : public CPageBase
{
	DECLARE_DYNAMIC(CPage4Offline)

public:
	CPage4Offline(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPage4Offline();

// 对话框数据
	enum { IDD = IDD_PAGE4_OFFLINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
