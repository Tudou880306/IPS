#pragma once
#include "afxwin.h"
#include "opencv2/opencv.hpp"

// CImageTool 对话框

class CImageTool : public CDialogEx
{
	DECLARE_DYNAMIC(CImageTool)

public:
	CImageTool(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CImageTool();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	CEdit m_edit_outprint;
	afx_msg void OnBnClickedButton1();
	CEdit m_edit_inputfile;
	CEdit m_edit_ouputfile;

	CString m_inputfile;
	CString m_outputfile;

	CString m_outprint;
	afx_msg void OnBnClickedButton2();
	CButton m_check_segment;
	void  DrawPicToHDC(IplImage *img, UINT ID);
	CStatic m_pic1;
	CStatic m_pic2;
	CStatic m_pic;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton10();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
};
