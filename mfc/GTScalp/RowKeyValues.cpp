#include "StdAfx.h"
#include "RowKeyValues.h"

CRowKeyValues::CRowKeyValues( int ix, CListCtrl *pListCtrl, const string &name ) {

  m_ix = ix;
  m_pListCtrl = pListCtrl;

  int iy = 0;
  m_psName = new CDMString( pListCtrl, ix, iy++);
  m_psName->SetValue( name.c_str() );
  m_vMarkers.push_back( m_psName ); // Name

  m_vMarkers.push_back( m_pdNetChange = new CDMDouble( pListCtrl, ix, iy++ ) ); // NetChange
  m_vMarkers.push_back( m_pdPrevClose = new CDMDouble( pListCtrl, ix, iy++ ) ); // PrevClose
  m_vMarkers.push_back( m_piBidSize = new CDMInt( pListCtrl, ix, iy++ ) );    // BidSize
  m_vMarkers.push_back( m_pdBid = new CDMDouble( pListCtrl, ix, iy++ ) ); // Bid
  m_vMarkers.push_back( m_piTradeSize = new CDMInt( pListCtrl, ix, iy++ ) );    // TradeSize
  m_vMarkers.push_back( m_pdTrade = new CDMDouble( pListCtrl, ix, iy++ ) );  // Trade
  m_vMarkers.push_back( m_piAskSize = new CDMInt( pListCtrl, ix, iy++ ) );    // AskSize
  m_vMarkers.push_back( m_pdAsk = new CDMDouble( pListCtrl, ix, iy++ ) ); // Ask
  m_vMarkers.push_back( m_pdLow = new CDMDouble( pListCtrl, ix, iy++ ) ); // Low
  m_vMarkers.push_back( m_pdHigh = new CDMDouble( pListCtrl, ix, iy++ ) ); // High
  m_vMarkers.push_back( m_piVolume = new CDMInt( pListCtrl, ix, iy++ ) );    // Volume
}

CRowKeyValues::~CRowKeyValues(void) {
  while ( !m_vMarkers.empty() ) {
    LPDELTAMARKERBASE &marker = m_vMarkers.back();
    delete marker;
    m_vMarkers.pop_back();
  }
  //m_vMarkers.clear();
  //delete m_psName;
  //delete m_pdPrevClose;
  //delete m_pdNetChange;
  //delete m_piBidSize;
  //delete m_pdBid;
  //delete m_piTradeSize;
  //delete m_pdTrade;
  //delete m_piAskSize;
  //delete m_pdAsk;
  //delete m_pdHigh;
  //delete m_pdLow;
  //delete m_piVolume;
}

void CRowKeyValues::HandleSymbolFundamental( IQFeedSymbol *pSym ) {

 // TODO look at setting precision at some point in time

  m_sFullName = pSym->m_sCompanyName;
  /*
  m_InfoTip.cbSize = sizeof( m_InfoTip );
  m_InfoTip.dwFlags = 0;
  m_InfoTip.iItem = m_ix;
  m_InfoTip.iSubItem = 0;
  LPWSTR wCompanyName;
  int nCompanyName = pSym->sCompanyName.GetLength() + 1;
  wCompanyName = new WCHAR[ nCompanyName ];
  int d = MultiByteToWideChar( CP_ACP, 0, (LPCTSTR) pSym->sCompanyName, -1, wCompanyName, nCompanyName );
  m_InfoTip.pszText = wCompanyName;
  int f = m_pListCtrl->SetInfoTip( &m_InfoTip );
  DWORD e = GetLastError();
  delete []wCompanyName;
  */
}

void CRowKeyValues::HandleSymbolSummary( IQFeedSymbol *pSym ) {
  //CString s;
  //s.Format( "%s summary", pSym->m_sSymbol );
  //theApp.pConsoleMessages->WriteLine( s );

  m_piTradeSize->SetValue( pSym->m_nTradeSize );
  m_pdTrade->SetValue( pSym->m_dblTrade );
  m_pdHigh->SetValue( pSym->m_dblHigh );
  m_pdLow->SetValue( pSym->m_dblLow );
  m_pdPrevClose->SetValue( pSym->m_dblClose );
  m_pdNetChange->SetValue( pSym->m_dblChange );
  m_piVolume->SetValue( pSym->m_nTotalVolume );
}

void CRowKeyValues::HandleSymbolUpdate( IQFeedSymbol *pSym ) {
  //CString s;
  //s.Format( "%s update", pSym->m_sSymbol );
  //theApp.pConsoleMessages->WriteLine( s );

  m_pdTrade->SetValue( pSym->m_dblTrade );
  //m_piTradeSize->SetValue( pSym->dblTrade );
  m_piBidSize->SetValue( pSym->m_nBidSize );
  m_pdBid->SetValue( pSym->m_dblBid );
  m_pdAsk->SetValue( pSym->m_dblAsk );
  m_piAskSize->SetValue( pSym->m_nAskSize );
  m_pdHigh->SetValue( pSym->m_dblHigh );
  m_pdLow->SetValue( pSym->m_dblLow );
  m_pdNetChange->SetValue( pSym->m_dblChange );
  m_piVolume->SetValue( pSym->m_nTotalVolume );

}
