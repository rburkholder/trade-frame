// TradeFrame.cpp : implementation file
//

#include "stdafx.h"
#include "GTScalp.h"
#include "TradeFrame.h"
//#include "Colour.h"
#include "VisibleElement.h"
#include "VisibleRow.h"
#include "DataRow.h"

#include <iostream>
using namespace std;

// CTradeFrame dialog

IMPLEMENT_DYNAMIC(CTradeFrame, CFrameWnd)

CTradeFrame::CTradeFrame( CTradingLogic *pTradingLogic ) {

  //bMouseIsDown = false;
  bSizing = false;
  bMoving = false;
  rectLocation = NULL;
  m_bAllowRedraw = true;

  m_RowCount = 0;

  //pLatestQuote = NULL;
  //pLatestTrade = NULL;
  
  cntVisibleRows = 0;
  ixFirstVisibleRow = 0; // first visible integerized price
  ixLastVisibleRow = 0;  // last visible integerized price

  cntSharesPerOrder = 100;

  cntFrameRows = 0;
  cntCenterRows = 0;
  ixHiRecenterFrame = 0;
  ixLoRecenterFrame = 0;

  m_DataRows.SetMultiplier( PriceMultiplier );
  m_DataRows.SetOnRowInsert( MakeDelegate( this, &CTradeFrame::HandleRowInsert ) );

  //crAccount1 = LightSeaGreen;
  //crAccount2 = LightGreen;
  //crPrice = LightSteelBlue;
  //crColumnHeader = LightGoldenrodYellow;

  ixLastPricePrint = 0;
  ixLastAskPrint = 0;
  ixLastBidPrint = 0;

  LastPrint = 0;

  for ( int ix = 0; ix < nLevels; ix++ ) {
    oldBidPrices[ ix ] = 0;
    oldBidShares[ ix ] = 0;
    oldAskPrices[ ix ] = 0;
    oldAskShares[ ix ] = 0;
  }

  m_pTradingLogic = pTradingLogic;

  bShift = false;
  bControl = false;
  bAlt = false;

  m_refresh.Add( MakeDelegate( this, &CTradeFrame::OnPeriodicRefresh ) );

  //_CrtSetBreakAlloc(999);
}

CTradeFrame::~CTradeFrame() {
  //CFrameWnd::~CFrameWnd();
  //TerminateThread( hScreenRefreshThread, 0 );
  m_refresh.Remove( MakeDelegate( this, &CTradeFrame::OnPeriodicRefresh ) );
}

BOOL CTradeFrame::Create(CWnd* pParentWnd) {

  //BOOL b = CFrameWnd::Create(nIDTemplate, pParentWnd);
  BOOL b = CFrameWnd::Create(NULL, "Trade Frame", 
    WS_VISIBLE|FWS_ADDTOTITLE|WS_OVERLAPPEDWINDOW|WS_BORDER|WS_SYSMENU, 
    CRect( CPoint( 100, 100 ), CSize( 400, 1000 ) ), pParentWnd );
  //BOOL b = CFrameWnd::Create(_T("STATIC"), "Trade Frame", WS_OVERLAPPEDWINDOW|WS_VISIBLE,
  //  CRect( 100, 100, 200, 200 ), pParentWnd, 2000 );
  //this->SetWindowTextA("Trade Frame");

  this->GetWindowRect(&rectSize);
  CString s;
  s.Format( "TF Window Sized to %dx%d", rectSize.Width(), rectSize.Height() );
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;

  CRect client;
  this->GetClientRect(&client);
  s.Format("TF Client Area is %dx%d", client.Width(), client.Height());
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;

  SetWindowTextA( m_sChartTitle.c_str() );
  
  CFrameWnd::EnableWindow();

  DrawRows();

  return b;
}

void CTradeFrame::DeleteAllRows() {
  CVisibleRow* pVRow;
  CDataRow* pDRow;

  for ( int ix = 0, iy = ixFirstVisibleRow; 
    ix < m_RowCount; 
    ix++, iy++ ) {
    pDRow = m_DataRows[ iy ];
    //pDRow ->SetOnStringUpdatedHandlers(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
    pDRow->UnsetMapToVisibleRow();
    pVRow = m_vpVisibleRows[ix];
    //pRow->DestroyWindow();
    delete pVRow;
  }
  m_RowCount = 0;
  cntVisibleRows = 0;
}

