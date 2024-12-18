
// ImageAnnotationView.cpp: CImageAnnotationView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ImageAnnotation.h"
#endif

#include "ImageAnnotationDoc.h"
#include "ImageAnnotationView.h"
#include <iostream>
#include <fstream>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageAnnotationView

IMPLEMENT_DYNCREATE(CImageAnnotationView, CView)

BEGIN_MESSAGE_MAP(CImageAnnotationView, CView)
	// 标准打印命令
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_NEW, &CImageAnnotationView::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CImageAnnotationView::OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, &CImageAnnotationView::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CImageAnnotationView::OnFileSaveAs)
	ON_COMMAND(ID_IMAGE_NEW, &CImageAnnotationView::OnNewAnnotationImage)
	ON_COMMAND(ID_CHECK_TAG, &CImageAnnotationView::OnCheckTag)
	ON_COMMAND(ID_EDIT_UNDO, &CImageAnnotationView::OnEditUndo)
	ON_COMMAND(ID_CLEAR, &CImageAnnotationView::OnClear)
END_MESSAGE_MAP()

// CImageAnnotationView 构造/析构

CImageAnnotationView::CImageAnnotationView() noexcept
{
}

CImageAnnotationView::~CImageAnnotationView()
{
}

BOOL CImageAnnotationView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CView::PreCreateWindow(cs);
}

// CImageAnnotationView 绘图

void CImageAnnotationView::OnDraw(CDC* /*pDC*/)
{
	CImageAnnotationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if (m_init)
	{
		PlotBuffer();
		// LoadImageFile(m_imagePath);
		// PlotAnchors();
	}
}

// CImageAnnotationView 诊断

#ifdef _DEBUG
void CImageAnnotationView::AssertValid() const
{
	CView::AssertValid();
}

void CImageAnnotationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageAnnotationDoc* CImageAnnotationView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageAnnotationDoc)));
	return (CImageAnnotationDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageAnnotationView 消息处理程序

/**
 * @brief Init buffer.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CRect rect;
	GetClientRect(rect);
	m_pDC = new CClientDC(this);
	m_pDCBuffer = new CDC;
	m_pDCBuffer->CreateCompatibleDC(m_pDC);
	m_pBitMapBuffer = new CBitmap;
	m_pBitMapBuffer->CreateCompatibleBitmap(m_pDC, rect.Width(), rect.Height());
	m_pDCBuffer->SelectObject(m_pBitMapBuffer);
}

/**
 * @brief Create a new project.
 * @param None
 * @retval None
 **/
void CImageAnnotationView::OnFileNew()
{
	OnNewAnnotationImage();
}

/**
 * @brief Open a exsiting image file and load the old project if user have.
 * @param None
 * @retval None
 **/
void CImageAnnotationView::OnFileOpen()
{
	CString imagePath = LoadImageFile();
	if (imagePath.IsEmpty())
	{
		return;
	}
	CString tagPath = imagePath.Left(imagePath.Find(_T('.'))) + _T("_tag.txt");
	for (std::vector<ProjectPath>::iterator path = m_projectPath.begin(); path != m_projectPath.end(); path++)
	{
		if (path->m_imagePath == imagePath)
		{
			LoadTagFile(tagPath);
		}
	}
	// PlotAnchors();
	m_state = STATE_INIT;
	PlotBuffer();
}

/**
 * @brief Create a new project.
 * @param None
 * @retval None
 **/
void CImageAnnotationView::OnNewAnnotationImage()
{
	ClearScreen();
	LoadImageFile();
	m_state = STATE_INIT;
}

/**
 * @brief Save the annotation information of the current page.
 * @param None
 * @retval None
 **/
void CImageAnnotationView::OnFileSave()
{
	std::ofstream os;
	CString strFilter = _T("文本文件(*.txt)|*.txt||");
	CString imageName = m_imagePath.Left(m_imagePath.Find(_T('.'))) + CString("_tag");
	CFileDialog dlg(FALSE, _T("txt"), imageName, NULL, strFilter);
	if (dlg.DoModal() != IDOK) return;
	CString strFileName;        
	strFileName = dlg.m_ofn.lpstrFile;
	os.open(strFileName);
	os << "Total Targets = " << m_anchors.size() << std::endl;
	int i = 1;
	for (std::vector<Anchor>::iterator anchor = m_anchors.begin(); anchor != m_anchors.end(); anchor++)
	{
		os << "Tag" << i << std::endl;
		os << "Point = " << anchor->m_x << ", " << anchor->m_y << std::endl;
		os << "W = " << anchor->m_w << std::endl;
		os << "H = " << anchor->m_h << std::endl;
		i++;
	}
	os.close();
	m_tagPath = strFileName;
	ProjectPath path(m_imagePath, m_tagPath);
	m_projectPath.push_back(path);
}

/**
 * @brief Save the annotation information of the current page.
 * @param None
 * @retval None
 **/
void CImageAnnotationView::OnFileSaveAs()
{
	OnFileSave();
}

/**
 * @brief Load a image file to the window.
 * @param filename, the path of the image.
 * @retval None
 **/
void CImageAnnotationView::LoadImageFile(CString filename)
{
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(&rect);
	CImage image;
	image.Load(filename);
	image.BitBlt(*pDC, m_x, m_y, rect.Width(), rect.Height(), 0, 0);
	m_w = image.GetWidth();
	m_h = image.GetHeight();
	m_imagePath = filename;
	image.Destroy();
	m_init = TRUE;
}

/**
 * @brief Load a image file selected in a dialog to the window.
 * @param filename, the path of the image.
 * @retval the path of the image.
 **/
CString CImageAnnotationView::LoadImageFile()
{
	m_anchors.clear();
	CFileDialog fileDlg(TRUE, _T("png"), NULL, 0, _T("image Files(*.bmp; *.jpeg;*.jpg;*.png)|*.JPG;*.PNG;*.BMP|All Files (*.*) |*.*|"), this);
	fileDlg.DoModal();
	CString filename = fileDlg.GetPathName();
	if (!filename.IsEmpty()) 
	{
		LoadImageFile(filename);
		return filename;
	}
	return filename;
}

/**
 * @brief Load a annotation file.
 * @param filename, the path of the annotation file.
 * @retval None.
 **/
void CImageAnnotationView::LoadTagFile(CString filename)
{
	m_anchors.clear();
	std::ifstream file(filename);
	std::string content;
	if (file.is_open())
	{
		std::string line;
		int state = 0;
		int x = 0, y = 0, w = 0, h = 0, num = 0, val = 0;
		while (std::getline(file, line))
		{
			val = 0;
			if (state == 0)
			{
				for (int i = 0; line[i] != '\0'; i++)
				{
					if ('0' <= line[i] && line[i] <= '9')
					{
						val = line[i] - '0' + val * 10;
					}
				}
				num = val;
				state++;
			}
			else if (state == 1)
			{
				state++;
			}
			else if (state == 2)
			{
				for (int i = 0; line[i] != '\0'; i++)
				{
					if ('0' <= line[i] && line[i] <= '9')
					{
						val = line[i] - '0' + val * 10;
					}
					else if (line[i] == ',')
					{
						x = val;
						val = 0;
					}
				}
				y = val;
				state++;
			}
			else if (state == 3)
			{
				for (int i = 0; line[i] != '\0'; i++)
				{
					if ('0' <= line[i] && line[i] <= '9')
					{
						val = line[i] - '0' + val * 10;
					}
				}
				w = val;
				state++;
			}
			else if (state == 4)
			{
				for (int i = 0; line[i] != '\0'; i++)
				{
					if ('0' <= line[i] && line[i] <= '9')
					{
						val = line[i] - '0' + val * 10;
					}
				}
				h = val;
				m_anchors.push_back(Anchor(x, y, w, h));
				state = 1;
			}
			content += line;
		}
	}
	else
	{
		AfxMessageBox(_T("Fail to open ") + filename);
	}
}

/**
 * @brief Clear current window.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::ClearScreen()
{
	CDC* pDC = GetDC();
	CRect rect;
	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, COLOR_WHITE);
}

/**
 * @brief Process left mouse button press.
 * @param point, the current point pointed by the mouse.
 * @retval None.
 **/
void CImageAnnotationView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_state == STATE_INIT && PointWithinBoundary(point))
	{
		m_paintStart = point;
		m_paintMove = point;
		Anchor anchor = Anchor(m_paintStart, m_paintMove);
		m_anchors.push_back(anchor);
		SetTimer(ID_TIMER_SCAN, 1, NULL);
		m_state = STATE_DRAW;
	}
	else if (m_state == STATE_EDIT && PointWithinBoundary(point))
	{
		SetTimer(ID_TIMER_SCAN, 1, NULL);
		m_state = STATE_EDITING;
	}
	CView::OnLButtonDown(nFlags, point);
}

