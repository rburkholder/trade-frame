// BasketTradeViewDialog.cpp : implementation file
//

// http://msdn.microsoft.com/en-us/library/6t3612sk(VS.80).aspx

#include "stdafx.h"
#include "BasketTradeViewDialog.h"
//#include "BasketTradeSymbolInfo.h"

#include <locale>
//#include <iomanip>
#include "boost/date_time/local_time/local_time_io.hpp"
#include "boost/date_time/time_facet.hpp"

// CBasketTradeViewDialog dialog

IMPLEMENT_DYNAMIC(CBasketTradeViewDialog, CDialog)

CBasketTradeViewDialog::CBasketTradeViewDialog(CBasketTradeModel *pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CBasketTradeViewDialog::IDD, pParent),
  m_pModel( pModel ), m_bSourceChanged( false ), bDialogReady( false )
{
  //http://www.boost.org/doc/libs/1_36_0/doc/html/date_time/date_time_io.html#date_time.io_tutorial

  boost::local_time::local_time_facet *facet = new boost::local_time::local_time_facet();
  facet->format( "%Y %m %d %H:%M:%S" );
  //m_ssDateTime.imbue( locale( std::locale::classic(), facet ) );
  m_ssDateTime.imbue( locale( m_ssDateTime.getloc(), facet ) );

  UINT id = IDD_DLGBASKETSYMBOLS;
  BOOL b = Create(id, pParent );
} 

CBasketTradeViewDialog::~CBasketTradeViewDialog() {
}

BOOL CBasketTradeViewDialog::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  int ix = 0;
  m_lcBasketSymbols.InsertColumn( ix++, "Sym", LVCFMT_LEFT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Current", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "High", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "OpnHi", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Open", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "OpnLo", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Low", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Filled", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Stop", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Quan", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Amt", LVCFMT_RIGHT, 70 );
  m_lcBasketSymbols.InsertColumn( ix++, "AvgCst", LVCFMT_RIGHT, 80 );
  m_lcBasketSymbols.InsertColumn( ix++, "UnrelPL", LVCFMT_RIGHT, 60 );
  m_lcBasketSymbols.InsertColumn( ix++, "RelPL", LVCFMT_RIGHT, 80 );
  m_lcBasketSymbols.InsertColumn( ix++, "RunPL", LVCFMT_CENTER, 80 );

  // initialize display with a top level totals record
  m_Totals.sSymbolName = _T( "Totals" );
  structDialogEntry entry( 0, &m_Totals );
  m_mapDialogEntry.insert( mapDialogEntry_t( m_Totals.sSymbolName, entry ) );
  m_lcBasketSymbols.InsertItem( 0, m_Totals.sSymbolName.c_str() );

  // add appropriate event handlers
  m_refresh.OnRefresh.Add( MakeDelegate( this, &CBasketTradeViewDialog::HandlePeriodicRefresh ) );
  m_pModel->OnBasketTradeSymbolInfoAddedToBasket.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded ) );

  bDialogReady = true;

  return TRUE;
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolInfo *pInfo ) {
  int ix = m_mapDialogEntry.size();
  structDialogEntry entry( ix, pInfo, pInfo->GetDialogFields() );
  m_mapDialogEntry.insert( mapDialogEntry_t( pInfo->GetDialogFields()->sSymbolName, entry ) );
  m_lcBasketSymbols.InsertItem( ix, pInfo->GetDialogFields()->sSymbolName.c_str() );
  pInfo->OnBasketTradeSymbolInfoChanged.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged ) );
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolInfo *pInfo ) {
  std::map<std::string, structDialogEntry>::iterator iter
    = m_mapDialogEntry.find( pInfo->GetDialogFields()->sSymbolName );
  if ( m_mapDialogEntry.end() != iter ) {
    iter->second.bChanged = true;
    m_bSourceChanged = true;
  }
}

