// CtlPendingOrders.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "CtlPendingOrders.h"

// Custom Control articles

// http://www.codeproject.com/miscctrl/customcontrol.asp
// http://www.catch22.net/tuts/custctrl.asp

// CCtlPendingOrders

// typically updated through events generated in CGTOrderX

IMPLEMENT_DYNCREATE(CCtlPendingOrders, CListCtrl)

#define IXID 0
#define IXTIME 1
#define IXSIDE 2
#define IXQUAN 3
#define IXPRICE 4
#define IXSTATUS 5
#define IXACTION 6

CCtlPendingOrders::CCtlPendingOrders(): CListCtrl() {
}

CCtlPendingOrders::~CCtlPendingOrders() {
}

void CCtlPendingOrders::InitControl() {
  InsertColumn(IXID, _T( "ID" ), LVCFMT_LEFT, 40 );
  InsertColumn(IXTIME, _T( "Time" ), LVCFMT_CENTER, 60 );
  InsertColumn(IXSIDE, _T( "Side" ), LVCFMT_CENTER, 50 );
  InsertColumn(IXQUAN, _T( "Quan" ), LVCFMT_CENTER, 40 );
  InsertColumn(IXPRICE, _T( "Price" ), LVCFMT_CENTER, 50 );
  InsertColumn(IXSTATUS, _T( "Status" ), LVCFMT_CENTER, 60 );
  InsertColumn(IXACTION, _T( "Action" ), LVCFMT_CENTER, 60 );
}

void CCtlPendingOrders::InsertOrder( const char* szID, const char* szTime, 
    const char* szSide, 
    const char* szQuan, const char* szPrice, const char* szStatus, DWORD_PTR object ) {
  InsertItem( 0, szID );
  SetItemText( 0, IXTIME, szTime );
  SetItemText( 0, IXSIDE, szSide );
  SetItemText( 0, IXQUAN, szQuan );
  SetItemText( 0, IXPRICE, szPrice );
  SetItemText( 0, IXSTATUS, szStatus );
  SetItemText( 0, IXACTION, _T( "Cancel" ) );
  SetItemData( 0, object );
}

void CCtlPendingOrders::UpdateOrderStatus( const char* szID, const char *szStatus ) {
  LVFINDINFO info;
  info.psz = szID;
  int ix = FindItem( &info );
  if ( -1 != ix ) {
    SetItemText( ix, 5, szStatus );
  }
}

void CCtlPendingOrders::DeleteOrder( const char* szID ) {
  LVFINDINFO info;
  info.psz = szID;
  int ix = FindItem( &info );
  if ( -1 != ix ) {
    DeleteItem( ix );
  }
}


BEGIN_MESSAGE_MAP(CCtlPendingOrders, CListCtrl)
  ON_WM_LBUTTONUP( )
END_MESSAGE_MAP()


// CCtlPendingOrders diagnostics

#ifdef _DEBUG
void CCtlPendingOrders::AssertValid() const {
	CListCtrl::AssertValid();
}

#ifndef _WIN32_WCE
void CCtlPendingOrders::Dump(CDumpContext& dc) const {
	CListCtrl::Dump(dc);
}
#endif
#endif //_DEBUG

// http://www.codeguru.com/cpp/controls/listview/selection/article.php/c933/

// CCtlPendingOrders message handlers

afx_msg void CCtlPendingOrders::OnLButtonUp( UINT nFlags, CPoint point ) {

  CListCtrl::OnLButtonUp(nFlags, point);

  LVHITTESTINFO Info;
  Info.pt = point;
  //if( ( index = HitTest( point, NULL )) != -1 ) {
          //SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
          //                LVIS_SELECTED | LVIS_FOCUSED);
  //}
  SubItemHitTest( &Info );
//  CString s;
//  s.Format( "item %d, subitem %d", Info.iItem, Info.iSubItem );
//  theApp.pConsoleMessages->WriteLine( s );
  if ( -1 != Info.iItem ) {
    if ( IXACTION == Info.iSubItem ) {
      if ( NULL != OnCancel ) {
        OnCancel( GetItemData( Info.iItem ) );
      }
    }
  }
}