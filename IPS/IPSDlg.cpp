
// IPSDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
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

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CIPSDlg �Ի���



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


// CIPSDlg ��Ϣ�������

BOOL CIPSDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);
	CWnd* pWnd = GetDlgItem(IDC_EDIT_Path);
	pWnd->SetWindowText(_T("yolov3.weights"));
	m_edit_weights = _T("yolov3.weights");

	m_listctrl_bbox.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listctrl_bbox.InsertColumn(0, _T("                 info           "), 0, 250);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetTimer(1, 25, NULL); //��ʱ������ʱʱ���֡��һ��
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CIPSDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	cimg.CopyOf(img); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(pDC);
}



void CIPSDlg::OnBnClickedBtnPlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CIPSDlg::OnBnClickedBtnScan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		m_edit_weights = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
	}

	//�༭������ʾ��ѡ����
	if (!m_edit_weights.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_Path);
		pWnd->SetWindowText(m_edit_weights);
	}
}


void CIPSDlg::OnBnClickedBtnPlay3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	else//ͼƬ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
		initPlayer(file, 1);
	}
}



void CIPSDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
		initPlayer(file,2);
	}
}


void CIPSDlg::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
		initPlayer(file, 3);
	}
}


void CIPSDlg::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		CString file = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
		initPlayer(file, 4);
	}
}


void CIPSDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for (int i = 0; i < PlayerVideoGroup.size(); i++)
	{
		if (((videoPlayer*)PlayerVideoGroup[i])->playernum == 4)
		{
			((videoPlayer*)PlayerVideoGroup[i])->isplay = !((videoPlayer*)PlayerVideoGroup[i])->isplay;
		}
	}
}
