
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
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CvCapture* Capture;
CvCapture* m_Video;
IplImage* frame;
IplImage* m_grabframe;
CRect rect;
CDC *pDC;
HDC hDC;
CWnd *pwnd;
int ImgNum = 0;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
//CIPSDlg dlg;
std::mutex mt,mt_temp, mt1,mt1_temp;
bool isplay1 = false;
bool isplay2 = false;
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
	m_listctrl_bbox.InsertColumn(0, _T("                 info           "),0,250);
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
cv::VideoCapture cap,cap1;
cv::Mat cur_frame, cur_frame1;
void *handle,*handle1;
std::vector<bbox_t>targetinfo, targetinfo1;
std::queue<std::vector<bbox_t>> result_queue;
cv::Mat  m_CvvImages,m_CvvImages1;
std::atomic<bool> flag = true;
std::atomic<bool> flag1 = true;
void output()
{
	while (1)
	{
		
			std::thread t_detect, t_cap;
			cv::Mat cap_frame, m_CvvImages_temp;
			while (!cur_frame.empty())
			{
				
				//if (t_cap.joinable())
				//{
				//	t_cap.join();
				//	OutputDebugString(_T("===============sssss\r\n"));
				//	//++fps_cap_counter;
				//	//mt.lock();
				//	cap_frame.copyTo(cur_frame);
				//	//mt.unlock();
				//}
				if (!t_cap.joinable())
				{
					t_cap = std::thread([&]()
					{
						while (1)
						{
							OutputDebugString(_T("===============t_cap\r\n"));
							if (isplay1)
							{
								cap >> cap_frame;

								if (flag)
								{
									//mt1.lock();
									//cur_frame1.copyTo(m_CvvImages1_temp); //复制该帧图像 
									m_CvvImages_temp = cap_frame.clone();
									flag = false;
									//mt1.unlock();
								}
								Sleep(25);
								
							}
							else
								Sleep(1000);
							
						}

						//cur_frame = cap_frame.clone();
						//std::cout << "cap_frame" << std::endl;
					});
				}
				//t_cap = std::thread([&]()
				//{
				//	OutputDebugString(_T("===============t_cap\r\n"));
				//	cap >> cap_frame;
				//	//cur_frame = cap_frame.clone();
				//	//std::cout << "cap_frame" << std::endl;
				//});

				//if (flag)
				//{
				//	mt.lock();
				//	cur_frame.copyTo(m_CvvImages_temp); //复制该帧图像 
				//	flag = false;
				//	mt.unlock();
				//}
				/*if (!isplay1)
				{
					t_detect.join();
				}
				else*/
				{
					if (!t_detect.joinable())
					{
						t_detect = std::thread([&]()
						{
							flag = true;

							while (1)
							{
								while (!cur_frame.empty() && isplay1)
								{
									//flag = true;
									OutputDebugString(_T("===============t_detect\r\n"));

									if (!flag)
									{
										SDLib_StartProcess_Mat(handle, m_CvvImages_temp, targetinfo);

										result_queue.push(targetinfo);
										mt_temp.lock();
										//m_CvvImages1_temp.copyTo(m_CvvImages1);
										m_CvvImages = m_CvvImages_temp.clone();

										mt_temp.unlock();
										flag = true;

									}
									//auto current_image = det_image;
									//cv:Mat  m_CvvImage, m_CvvImage1;
									//SDLib_Release(handle);

									//mt.lock();
									//SDLib_StartProcess_Mat(handle, m_CvvImages_temp, targetinfo);

									////	m_CvvImages = cur_frame.clone(); //复制该帧图像 
									//mt.unlock();
									//mt_temp.lock();
									//m_CvvImages_temp.copyTo(m_CvvImages);

									//mt_temp.unlock();
									cv::waitKey(3);
									Sleep(5);
								}
								Sleep(500);
							}
							

						});

					}
				}
				//std::shared_ptr<image_t>  det_image = mat_to_image_resize(cur_frame);
				
				//cap >> cur_frame;

				
			/*
			cap >> cur_frame;

			//cv:Mat  m_CvvImage;
			//SDLib_Release(handle);
			mt.lock();
			dlg.m_CvvImage = cur_frame.clone(); //复制该帧图像 
			SDLib_StartProcess_Mat(handle, dlg.m_CvvImage, targetinfo);
			mt.unlock();
			//dlg.DrawPicToHDC((IplImage*)&IplImage(dlg.m_CvvImage), IDC_STATIC_1);
			Sleep(20);
			*/
				Sleep(10);
		}
		
	}
	
}
void output1()
{
	while (1)
	{
		

			std::thread t_detect, t_cap;
			cv::Mat cap_frame, m_CvvImages1_temp;
			while (!cur_frame1.empty())
			{
				
				//if (t_cap.joinable())
				//{
				//	t_cap.join();
				//	OutputDebugString(_T("===============sssss\r\n"));
				//	//++fps_cap_counter;
				//	//mt.lock();
				//	//cap_frame.copyTo(cur_frame1);
				//	cur_frame1 = cap_frame.clone();
				//	//mt.unlock();
				//}
				if (!t_cap.joinable())
				{
					t_cap = std::thread([&]()
					{
						while (1)
						{
							OutputDebugString(_T("===============t_cap\r\n"));
							if (isplay2)
							{
								cap1 >> cap_frame;
								if (flag1)
								{
									//mt1.lock();
									//cur_frame1.copyTo(m_CvvImages1_temp); //复制该帧图像 
									m_CvvImages1_temp = cap_frame.clone();
									flag1 = false;
									//mt1.unlock();
								}
									Sleep(25);
								
							}
							else
								Sleep(1000);
						}
						
						//cur_frame = cap_frame.clone();
						//std::cout << "cap_frame" << std::endl;
					});
				}
				

				//if (flag1)
				//{
				//	mt1.lock();
				//	 //cur_frame1.copyTo(m_CvvImages1_temp); //复制该帧图像 
				//	m_CvvImages1_temp = cur_frame1.clone();
				//	flag1 = false;
				//	mt1.unlock();
				//}
				//std::shared_ptr<image_t>  det_image = mat_to_image_resize(cur_frame);
				/*if (!isplay2)
				{
					t_detect.join();
				}
				else*/
				{
					if (!t_detect.joinable())
					{
						t_detect = std::thread([&]()
						{
							flag1 = true;
							while (1)
							{
								while (!m_CvvImages1_temp.empty() && isplay2)
								{
									OutputDebugString(_T("===============t_detect\r\n"));
									//auto current_image = det_image;
									//cv:Mat  m_CvvImage, m_CvvImage1;
									//SDLib_Release(handle);

									//mt1.lock();
									if (!flag1)
									{
										SDLib_StartProcess_Mat(handle1, m_CvvImages1_temp, targetinfo1);
										result_queue.push(targetinfo1);
										mt1_temp.lock();
										//m_CvvImages1_temp.copyTo(m_CvvImages1);
										m_CvvImages1 = m_CvvImages1_temp.clone();

										mt1_temp.unlock();
										flag1 = true;

									}

									//	m_CvvImages1_temp.copyTo(m_CvvImages1);
									//	m_CvvImages = cur_frame.clone(); //复制该帧图像 
									//mt1.unlock();

									
									cv::waitKey(3);
									Sleep(5);
								}
								Sleep(500);
							}
							

						});

					}
					//Sleep(25);
				}
				
				//cap >> cur_frame;

				Sleep(10);
		}
		
	}
	
}

