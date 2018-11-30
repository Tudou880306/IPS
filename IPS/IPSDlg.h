
// IPSDlg.h : 头文件
//

#pragma once
#include <opencv2/opencv.hpp>	
#include "opencv2/videoio/videoio.hpp"
#include "CvvImage.h"
#include "afxwin.h"
#include "afxcmn.h"
using namespace cv;
// CIPSDlg 对话框
class CIPSDlg : public CDialogEx
{
// 构造
public:
	CIPSDlg(CWnd* pParent = NULL);	// 标准构造函数
	void  DrawPicToHDC(IplImage *img, UINT ID);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IPS_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	//afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

public:
	 afx_msg void OnBnClickedButton1();
	 afx_msg void OnBnClickedButton2();

	 CListCtrl m_listctrl_bbox;
	 afx_msg void OnBnClickedBtnPlay2();
	 afx_msg void OnBnClickedBtnScan();
private:
public:
	afx_msg void OnBnClickedBtnPlay3();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton3();
};