void CTradeFrame::DrawRows() {

  CRect rectClient;
  CFrameWnd::GetClientRect(&rectClient);
  CRect rectWindow;
  CFrameWnd::GetWindowRect( &rectWindow );
  int dX = rectWindow.Width() - rectClient.Width();
  int dY = rectWindow.Height() - rectClient.Height();

  CVisibleRow* pRow;
  ASSERT(m_RowCount == 0); // ensure we are starting with an empty vector
  //m_RowCount = ( rectClient.Height() - 2 * BorderWidth ) / ( RowHeight + 1 );
  m_RowCount = ( rectClient.Height() - 2 * BorderWidth ) / ( RowHeight );
  m_vpVisibleRows.resize( m_RowCount );
  cntVisibleRows = m_RowCount - 1;  // first row is header row
  cntFrameRows = cntVisibleRows / 10;  // when move into frame then recenter
  cntCenterRows = ( ( cntVisibleRows - cntFrameRows ) / 2 ) - 1; // eliminates up/down jitter
  int yOffset = BorderWidth;  // start offset with border

  pRow = new CVisibleRow();
  pRow->Create( RowHeight, CPoint( BorderWidth, yOffset ), this, true );
  m_vpVisibleRows[0] = pRow;
  //yOffset += RowHeight + 1;
  yOffset += RowHeight;

  for ( int ix = 1; ix < m_RowCount; ix++ ) {
    pRow = new CVisibleRow();
    m_vpVisibleRows[ix] = pRow;
    pRow->Create( RowHeight, CPoint( BorderWidth, yOffset ), this, false );
    //yOffset += RowHeight + 1;
    yOffset += RowHeight;
  }
  
  CFrameWnd::MoveWindow( rectWindow.left, rectWindow.top, 
    CVisibleRow::RowWidth() + 2 * BorderWidth + dX, yOffset + BorderWidth + dY );

}

void CTradeFrame::NewLevel2Ask( int cntLevels, long *pShares, double *pPrices ) {
  for ( int ix = 0; ix < nLevels; ix++ ) {
    if ( 0 != oldAskShares[ ix ] ) { // this needs to be optimized a bunch
      //unsigned int ixAskPrice = CDataRow::DoubleToInt( pShares[ ix ], PriceMultiplier );
      //m_DataRows[ oldAskPrices[ ix ] ]->SetAskQuan( 0 );
      m_DataRows[ oldAskPrices[ ix ] ]->m_pAskQuan->Set( 0 );
      oldAskShares[ ix ] = 0;
    }
  }
  for ( int ix = 0; ix < cntLevels; ix++ ) {
    unsigned int ixAskPrice = CDataRow::DoubleToInt( pPrices[ ix ], PriceMultiplier );
    oldAskPrices[ ix ] = ixAskPrice;
    oldAskShares[ ix ] = pShares[ ix ];
    //m_DataRows[ ixAskPrice ]->SetAskQuan( pShares[ ix ] );
    m_DataRows[ ixAskPrice ]->m_pAskQuan->Set( pShares[ ix ] );
  }
}

void CTradeFrame::NewLevel2Bid( int cntLevels, long *pShares, double *pPrices ) {
  for ( int ix = nLevels - 1; ix >= 0; ix-- ) { // optimize order for rows[] access
    if ( 0 != oldBidShares[ ix ] ) { // this needs to be optimized a bunch
      //unsigned int ixAskPrice = CDataRow::DoubleToInt( pShares[ ix ], PriceMultiplier );
      //m_DataRows[ oldBidPrices[ ix ] ]->SetBidQuan( 0 );
      m_DataRows[ oldBidPrices[ ix ] ]->m_pBidQuan->Set( 0 );
      oldBidShares[ ix ] = 0;
    }
  }
  for ( int ix = cntLevels - 1; ix >= 0; ix-- ) {
    unsigned int ixBidPrice = CDataRow::DoubleToInt( pPrices[ ix ], PriceMultiplier );
    oldBidPrices[ ix ] = ixBidPrice;
    oldBidShares[ ix ] = pShares[ ix ];
    //m_DataRows[ ixBidPrice ]->SetBidQuan( pShares[ ix ] );
    m_DataRows[ ixBidPrice ]->m_pBidQuan->Set( pShares[ ix ] );
  }
}