/**
 * @brief Process left mouse button lifting.
 * @param point, the current point pointed by the mouse.
 * @retval None.
 **/
void CImageAnnotationView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_state == STATE_DRAW)
	{
		m_paintEnd = point;
		if (!PointWithinBoundary(m_paintEnd))
		{
			m_anchors.pop_back();
		}	
		KillTimer(ID_TIMER_SCAN);
		m_state = STATE_INIT;
	}
	else if (m_state == STATE_EDITING)
	{
		if (!PointWithinBoundary(m_editedAnchor->m_start) || !PointWithinBoundary(m_editedAnchor->m_end))
		{
			m_anchors.pop_back();
		}
		KillTimer(ID_TIMER_SCAN);
		m_state = STATE_INIT;
	}
	if (m_state == STATE_INIT)
	{
		PlotBuffer();
	}
	CView::OnLButtonUp(nFlags, point);
}

/**
 * @brief Process left mouse button moving.
 * @param point, the current point pointed by the mouse.
 * @retval None.
 **/
void CImageAnnotationView::OnMouseMove(UINT nFlags, CPoint point)
{
	m_paintMove = point;
	if (m_state == STATE_DRAW)
	{
		m_anchors.pop_back();
		Anchor anchor(m_paintStart, m_paintMove);
		m_anchors.push_back(anchor);
	}
	
	else if (m_state == STATE_INIT)
	{
		COLORREF color = m_pDC->GetPixel(point);
		if (color == COLOR_GREEN)
		{
			
			std::vector<Anchor>::iterator anchor = FindAnchor(point);
			if (anchor != m_anchors.end())
			{
				HCURSOR hCur = LoadCursor(NULL, IDC_SIZEALL);
				SetCursor(hCur);
				m_editedAnchor = anchor;
				m_state = STATE_EDIT;
			}
		}
	}
	else if (m_state == STATE_EDIT)
	{
		COLORREF color = m_pDC->GetPixel(point);
		if (color != COLOR_GREEN)
		{
			m_state = STATE_INIT;
		}
		else
		{
			std::vector<Anchor>::iterator anchor = FindAnchor(point);
			if (anchor != m_anchors.end())
			{
				HCURSOR hCur = LoadCursor(NULL, IDC_SIZEALL);
				SetCursor(hCur);
				m_editedAnchor = anchor;
			}
		}
	}
	else if (m_state == STATE_EDITING)
	{
		std::vector<Anchor>::iterator anchor = m_editedAnchor;
		if (anchor != m_anchors.end())
		{
			HCURSOR hCur = LoadCursor(NULL, IDC_SIZEALL);
			SetCursor(hCur);
			if (m_editState == EDIT_LEFT)
			{
				UINT x = point.x;
				UINT y = anchor->m_y;
				UINT w = anchor->m_right - point.x;
				UINT h = anchor->m_h;
				m_anchors.erase(anchor);
				m_anchors.push_back(Anchor(x, y, w, h));
				m_editedAnchor = m_anchors.end() - 1;
			}
			else if (m_editState == EDIT_RIGHT)
			{
				UINT x = anchor->m_x;
				UINT y = anchor->m_y;
				UINT w = point.x - anchor->m_x;
				UINT h = anchor->m_h;
				m_anchors.erase(anchor);
				m_anchors.push_back(Anchor(x, y, w, h));
				m_editedAnchor = m_anchors.end() - 1;
			}
			else if (m_editState == EDIT_UP)
			{
				UINT x = anchor->m_x;
				UINT y = point.y;
				UINT w = anchor->m_right - anchor->m_left;
				UINT h = anchor->m_down - point.y;
				m_anchors.erase(anchor);
				m_anchors.push_back(Anchor(x, y, w, h));
				m_editedAnchor = m_anchors.end() - 1;
			}
			else if (m_editState == EDIT_DOWN)
			{
				UINT x = anchor->m_x;
				UINT y = anchor->m_y;
				UINT w = anchor->m_right - anchor->m_left;
				UINT h = point.y - anchor->m_up;
				m_anchors.erase(anchor);
				m_anchors.push_back(Anchor(x, y, w, h));
				m_editedAnchor = m_anchors.end() - 1;
			}
		}
		
	}
	
	CView::OnMouseMove(nFlags, point);
}

