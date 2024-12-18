#pragma once
#include "afxdialogex.h"
#include "Anchor.h"
#include <vector>

// CheckTagDlg 对话框

class CheckTagDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CheckTagDlg)

public:
	CheckTagDlg(CWnd* pParent = nullptr);   // 标准构造函数
	CheckTagDlg(std::vector<ProjectPath> projectPath, CWnd* pParent = nullptr);
	virtual ~CheckTagDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHECK_TAG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_state = 0;
	CString m_imagePath{};					// The image path that will be read
	CString m_tagPath{};					// The annotation path that will be read
	CListCtrl m_list{};						// The list that show all project paths
	CString m_selectedImagePath{};			// The image path selected by mouse
	CString m_selectedTagPath{};			// The annotation path selected by mouse
	std::vector<ProjectPath> m_projectPath;	// A vector that stores project path
	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedPathList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
};
