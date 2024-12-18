
// ImageAnnotationView.h: CImageAnnotationView 类的接口
//

#pragma once
#define DEFAULT_ORIGIN_X	0
#define DEFAULT_ORIGIN_Y	0
#define DEFAULT_BORDER_WIDTH	0
#define DEFAULT_BORDER_HEIGHT	0
#define LINE_WIDTH 3
#define SOLID_LINE	0
#define COLOR_WHITE	RGB(255, 255, 255)
#define COLOR_BLACK	RGB(0, 0, 0)
#define COLOR_RED	RGB(255, 0, 0)
#define COLOR_GREEN RGB(0, 255, 0)
#define COLOR_BLUE	RGB(0, 0, 255)
#define EDIT_NONE	0
#define EDIT_UP		1
#define EDIT_DOWN	2
#define EDIT_LEFT	3
#define EDIT_RIGHT	4
#define EDITING_UP		1
#define EDITING_DOWN	2
#define EDITING_LEFT	3
#define EDITING_RIGHT	4
#define STATE_NONE		0
#define STATE_INIT		1
#define STATE_DRAW		2
#define STATE_EDIT		3
#define STATE_EDITING	4

#include <vector>
#include "Anchor.h"
#include "CheckTagDlg.h"

class CImageAnnotationView : public CView
{
protected: // 仅从序列化创建
	CImageAnnotationView() noexcept;
	DECLARE_DYNCREATE(CImageAnnotationView)

// 特性
public:
	CImageAnnotationDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CImageAnnotationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	UINT m_x = DEFAULT_ORIGIN_X;		// The origin point's coordinate x of the image
	UINT m_y = DEFAULT_ORIGIN_Y;		// The origin point's coordinate y of the image
	UINT m_w = DEFAULT_BORDER_WIDTH;	// The width of the current image
	UINT m_h = DEFAULT_BORDER_HEIGHT;	// The height of the current image
	CString m_imagePath{};		// The path of the image being processed
	CString m_tagPath{};		// The path of the image's annotation will be saved
	BOOL m_init = FALSE;		// The flag that indicates whether system has been init
	BOOL m_isPainting = FALSE;	// The flag that indiactes whether user is painting anchor
	UINT m_editState = EDIT_NONE;
	UINT m_state = 0;
	std::vector<Anchor>::iterator m_editedAnchor{};
	CPoint m_paintStart;		// The start point of the anchor being painted
	CPoint m_paintEnd;			// The end point of the anchor being painted
	CPoint m_paintMove;			// The point that the mouse is on
	CDC* m_pDCBuffer{};			// The buffer of the window
	CClientDC* m_pDC{};			// The window displayed to user
	CBitmap* m_pBitMapBuffer{};	// The buffer of bit map 
	std::vector<Anchor>m_anchors{};				// A vector that stores anchors
	std::vector<ProjectPath>m_projectPath{};	// A vector that stores project path
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnNewAnnotationImage();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void ClearScreen();
	CString LoadImageFile();
	void LoadImageFile(CString filename);
	void LoadTagFile(CString filename);
	BOOL PointWithinBoundary(CPoint point);
	void PlotAnchorInfo(Anchor anchor);
	void PlotAnchor(Anchor anchor);
	void PlotAnchors();
	void PlotAnchorInfoBuffer(Anchor anchor);
	void PlotAnchorBuffer(Anchor anchor);
	void PlotAnchorsBuffer();
	void PlotImageBuffer();
	void PlotBuffer();
	std::vector<Anchor>::iterator FindAnchor(CPoint point/*, HCURSOR& hCur*/);
	afx_msg void OnCheckTag();
	afx_msg void OnEditUndo();
	virtual void OnInitialUpdate();
	afx_msg void OnClear();
};

#ifndef _DEBUG  // ImageAnnotationView.cpp 中的调试版本
inline CImageAnnotationDoc* CImageAnnotationView::GetDocument() const
   { return reinterpret_cast<CImageAnnotationDoc*>(m_pDocument); }
#endif

