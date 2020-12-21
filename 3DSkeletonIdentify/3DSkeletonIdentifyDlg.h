
// 3DSkeletonIdentifyDlg.h: 头文件
//

#pragma once
#include "main_process.h"

// CMy3DSkeletonIdentifyDlg 对话框
class CMy3DSkeletonIdentifyDlg : public CDialogEx
{
// 构造
public:
	CMy3DSkeletonIdentifyDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CMy3DSkeletonIdentifyDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MY3DSKELETONIDENTIFY_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnstart();
	afx_msg void OnBnClickedBtndisplayjoint();
// 开发者实现
	Processer* processer = NULL;
	bool displayJoints;
	
	CEdit editLabel;
	CButton btnLogIn;
	CStatic staticText;
	afx_msg void OnBnClickedBtnlogin();
	CEdit editLog;
};
