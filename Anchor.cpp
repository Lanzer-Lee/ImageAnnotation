#include "pch.h"
#include "Anchor.h"

/**
 * @brief Init a anchor default.
 * @param None.
 * @retval None.
 **/
Anchor::Anchor()
{
	m_x = 0;
	m_y = 0;
	m_w = 0;
	m_h = 0;
	m_left = 0;
	m_right = 0;
	m_up = 0;
	m_down = 0;
	m_start = CPoint(0, 0);
	m_end = CPoint(0, 0);
}

/**
 * @brief Init a anchor through start point and end point.
 * @param start, start ponit.
 * @param end, end point
 * @retval None.
 **/
Anchor::Anchor(CPoint start, CPoint end)
{
	m_start = start;
	m_end = end;
	m_left = min(start.x, end.x);
	m_right = max(start.x, end.x);
	m_up = min(start.y, end.y);
	m_down = max(start.y, end.y);
	m_x = m_left;
	m_y = m_up;
	m_w = m_right - m_left;
	m_h = m_down - m_up;
}

/**
 * @brief Init a anchor through coordinate x, y and its width, height.
 * @param x, the origin coordinate x of anchor
 * @param y, the origin coordinate y of anchor
 * @param w, the width of anchor
 * @param h, the height of anchor 
 * @retval None.
 **/
Anchor::Anchor(UINT x, UINT y, UINT w, UINT h) 
{
	m_x = x;
	m_y = y;
	m_w = w;
	m_h = h;
	m_start = CPoint(m_x, m_y);
	m_end = CPoint(m_x + m_w, m_y + m_h);
	m_left = min(m_start.x, m_end.x);
	m_right = max(m_start.x, m_end.x);
	m_up = min(m_start.y, m_end.y);
	m_down = max(m_start.y, m_end.y);
}

/**
 * @brief Delete a anchor.
 * @param None.
 * @retval None.
 **/
Anchor::~Anchor()
{

}


/**
 * @brief Init a default project path.
 * @param None.
 * @retval None.
 **/
ProjectPath::ProjectPath()
{
	m_imagePath = _T("None");
	m_imagePath = _T("None");
}

/**
 * @brief Init a project path.
 * @param imagePath, the path of image.
 * @param tagPath, the path of annotation
 * @retval None.
 **/
ProjectPath::ProjectPath(CString imagePath, CString tagPath)
{
	m_imagePath = imagePath;
	m_tagPath = tagPath;
}

/**
 * @brief Delete a project path.
 * @param None.
 * @retval None.
 **/
ProjectPath::~ProjectPath()
{

}