void CTradeFrame::NewQuoteLevel1( const Quote &quote ) {
  //CString s;
  //s.Format( "TL: Bid %d@%0.2f, Ask %d@%0.2f",
  //  BidSize, BidPrice, AskSize, AskPrice );
  //theApp.pConsoleMessages->WriteLine( s );

  unsigned int ixAskPrice = CDataRow::DoubleToInt( quote.m_dblAsk, PriceMultiplier );
  unsigned int ixBidPrice = CDataRow::DoubleToInt( quote.m_dblBid, PriceMultiplier );

  if ( ( 0 != ixLastAskPrint ) && ( ixLastAskPrint != ixAskPrice ) ) {
    //m_DataRows[ ixLastAskPrint ]->SetAskQuan( 0 );
    m_DataRows[ ixLastAskPrint ]->m_pAskQuan->Set( 0 );
    //m_DataRows[ ixLastAskPrint ]->m_pvr->HighlightAsk( false );
  }
  if ( ( 0 != ixLastBidPrint ) && ( ixLastBidPrint != ixBidPrice ) ) {
    //m_DataRows[ ixLastBidPrint ]->SetBidQuan( 0 );
    m_DataRows[ ixLastBidPrint ]->m_pBidQuan->Set( 0 );
    //m_DataRows[ ixLastBidPrint ]->m_pvr->HighlightBid( false );
  }

  unsigned int ixHiPrice = __max( ixAskPrice, ixBidPrice );
  unsigned int ixLoPrice = __min( ixAskPrice, ixBidPrice );
  unsigned int cntDif = ixHiPrice - ixLoPrice + 1;
  if ( cntDif <= cntCenterRows ) {
    // fits in between frame so recenter on edge
    //RecenterVisible( ixAskPrice );
    //RecenterVisible( ixBidPrice );
  }
  else {
    //recenter on mid point, as it doesn't fit within frame
    unsigned int ixMidPoint = ( ixHiPrice + ixLoPrice ) / 2;
    RecenterVisible( ixMidPoint );
  }

  m_DataRows[ ixAskPrice ]->m_pAskQuan->Set( quote.m_nAskSize ) ;
//  m_DataRows[ ixAskPrice ]->m_pvr->HighlightAsk( true );

  m_DataRows[ ixBidPrice ]->m_pBidQuan->Set( quote.m_nBidSize );
//  m_DataRows[ ixBidPrice ]->m_pvr->HighlightBid( true );

  m_latestQuote = quote;
  dblQuoteMidPoint = ( quote.m_dblAsk + quote.m_dblBid ) / 2;

  ixLastAskPrint = ixAskPrice;
  ixLastBidPrint = ixBidPrice;
}

void CTradeFrame::NewPrint( const Trade &trade ) {

  //CString s;
  //s.Format( "TL: Price %d@%0.2f", Shares, Price );
  //theApp.pConsoleMessages->WriteLine( s );

  unsigned int ixPrice = CDataRow::DoubleToInt( trade.m_dblTrade, PriceMultiplier );
  if ( 0 != ixLastPricePrint ) {
    m_DataRows[ ixLastPricePrint ]->m_pPrice->Set( 0 );
    //m_DataRows[ ixLastPricePrint ]->m_pvr->HighlightPrice( false );
  }
  RecenterVisible( ixPrice );
  CDataRow *pRow = m_DataRows[ ixPrice ];
  pRow->m_pPrice->Set( trade.m_nTradeSize );
  //pRow->m_pvr->HighlightPrice( true );
  int nTicks = pRow->m_pTicks->Get();
  pRow->m_pTicks->Set( nTicks + 1 );
  int nVolume = pRow->m_pVolume->Get();
  pRow->m_pVolume->Set( nVolume + trade.m_nTradeSize );

  if ( !m_latestQuote.IsNull() ) {
    if ( trade.m_dblTrade >= dblQuoteMidPoint ) {
      pRow->m_pTickBuyVolume->Set( pRow->m_pTickBuyVolume->Get() + trade.m_nTradeSize );
    }
    else {
      pRow->m_pTickSellVolume->Set( pRow->m_pTickSellVolume->Get() + trade.m_nTradeSize );
    }
  }
  m_latestTrade = trade;

  ixLastPricePrint = ixPrice;
  LastPrint = trade.m_dblTrade;
}

void CTradeFrame::AppendStaticIndicator( double price, const char *ind ) {
  unsigned int ix = CDataRow::DoubleToInt( price, PriceMultiplier );
  m_DataRows[ ix ]->m_pStaticIndicators->Set( ind );
}

