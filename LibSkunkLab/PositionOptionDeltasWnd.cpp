#include "StdAfx.h"
#include "PositionOptionDeltasWnd.h"

#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CPositionOptionDeltasWnd, CGUIFrameBase)

CPositionOptionDeltasWnd::CPositionOptionDeltasWnd( CWnd* pParent )
: CGUIFrameBase( pParent ), m_bDialogReady( false )  
{
}

CPositionOptionDeltasWnd::~CPositionOptionDeltasWnd(void) {
}

BEGIN_MESSAGE_MAP(CPositionOptionDeltasWnd, CGUIFrameBase)
//  ON_WM_MOVING( )
  ON_WM_MOVE( )
//  ON_WM_SIZING( )
  ON_WM_SIZE( )
  ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CPositionOptionDeltasWnd::Create() {
  BOOL b = CGUIFrameBase::Create();
  return b;
}

int CPositionOptionDeltasWnd::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
  int i = CGUIFrameBase::OnCreate( lpCreateStruct );

  CRect clientRect;
  CGUIFrameBase::GetClientRect(&clientRect);
  CRect reportRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );

  m_vuDeltas.Create( LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL, reportRect, this, 1 );
  m_vuDeltas.ShowWindow( 1 );
  // LVS_OWNERDRAWFIXED 

  m_refresh.Add( MakeDelegate( &m_vuDeltas, &CPositionOptionDeltasVu::HandlePeriodicRefresh ) ) ;

  m_bDialogReady = true;

  return i;
}
void CPositionOptionDeltasWnd::OnDestroy()  {
  m_bDialogReady = false;
  m_refresh.Remove( MakeDelegate( &m_vuDeltas, &CPositionOptionDeltasVu::HandlePeriodicRefresh ) ) ;
	CGUIFrameBase::OnDestroy();
}

afx_msg void CPositionOptionDeltasWnd::OnSize( UINT nType, int cx, int cy ) {  // new width, new height
  if ( m_bDialogReady ) {
    CGUIFrameBase::OnSize(nType,cx,cy);
    CRect clientRect;
    CFrameWnd::GetClientRect(&clientRect);
    CRect chartRect( clientRect.left + 5, clientRect.top + 5, clientRect.right - 5, clientRect.bottom - 5 );
    m_vuDeltas.MoveWindow( &chartRect );
    //m_prtView->SetChartDimensions( chartRect.Width(), chartRect.Height() );
    //std::cout << "size " << chartRect.Width() << ", " << chartRect.Height() << endl;
  }
}

afx_msg void CPositionOptionDeltasWnd::OnMove( int x, int y ) {
  LOG << "wpod move to " << x << "," << y;
}