void CBasketTradeViewDialog::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pRefresh ) {
  //ptime now;
  //now = m_ts.Internal();
  //m_ssDateTime << now << "." << now.time_of_day().fractional_seconds();
  //m_ssDateTime.flush();
  m_ssDateTime.str( "" );
  m_ssDateTime << m_ts.Internal();
  m_lblDateTime.SetWindowTextA( m_ssDateTime.str().c_str() );
  if ( m_bSourceChanged ) {
    m_Totals.nPositionSize = 0;
    m_Totals.dblPositionSize = 0;
    m_Totals.dblAverageCost = 0;
    m_Totals.dblUnRealizedPL = 0;
    m_Totals.dblRealizedPL = 0;
    m_Totals.dblRunningPL = 0;
    char conv[ 30 ];
    for ( std::map<std::string, structDialogEntry>::iterator iter = m_mapDialogEntry.begin();
      iter != m_mapDialogEntry.end(); ++iter ) {
        if ( 0 != iter->second.ix ) {  // 0th entry is our summary record, so don't add it to itself
          m_Totals.nPositionSize += iter->second.pFields->nPositionSize;
          m_Totals.dblPositionSize += iter->second.pFields->dblPositionSize;
          m_Totals.dblAverageCost += iter->second.pFields->dblAverageCost;
          m_Totals.dblUnRealizedPL += iter->second.pFields->dblUnRealizedPL;
          m_Totals.dblRealizedPL += iter->second.pFields->dblRealizedPL;
          m_Totals.dblRunningPL = m_Totals.dblUnRealizedPL + m_Totals.dblRealizedPL;
          if ( iter->second.bChanged ) {
            iter->second.bChanged = false;
            const CBasketTradeSymbolInfo::structFieldsForDialog *pFields = iter->second.pFields;
            int ix = iter->second.ix;
            int iy = 0;
            sprintf( conv, "%.2f", pFields->dblCurrentPrice ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblHigh ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblOpenRangeHigh ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblOpen ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblOpenRangeLow ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblLow ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblFilledPrice ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblStop ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv,   "%d", pFields->nPositionSize ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblPositionSize ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblAverageCost ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblUnRealizedPL ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
            sprintf( conv, "%.2f", pFields->dblRealizedPL ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          }
        }
    }
    int iy = 8;
    sprintf( conv,   "%d", m_Totals.nPositionSize ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    sprintf( conv, "%.2f", m_Totals.dblPositionSize ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    sprintf( conv, "%.2f", m_Totals.dblAverageCost ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    sprintf( conv, "%.2f", m_Totals.dblUnRealizedPL ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    sprintf( conv, "%.2f", m_Totals.dblRealizedPL ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    sprintf( conv, "%.2f", m_Totals.dblRunningPL ); m_lcBasketSymbols.SetItemText( 0, ++iy, conv );
    m_bSourceChanged = false;
  }
}

void CBasketTradeViewDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LISTBASKETSYMBOLS, m_lcBasketSymbols);
  DDX_Control(pDX, LBL_DTEXEC, m_lblDateTime);
}


BEGIN_MESSAGE_MAP(CBasketTradeViewDialog, CDialog)
//  ON_WM_MOVING( )
//  ON_WM_MOVE( )
//  ON_WM_SIZING( )
  ON_WM_SIZE( )
END_MESSAGE_MAP()


// CBasketTradeViewDialog message handlers

afx_msg void CBasketTradeViewDialog::OnSize( UINT i, int x, int y ) {
  if ( bDialogReady ) {
    CRect rect1, rect2, rect3;
    //GetClientRect( &rect1 );
    //rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
    //kv1.MoveWindow( &rect2 );
    //GetWindowRect( &rect1 );
    
    //CWnd::SetWindowPos( &CWnd::wndTop, rect1.left, rect1.top, x, y, 0 );
    CWnd::GetClientRect( &rect1 );
    rect2.SetRect( rect1.left + 10, rect1.top + 10, rect1.right - 10, rect1.bottom - 20 );
    m_lcBasketSymbols.MoveWindow( &rect2 );

    m_lblDateTime.GetWindowRect( &rect3 );
    rect2.SetRect( rect1.left + 10, rect1.bottom - 20, rect1.left + 10 + rect3.right - rect3.left, rect1.bottom - 5 );
    m_lblDateTime.MoveWindow( &rect2 );
  }
}