void CTradeFrame::AddDynamicIndicator( double price, const char *ind ) {
  unsigned int ix = CDataRow::DoubleToInt( price, PriceMultiplier );
  m_DataRows[ ix ]->m_pDynamicIndicators->Add( ind );
}

void CTradeFrame::RemoveDynamicIndicator( double price, const char *ind ) {
  unsigned int ix = CDataRow::DoubleToInt( price, PriceMultiplier );
  m_DataRows[ ix ]->m_pDynamicIndicators->Remove( ind );
}

void CTradeFrame::HandleAcct1OnOpenPosition( double dblOpenPrice, char chOpenSide, int nOpenShares ) {
  nAcct1OpenShares = nOpenShares;
  chAcct1OpenSide = chOpenSide;
  dblAcct1OpenPrice = dblOpenPrice;
  Acct1Value = dblOpenPrice * nOpenShares * ( 'B' == chOpenSide ? 1 : -1 );
  //Acct1Value = dblOpenPrice * nOpenShares;
  UpdateProfitLossIndications();
}

void CTradeFrame::HandleAcct2OnOpenPosition( double dblOpenPrice, char chOpenSide, int nOpenShares ) {
  nAcct2OpenShares = nOpenShares;
  chAcct2OpenSide = chOpenSide;
  dblAcct2OpenPrice = dblOpenPrice;
  Acct2Value = dblOpenPrice * nOpenShares * ( 'B' == chOpenSide ? 1 : -1 );
  //Acct2Value = dblOpenPrice * nOpenShares;
  UpdateProfitLossIndications();
}

void CTradeFrame::UpdateProfitLossIndications() {
  AcctValue = Acct1Value + Acct2Value;
  double t1, t2;

  CDataRow *pRow;
  if ( 0 != LastPrint ) {
    for ( unsigned int iy = ixFirstVisibleRow; iy <= ixLastVisibleRow; iy++ ) {
      pRow = m_DataRows[ iy ];
      double t3 = pRow->m_dblPrice;
      //double t3 = m_DataRows[ iy ]->dblPrice;
      t1 = ( t3 - dblAcct1OpenPrice ) * nAcct1OpenShares * ( 'B' == chAcct1OpenSide ? 1 : -1 );
      t2 = ( t3 - dblAcct2OpenPrice ) * nAcct2OpenShares * ( 'B' == chAcct2OpenSide ? 1 : -1 );
      pRow->m_pAcct1PL->Set( t1 );
      pRow->m_pAcct2PL->Set( t2 );
      pRow->m_pAcctPL->Set( t1 + t2 );
      //m_DataRows[ iy ]->SetAcct1PL( t1 );
      //m_DataRows[ iy ]->SetAcct2PL( t2 );
      //m_DataRows[ iy ]->SetAcctPL( t1 + t2 );
    }
  }
}

void CTradeFrame::RecenterVisible( unsigned int ixPrice ) {
  // only does something if ixPrice moves outside of window
  //unsigned int ix = CDataRow::DoubleToInt( Price, PriceMultiplier );
  if ( ixPrice <= ixLoRecenterFrame || ixPrice >= ixHiRecenterFrame ) {
    // recalibrate mappings
    if ( ixFirstVisibleRow != ixLastVisibleRow ) {
      for ( unsigned int iy = ixFirstVisibleRow; iy <= ixLastVisibleRow; iy++ ) {
        // remove existing string update events
        CDataRow *pDRow = m_DataRows[ iy ];
        //pDRow -> SetOnStringUpdatedHandlers(NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
        pDRow->UnsetMapToVisibleRow();
      }
    }
    ixFirstVisibleRow = ixPrice - ( cntVisibleRows / 2 );
    ixLastVisibleRow = ixFirstVisibleRow + cntVisibleRows - 1;
    ixHiRecenterFrame = ixLastVisibleRow - cntFrameRows;
    ixLoRecenterFrame = ixFirstVisibleRow + cntFrameRows;
    for ( unsigned int ix = cntVisibleRows, iy = ixFirstVisibleRow;
      ix >= 1;
      ix--, iy++ ) {
        CVisibleRow *pVRow = m_vpVisibleRows[ ix ];
        CDataRow *pDRow = m_DataRows[ iy ];
        pDRow->SetMapToVisibleRow( pVRow );
        pVRow->m_veAcct1BidPending.SetOnLeftClick(  MakeDelegate( pDRow, &CDataRow::HandleAccount1BidLClick ) );
        pVRow->m_veAcct2BidPending.SetOnLeftClick(  MakeDelegate( pDRow, &CDataRow::HandleAccount2BidLClick ) );
        pVRow->m_veAcct1BidPending.SetOnRightClick( MakeDelegate( pDRow, &CDataRow::HandleAccount1BidRClick ) );
        pVRow->m_veAcct2BidPending.SetOnRightClick( MakeDelegate( pDRow, &CDataRow::HandleAccount2BidRClick ) );
        pVRow->m_veAcct1AskPending.SetOnLeftClick(  MakeDelegate( pDRow, &CDataRow::HandleAccount1AskLClick ) );
        pVRow->m_veAcct2AskPending.SetOnLeftClick(  MakeDelegate( pDRow, &CDataRow::HandleAccount2AskLClick ) );
        pVRow->m_veAcct1AskPending.SetOnRightClick( MakeDelegate( pDRow, &CDataRow::HandleAccount1AskRClick ) );
        pVRow->m_veAcct2AskPending.SetOnRightClick( MakeDelegate( pDRow, &CDataRow::HandleAccount2AskRClick ) );
    }
    UpdateProfitLossIndications();
  }
}

