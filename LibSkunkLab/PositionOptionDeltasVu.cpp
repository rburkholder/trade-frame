#include "StdAfx.h"
#include "PositionOptionDeltasVu.h"

#include "PositionOptionDeltasMacros.h"

#include "Log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CPositionOptionDeltasVu, CListCtrl)

CPositionOptionDeltasVu::CPositionOptionDeltasVu(void) 
: CListCtrl()
{
}

CPositionOptionDeltasVu::~CPositionOptionDeltasVu(void) {
}

BEGIN_MESSAGE_MAP(CPositionOptionDeltasVu, CListCtrl)
  ON_WM_DESTROY( )
  ON_WM_CLOSE( )
  ON_WM_CREATE( )
  ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomDraw )
END_MESSAGE_MAP()


afx_msg int CPositionOptionDeltasVu::OnCreate( LPCREATESTRUCT lpCreateStruct ) {

  int i  = CListCtrl::OnCreate( lpCreateStruct );

  CListCtrl::SetView( LV_VIEW_DETAILS );

  int ix = 0;
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_InsertColumn, ix )
  // InsertColumn( ix++, "UndSym", LVCFMT_LEFT, 50 );

  return i;
}

afx_msg void CPositionOptionDeltasVu::OnDestroy( ) {
  CListCtrl::OnDestroy();
}

afx_msg void CPositionOptionDeltasVu::OnClose( ) {
  CListCtrl::OnClose();
}

afx_msg void CPositionOptionDeltasVu::OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult ) {
  NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
  LOG << "in CPositionOptionDeltasVu::OnCustomDraw";
}
