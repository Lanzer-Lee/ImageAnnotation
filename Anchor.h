#pragma once
class Anchor
{
public:
	CPoint m_start{};		// The start point of the anchor
	CPoint m_end{};			// The end point of the anchor
	UINT m_w{};				// The width of the anchor
	UINT m_h{};				// The height of the anchor
	UINT m_x{};				// The left up point's coordinate x of the anchor
	UINT m_y{};				// The left up point's coordinate y of the anchor
	UINT m_left{};			// The left boundary's coordinate x of the anchor
	UINT m_right{};			// The right boundary's coordinate x of the anchor
	UINT m_up{};			// The up boundary's coordinate y of the anchor
	UINT m_down{};			// The down boundary's coordinate y of the anchor
public:
	Anchor();
	Anchor(CPoint startPoint, CPoint endPoint);
	Anchor(UINT x, UINT y, UINT width, UINT height);
	~Anchor();
};

class ProjectPath
{
public:
	CString m_imagePath{};		// The path of the image being processed
	CString m_tagPath{};		// The path of the image's annotation will be saved
	ProjectPath();
	ProjectPath(CString imagePath, CString tagPath);
	~ProjectPath();
};
