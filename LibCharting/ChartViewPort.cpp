#include "StdAfx.h"
#include "ChartViewPort.h"

#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartViewPort, CGUIFrameBase)

CChartViewPort::CChartViewPort( CChartDataView *cdv, CWnd* pParent )
: CGUIFrameBase()
{
  CGUIFrameBase::SetPosition( -1000, 0, -200, 900 );
  CGUIFrameBase::Create( );

  CRect chartRect( 5, 5, 10, 10 );
  m_cm.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, chartRect, this, 65500 );
  SetChartMasterSize();
} 

CChartViewPort::~CChartViewPort(void) {
}

void CChartViewPort::SetChartMasterSize( void ) {
  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  m_cm.SetChartDimensions( chartRect.Width(), chartRect.Height() );
  m_cm.setMouseUsage( Chart::MouseUsageScroll );
  m_cm.updateViewPort( true, true );
}

BEGIN_MESSAGE_MAP(CChartViewPort, CGUIFrameBase)
	ON_WM_DESTROY()
  ON_WM_SIZE()
  ON_WM_MOUSEWHEEL()
  ON_WM_HSCROLL( )
  ON_CONTROL(CVN_ViewPortChanged, 65500, OnViewPortChanged)
  //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CChartViewPort::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CChartViewPort::OnSize(UINT nType, int cx, int cy) {
  CGUIFrameBase::OnSize(nType,cx,cy);
  SetChartMasterSize();
}

void CChartViewPort::OnViewPortChanged() {
  LOG << "vp h=" << m_cm.getViewPortHeight()
    << ", l=" << m_cm.getViewPortLeft() 
    << ", t=" << m_cm.getViewPortTop()
    << ", w=" << m_cm.getViewPortWidth();
}

afx_msg BOOL CChartViewPort::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt ) {
  LOG << "mw "
    << "flags=" << nFlags
    << ", zDelta=" << zDelta
    << ", pt=" << pt.x << "," << pt.y;
  return 0;
}

afx_msg void CChartViewPort::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) {
  LOG << "sb "
    << "nSBCode=" << nSBCode
    << ", nPos=" << nPos;
}
