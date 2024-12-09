// TradeFrameView.cpp : implementation file
//

#include "stdafx.h"
//#include "TradeFrame.h"
#include "TradeFrameView.h"
//#include "boost/foreach.hpp"

#define foreach BOOST_FOREACH


// CTradeFrameView

IMPLEMENT_DYNAMIC(CTradeFrameView, CWnd)

char *CTradeFrameView::m_rColumnHeaderText[] = {  // keep in sync with enumColumnIndex
  "Acct1 P/L",
  "Acct2 P/L",
  "Acct P/L",
  "Bid Pend 1", 
  "Bid Pend 2",
  "Bid Pend",
  "Bid",
  "Price",
  "Ask",
  "Ask Pend 1",
  "Ask Pend 2",
  "Ask Pend",
  "Ticks",
  "Volume",
  "Histogram",
  "Static Ind",
  "Dynamic Ind"
};

CTradeFrameView::CTradeFrameView( unsigned int nRows, bool bUseAcct1, bool bUseAcct2 ) :
  m_nColumns( 0 ), m_nRows( nRows ), m_bUseAcct1( bUseAcct1 ), m_bUseAcct2( bUseAcct2 ),
    m_nInsideBidRow( -1 ), m_nInsideAskRow( -1 ), m_nTradeRow( -1 ),
    m_bArmedForTrading( false )
{
  for ( short ix = 0; ix < _ColumnCount; ++ix ) {
    m_rnColumnTranslate[ ix ] = -1;  // default to no columns are displayed
  }

  // at some point in time, will probably change this again in order to support
  //  user choice in columns and their locations
  if ( bUseAcct1 || bUseAcct2 ) {
    if ( bUseAcct1 && bUseAcct2 ) {
      m_rnColumnTranslate[ Acct1PL ] = m_nColumns++;
      m_rnColumnTranslate[ Acct2PL ] = m_nColumns++;
      m_rnColumnTranslate[ AcctPL  ] = m_nColumns++;
      m_rnColumnTranslate[ BidPend1  ] = m_nColumns++;
      m_rnColumnTranslate[ BidPend2  ] = m_nColumns++;
    }
    else {
      m_rnColumnTranslate[ AcctPL ] = m_nColumns++;
      m_rnColumnTranslate[ BidPend  ] = m_nColumns++;
    }
  }
  m_rnColumnTranslate[ Bid  ] = m_nColumns++;
  m_rnColumnTranslate[ Price  ] = m_nColumns++;
  m_rnColumnTranslate[ Ask  ] = m_nColumns++;
  if ( bUseAcct1 || bUseAcct2 ) {
    if ( bUseAcct1 && bUseAcct2 ) {
      m_rnColumnTranslate[ AskPend1  ] = m_nColumns++;
      m_rnColumnTranslate[ AskPend2  ] = m_nColumns++;
    }
    else {
      m_rnColumnTranslate[ AskPend  ] = m_nColumns++;
    }
  }
  m_rnColumnTranslate[ Ticks  ] = m_nColumns++;
  m_rnColumnTranslate[ Volume  ] = m_nColumns++;
  m_rnColumnTranslate[ Histogram  ] = m_nColumns++;
  m_rnColumnTranslate[ StaticIndicators ] = m_nColumns++;
  m_rnColumnTranslate[ DynamicIndicators  ] = m_nColumns++;

  if ( bUseAcct1 || bUseAcct2 ) {
    if ( bUseAcct1 && bUseAcct2 ) {
      m_eViewStyle = TwoAccounts;
    }
    else {
      m_eViewStyle = OneAccount;
    }
  }
  else {
    m_eViewStyle = NoAccounts;
  }
}

CTradeFrameView::~CTradeFrameView() {
}

BEGIN_MESSAGE_MAP(CTradeFrameView, CWnd)
    ON_WM_CREATE ()
    ON_WM_SIZE ()
    ON_WM_PAINT ()
END_MESSAGE_MAP()

int CTradeFrameView::OnCreate (LPCREATESTRUCT lpCreateStruct) {
  if (CWnd::OnCreate (lpCreateStruct) == -1)
    return -1;

  CClientDC dc (this);
  m_nCellWidth = dc.GetDeviceCaps (LOGPIXELSX);
  m_nCellHeight = dc.GetDeviceCaps (LOGPIXELSY) / 4;
  //m_nRibbonWidth = m_nCellWidth / 2;
  //m_nViewWidth = (26 * m_nCellWidth) + m_nRibbonWidth;
  m_nViewWidth = 26 * m_nCellWidth;
  m_nViewHeight = m_nCellHeight * 100;
  return 0;

}

void CTradeFrameView::OnSize (UINT nType, int cx, int cy) {
}

void CTradeFrameView::OnPaint () {
}