void CTradeFrame::HandleRowInsert( CDataRow *pDataRow ) {
  pDataRow->SetOnAccount1BidLClick( MakeDelegate( this, &CTradeFrame::HandleAcct1BidLClk ) );
  pDataRow->SetOnAccount2BidLClick( MakeDelegate( this, &CTradeFrame::HandleAcct2BidLClk ) );
  pDataRow->SetOnAccount1BidRClick( MakeDelegate( this, &CTradeFrame::HandleAcct1BidRClk ) );
  pDataRow->SetOnAccount2BidRClick( MakeDelegate( this, &CTradeFrame::HandleAcct2BidRClk ) );
  pDataRow->SetOnAccount1AskLClick( MakeDelegate( this, &CTradeFrame::HandleAcct1AskLClk ) );
  pDataRow->SetOnAccount2AskLClick( MakeDelegate( this, &CTradeFrame::HandleAcct2AskLClk ) );
  pDataRow->SetOnAccount1AskRClick( MakeDelegate( this, &CTradeFrame::HandleAcct1AskRClk ) );
  pDataRow->SetOnAccount2AskRClick( MakeDelegate( this, &CTradeFrame::HandleAcct2AskRClk ) );
}

void CTradeFrame::HandleAcct1BidLClk( int ixPrice ) {
  m_pTradingLogic->Acct1LmtBuy( cntSharesPerOrder, CDataRow::IntToDouble( ixPrice, PriceMultiplier ) );
}

void CTradeFrame::HandleAcct2BidLClk( int ixPrice ) {
  m_pTradingLogic->Acct2LmtBuy( cntSharesPerOrder, CDataRow::IntToDouble( ixPrice, PriceMultiplier ) );
}

void CTradeFrame::HandleAcct1BidRClk( int ixPrice ) {
  m_pTradingLogic->Acct1CancelBid();
}

void CTradeFrame::HandleAcct2BidRClk( int ixPrice ) {
  m_pTradingLogic->Acct2CancelBid();
}

void CTradeFrame::HandleAcct1AskLClk( int ixPrice ) {
  m_pTradingLogic->Acct1LmtSell( cntSharesPerOrder, CDataRow::IntToDouble( ixPrice, PriceMultiplier ) );
}

void CTradeFrame::HandleAcct2AskLClk( int ixPrice ) {
  m_pTradingLogic->Acct2LmtSell( cntSharesPerOrder, CDataRow::IntToDouble( ixPrice, PriceMultiplier ) );
}

void CTradeFrame::HandleAcct1AskRClk( int ixPrice ) {
  m_pTradingLogic->Acct1CancelAsk();
}

void CTradeFrame::HandleAcct2AskRClk( int ixPrice ) {
  m_pTradingLogic->Acct2CancelAsk();
}

void CTradeFrame::DoDataExchange(CDataExchange* pDX) {
	CFrameWnd::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTradeFrame, CFrameWnd)
//  ON_WM_MOUSEMOVE()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_SYSKEYDOWN( )
  ON_WM_SYSKEYUP( )
//  ON_WM_CHAR()
//  ON_WM_SYSCHAR()
//  ON_WM_KILLFOCUS( )
//  ON_WM_SETFOCUS()
  ON_WM_MOVING( )
