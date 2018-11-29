
// IPSDlg.cpp : 实现文件
////

#include "stdafx.h"
#include "IPS.h"
#include "IPSDlg.h"
#include "afxdialogex.h"
#include <thread>
#include<chrono>
#include <mutex>
#include <atomic>
#include "ShipDetectorDll.h"
#include "player.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
//CIPSDlg dlg;
std::mutex  mt_temp,  mt2_temp, mt3_temp, mt4_temp,mt_queue;
static CString m_edit_weights;

std::vector<Player*>PlayerVideoGroup;
std::vector<Player*> PlayerImageGroup;
std::queue<std::vector<bbox_t>> result_queue;
cv::Mat  m_CvvImages, m_CvvImages2, m_CvvImages3, m_CvvImages4;;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIPSDlg 对话框



CIPSDlg::CIPSDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_IPS_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CIPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_listctrl_bbox);
}

BEGIN_MESSAGE_MAP(CIPSDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PLAY, &CIPSDlg::OnBnClickedBtnPlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CIPSDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CIPSDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_PLAY2, &CIPSDlg::OnBnClickedBtnPlay2)
	ON_BN_CLICKED(IDC_BTN_SCAN, &CIPSDlg::OnBnClickedBtnScan)
	ON_BN_CLICKED(IDC_BTN_PLAY3, &CIPSDlg::OnBnClickedBtnPlay3)
	ON_BN_CLICKED(IDC_BUTTON6, &CIPSDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON5, &CIPSDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON9, &CIPSDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON7, &CIPSDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CIPSDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON4, &CIPSDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, &CIPSDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CIPSDlg 消息处理程序

BOOL CIPSDlg::OnInitDialog()
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

	ShowWindow(SW_MINIMIZE);
	CWnd* pWnd = GetDlgItem(IDC_EDIT_Path);
	pWnd->SetWindowText(_T("yolov3.weights"));
	m_edit_weights = _T("yolov3.weights");

	m_listctrl_bbox.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listctrl_bbox.InsertColumn(0, _T("                 info           "), 0, 250);
	// TODO: 在此添加额外的初始化代码
	SetTimer(1, 25, NULL); //定时器，定时时间和帧率一致
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CIPSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CIPSDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CIPSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void  CIPSDlg::DrawPicToHDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}



void CIPSDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_edit_weights.IsEmpty())
	{
		return;
	}

	//USES_CONVERSION;
	//handle = SDLib_Init(T2A(m_edit_weights.GetBuffer(0)));
	//handle1 = SDLib_Init(T2A(m_edit_weights.GetBuffer(0)));
	
}

void CIPSDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	OutputDebugString(_T("==============OnTimer\n"));
	//if (p_mat!=NULL)
	{

		mt_temp.lock();
		DrawPicToHDC((IplImage*)&IplImage(m_CvvImages), IDC_STATIC_1);
		mt_temp.unlock();
	}
	//if (isplay2)
	{
		mt2_temp.lock();
		DrawPicToHDC((IplImage*)&IplImage(m_CvvImages2), IDC_STATIC_2);
		mt2_temp.unlock();
	}
	mt3_temp.lock();
	DrawPicToHDC((IplImage*)&IplImage(m_CvvImages3), IDC_STATIC_3);
	mt3_temp.unlock();
	mt4_temp.lock();
	DrawPicToHDC((IplImage*)&IplImage(m_CvvImages4), IDC_STATIC_4);
	mt4_temp.unlock();

	if (result_queue.size() > 0)
	{
		mt_queue.lock();
		std::vector<bbox_t>temp = result_queue.front();
		for (int i = 0; i < temp.size(); i++)
		{
			CString str;
			str.Format(_T("obj_id%d track_id %d x:%d y:%d w:%d h:%d"), temp[i].obj_id, temp[i].track_id, temp[i].x, temp[i].y, temp[i].w, temp[i].h);
			m_listctrl_bbox.InsertItem(m_listctrl_bbox.GetItemCount(), str);
		}
		result_queue.pop();
		mt_queue.unlock();
	}

	CDialogEx::OnTimer(nIDEvent);
}



void CIPSDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
//	isplay1 = !isplay1;
	for (int i =0;i<PlayerVideoGroup.size();i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 1)
		{
			((videoPlayer*)PlayerVideoGroup[i])->isplay = !((videoPlayer*)PlayerVideoGroup[i])->isplay;
		}
	}
}


void CIPSDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
//	isplay2 = !isplay2;
	for (int i = 0; i < PlayerVideoGroup.size(); i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 2)
		{
			((videoPlayer*)PlayerVideoGroup[i])->isplay = !((videoPlayer*)PlayerVideoGroup[i])->isplay;
		}
	}

}


void CIPSDlg::OnBnClickedBtnPlay2()
{
//	isplay2 = false;
//	isplay1 = false;
	// TODO: 在此添加控件通知处理程序代码
}


void CIPSDlg::OnBnClickedBtnScan()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("文件打开对话框");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		m_edit_weights = fileDlg.GetPathName();//文件名+后缀  
		UpdateData(FALSE);
	}

	//编辑框中显示所选内容
	if (!m_edit_weights.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_Path);
		pWnd->SetWindowText(m_edit_weights);
	}
}


void CIPSDlg::OnBnClickedBtnPlay3()
{
	// TODO: 在此添加控件通知处理程序代码
//	isplay2 = true;
//	isplay1 = true;
}

