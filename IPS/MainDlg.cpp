// MainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "IPS.h"
#include "MainDlg.h"
#include "afxdialogex.h"


// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)
CImageTool m_imagetooldlg;

CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_MAIN, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMainDlg::OnSelchangeTab1)
END_MESSAGE_MAP()


// CMainDlg 消息处理程序


BOOL CMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_SHOWNORMAL);

	m_tabCtrl.InsertItem(0, _T("Detector"));
	m_tabCtrl.InsertItem(1, _T("Tool"));
	// TODO:  在此添加额外的初始化
	m_IPSdlg.Create(IDD_IPS_DIALOG, GetDlgItem(IDC_TAB1));
	m_imagetooldlg.Create(IDD_DIALOG1, GetDlgItem(IDC_TAB1));

	CRect rec;
	m_tabCtrl.GetClientRect(&rec);
	rec.top += 22;
	rec.bottom -= 5;
	rec.left += 3;
	rec.right -= 5;

	m_IPSdlg.MoveWindow(&rec);
	m_imagetooldlg.MoveWindow(&rec);

	m_IPSdlg.ShowWindow(TRUE);
	
	m_tabCtrl.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void CMainDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int CurSel = m_tabCtrl.GetCurSel();
	switch (CurSel)
	{
	case 0:
		m_IPSdlg.ShowWindow(TRUE);
		m_imagetooldlg.ShowWindow(FALSE);
		break;
	case 1:
		m_IPSdlg.ShowWindow(FALSE);
		m_imagetooldlg.ShowWindow(TRUE);
		break;
	default:
		break;
	}
	*pResult = 0;
}
