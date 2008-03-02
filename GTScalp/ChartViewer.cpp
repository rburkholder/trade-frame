///////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2006 Advanced Software Engineering Limited
//
// You may use and modify the code in this file in your application, provided the code and
// its modifications are used only in conjunction with ChartDirector. Usage of this software
// is subjected to the terms and condition of the ChartDirector license.
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// CChartViewer Implementation
//
// The CChartViewer is a subclass of CStatic for displaying chart images. It extends CStatic 
// to support to support image maps, clickable hot spots with tool tips, zooming and scrolling
// support and image update rate control.
//
// To use CChartViewer, create the layout template using the Resource Editor with CStatic 
// (Picture) controls configured to display bitmaps. Remember to check the "Notify" check 
// box for the control. Create member variables for them using the ClassWizard as usual. 
// Then edit the header file to include "ChartViewer.h" and change CStatic to CChartViewer.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ChartViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChartViewer

//
// Build in mouse cursors for zooming and scrolling support
//
static HCURSOR getZoomInCursor();
static HCURSOR getZoomOutCursor();
static HCURSOR getNoZoomCursor();
static HCURSOR getNoMove2DCursor();
static HCURSOR getNoMoveHorizCursor();
static HCURSOR getNoMoveVertCursor();

//
// Constructor
//
CChartViewer::CChartViewer()
{
	// current chart and hot spot tester
	m_currentChart = 0;
	m_hotSpotTester = 0;

	// create the tool tip control
	m_ToolTip.Create(this);
	m_ToolTip.Activate(TRUE);

	// initialize chart configuration
	m_selectBoxLineColor = RGB(0, 0, 0);
	m_selectBoxLineWidth = 2;
	m_mouseUsage = Chart::MouseUsageDefault;
	m_zoomDirection = Chart::DirectionHorizontal;
	m_zoomInRatio = 2;
	m_zoomOutRatio = 0.5;
	m_scrollDirection = Chart::DirectionHorizontal;
	m_minDragAmount = 5;
	m_updateInterval = 20;

	// current state of the mouse
	m_isOnPlotArea = false;
	m_isPlotAreaMouseDown = false;
	m_isDragScrolling = false;
	m_currentHotSpot = -1;
	m_isClickable = false;

	// chart update rate support
	m_needUpdateChart = false;
	m_needUpdateImageMap = false;
	m_holdTimerActive = false;
}

BEGIN_MESSAGE_MAP(CChartViewer, CStatic)
	//{{AFX_MSG_MAP(CChartViewer)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChartViewer message handlers

//
// Free resources
//
void CChartViewer::OnDestroy() 
{
	CStatic::OnDestroy();
	
	// Free HBITMAP
	HBITMAP b = SetBitmap(0);
	if (0 != b)
		DeleteObject(b);

	// Free hot spot tester
	if (0 != m_hotSpotTester)
		delete m_hotSpotTester;
	m_hotSpotTester = 0;
}

//
// MouseMove event handler
//
void CChartViewer::OnMouseMove(UINT nFlags, CPoint point) 
{
	//
	// Call OnPlotAreaMouseDrag if mouse is dragging inside the plot area
	//

	m_isOnPlotArea = m_isPlotAreaMouseDown || inPlotArea(point.x, point.y);
	if (m_isPlotAreaMouseDown)
		OnPlotAreaMouseDrag(nFlags, point);

	//
	// Show hot spot tool tips if necessary
	//

	if (nFlags != 0)
	{
		// Hide tool tips if mouse button is pressed.
		m_ToolTip.UpdateTipText(_T(""), this);
	}
	else
	{
		// Use the ChartDirector ImageMapHandler to determine if the mouse is over a hot spot
		int hotSpotNo = 0;
		if (0 != m_hotSpotTester)
			hotSpotNo = m_hotSpotTester->getHotSpot(point.x, point.y);

		// If the mouse is in the same hot spot since the last mouse move event, there is no need
		// to update the tool tip.
		if (hotSpotNo != m_currentHotSpot)
		{
			// Hot spot has changed - update tool tip text
			m_currentHotSpot = hotSpotNo;

			if (hotSpotNo == 0)
			{
				// Mouse is not actually on hanlder hot spot - use default tool tip text and reset
				// the clickable flag.
				m_ToolTip.UpdateTipText(m_defaultToolTip, this);
				m_isClickable = false;
			}
			else
			{
				// Mouse is on a hot spot. In this implementation, we consider the hot spot as 
				// clickable if its href ("path") parameter is not empty.
				const char *path = m_hotSpotTester->getValue("path");
				m_isClickable = ((0 != path) && (0 != *path));

				// Use the title attribute as the tool tip. Note that ChartDirector uses UTF8, 
				// while MFC uses TCHAR, so we use the utility UTF8toTCHAR for conversion.
				m_ToolTip.UpdateTipText(UTF8toTCHAR(m_hotSpotTester->getValue("title")), this);
			}
		}
	}

	CStatic::OnMouseMove(nFlags, point);
}