void outputresult(cv::Mat mat, std::vector<bbox_t> targetinfo)
{
	mt_temp.lock();
	m_CvvImages = mat.clone();
	mt_temp.unlock();

	mt_queue.lock();
	result_queue.push(targetinfo);
	mt_queue.unlock();
}
void outputresult2(cv::Mat mat, std::vector<bbox_t> targetinfo)
{
	mt2_temp.lock();
	m_CvvImages2 = mat.clone();
	mt2_temp.unlock();

	mt_queue.lock();
	result_queue.push(targetinfo);
	mt_queue.unlock();
}

void outputresult3(cv::Mat mat, std::vector<bbox_t> targetinfo)
{
	mt3_temp.lock();
	m_CvvImages3 = mat.clone();
	mt3_temp.unlock();

	mt_queue.lock();
	result_queue.push(targetinfo);
	mt_queue.unlock();
}

void outputresult4(cv::Mat mat, std::vector<bbox_t> targetinfo)
{
	mt4_temp.lock();
	m_CvvImages4 = mat.clone();
	mt4_temp.unlock();

	mt_queue.lock();
	result_queue.push(targetinfo);
	mt_queue.unlock();
}
void initPlayer(CString file, int num)
{
	USES_CONVERSION;
	std::string filename = T2A(file.GetBuffer(0));
	std::string const protocol = filename.substr(0, 7);
	std::string const file_ext = filename.substr(filename.find_last_of(".") + 1);
	if (file_ext == "avi" || file_ext == "mp4" || file_ext == "mjpg" || file_ext == "mov" || 	// video file
		protocol == "rtmp://" || protocol == "rtsp://" || protocol == "http://" || protocol == "https:/")	// video network stream
	{

		Player*  p = NULL;
		for (int i = 0; i < PlayerVideoGroup.size(); i++)
		{
			if (((videoPlayer*)PlayerVideoGroup[i])->playernum == num)
			{
				p = PlayerVideoGroup[i];
			}
		}
		if (p != NULL)
		{
			((videoPlayer*)p)->stop();
			((videoPlayer*)p)->file = file;
			((videoPlayer*)p)->isplay = true;
			((videoPlayer*)p)->start();
		}
		else
		{
			Ifactory *factory = new videoPlayerFactory();
			Player *player = factory->CreatePlayer();
			player->playernum = num;
			player->init(T2A(m_edit_weights.GetBuffer(0)));
			((videoPlayer*)player)->init();
			((videoPlayer*)player)->file = file;
			((videoPlayer*)player)->isplay = true;
			switch (num)
			{
			case 1:
				((videoPlayer*)player)->outresult = outputresult;
				break;
			case 2:
				((videoPlayer*)player)->outresult = outputresult2;
				break;
			case 3:
				((videoPlayer*)player)->outresult = outputresult3;
				break;
			case 4:
				((videoPlayer*)player)->outresult = outputresult4;
				break;
			default:
				break;
			}
			player->start();
			PlayerVideoGroup.push_back(player);
		}

	}
	else//图片
	{
		Player*  ptemp = NULL;
		for (int i = 0; i < PlayerVideoGroup.size(); i++)
		{
			if ((PlayerVideoGroup[i])->playernum == num)
			{
				((videoPlayer*)PlayerVideoGroup[i])->stop();
			}
		}

		cv::Mat cur_frame = cv::imread(filename);

		Player*  p = NULL;
		for (int i = 0; i < PlayerImageGroup.size(); i++)
		{
			if ((PlayerImageGroup[i])->playernum == num)
			{
				p = PlayerImageGroup[i];
			}
		}
		if (p != NULL)
		{
			p->cur_frame = cur_frame.clone();
			p->start();
			m_CvvImages = p->cur_frame.clone();
		}
		else
		{
			Ifactory *factory = new imagePlayerFactory();
			Player *player = factory->CreatePlayer();
			player->playernum = num;
			player->init(T2A(m_edit_weights.GetBuffer(0)));
			player->cur_frame = cur_frame.clone();
			switch (num)
			{
			case 1:
				(player)->outresult = outputresult;
				break;
			case 2:
				(player)->outresult = outputresult2;
				break;
			case 3:
				(player)->outresult = outputresult3;
				break;
			case 4:
				(player)->outresult = outputresult4;
				break;
			default:
				break;
			}
			player->start();
			PlayerImageGroup.push_back(player);
		}
	}
}
void CIPSDlg::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("文件打开对话框");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//文件名+后缀  
		UpdateData(FALSE);
		initPlayer(file, 1);
	}
}



void CIPSDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("文件打开对话框");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//文件名+后缀  
		UpdateData(FALSE);
		initPlayer(file,2);
	}
}


void CIPSDlg::OnBnClickedButton9()
{
	// TODO: 在此添加控件通知处理程序代码
	Player*  p;
	for (int i = 0; i < PlayerVideoGroup.size(); i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 1)
		{
			((videoPlayer*)PlayerVideoGroup[i])->stop();
		}
	}
}


void CIPSDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("文件打开对话框");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//文件名+后缀  
		UpdateData(FALSE);
		initPlayer(file, 3);
	}
}


void CIPSDlg::OnBnClickedButton8()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("文件打开对话框");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//文件名+后缀  
		UpdateData(FALSE);
		initPlayer(file, 4);
	}
}


void CIPSDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < PlayerVideoGroup.size(); i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 3)
		{
			((videoPlayer*)PlayerVideoGroup[i])->isplay = !((videoPlayer*)PlayerVideoGroup[i])->isplay;
		}
	}
}


void CIPSDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < PlayerVideoGroup.size(); i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 4)
		{
			((videoPlayer*)PlayerVideoGroup[i])->isplay = !((videoPlayer*)PlayerVideoGroup[i])->isplay;
		}
	}
}