void CIPSDlg::OnBnClickedBtnPlay()
{
	// TODO: 在此添加控件通知处理程序代码

		
		// TODO: Add your command handler code here
		if (m_edit_weights.IsEmpty())
		{
			return;
		}

		USES_CONVERSION;
		handle = SDLib_Init(T2A(m_edit_weights.GetBuffer(0)));
		handle1 = SDLib_Init(T2A(m_edit_weights.GetBuffer(0)));
		//cv::VideoCapture cap("test.mp4"); 
		cap.open("test.mp4");
		cap1.open("test1.mp4");
		cap >> cur_frame;
		cap1 >> cur_frame1;
		int fps = cap.get(CV_CAP_PROP_FPS);
		CString temp;
		temp.Format(_T("%d"), fps);
		OutputDebugString(LPCWSTR(temp));
		//frame = cvQueryFrame(Capture); //从摄像头或者文件中抓取并返回一帧
		//pDC = GetDlgItem(IDC_PIC_STATIC_1)->GetDC();//GetDlgItem(IDC_PIC_STATIC)意思为获取显示控件的句柄（句柄就是指针），获取显示控件的DC
		//GetDlgItem(IDC_PIC_STATIC_1)->GetClientRect(&rect);
		//hDC = pDC->GetSafeHdc();//获取显示控件的句柄

		
		//m_CvvImage= cur_frame.clone(); //复制该帧图像   
		//m_CvvImage.DrawToHDC(hDC, &rect); //显示到设备的矩形框内
		//DrawPicToHDC((IplImage*)&IplImage(m_CvvImage), IDC_STATIC_1);
		//ReleaseDC(pDC);
		
		std::thread t(output);
		t.detach();
		
		std::thread t1(output1);
		t1.detach();

		
		

		

		



}

void CIPSDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*mt.lock();
	cv::Mat temp = dlg.m_CvvImage.clone();
	mt.unlock();
	mt1.lock();
	cv::Mat temp1 = dlg.m_CvvImage1.clone();
	mt1.unlock();
	if (!temp.empty())
	{
		DrawPicToHDC((IplImage*)&IplImage(temp), IDC_STATIC_1);
	}
	if (!temp1.empty())
	{
		DrawPicToHDC((IplImage*)&IplImage(temp1), IDC_STATIC_2);
	}*/
	OutputDebugString(_T("==============OnTimer\n"));
	if (isplay1)
	{
		mt_temp.lock();
		DrawPicToHDC((IplImage*)&IplImage(m_CvvImages), IDC_STATIC_1);
		mt_temp.unlock();
	}
	if (isplay2)
	{
		mt1_temp.lock();
		DrawPicToHDC((IplImage*)&IplImage(m_CvvImages1), IDC_STATIC_2);
		mt1_temp.unlock();
	}
	
	if (result_queue.size()>0)
	{
		std::vector<bbox_t>temp = result_queue.front();

		for (int i = 0;i<temp.size();i++)
		{
			CString str;
			str.Format(_T("obj_id%d track_id %d x:%d y:%d w:%d h:%d"), temp[i].obj_id, temp[i].track_id, temp[i].x, temp[i].y, temp[i].w, temp[i].h);
			m_listctrl_bbox.InsertItem(m_listctrl_bbox.GetItemCount(), str);
		}
		result_queue.pop();

	}

	/*
	cap >> cur_frame;
		cap1 >> cur_frame1;

		cv:Mat  m_CvvImage,m_CvvImage1;
		//SDLib_Release(handle);
		m_CvvImage = cur_frame.clone(); //复制该帧图像 
		m_CvvImage1 = cur_frame1.clone();
		int count = cap.get(CV_CAP_PROP_POS_FRAMES);
		//if (count %25==0)
		{
			SDLib_StartProcess_Mat(handle, m_CvvImage, targetinfo);
			//SDLib_StartProcess_Mat(handle, m_CvvImage1, targetinfo1);
			//DrawPicToHDC((IplImage*)&IplImage(m_CvvImage), IDC_STATIC_1);
			//DrawPicToHDC((IplImage*)&IplImage(m_CvvImage1), IDC_STATIC_2);
		}
		*/
		
		CDialogEx::OnTimer(nIDEvent);
	
}



void CIPSDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	isplay1 = !isplay1;

	
	
	
}


void CIPSDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	isplay2 = !isplay2;
	
}


void CIPSDlg::OnBnClickedBtnPlay2()
{
	isplay2 = false;
	isplay1 = false;
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
	isplay2 = true;
	isplay1 = true;
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
		USES_CONVERSION;
		cap.open(T2A(file.GetBuffer(0)));
		isplay1 = true;

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
		USES_CONVERSION;
		cap1.open(T2A(file.GetBuffer(0)));
		isplay2 = true;

	}
}
