// ImageTool.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "IPS.h"
#include "ImageTool.h"
#include "afxdialogex.h"
#include <io.h> 
#include <direct.h> 
#include <iostream>
#include <fstream>
#include "CvvImage.h"
#include "thread"
#include "MainDlg.h"
#include "yolomark.h"
// CImageTool �Ի���
extern CImageTool m_imagetooldlg;
IMPLEMENT_DYNAMIC(CImageTool, CDialogEx)
CImageTool::CImageTool(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CImageTool::~CImageTool()
{
}

void CImageTool::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT3, m_edit_outprint);
	DDX_Control(pDX, IDC_EDIT1, m_edit_inputfile);
	DDX_Control(pDX, IDC_EDIT2, m_edit_ouputfile);
	DDX_Control(pDX, IDC_CHECK1, m_check_segment);
	DDX_Control(pDX, IDC_STATIC_PIC1, m_pic1);
	DDX_Control(pDX, IDC_STATIC_PIC2, m_pic2);
	DDX_Control(pDX, IDC_STATIC_PIC, m_pic);
}


BEGIN_MESSAGE_MAP(CImageTool, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON3, &CImageTool::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CImageTool::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImageTool::OnBnClickedButton2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON4, &CImageTool::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CImageTool::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON10, &CImageTool::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CImageTool::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CImageTool::OnBnClickedButton12)
END_MESSAGE_MAP()

void  CImageTool::DrawPicToHDC(IplImage *img, UINT ID)
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
int flag = 0; //�Ƿ�ָ�ͼ�� 0==���ָ�  1==�ָ�
// CImageTool ��Ϣ�������
void segmentImg()
{
	try
	{
		int nFrmNum = 0;
		
		
		USES_CONVERSION;
		std::string filename = T2A(m_imagetooldlg.m_inputfile.GetBuffer());
		std::string outfilename = T2A(m_imagetooldlg.m_outputfile.GetBuffer());
		cv::Mat  cur_frame;
		//CvCapture* pCapture = cvCaptureFromFile(c_file.name);
		//filename = argv[1];
		if (m_imagetooldlg.m_inputfile.IsEmpty())
		{
			return;
		}
		//sprintf(filename, (char*)argv[1]);
		
		cv::VideoCapture cap;
		cap.open(filename);
		cap >> cur_frame;
		char imgname[512];
		char imgnamesub1[512];
		char imgnamesub2[512];
		//IplImage* src;
		int numFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
		int fps = cap.get(CV_CAP_PROP_FPS);

		std::cout << "video Frames count:  " << numFrames << std::endl;
		std::cout << "video fps:  " << fps << std::endl;
		int tinttime = 500;
		int intval = numFrames / tinttime;
		if (intval > 100 || intval < 0)
		{
			intval = 50;
		}
		std::cout << "video intval:  " << intval << std::endl;

		while (!cur_frame.empty())
		{
			nFrmNum++;
			if (nFrmNum%intval == 0)
			{

				//IplImage* image =(IplImage*) &cur_frame;// cvCreateImage(cvSize(cur_frame.cols, cur_frame.rows), 8, 3);
				if (cur_frame.data == NULL)
				{
					continue;
				}

				std::string fullname = outfilename;
				int index = fullname.find_last_of("\\");
				std::string name = fullname.substr(index + 1, fullname.length() - index - 1);
				if (!flag)
				{
					sprintf_s(imgname, "%s\\%s_%d.jpg", outfilename, name.c_str(), nFrmNum + 1000000);

				}
				else
				{
					sprintf_s(imgnamesub1, "%s\\%s_%d_%d.jpg", outfilename, name.c_str(), 1, nFrmNum + 1000000);
					sprintf_s(imgnamesub2, "%s\\%s_%d_%d.jpg", outfilename, name.c_str(), 2, nFrmNum + 1000000);
				}
				char cmd[256];
				sprintf_s(cmd, "mkdir \"%s\"", outfilename);
				if (_access(outfilename.data(), 0) == -1)
				{
					system(cmd);
				}
				if (!flag)
				{
					IplImage *image = &(IplImage)(cur_frame);
					//cvConvert(&cur_frame image);
					IplImage* src = cvCreateImage(cvSize(image->width, image->height), 8, 3);
					cvResize(image, src);

					//cv::imwrite(imgname, image);
					cvSaveImage(imgname, src);
					//cvNamedWindow("", 0);
					//cvShowImage("", src);
					m_imagetooldlg.DrawPicToHDC(src, IDC_STATIC_PIC);
					cvWaitKey(1);
					cvReleaseImage(&src);
				}
				else
				{
					cv::Rect rect(0, 0, cur_frame.cols / 2, cur_frame.rows);   //����һ��Rect������cv�е��࣬�ĸ���������x,y,width,height
					cv::Mat image_cut1 = cv::Mat(cur_frame, rect);

					IplImage *image1 = &(IplImage)(image_cut1);
					//cvConvert(&cur_frame image);
					IplImage* src1 = cvCreateImage(cvSize(image1->width, image1->height), 8, 3);
					cvResize(image1, src1);

					//cv::imwrite(imgname, image);
					cvSaveImage(imgnamesub1, src1);
					//cvNamedWindow("", 0);
					//cvShowImage("0", src1);
					m_imagetooldlg.DrawPicToHDC(src1, IDC_STATIC_PIC1);
					cvWaitKey(1);
					cvReleaseImage(&src1);


					cv::Rect rect2(cur_frame.cols / 2, 0, cur_frame.cols / 2, cur_frame.rows);   //����һ��Rect������cv�е��࣬�ĸ���������x,y,width,height
					cv::Mat image_cut2 = cv::Mat(cur_frame, rect2);

					IplImage *image2 = &(IplImage)(image_cut2);
					//cvConvert(&cur_frame image);
					IplImage* src2 = cvCreateImage(cvSize(image2->width, image2->height), 8, 3);
					cvResize(image2, src2);

					//cv::imwrite(imgname, image);
					cvSaveImage(imgnamesub2, src2);
					m_imagetooldlg.DrawPicToHDC(src2, IDC_STATIC_PIC2);
					//cvNamedWindow("", 0);
					//cvShowImage("1", src2);
					cvWaitKey(1);
					cvReleaseImage(&src2);

				}
				//IplImage *image = (IplImage *)&IplImage(cur_frame);






			}
			CString str;
			str.Format(_T("processed ..... %.3f %% ", ), (double)100 * nFrmNum / numFrames);
			
			//printf("processed ..... %.3f %% \r", (double)100 * nFrmNum / numFrames); // '\r'
			m_imagetooldlg.m_outprint = str;
			
			cap >> cur_frame;
			char key = cvWaitKey(1);

		}
		nFrmNum = 0;
		//cvReleaseImage(&src);
		//cvReleaseCapture(&pCapture);
		//}
		//�ͷŸ�˹ģ�Ͳ���ռ���ڴ�   
		//cvReleaseCapture(&pCapture);
	}
	catch (...)
	{
		cvWaitKey(0);
	}
}
void CImageTool::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	flag = m_check_segment.GetCheck();
	if (flag)
	{
		m_pic.ShowWindow(FALSE);
		m_pic1.ShowWindow(TRUE);
		m_pic2.ShowWindow(TRUE);
	}
	else
	{
		m_pic.ShowWindow(TRUE);
		m_pic1.ShowWindow(FALSE);
		m_pic2.ShowWindow(FALSE);
	}
	std::thread t(segmentImg);
	t.detach();

}


