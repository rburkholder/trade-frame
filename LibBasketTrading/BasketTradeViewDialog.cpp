// BasketTradeViewDialog.cpp : implementation file
//

// http://msdn.microsoft.com/en-us/library/6t3612sk(VS.80).aspx

#include "stdafx.h"
#include "BasketTradeViewDialog.h"
//#include "BasketTradeSymbolInfo.h"

// CBasketTradeViewDialog dialog

IMPLEMENT_DYNAMIC(CBasketTradeViewDialog, CDialog)

CBasketTradeViewDialog::CBasketTradeViewDialog(CBasketTradeModel *pModel, CWnd* pParent /*=NULL*/)
	: CDialog(CBasketTradeViewDialog::IDD, pParent),
  m_pModel( pModel ), m_bSourceChanged( false ), bDialogReady( false )
{
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
  m_lcBasketSymbols.InsertColumn( ix++, "Size", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Size", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "AvgCst", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "UnrelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "RelPL", LVCFMT_RIGHT, 50 );
  m_lcBasketSymbols.InsertColumn( ix++, "Hit", LVCFMT_CENTER, 50 );

  m_refresh.OnRefresh.Add( MakeDelegate( this, &CBasketTradeViewDialog::HandlePeriodicRefresh ) );
  m_pModel->OnBasketTradeSymbolInfoAddedToBasket.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded ) );

  bDialogReady = true;

  return TRUE;
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoAdded( CBasketTradeSymbolInfo *pInfo ) {
  int ix = m_mapDialogEntry.size();
  structDialogEntry entry( ix, pInfo );
  m_mapDialogEntry.insert( mapDialogEntry_t( pInfo->GetDialogFields().sSymbolName, entry ) );
  m_lcBasketSymbols.InsertItem( ix, pInfo->GetDialogFields().sSymbolName.c_str() );
  pInfo->OnBasketTradeSymbolInfoChanged.Add( 
    MakeDelegate( this, &CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged ) );
}

void CBasketTradeViewDialog::HandleBasketTradeSymbolInfoChanged( CBasketTradeSymbolInfo *pInfo ) {
  std::map<std::string, structDialogEntry>::iterator iter
    = m_mapDialogEntry.find( pInfo->GetSymbolName() );
  if ( m_mapDialogEntry.end() != iter ) {
    iter->second.bChanged = true;
    m_bSourceChanged = true;
  }
}

void CBasketTradeViewDialog::HandlePeriodicRefresh( CGeneratePeriodicRefresh *pRefresh ) {
  if ( m_bSourceChanged ) {
    for ( std::map<std::string, structDialogEntry>::iterator iter = m_mapDialogEntry.begin();
      iter != m_mapDialogEntry.end(); ++iter ) {
        if ( iter->second.bChanged ) {
          iter->second.bChanged = false;
          const CBasketTradeSymbolInfo::structFieldsForDialog &flds = iter->second.pInfo->GetDialogFields();
          int ix = iter->second.ix;
          char conv[ 30 ];
          int iy = 0;
          sprintf( conv, "%.2f", flds.dblCurrentPrice ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblHigh ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblOpenRangeHigh ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblOpen ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblOpenRangeLow ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblLow ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblFilledPrice ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblStop ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv,   "%d", flds.nPositionSize ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblPositionSize ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblAverageCost ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblUnRealizedPL ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
          sprintf( conv, "%.2f", flds.dblRealizedPL ); m_lcBasketSymbols.SetItemText( ix, ++iy, conv );
        }
    }
    m_bSourceChanged = false;
  }
}

void CBasketTradeViewDialog::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LISTBASKETSYMBOLS, m_lcBasketSymbols);
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
    CRect rect1, rect2;
    //GetClientRect( &rect1 );
    //rect2.SetRect( 5, 5, rect1.right - 5, rect1.bottom - 5 );
    //kv1.MoveWindow( &rect2 );
    GetWindowRect( &rect1 );
    
    //CWnd::SetWindowPos( &CWnd::wndTop, rect1.left, rect1.top, x, y, 0 );
    CWnd::GetClientRect( &rect1 );
    rect2.SetRect( rect1.left + 10, rect1.top + 10, rect1.right - 10, rect1.bottom - 10 );
    m_lcBasketSymbols.MoveWindow( &rect2 );
  }
}

