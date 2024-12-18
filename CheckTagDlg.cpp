// CheckTagDlg.cpp: 实现文件
//

#include "pch.h"
#include "afxdialogex.h"
#include "CheckTagDlg.h"
#include "ImageAnnotation.h"

// CheckTagDlg 对话框

IMPLEMENT_DYNAMIC(CheckTagDlg, CDialogEx)

CheckTagDlg::CheckTagDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHECK_TAG_DIALOG, pParent)
{

}


CheckTagDlg::CheckTagDlg(std::vector<ProjectPath> projectPath, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHECK_TAG_DIALOG, pParent)
{
	m_projectPath = projectPath;
}


CheckTagDlg::~CheckTagDlg()
{
}

void CheckTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PATH_LIST, m_list);
}


BEGIN_MESSAGE_MAP(CheckTagDlg, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PATH_LIST, &CheckTagDlg::OnLvnItemchangedPathList)
	ON_BN_CLICKED(IDOK, &CheckTagDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CheckTagDlg 消息处理程序

/**
 * @brief Init the dialog and the list in the dialog.
 * @param None.
 * @retval None.
 **/
BOOL CheckTagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_list.ModifyStyle(0, LVS_SHOWSELALWAYS);
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_ONECLICKACTIVATE);
	m_list.InsertColumn(0, _T("图像路径"), LVCFMT_CENTER, 250);
	m_list.InsertColumn(1, _T("标注路径"), LVCFMT_CENTER, 250);
	int line = 0;
	for (std::vector<ProjectPath>::iterator path = m_projectPath.begin(); path != m_projectPath.end(); path++)
	{
		m_list.InsertItem(line, path->m_imagePath);
		m_list.SetItemText(line, 1, path->m_tagPath);
		line++;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

/**
 * @brief Process the corresponding items in the selection list.
 * @param None.
 * @retval None.
 **/
void CheckTagDlg::OnLvnItemchangedPathList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	CString imagePath = m_list.GetItemText(pNMLV->iItem, 0);
	CString tagPath = m_list.GetItemText(pNMLV->iItem, 1);
	m_selectedImagePath = imagePath;
	m_selectedTagPath = tagPath;
	m_state = 1;
	*pResult = 0;
}

/**
 * @brief Process confirmation selection.
 * @param None.
 * @retval None.
 **/
void CheckTagDlg::OnBnClickedOk()
{
	if (m_state == 1)
	{
		m_imagePath = m_selectedImagePath;
		m_tagPath = m_selectedTagPath;
		m_state = 2;
	}
	CDialogEx::OnOK();
}
