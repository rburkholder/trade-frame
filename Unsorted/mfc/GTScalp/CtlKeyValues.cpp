// CtlKeyValues.cpp : implementation file
//

#include "stdafx.h"
//#include "GTScalp.h"
#include "Resource.h"
#include "CtlKeyValues.h"
//#include "RowKeyValues.h"


// CCtlKeyValues

IMPLEMENT_DYNAMIC(CCtlKeyValues, CCtlListCtrlBase)

CCtlKeyValues::CCtlKeyValues(): CCtlListCtrlBase() {
 
}

CCtlKeyValues::~CCtlKeyValues() {
  while ( !m_vRowKeyValues.empty() ) {
    LPROWKEYVALUES &kv = m_vRowKeyValues.back();
    delete kv;
    m_vRowKeyValues.pop_back();
  }
}

void CCtlKeyValues::InitControl() {

  CCtlListCtrlBase::InitControl();

  //EnableToolTips();

  m_ColumnHeaders.Append( new CLVColumn( _T( "Contract" ),LVCFMT_LEFT, 80 ) );
//  InsertColumn(IXPOSITION , _T( "Position" ), LVCFMT_RIGHT, 40 );
  m_ColumnHeaders.Append( new CLVColumn( _T( "+/-" ), LVCFMT_RIGHT, 50 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Prv Cls" ), LVCFMT_RIGHT, 70 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "B#" ), LVCFMT_CENTER, 40 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Bid" ), LVCFMT_RIGHT, 80 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "T#" ), LVCFMT_CENTER, 40 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Price" ), LVCFMT_RIGHT, 80 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "A#" ), LVCFMT_CENTER, 40 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Ask" ), LVCFMT_RIGHT, 80 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Low" ), LVCFMT_RIGHT, 70 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "High" ), LVCFMT_RIGHT, 70 ) );
  m_ColumnHeaders.Append( new CLVColumn( _T( "Volume" ), LVCFMT_RIGHT, 70 ) );
//  InsertColumn(IXPNL , _T( "PnL" ), LVCFMT_RIGHT, 40 );
//  InsertColumn(IXDEFAULTQUAN , _T( "Dflt Quan" ), LVCFMT_RIGHT, 40 );

  //CListCtrl::SubItemHitTest

  //m_nRowCount = 0;

}

CRowKeyValues *CCtlKeyValues::InsertSymbol( const char *szSymbol ) {

  InsertItem( m_nRowCount, szSymbol );
  CRowKeyValues *row = new CRowKeyValues( m_nRowCount++, this, szSymbol );
  m_vRowKeyValues.push_back( row );

  std::vector<CRowKeyValues *>::size_type i;
  i = m_vRowKeyValues.size();

  return row;
}

LPCTSTR CCtlKeyValues::GetFullName( int ix ) {
  return m_vRowKeyValues[ ix ]->GetFullName();
}


void CCtlKeyValues::fn( NMHDR * nm, LRESULT * pResult ) {
  if ( 0 == nm ) {
  }
}

afx_msg void CCtlKeyValues::OnLButtonUp( UINT, CPoint ) {
}

afx_msg void CCtlKeyValues::OnLButtonDown( UINT, CPoint ) {
}

BEGIN_MESSAGE_MAP(CCtlKeyValues, CCtlListCtrlBase)
//  ON_NOTIFY_EX( TTN_NEEDTEXT, 0, memberFxn )
//  ON_NOTIFY_EX( LVN_GETINFOTIP, IDC_KEYVALUES, memberFxn )
//  ON_NOTIFY_EX( NM_HOVER, IDC_KEYVALUES, memberFxn )
//  ON_NOTIFY_EX( LVN_KEYDOWN, IDC_KEYVALUES, memberFxn )
  ON_NOTIFY( LVN_KEYDOWN, IDC_KEYVALUES, fn )
  ON_NOTIFY( NM_HOVER, IDC_KEYVALUES, fn )
  ON_NOTIFY( LVN_GETINFOTIP, IDC_KEYVALUES, fn )
  ON_WM_LBUTTONUP( )
  ON_WM_LBUTTONDOWN( )
  //ON_NOTIFY(NM_CUSTOMDRAW, IDC_KEYVALUES, &CCtlKeyValues::OnNMCustomdrawKeyvalues2)
END_MESSAGE_MAP()



// CCtlKeyValues message handlers

void CCtlKeyValues::OnNMCustomdrawKeyvalues(NMHDR *pNMHDR, LRESULT *pResult) {
  //LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
  LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
  
  *pResult = CDRF_DODEFAULT;

  if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage ) {
    *pResult = CDRF_NOTIFYITEMDRAW;
  }
  else {
    if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage ) {
      *pResult = CDRF_NOTIFYSUBITEMDRAW;
    }
    else { 
      if ( ( CDDS_ITEMPREPAINT | CDDS_SUBITEM ) == pLVCD->nmcd.dwDrawStage ) {

        std::vector <CRowKeyValues *>::size_type i;
        i = m_vRowKeyValues.size();
        if ( 0 < i ) {
          CRowKeyValues *row;
          row = m_vRowKeyValues[pLVCD->nmcd.dwItemSpec ];
          CDeltaMarkerBase *pMarker;
          pMarker = row->GetMarker( pLVCD->iSubItem );
          
          pLVCD->clrText = pMarker->colForeground;
          pLVCD->clrTextBk = pMarker->colBackground;
          //*pResult = CDRF_DODEFAULT;
        }
      }
    }
  }
}