//
// Intercept WM_SETCURSOR to change the mouse cursor.
//
BOOL CChartViewer::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_isOnPlotArea)
	{
		switch (m_mouseUsage)
		{
		case Chart::MouseUsageZoomIn:
			if (canZoomIn(m_zoomDirection))
				::SetCursor(getZoomInCursor());
			else
				::SetCursor(getNoZoomCursor());
			return TRUE;
		case Chart::MouseUsageZoomOut:
			if (canZoomOut(m_zoomDirection))
				::SetCursor(getZoomOutCursor());
			else
				::SetCursor(getNoZoomCursor());
			return TRUE;
		}
	}

	if (m_isClickable)
    {
		// Hand cursor = IDC_HAND = 32649
		HCURSOR h = AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(32649));
		if (NULL != h)
			::SetCursor(h);
		return TRUE;
    }

	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

//
// Mouse left button down event.
//
void CChartViewer::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (inPlotArea(point.x, point.y) && (m_mouseUsage != Chart::MouseUsageDefault))
	{
		// Mouse usage is for drag to zoom/scroll. Capture the mouse to prepare for dragging and 
		// save the mouse down position to draw the selection rectangle.
		SetCapture();
		m_isPlotAreaMouseDown = true;
		m_plotAreaMouseDownXPos = point.x;
		m_plotAreaMouseDownYPos = point.y;
		startDrag();
	}
	else
		CStatic::OnLButtonDown(nFlags, point);
}

//
// Mouse left button up event.
//
void CChartViewer::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_isPlotAreaMouseDown)
	{
		// Release the mouse capture.
		ReleaseCapture();
		m_isPlotAreaMouseDown = false;
		setRectVisible(false);
		bool hasUpdate = false;

		switch (m_mouseUsage)
		{
		case Chart::MouseUsageZoomIn :
			if (canZoomIn(m_zoomDirection))
			{
				if (isDrag(m_zoomDirection, point))
					// Zoom to the drag selection rectangle.
					hasUpdate = zoomTo(m_zoomDirection, 
						m_plotAreaMouseDownXPos, m_plotAreaMouseDownYPos, point.x, point.y);
				else
					// User just click on a point. Zoom-in around the mouse cursor position.
					hasUpdate = zoomAt(m_zoomDirection, point.x, point.y, m_zoomInRatio);
			}
			break;
		case Chart::MouseUsageZoomOut:
			// Zoom out around the mouse cursor position.
			if (canZoomOut(m_zoomDirection))
				hasUpdate = zoomAt(m_zoomDirection, point.x, point.y, m_zoomOutRatio);
			break;
		default :
			if (m_isDragScrolling)
				// Drag to scroll. We can update the image map now as scrolling has finished.
				updateViewPort(false, true);
			else
				// Is not zooming or scrolling, so is just a normal click event.
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED), (LPARAM)m_hWnd);
			break;
		}

		m_isDragScrolling = false;
		if (hasUpdate)
			// View port has changed - update it.
			updateViewPort(true, true);
	}
	else
		CStatic::OnLButtonUp(nFlags, point);
}

//
// Chart hold timer.
//
void CChartViewer::OnTimer(UINT nIDEvent) 
{
	// Reset the timer
	KillTimer(1);
	m_holdTimerActive = false;

	// If has pending chart update request, handles them now.
	if (m_needUpdateChart || m_needUpdateImageMap)
		updateViewPort(m_needUpdateChart, m_needUpdateImageMap);
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer overrides

void CChartViewer::PreSubclassWindow() 
{
	CStatic::PreSubclassWindow();
	// Connects the CChartViewer to the CToolTipCtrl control
	m_ToolTip.AddTool(this);
	m_ToolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, SHRT_MAX);
}