void CImageTool::OnBnClickedButton1()
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
		m_inputfile = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
	}

	//�༭������ʾ��ѡ����
	if (!m_inputfile.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT1);
		pWnd->SetWindowText(m_inputfile);
	}
}


void CImageTool::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;
	TCHAR name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = _T("ѡ���ļ���Ŀ¼");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return ;
	SHGetPathFromIDList(idl, m_outputfile.GetBuffer(MAX_PATH));
	m_outputfile.ReleaseBuffer();
	if (m_outputfile.IsEmpty())
		return ;
	if (m_outputfile.Right(1) != "\\")
		m_outputfile += "\\";


	//�༭������ʾ��ѡ����
	if (!m_outputfile.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT2);
		pWnd->SetWindowText(m_outputfile);
	}
}


void CImageTool::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CWnd* pWnd = GetDlgItem(IDC_EDIT3);
	pWnd->SetWindowText(m_outprint);
	UpdateData(FALSE);

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CImageTool::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	SetTimer(1, 100, NULL);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CImageTool::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	USES_CONVERSION;
	CString imgpath;
	CWnd* pWnd1 = GetDlgItem(IDC_EDIT_IMGPATH);
	pWnd1->GetWindowTextW(imgpath);

	CString train;
	CWnd* pWnd2 = GetDlgItem(IDC_EDIT_TRAIN);
	pWnd2->GetWindowTextW(train);

	CString objname;
	CWnd* pWnd3 = GetDlgItem(IDC_EDIT_OBJNAMES);
	pWnd3->GetWindowTextW(objname);

	YoloMark *mark = YoloMark::GetInstance();
	std::string imgpath_str = T2A(imgpath.GetBuffer(0));
	std::string train_str = T2A(train.GetBuffer(0));
	std::string objname_str = T2A(objname.GetBuffer(0));
	mark->start(imgpath_str, train_str, objname_str);
}


void CImageTool::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	BROWSEINFO bi;
	TCHAR name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = _T("ѡ���ļ���Ŀ¼");
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	SHGetPathFromIDList(idl, m_outputfile.GetBuffer(MAX_PATH));
	m_outputfile.ReleaseBuffer();
	if (m_outputfile.IsEmpty())
		return;
	if (m_outputfile.Right(1) != "\\")
		m_outputfile += "\\";


	//�༭������ʾ��ѡ����
	if (!m_outputfile.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_IMGPATH);
		pWnd->SetWindowText(m_outputfile);
	}
}


void CImageTool::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	CString m_input;
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		m_input = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
	}

	//�༭������ʾ��ѡ����
	if (!m_input.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_TRAIN);
		pWnd->SetWindowText(m_input);
	}
}


void CImageTool::OnBnClickedButton11()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);
	CFileDialog fileDlg(TRUE);
	fileDlg.m_ofn.lpstrTitle = _T("�ļ��򿪶Ի���");
	fileDlg.m_ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	//fileDlg.m_ofn.lpstrDefExt="*.exe";
	CString m_input;
	if (IDOK == fileDlg.DoModal())
	{
		//CFile file(fileDlg.GetFileName(),CFile::modeRead);
		m_input = fileDlg.GetPathName();//�ļ���+��׺  
		UpdateData(FALSE);
	}

	//�༭������ʾ��ѡ����
	if (!m_input.IsEmpty())
	{
		CWnd* pWnd = GetDlgItem(IDC_EDIT_OBJNAMES);
		pWnd->SetWindowText(m_input);
	}
}


void CImageTool::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	YoloMark *mark = YoloMark::GetInstance();
	mark->quit = true;
	
}
