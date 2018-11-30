#pragma once
#include "afxcmn.h"
#include "IPSDlg.h"
#include "ImageTool.h"
// CMainDlg 对话框

class CMainDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	HICON m_hIcon;
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabCtrl;
	virtual BOOL OnInitDialog();
public:
	CIPSDlg m_IPSdlg;
	afx_msg void OnPaint();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
};