/**
 * @brief Find the anchor near the point.
 * @param point, the current point pointed by the mouse.
 * @retval The anchor near the point if exist.
 **/
std::vector<Anchor>::iterator CImageAnnotationView::FindAnchor(CPoint point/*, HCURSOR& hCur*/)
{
	UINT threshold = 6;
	for (std::vector<Anchor>::iterator anchor = m_anchors.begin(); anchor != m_anchors.end(); anchor++)
	{
		if (anchor->m_up - threshold <= point.y && point.y <= anchor->m_up + threshold && anchor->m_x <= point.x && point.x <= anchor->m_x + anchor->m_w)
		{
			//hCur = LoadCursor(NULL, IDC_SIZENS);
			m_editState = EDIT_UP;
			return anchor;
		}
		else if (anchor->m_down - threshold <= point.y && point.y <= anchor->m_down + threshold && anchor->m_x <= point.x && point.x <= anchor->m_x + anchor->m_w)
		{
			//hCur = LoadCursor(NULL, IDC_SIZENS);
			m_editState = EDIT_DOWN;
			return anchor;
		}
		else if (anchor->m_left - threshold <= point.x && point.x <= anchor->m_left + threshold && anchor->m_y <= point.y && point.y <= anchor->m_y + anchor->m_h)
		{
			//hCur = LoadCursor(NULL, IDC_SIZEWE);
			m_editState = EDIT_LEFT;
			return anchor;
		}
		else if (anchor->m_right - threshold <= point.x && point.x <= anchor->m_right + threshold && anchor->m_y <= point.y && point.y <= anchor->m_y + anchor->m_h)
		{
			//hCur = LoadCursor(NULL, IDC_SIZEWE);
			m_editState = EDIT_RIGHT;
			return anchor;
		}
	}
	return m_anchors.end();
}

/**
 * @brief Determine whether a point is within the boundary.
 * @param point, the point to be determined.
 * @retval Whether a point is within the boundary, TRUE or FALSE.
 **/
