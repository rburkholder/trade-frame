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
  m_refresh.Add( MakeDelegate( &m_vuDeltas, &CPositionOptionDeltasVu::HandlePeriodicRefresh ) ) ;
  m_bDialogReady = true;

  return i;
}
void CPositionOptionDeltasWnd::OnDestroy()  {
  m_bDialogReady = false;
  m_refresh.Remove( MakeDelegate( &m_vuDeltas, &CPositionOptionDeltasVu::HandlePeriodicRefresh ) ) ;
	CGUIFrameBase::OnDestroy();
}

afx_msg void CPositionOptionDeltasWnd::OnSize( UINT i, int nx, int ny ) {  // new width, new height
  if ( m_bDialogReady ) {
    //SetClientPositions();
  }
}

afx_msg void CPositionOptionDeltasWnd::OnMove( int x, int y ) {
  LOG << "wpod move to " << x << "," << y;
}

