﻿
// 3DSkeletonIdentifyDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "3DSkeletonIdentify.h"
#include "3DSkeletonIdentifyDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <cstring>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CMy3DSkeletonIdentifyDlg 对话框



CMy3DSkeletonIdentifyDlg::CMy3DSkeletonIdentifyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY3DSKELETONIDENTIFY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CMy3DSkeletonIdentifyDlg::~CMy3DSkeletonIdentifyDlg()
{
	if (NULL != this->processer)
	{
		delete this->processer;
	}
}

void CMy3DSkeletonIdentifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy3DSkeletonIdentifyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BtnStart, &CMy3DSkeletonIdentifyDlg::OnBnClickedBtnstart)
	ON_BN_CLICKED(BtnDisplayJoint, &CMy3DSkeletonIdentifyDlg::OnBnClickedBtndisplayjoint)
END_MESSAGE_MAP()


// CMy3DSkeletonIdentifyDlg 消息处理程序

BOOL CMy3DSkeletonIdentifyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// TODO: 在此添加额外的初始化代码
	// 参数初始化
	this->displayJoints = false;
	this->processer = NULL;
	// 显示窗口初始化
	cv::namedWindow("Rgb");
	HWND hWnd = (HWND)cvGetWindowHandle("Rgb");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(PicDisplayRgb)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);
	cv::Mat front = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
	putText(front, "Waiting Start...", cv::Point(0, 240), cv::FONT_HERSHEY_COMPLEX, 1,
		cv::Scalar(0, 0, 255), 1, 8, 0);
	imshow("Rgb", front);
	cv::waitKey(10);
	//隐藏控件
	GetDlgItem(BtnDisplayJoint)->ShowWindow(false);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMy3DSkeletonIdentifyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMy3DSkeletonIdentifyDlg::OnPaint()
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
HCURSOR CMy3DSkeletonIdentifyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy3DSkeletonIdentifyDlg::OnBnClickedBtnstart()
{
	// TODO: 在此添加控件通知处理程序代码
	if (NULL != this->processer)
	{
		GetDlgItem(BtnStart)->EnableWindow(0);
		delete this->processer;
		this->processer = NULL;
		cv::Mat front = cv::Mat(480, 640, CV_8UC3, cv::Scalar(0, 0, 0));
		putText(front, "Waiting Start...", cv::Point(0, 240), cv::FONT_HERSHEY_COMPLEX, 1,
			cv::Scalar(0, 0, 255), 1, 8, 0);
		imshow("Rgb", front);
		GetDlgItem(BtnStart)->EnableWindow(1);
		SetDlgItemText(BtnStart, L"开始");
		GetDlgItem(BtnDisplayJoint)->ShowWindow(false);
	}
	else
	{
		GetDlgItem(BtnStart)->EnableWindow(0);
		this->processer = new Processer(&this->displayJoints);
		this->processer->begin_detect();
		GetDlgItem(BtnStart)->EnableWindow(1);
		SetDlgItemText(BtnStart, L"停止");
		GetDlgItem(BtnDisplayJoint)->ShowWindow(true);
	}
	
	
}



void CMy3DSkeletonIdentifyDlg::OnBnClickedBtndisplayjoint()
{
	// TODO: 在此添加控件通知处理程序代码
	CString btnText;
	GetDlgItemText(BtnDisplayJoint, btnText);
	if (btnText == L"显示骨骼")
	{
		this->displayJoints = true;
		SetDlgItemText(BtnDisplayJoint, L"隐藏骨骼");
	}
	else
	{
		this->displayJoints = false;
		SetDlgItemText(BtnDisplayJoint, L"显示骨骼");
	}
}