//  ON_WM_MOVE( )
  ON_WM_SIZING( )
//  ON_WM_SIZE( )
  ON_WM_LBUTTONUP( )
  ON_WM_RBUTTONUP( )
//  ON_WM_NCLBUTTONDOWN( )
//  ON_WM_NCLBUTTONUP( )
//  ON_WM_NCHITTEST( )
  //ON_WM_NCCALCSIZE( )
  ON_WM_NCMOUSEMOVE( )
  ON_WM_CTLCOLOR( )
  ON_WM_ERASEBKGND( )
  ON_WM_DESTROY( )
  //ON_MESSAGE( WM_CUSTOMREDRAW, OnCustomRedraw )
END_MESSAGE_MAP()


// CTradeFrame message handlers

void CTradeFrame::OnPeriodicRefresh( CGeneratePeriodicRefresh *p ) {
  // don't draw anything if ixFirstVisibleRow or ixLastVisibleRow are 0
  if ( m_bAllowRedraw && ( ixFirstVisibleRow != ixLastVisibleRow ) ) {
    for ( unsigned int iy = ixFirstVisibleRow; iy <= ixLastVisibleRow; iy++ ) {
      CDataRow *pDRow = m_DataRows[ iy ];
      pDRow->CheckRefresh();
    }
  }
}

afx_msg void CTradeFrame::OnDestroy( ) {
  DeleteAllRows();
  CFrameWnd::OnDestroy();
//  if ( NULL != this ) {
//    delete(this);  // not a good thing, must be someway to test for existance
//  }
}

afx_msg BOOL CTradeFrame::OnEraseBkgnd(CDC* pDC) {
  return CFrameWnd::OnEraseBkgnd(pDC);
}

afx_msg HBRUSH CTradeFrame::OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor ) {
// Call original functions
HBRUSH hbr = CFrameWnd::OnCtlColor(pDC, pWnd, nCtlColor);

// Return result of original function
return hbr;

}

afx_msg void CTradeFrame::OnNcCalcSize( BOOL b, NCCALCSIZE_PARAMS FAR* params) {
  CString s;
  s.Format( "calc size: %d, %d, %d, %d, %d", b, params->lppos->x, params->lppos->y,
    params->lppos->cx, params->lppos->cy );
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
//  DeleteAllRows();
//  DrawRows();
  CFrameWnd::OnNcCalcSize(b, params);
}

afx_msg void CTradeFrame::OnMouseMove(UINT nFlags, CPoint point) {
  CString s;
  s.Format( "MouseMove: flags=%d, point=%d,%d", nFlags, point.x, point.y );
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnMouseMove(nFlags, point);
}

afx_msg void CTradeFrame::OnMoving(UINT nSide, LPRECT lpRect) {
//  CString s;
//  s.Format( "OnMoving: %d, %d, %d, %d, %d", 
//    nSide, lpRect->left, lpRect->top, lpRect->bottom, lpRect->right);
//  theApp.pConsoleMessages->WriteLine(s);
  bMoving = true;
  rectLocation = *lpRect;
  CFrameWnd::OnMoving(nSide, lpRect);
}

afx_msg void CTradeFrame::OnMove(int x, int y) {
  CString s;
  s.Format("OnMove: %d, %d", x, y);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnMove(x,y);
}

afx_msg void CTradeFrame::OnSizing(UINT nSide, LPRECT lpRect) {
//  CString s;
//  s.Format( "OnSizing: %d, %d, %d, %d, %d", 
//    nSide, lpRect->left, lpRect->top, lpRect->bottom, lpRect->right);
//  theApp.pConsoleMessages->WriteLine(s);
  bSizing = true;
  rectSize = *lpRect;
  CFrameWnd::OnSizing(nSide, lpRect);
}

afx_msg void CTradeFrame::OnSize(UINT nType, int cx, int cy) {
  CString s;
  s.Format("OnSize: %d, %d", cx, cy);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnSize(nType,cx,cy);
}