BOOL CImageAnnotationView::PointWithinBoundary(CPoint point)
{
	if (m_x < point.x && point.x < m_x + m_w && m_y < point.y && point.y < m_y + m_h)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/**
 * @brief Plot the information of anchor on the window.
 * @param anchor, the anchor to be ploted.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchorInfo(Anchor anchor)
{
	CDC* pDC = GetDC();
	CFont newFont;
	CFont* defaultFont = pDC->SelectObject(&newFont);
	VERIFY(newFont.CreatePointFont(250, _T("宋体"), pDC));
	pDC->SetTextColor(COLOR_GREEN);
	pDC->SetBkMode(TRANSPARENT);
	CString text;
	text.Format(
		_T("Point(%d, %d), W=%d, H=%d"), 
		anchor.m_left, 
		anchor.m_up, 
		anchor.m_right - anchor.m_left, 
		anchor.m_down - anchor.m_up
	);
	pDC->TextOutW(anchor.m_right, anchor.m_up - 20, text);
}

/**
 * @brief Timer to runtime display anchor.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::OnTimer(UINT_PTR nIDEvent)
{
	Invalidate(FALSE);
	PlotBuffer();
	CView::OnTimer(nIDEvent);
}

/**
 * @brief Plot a anchor on the window.
 * @param anchor, the anchor to be ploted.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchor(Anchor anchor)
{
	CPen* pNewPen, * pOldPen;
	CDC* pDC = GetDC();
	pNewPen = new CPen;
	if (m_isPainting && !PointWithinBoundary(anchor.m_end))
	{
		pNewPen->CreatePen(SOLID_LINE, LINE_WIDTH, COLOR_RED);
	}
	else
	{
		pNewPen->CreatePen(SOLID_LINE, LINE_WIDTH, COLOR_GREEN);
		pOldPen = pDC->SelectObject(pNewPen);
		pDC->MoveTo(anchor.m_left, anchor.m_up);
		pDC->LineTo(anchor.m_right, anchor.m_up);
		pDC->LineTo(anchor.m_right, anchor.m_down);
		pDC->LineTo(anchor.m_left, anchor.m_down);
		pDC->LineTo(anchor.m_left, anchor.m_up);
		delete pNewPen;
	}
}

/**
 * @brief Plot all anchors on the window.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchors()
{
	if (m_isPainting)
	{
		LoadImageFile(m_imagePath);
		for (std::vector<Anchor>::iterator anchor = m_anchors.begin(); anchor != m_anchors.end(); anchor++)
		{
			PlotAnchor(*anchor);
			PlotAnchorInfo(*anchor);
		}
		PlotAnchor(Anchor(m_paintStart, m_paintMove));
	}
	else
	{
		for (std::vector<Anchor>::iterator anchor = m_anchors.begin(); anchor != m_anchors.end(); anchor++)
		{
			PlotAnchor(*anchor);
			PlotAnchorInfo(*anchor);
		}
	}
}

/**
 * @brief Plot the information of anchor in the buffer.
 * @param anchor, the anchor to be ploted.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchorInfoBuffer(Anchor anchor)
{
	CFont newFont;
	CFont* defaultFont = m_pDCBuffer->SelectObject(&newFont);
	VERIFY(newFont.CreatePointFont(250, _T("宋体"), m_pDCBuffer));
	m_pDCBuffer->SetTextColor(COLOR_GREEN);
	m_pDCBuffer->SetBkMode(TRANSPARENT);
	CString text;
	text.Format(
		_T("Point(%d, %d), W=%d, H=%d"),
		anchor.m_left,
		anchor.m_up,
		anchor.m_right - anchor.m_left,
		anchor.m_down - anchor.m_up
	);
	m_pDCBuffer->TextOutW(anchor.m_right, anchor.m_up - 20, text);
}

/**
 * @brief Plot a anchor in the buffer.
 * @param anchor, the anchor to be ploted.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchorBuffer(Anchor anchor)
{
	CPen* pNewPen, * pOldPen;
	pNewPen = new CPen;
	if (!PointWithinBoundary(anchor.m_end))
	{
		pNewPen->CreatePen(SOLID_LINE, LINE_WIDTH, COLOR_RED);
	}
	else
	{
		pNewPen->CreatePen(SOLID_LINE, LINE_WIDTH, COLOR_GREEN);
	}
	pOldPen = m_pDCBuffer->SelectObject(pNewPen);
	m_pDCBuffer->MoveTo(anchor.m_left, anchor.m_up);
	m_pDCBuffer->LineTo(anchor.m_right, anchor.m_up);
	m_pDCBuffer->LineTo(anchor.m_right, anchor.m_down);
	m_pDCBuffer->LineTo(anchor.m_left, anchor.m_down);
	m_pDCBuffer->LineTo(anchor.m_left, anchor.m_up);
	delete pNewPen;
}

/**
 * @brief Plot all anchors in the buffer.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::PlotAnchorsBuffer()
{
	for (std::vector<Anchor>::iterator anchor = m_anchors.begin(); anchor != m_anchors.end(); anchor++)
	{
		PlotAnchorBuffer(*anchor);
		PlotAnchorInfoBuffer(*anchor);
	}
}

/**
 * @brief Plot image in the buffer.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::PlotImageBuffer()
{
	CRect rect;
	GetClientRect(&rect);
	CImage image;
	image.Load(m_imagePath);
	image.BitBlt(*m_pDCBuffer, m_x, m_y, rect.Width(), rect.Height(), 0, 0);
	image.Destroy();
}

/**
 * @brief Plot buffer to the window.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::PlotBuffer()
{
	CRect rect;
	GetClientRect(&rect);

	CBrush brush(COLOR_WHITE);
	m_pDCBuffer->FillRect(rect, &brush);

	PlotImageBuffer();

	PlotAnchorsBuffer();

	m_pDC->BitBlt(m_x, m_y, rect.Width(), rect.Height(), m_pDCBuffer, 0, 0, SRCCOPY);
}

/**
 * @brief Check history annotation information.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::OnCheckTag()
{
	CheckTagDlg dlg(m_projectPath);
	dlg.DoModal();
	if (dlg.m_state == 2)
	{
		ClearScreen();
		LoadImageFile(dlg.m_imagePath);
		LoadTagFile(dlg.m_tagPath);
		PlotAnchors();
		dlg.m_state = 0;
	}
}

/**
 * @brief Revoke the last annotation.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::OnEditUndo()
{
	if (m_anchors.empty()) 
	{
		return;
	}
	else 
	{
		m_anchors.pop_back();
		PlotBuffer();
	}
}

/**
 * @brief Clear all annotations.
 * @param None.
 * @retval None.
 **/
void CImageAnnotationView::OnClear()
{
	if (m_anchors.empty())
	{
		return;
	}
	else
	{
		m_anchors.clear();
		PlotBuffer();
	}
}
