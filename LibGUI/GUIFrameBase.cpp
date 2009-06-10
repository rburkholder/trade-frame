#include "StdAfx.h"
#include "GUIFrameBase.h"

#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CGUIFrameBase, CFrameWnd)

CGUIFrameBase::CGUIFrameBase(CWnd* pParent /*=NULL*/) 
: CFrameWnd(), m_pParent( pParent )
{
  SetPosition( 10, 10, 100, 100 );  // may not work on pass through here, only when created
}

CGUIFrameBase::~CGUIFrameBase(void) {
}

void CGUIFrameBase::SetPosition( long left, long top, long right, long bottom ) {
  m_position.SetRect( left, top, right, bottom );
  if ( NULL != CFrameWnd::m_hWnd ) {
    CFrameWnd::MoveWindow( m_position );
  }
}

void CGUIFrameBase::SetTitleBarText( const std::string &sTitle ) {
  m_sDialogTitle = sTitle;
  if ( NULL != CFrameWnd::m_hWnd ) {
    CFrameWnd::SetWindowTextA( m_sDialogTitle.c_str() );
  }
}

BOOL CGUIFrameBase::Create( void ) {
  BOOL b = CFrameWnd::Create(NULL, m_sDialogTitle.c_str(), WS_OVERLAPPEDWINDOW, m_position, m_pParent );
  return b;
}

BEGIN_MESSAGE_MAP(CGUIFrameBase, CFrameWnd)
  ON_WM_CREATE()
	ON_WM_DESTROY()
  ON_WM_SIZE( )
  //ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

int CGUIFrameBase::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
  return CFrameWnd::OnCreate( lpCreateStruct );
}
void CGUIFrameBase::OnDestroy()  {
	CFrameWnd::OnDestroy();
}

afx_msg void CGUIFrameBase::OnSize(UINT nType, int cx, int cy) {
  CFrameWnd::OnSize(nType, cx, cy);
}

afx_msg void CGUIFrameBase::OnMove( int x, int y ) {
  LOG << "GUIFrameBase move to " << x << "," << y;
}
