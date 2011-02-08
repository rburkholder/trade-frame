#include "StdAfx.h"
#include "ChartViewPort.h"

#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartViewPort, CGUIFrameBase)

#define ID_CM 65500

CChartViewPort::CChartViewPort( CChartDataView *cdv, CWnd* pParent )
: CGUIFrameBase(), m_bKeyOnShift( false ), m_bKeyOnControl( false )
{
  CGUIFrameBase::SetPosition( -1000, 0, -200, 900 );
  CGUIFrameBase::Create( );

  CRect rectChart;
  CGUIFrameBase::GetClientRect( &rectChart );

  //CRect rectSB;
  //m_hscroll.Create( SBS_HORZ | SBS_BOTTOMALIGN, rectChart, this, 65500 );
  //m_hscroll.EnableWindow( false );

  //rectChart.bottom -= rectSB.Height() + 1;
  m_cm.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY, rectChart, this, ID_CM );

  //SetChartMasterSize();

  m_refresh.Add( MakeDelegate( this, &CChartViewPort::HandlePeriodicRefresh ) );
} 

CChartViewPort::~CChartViewPort(void) {
  m_refresh.Remove( MakeDelegate( this, &CChartViewPort::HandlePeriodicRefresh ) );
}

void CChartViewPort::SetChartMasterSize( void ) {
  //SCROLLINFO si;
  //si.cbSize = sizeof( SCROLLINFO );
  CGUIFrameBase::ShowScrollBar( SB_HORZ, true );
  //CGUIFrameBase::EnableScrollBar( SB_HORZ, ESB_DISABLE_BOTH );
  //CGUIFrameBase::EnableScrollBarCtrl( SB_HORZ, false );
  if ( m_cm.isCreated() ) {
    CRect rectClient;
    //CRect rectSB;
    CRect rectTemp;
    CGUIFrameBase::GetClientRect(&rectClient);  // 0,0, width, height
    m_cm.SetWindowPos( &CWnd::wndTop, 0, 0, rectClient.Width(), rectClient.Height(), SWP_NOCOPYBITS );
    //CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
    m_cm.SetChartDimensions( rectClient.Width(), rectClient.Height() );
    m_cm.setMouseUsage( Chart::MouseUsageDefault );
    m_cm.setZoomDirection( Chart::DirectionHorizontal );
    m_cm.setScrollDirection( Chart::DirectionHorizontal );
    //m_cm.setZoomInWidthLimit( 60 * 1 );
    //m_cm.setViewPortWidth( 60 * 5 );
    m_cm.updateViewPort( true, false );
  }
}

BEGIN_MESSAGE_MAP(CChartViewPort, CGUIFrameBase)
  ON_WM_CREATE()
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

  ON_BN_CLICKED(ID_CM, OnChartViewer)
  ON_CONTROL(CVN_ViewPortChanged, ID_CM, OnViewPortChanged)
  //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

afx_msg int CChartViewPort::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
  CGUIFrameBase::OnCreate( lpCreateStruct );
  SetChartMasterSize();
  return 0;  // -1 to destroy
}

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
  if ( m_cm.needUpdateChart() ) m_cm.DrawChart( true );
}

void CChartViewPort::OnChartViewer() {  // when chart is clicked in normal mode

  ImageMapHandler *handler = m_cm.getImageMapHandler();
  if (0 != handler) {
    //
    // Query the ImageMapHandler to see if the mouse is on a clickable hot spot. We 
    // consider the hot spot as clickable if its href ("path") parameter is not empty.
    //
    const char *path = handler->getValue("path");
    if ((0 != path) && (0 != *path))
    {
        // In this sample code, we just show all hot spot parameters.
        //CHotSpotDlg hs;
        //hs.SetData(handler);
        //hs.DoModal();
    }
  }
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
  switch ( nSBCode ) {
    case SB_LINELEFT:
      break;
    case SB_LINERIGHT:
      break;
    case SB_PAGELEFT:
      break;
    case SB_PAGERIGHT:
      break;
    case SB_THUMBPOSITION:  // issued when thumb released
      break;
    case SB_THUMBTRACK:  // issued during thumb move
      break;
    case SB_ENDSCROLL:
      break;
    case SB_RIGHT:
      break;
    case SB_LEFT:
      break;
  }
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
    //m_cm.updateViewPort( true, true );
    m_cm.DrawChart( false );
  }
}

