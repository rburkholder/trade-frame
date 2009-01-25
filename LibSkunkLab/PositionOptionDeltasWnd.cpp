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
  m_bDialogReady = true;
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

  // this stuff should be pulled from the record structure

  int ix = 0;
  m_vuDeltas.InsertColumn( ix++, "UndSym", LVCFMT_LEFT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Sym", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Strk", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Expiry", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Bid", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "BidSz", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Sprd", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Ask", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "AskSz", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Pos", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "AvgCst", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Delta", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "Gamma", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "UnRlPL", LVCFMT_RIGHT, 50 );
  m_vuDeltas.InsertColumn( ix++, "RlPL", LVCFMT_RIGHT, 50 );

  return i;
}
void CPositionOptionDeltasWnd::OnDestroy()  {
	CGUIFrameBase::OnDestroy();
}

afx_msg void CPositionOptionDeltasWnd::OnSize( UINT i, int nx, int ny ) {  // new width, new heigh
  if ( m_bDialogReady ) {
    //SetClientPositions();
  }
}

afx_msg void CPositionOptionDeltasWnd::OnMove( int x, int y ) {
  LOG << "wpod move to " << x << "," << y;
}

