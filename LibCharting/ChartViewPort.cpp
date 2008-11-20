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
: CGUIFrameBase(), m_bKeyOnShift( false ), m_bKeyOnControl( false )
{
  CGUIFrameBase::SetPosition( -1000, 0, -200, 900 );
  CGUIFrameBase::Create( );

  CRect chartRect( 5, 5, 10, 10 );
  m_cm.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, chartRect, this, 65500 );
  SetChartMasterSize();

  m_refresh.Add( MakeDelegate( this, &CChartViewPort::HandlePeriodicRefresh ) );
} 

CChartViewPort::~CChartViewPort(void) {
  m_refresh.Remove( MakeDelegate( this, &CChartViewPort::HandlePeriodicRefresh ) );
}

void CChartViewPort::SetChartMasterSize( void ) {
  CRect clientRect;
  CFrameWnd::GetClientRect(&clientRect);
  CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
  m_cm.SetChartDimensions( chartRect.Width(), chartRect.Height() );
  m_cm.setMouseUsage( Chart::MouseUsageDefault );
  m_cm.setZoomDirection( Chart::DirectionHorizontal );
  m_cm.setScrollDirection( Chart::DirectionHorizontal );
  //m_cm.setZoomInWidthLimit( 60 * 1 );
  //m_cm.setViewPortWidth( 60 * 5 );
  m_cm.updateViewPort( true, false );
}

BEGIN_MESSAGE_MAP(CChartViewPort, CGUIFrameBase)
	ON_WM_DESTROY()
  ON_WM_SIZE()
//  ON_WM_SIZING( )
  ON_WM_MOUSEMOVE( )
  ON_WM_MOUSEWHEEL()
  ON_WM_HSCROLL( )
//  ON_WM_INITMENUPOPUP( )
  ON_WM_KEYDOWN( )
  ON_WM_KEYUP( )
//  ON_WM_SETCURSOR( )

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
//  LOG << "vp h=" << m_cm.getViewPortHeight()
//    << ", l=" << m_cm.getViewPortLeft() 
//    << ", t=" << m_cm.getViewPortTop()
//    << ", w=" << m_cm.getViewPortWidth();
  if ( m_cm.needUpdateChart() ) m_cm.DrawChart();
}

afx_msg BOOL CChartViewPort::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt ) {
  LOG << "mw "
    << "flags=" << nFlags
    << ", zDelta=" << zDelta
    << ", pt=" << pt.x << "," << pt.y;
  return 0;
}

afx_msg void CChartViewPort::OnMouseMove( UINT nFlags, CPoint point ) {
  LOG << "mm "
    << "nFlags=" << nFlags
    << ", point=" << point.x << "," << point.y;
}

afx_msg void CChartViewPort::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) {
  LOG << "sb "
    << "nSBCode=" << nSBCode
    << ", nPos=" << nPos;
}

afx_msg void CChartViewPort::OnInitMenuPopup( CMenu *, UINT, BOOL ) {
}

afx_msg void CChartViewPort::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  //LOG << "kd " << "nChar=" << nChar << ", nRepCnt=" << nRepCnt << ", nFlags=" << nFlags;
  switch ( nChar ) {
    case 16: // shift
      m_bKeyOnShift = true;
      SetMouseUsage();
      break; 
    case 17: // control
      m_bKeyOnControl = true;
      SetMouseUsage();
      break;
  }
}

afx_msg void CChartViewPort::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  //LOG << "ku " << "nChar=" << nChar << ", nRepCnt=" << nRepCnt << ", nFlags=" << nFlags;
  switch ( nChar ) {
    case 16: // shift
      m_bKeyOnShift = false;
      SetMouseUsage();
      break; 
    case 17: // control
      m_bKeyOnControl = false;
      SetMouseUsage();
      break;
  }
}

void CChartViewPort::SetMouseUsage( void ) {
  if ( m_bKeyOnShift && m_bKeyOnControl ) { // both are on
    m_cm.setMouseUsage( Chart::MouseUsageScroll );
  }
  else {
    if ( m_bKeyOnShift == m_bKeyOnControl ) { // both are off
      m_cm.setMouseUsage( Chart::MouseUsageDefault );
    }
    else {
      if ( m_bKeyOnShift ) {
        m_cm.setMouseUsage( Chart::MouseUsageZoomIn );
      }
      if ( m_bKeyOnControl ) {
        m_cm.setMouseUsage( Chart::MouseUsageZoomOut );
      }
    }
  }
}

afx_msg void CChartViewPort::OnSizing( UINT, LPRECT ) {
}

afx_msg BOOL CChartViewPort::OnSetCursor( CWnd*, UINT, UINT ) {
  return 0;
}

void CChartViewPort::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pMsg ){
  if ( m_cm.GetChartDataViewChanged() ) {
    m_cm.updateViewPort( true, true );
  }
}