BOOL CChartViewer::PreTranslateMessage(MSG* pMsg) 
{
	// Remember to forward mouse messages to the CToolTipCtrl
	BOOL res = CStatic::PreTranslateMessage(pMsg);
	m_ToolTip.RelayEvent(pMsg);
	return res;
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer properties

//
// Set the chart to the control
//
void CChartViewer::setChart(BaseChart *c)
{
	// In case the user forgets to check the "Notify" check box in the Dialog editor, we set it
	// ourselves so the CChartViewer control can receive mouse events.
	if ((GetStyle() & SS_NOTIFY) == 0)
		ModifyStyle(0, SS_NOTIFY);

	// Get the HBITMAP and metrics for the chart
	HBITMAP chartBMP = 0;
	const char *chartMetrics = 0;

	if (0 != c)
	{
		// Output chart as Device Indpendent Bitmap with file headers
		MemBlock m = c->makeChart(Chart::BMP);

		// MFC expects HBITMAP, so convert from DIB to HBITMAP.	
		CDC *cdc = GetDC();
		chartBMP = CreateDIBitmap(
			cdc->m_hDC, 
			(const struct tagBITMAPINFOHEADER *)(m.data + 14), 
			CBM_INIT, 
			m.data + *(int *)(m.data + 10), 
			(const struct tagBITMAPINFO *)(m.data + 14),
			DIB_RGB_COLORS);
		ReleaseDC(cdc);

		// Get chart metrics
		chartMetrics = c->getChartMetrics();
	}

	// Set the HBITMAP for display
	m_currentChart = c;
	if (0 != (chartBMP = SetBitmap(chartBMP)))
		DeleteObject(chartBMP);

	// Set the chart metrics and clear the image map
	setChartMetrics(chartMetrics);
	setImageMap(0);
}

//
// Get back the same BaseChart pointer provided by the previous setChart call.
//
BaseChart *CChartViewer::getChart()
{
	return m_currentChart;
}

//
// Set image map used by the chart
//
void CChartViewer::setImageMap(const char *imageMap)
{
	//delete the existing ImageMapHandler
	if (0 != m_hotSpotTester)
		delete m_hotSpotTester;
	m_currentHotSpot = -1;
	m_isClickable = false;
	
	//create a new ImageMapHandler to represent the image map
	if ((0 == imageMap) || (0 == *imageMap))
		m_hotSpotTester = 0;
	else
		m_hotSpotTester = new ImageMapHandler(imageMap);
}

//
// Get the image map handler for the chart
//
ImageMapHandler *CChartViewer::getImageMapHandler()
{
	return m_hotSpotTester;
}

//
// Set the default tool tip to use
//
void CChartViewer::setDefaultToolTip(LPCTSTR text)
{
	m_defaultToolTip = text;
}

//
// Get the CToolTipCtrl for managing tool tips.
//
CToolTipCtrl *CChartViewer::getToolTipCtrl()
{
	return &m_ToolTip;
}

//
// Set the border width of the selection box
//
void CChartViewer::setSelectionBorderWidth(int width)
{
	m_selectBoxLineWidth = width;
}

//
// Get the border with of the selection box.
//
int CChartViewer::getSelectionBorderWidth()
{
	return m_selectBoxLineWidth;
}

//
// Set the border color of the selection box
//
void CChartViewer::setSelectionBorderColor(COLORREF c)
{
	m_selectBoxLineColor = c;
	if (m_TopLine.m_hWnd != 0)
	{
		m_TopLine.SetColor(c);
		m_LeftLine.SetColor(c);
		m_BottomLine.SetColor(c);
		m_RightLine.SetColor(c);
	}
}

//
// Get the border color of the selection box.
//
COLORREF CChartViewer::getSelectionBorderColor()
{
	return m_selectBoxLineColor;
}

//
// Set the mouse usage mode
//
void CChartViewer::setMouseUsage(int mouseUsage)
{
	m_mouseUsage = mouseUsage;
}

//
// Get the mouse usage mode
//
int CChartViewer::getMouseUsage()
{
	return m_mouseUsage;
}

//
// Set the zoom direction
//
void CChartViewer::setZoomDirection(int direction)
{
	m_zoomDirection = direction;
}

//
// Get the zoom direction
//
int CChartViewer::getZoomDirection()
{
	return m_zoomDirection;
}

//
// Set the scroll direction
//
void CChartViewer::setScrollDirection(int direction)
{
	m_scrollDirection = direction;
}

//
// Get the scroll direction
//
int CChartViewer::getScrollDirection()
{
	return m_scrollDirection;
}

//
// Set the zoom-in ratio for mouse click zoom-in
//
void CChartViewer::setZoomInRatio(double ratio)
{
	m_zoomInRatio = ratio;
}

//
// Get the zoom-in ratio for mouse click zoom-in
//
double CChartViewer::getZoomInRatio()
{
	return m_zoomInRatio;
}

//
// Set the zoom-out ratio
//
void CChartViewer::setZoomOutRatio(double ratio)
{
	m_zoomOutRatio = ratio;
}

//
// Get the zoom-out ratio
//
double CChartViewer::getZoomOutRatio()
{
	return m_zoomOutRatio;	
}

//
// Set the minimum mouse drag before the dragging is considered as real. This is to avoid small 
// mouse vibrations triggering a mouse drag.
//
void CChartViewer::setMinimumDrag(int offset)
{
	m_minDragAmount = offset;
}

//
// Get the minimum mouse drag before the dragging is considered as real.
//
int CChartViewer::getMinimumDrag()
{
	return m_minDragAmount;
}

//
// Set the minimum interval between ViewPortChanged events. This is to avoid the chart being 
// updated too frequently. (Default is 20ms between chart updates.) Multiple update events
// arrived during the interval will be merged into one chart update and executed at the end
// of the interval.
//
void CChartViewer::setUpdateInterval(int interval)
{
	m_updateInterval = interval;
}

//
// Get the minimum interval between ViewPortChanged events. 
//
int CChartViewer::getUpdateInterval()
{
	return m_updateInterval;
}

//
// Check if there is a pending chart update request. 
//
bool CChartViewer::needUpdateChart()
{
	return m_needUpdateChart;
}

//
// Check if there is a pending image map update request. 
//
bool CChartViewer::needUpdateImageMap()
{
	return m_needUpdateImageMap;
}

/////////////////////////////////////////////////////////////////////////////
// CChartViewer methods

//
// Create the edges for the selection rectangle
//
void CChartViewer::initRect()
{
	m_TopLine.Create(GetParent(), m_selectBoxLineColor);
	m_LeftLine.Create(GetParent(), m_selectBoxLineColor);
	m_BottomLine.Create(GetParent(), m_selectBoxLineColor);
	m_RightLine.Create(GetParent(), m_selectBoxLineColor);
}

//
// Set selection rectangle position and size
//
void CChartViewer::drawRect(int x, int y, int width, int height)
{
	// Create the edges of the rectangle if not already created
	if (m_TopLine.m_hWnd == 0)
		initRect();

	// width < 0 is interpreted as the rectangle extends to the left or x.
	// height <0 is interpreted as the rectangle extends to above y.
	if (width < 0)
		x -= (width = -width);
	if (height < 0)
		y -= (height = -height);

	// Compute the position of the selection rectangle as relative to the parent window
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + width;
	rect.bottom = y + height;
	MapWindowPoints(GetParent(), &rect);

	// Put the edges along the sides of the rectangle
	m_TopLine.MoveWindow(rect.left, rect.top, rect.right - rect.left, m_selectBoxLineWidth);
	m_LeftLine.MoveWindow(rect.left, rect.top, m_selectBoxLineWidth, rect.bottom - rect.top);
	m_BottomLine.MoveWindow(rect.left, rect.bottom - m_selectBoxLineWidth + 1, 
		rect.right - rect.left, m_selectBoxLineWidth);
	m_RightLine.MoveWindow(rect.right - m_selectBoxLineWidth + 1, rect.top, 
		m_selectBoxLineWidth, rect.bottom - rect.top);
}

//
// Show/hide selection rectangle
//
void CChartViewer::setRectVisible(bool b)
{
	// Create the edges of the rectangle if not already created
	if (b && (m_TopLine.m_hWnd == 0)) 
		initRect();

	// Show/hide the edges
	if (m_TopLine.m_hWnd != 0)
	{
		int state = b ? SW_SHOW : SW_HIDE;
		m_TopLine.ShowWindow(state);
		m_LeftLine.ShowWindow(state);
		m_BottomLine.ShowWindow(state);
		m_RightLine.ShowWindow(state);
	}
}

//
// Determines if the mouse is dragging.
//
bool CChartViewer::isDrag(int direction, CPoint point)
{
	// We only consider the mouse is dragging it is has dragged more than m_minDragAmount. This is
	// to avoid small mouse vibrations triggering a mouse drag.
	int spanX = abs(point.x - m_plotAreaMouseDownXPos);
	int spanY = abs(point.y - m_plotAreaMouseDownYPos);
	return ((direction != Chart::DirectionVertical) && (spanX >= m_minDragAmount)) ||
		((direction != Chart::DirectionHorizontal) && (spanY >= m_minDragAmount));
}

//
// Process mouse dragging over the plot area
//
void CChartViewer::OnPlotAreaMouseDrag(UINT nFlags, CPoint point)
{
	switch (m_mouseUsage)
	{
		case Chart::MouseUsageZoomIn :
		{
			//
			// Mouse is used for zoom in. Draw the selection rectangle if necessary.
			//

			bool isDragZoom = canZoomIn(m_zoomDirection) && isDrag(m_zoomDirection, point);
			if (isDragZoom)
			{
				int spanX = m_plotAreaMouseDownXPos - point.x;
				int spanY = m_plotAreaMouseDownYPos - point.y;

				switch (m_zoomDirection)
				{
				case Chart::DirectionHorizontal:
					drawRect(point.x, getPlotAreaTop(), spanX, getPlotAreaHeight());
					break;
				case Chart::DirectionVertical:
					drawRect(getPlotAreaLeft(), point.y, getPlotAreaWidth(), spanY);
					break;
				default:
					drawRect(point.x, point.y, spanX, spanY);
					break;
				}
			}
			setRectVisible(isDragZoom);
			break;
		}
		case Chart::MouseUsageScroll :
		{
			//
			// Mouse is used for drag scrolling. Scroll and update the view port.
			//

			if (m_isDragScrolling || isDrag(m_scrollDirection, point))
			{
				m_isDragScrolling = true;
				switch (m_scrollDirection)
				{
				case Chart::DirectionHorizontal:
					::SetCursor(getNoMoveHorizCursor());
					break;
				case Chart::DirectionVertical:
					::SetCursor(getNoMoveVertCursor());
					break;
				default :
					::SetCursor(getNoMove2DCursor());
					break;
				}
								
				if (dragTo(m_scrollDirection, 
					point.x - m_plotAreaMouseDownXPos, point.y - m_plotAreaMouseDownYPos))
					updateViewPort(true, false);
			}
		}
	}
}

//
// Trigger the ViewPortChanged event
//
void CChartViewer::updateViewPort(bool needUpdateChart, bool needUpdateImageMap)
{
	// Merge the current update requests with any pending requests.
	m_needUpdateChart = m_needUpdateChart || needUpdateChart;
	m_needUpdateImageMap = needUpdateImageMap;

	// Hold timer has not expired, so do not update chart immediately. Keep the requests pending.
	if (m_holdTimerActive)
		return;

	// Can update chart now. Trigger the ViewPortChanged event.
	validateViewPort();
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), CVN_ViewPortChanged), 
		(LPARAM)m_hWnd);

	// Clear any pending updates.
	m_needUpdateChart = false;
	m_needUpdateImageMap = false;

	// Set hold timer to prevent multiple chart updates within a short period.
	if (m_updateInterval > 0)
	{
		m_holdTimerActive = true;
		SetTimer(1, m_updateInterval, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl
//
// A rectangle with a background color. Use as thick edges for the selection
// rectangle.
//

//
// Create control with a given color
//
BOOL CRectCtrl::Create(CWnd *pParentWnd, COLORREF c)
{
	SetColor(c);

	RECT r;
	r.left = r.top = r.right = r.bottom = 0;
	return CStatic::Create(0, WS_CHILD, r, pParentWnd);
}

BEGIN_MESSAGE_MAP(CRectCtrl, CStatic)
	//{{AFX_MSG_MAP(CRectCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl message handlers

HBRUSH CRectCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return (HBRUSH)m_Color.m_hObject;
}

/////////////////////////////////////////////////////////////////////////////
// CRectCtrl proporties

//
// Set the background color
//
void CRectCtrl::SetColor(COLORREF c)
{
	m_Color.CreateSolidBrush(c);
}

/////////////////////////////////////////////////////////////////////////////
// Build in mouse cursors for zooming and scrolling support
//

static const int zoomInCursorA[] = 
{ 
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int zoomInCursorB[] = 
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01e00,
0x00f83e00,
0x00f83e00,
0x00183000,
0x00f83e00,
0x00f83e00,
0x00f01e00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static const int zoomOutCursorA[] = 
{ 
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int zoomOutCursorB[] = 
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01f00,
0x00f83f00,
0x00f83f00,
0x00183000,
0x00f83f00,
0x00f83f00,
0x00f01f00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static const int noZoomCursorA[] = 
{ 
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int noZoomCursorB[] = 
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f01f00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static int noMove2DCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff0ff0ff,
0xff07e0ff,
0xff03c0ff,
0xff03c0ff,
0xfffc3fff,
0x7ffc3ffe,
0x3ffc3ffc,
0x1f7c3ef8,
0x0f3c3cf0,
0x071c38e0,
0x071c38e0,
0x0f3c3cf0,
0x1f7c3ef8,
0x3ffc3ffc,
0x7ffc3ffe,
0xfffc3fff,
0xff03c0ff,
0xff03c0ff,
0xff07e0ff,
0xff0ff0ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMove2DCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00100800,
0x00081000,
0x00042000,
0x00fc3f00,
0x0003c000,
0x80024001,
0x40024002,
0x20824104,
0x10424208,
0x08224410,
0x08224410,
0x10424208,
0x20824104,
0x40024002,
0x80024001,
0x0003c000,
0x00fc3f00,
0x00042000,
0x00081000,
0x00100800,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000
};

static int noMoveHorizCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xfffc3fff,
0x7ffc3ffe,
0x3ffc3ffc,
0x1f7c3ef8,
0x0f3c3cf0,
0x071c38e0,
0x071c38e0,
0x0f3c3cf0,
0x1f7c3ef8,
0x3ffc3ffc,
0x7ffc3ffe,
0xfffc3fff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMoveHorizCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x0003c000,
0x80024001,
0x40024002,
0x20824104,
0x10424208,
0x08224410,
0x08224410,
0x10424208,
0x20824104,
0x40024002,
0x80024001,
0x0003c000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static int noMoveVertCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff0ff0ff,
0xff07e0ff,
0xff03c0ff,
0xff03c0ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff03c0ff,
0xff03c0ff,
0xff07e0ff,
0xff0ff0ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMoveVertCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00100800,
0x00081000,
0x00042000,
0x00fc3f00,
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000,
0x00fc3f00,
0x00042000,
0x00081000,
0x00100800,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000
};

static HCURSOR hZoomInCursor = 0;
static HCURSOR hZoomOutCursor = 0;
static HCURSOR hNoZoomCursor = 0;
static HCURSOR hNoMove2DCursor = 0;
static HCURSOR hNoMoveHorizCursor = 0;
static HCURSOR hNoMoveVertCursor = 0;

class FreeCursors
{
public:
	~FreeCursors()
	{
		if (0 != hZoomInCursor)
			DestroyCursor(hZoomInCursor);
		if (0 != hZoomOutCursor)
			DestroyCursor(hZoomOutCursor);
		if (0 != hNoZoomCursor)
			DestroyCursor(hNoZoomCursor);
		if (0 != hNoMove2DCursor)
			DestroyCursor(hNoMove2DCursor);
		if (0 != hNoMoveHorizCursor)
			DestroyCursor(hNoMoveHorizCursor);
		if (0 != hNoMoveVertCursor)
			DestroyCursor(hNoMoveVertCursor);
	}
} dummyFreeCursorObj;

static HCURSOR getZoomInCursor()
{
	if (0 == hZoomInCursor)
	{
		hZoomInCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			zoomInCursorA, zoomInCursorB);
	}
	return hZoomInCursor;
}

static HCURSOR getZoomOutCursor()
{
	if (0 == hZoomOutCursor)
	{
		hZoomOutCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			zoomOutCursorA, zoomOutCursorB);
	}
	return hZoomOutCursor;
}

static HCURSOR getNoZoomCursor()
{
	if (0 == hNoZoomCursor)
	{
		hNoZoomCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			noZoomCursorA, noZoomCursorB);
	}
	return hNoZoomCursor;
}

static HCURSOR getNoMove2DCursor()
{
	if (0 == hNoMove2DCursor)
	{
		hNoMove2DCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			noMove2DCursorA, noMove2DCursorB);
	}
	return hNoMove2DCursor;
}

static HCURSOR getNoMoveHorizCursor()
{
	if (0 == hNoMoveHorizCursor)
	{
		hNoMoveHorizCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			noMoveHorizCursorA, noMoveHorizCursorB);
	}
	return hNoMoveHorizCursor;
}

static HCURSOR getNoMoveVertCursor()
{
	if (0 == hNoMoveVertCursor)
	{
		hNoMoveVertCursor = CreateCursor(AfxGetApp()->m_hInstance, 15, 15, 32, 32, 
			noMoveVertCursorA, noMoveVertCursorB);
	}
	return hNoMoveVertCursor;
}