afx_msg void CTradeFrame::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  CString s;
  s.Format( "CTradeFrame::OnChar: char %d, repcnt %d, flags %4x", 
    nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnChar( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnSysChar( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  CString s;
  s.Format( "CTradeFrame::OnSysChar: char %d, repcnt %d, flags %4x", 
    nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnChar( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags) {
  //CString s;
  //s.Format( "CTradeFrame::OnSysKeyDown: char %d, repcnt %d, flags %4x", 
  //  nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  switch ( nChar ) {
    case 16: 
      bShift = true;
      break;
    case 17:
      bControl = true;
      break;
    case 18:
      bAlt = true;
      break;
  }
  CFrameWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnSysKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  //CString s;
  //s.Format( "CTradeFrame::OnSysKeyUp: char %d, repcnt %d, flags %4x", 
  //  nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  switch ( nChar ) {
    case 16: 
      bShift = false;
      break;
    case 17:
      bControl = false;
      break;
    case 18:
      bAlt = false;
      break;
  }
  CFrameWnd::OnKeyUp( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags) {
  //CString s;
  //s.Format( "CTradeFrame::OnKeyDown: char %d, repcnt %d, flags %4x", 
  //  nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  switch ( nChar ) {
    case 16: 
      bShift = true;
      break;
    case 17:
      bControl = true;
      break;
    case 18:
      bAlt = true;
      break;
  }
  CFrameWnd::OnKeyDown( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags ) {
  //CString s;
  //s.Format( "CTradeFrame::OnKeyUp: char %d, repcnt %d, flags %4x", 
  //  nChar, nRepCnt, nFlags);
  //theApp.pConsoleMessages->WriteLine(s);
  switch ( nChar ) {
    case 16: 
      bShift = false;
      break;
    case 17:
      bControl = false;
      break;
    case 18:
      bAlt = false;
      break;
  }
  CFrameWnd::OnKeyUp( nChar, nRepCnt, nFlags );
}

afx_msg void CTradeFrame::OnLButtonUp(UINT nFlags, CPoint point) {
  CString s;
  s.Format( "CTradeFrame::OnLButtonUp: %d, %d, %d: %d, %d, %d", 
    nFlags, point.x, point.y, bShift, bControl, bAlt);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnLButtonUp(nFlags, point);
}

afx_msg void CTradeFrame::OnRButtonUp(UINT nFlags, CPoint point) {
  CString s;
  s.Format( "CTradeFrame::OnRButtonUp: %d, %d, %d: %d, %d, %d", 
    nFlags, point.x, point.y, bShift, bControl, bAlt);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnRButtonUp(nFlags, point);
}

afx_msg void CTradeFrame::OnNcMouseMove( UINT nHitTest, CPoint point ) {
  CString s;
  //s.Format( "MouseMove: hit=%d, point=%d,%d", nHitTest, point.x, point.y );
  if ( bMoving ) {
    bMoving = false;
    s.Format( "Windows Moved to %d, %d", rectLocation.left, rectLocation.top);
    //theApp.pConsoleMessages->WriteLine(s);
    cout << s << endl;
  }
  if ( bSizing ) {
    bSizing = false;
    s.Format( "Window Sized to %dx%d", rectSize.Width(), rectSize.Height() );
    //theApp.pConsoleMessages->WriteLine(s);
    cout << s << endl;
    CRect client;
    this->GetClientRect(&client);
    s.Format("Client Area is %dx%d", client.Width(), client.Height());
    //theApp.pConsoleMessages->WriteLine(s);
    cout << s << endl;

    DeleteAllRows();
    DrawRows();
  }

  CFrameWnd::OnNcMouseMove(nHitTest, point);
}

//afx_msg UINT CTradeFrame::OnNcHitTest( CPoint point ) {
//  return CFrameWnd::OnNcHitTest( point );
//}

afx_msg void CTradeFrame::OnNcLButtonDown( UINT nHitTest, CPoint point ) {

  CString s;
  s.Format( "OnNcLButtonDown: %d, %d, %d", 
    nHitTest, point.x, point.y);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnNcLButtonDown(nHitTest, point);
}

afx_msg void CTradeFrame::OnNcLButtonUp( UINT nHitTest, CPoint point ) {
  CString s;
  s.Format( "OnNcLButtonUp: %d, %d, %d", 
    nHitTest, point.x, point.y);
  //theApp.pConsoleMessages->WriteLine(s);
  cout << s << endl;
  CFrameWnd::OnNcLButtonUp(nHitTest, point);
}

BOOL CTradeFrame::PreTranslateMessage(MSG* pMsg) {
//  CString s;
//  s.Format( "msg: %4x, %8x, %8x", pMsg->message, pMsg->lParam, pMsg->wParam );
//  theApp.pConsoleMessages->WriteLine(s);
  return CFrameWnd::PreTranslateMessage(pMsg);
}

