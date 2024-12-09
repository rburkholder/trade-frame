#include "StdAfx.h"
#include "GTScalp.h"
#include "TradingLogic.h"
#include "GTOrderX.h"
#include "Colour.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Colour::enumColour MDColors[] = { 
  Colour::LightYellow, Colour::LightYellow, Colour::PaleGoldenrod, Colour::Wheat, Colour::Goldenrod, Colour::DarkGoldenrod,
  Colour::DarkKhaki, Colour::DarkOrange, Colour::Orange, Colour::OrangeRed, Colour::Red, Colour::Crimson, Colour::Black };  // Black is not used, simply an end of list indicator

CTradingLogic::CTradingLogic( CString sSymbol, CGTSessionX *pGTSession1, CGTSessionX *pGTSession2 ) {
  
  string s;
  m_bFirstTradeFound = false;

  //m_bAllowTrades = false;  // allows trades to be executed by broker
  m_state = EState::Init;

  m_sSymbol = sSymbol;
  stkSession1 = (CGTStockX*) pGTSession1->CreateStock( m_sSymbol );
  stkSession2 = (CGTStockX*) pGTSession2->CreateStock( m_sSymbol );
  stkSession2 ->CancelLevel2();
  stkSession2 ->CancelQuote();
  stkSession2 ->CancelChart();

  pTradeFrame = new CTradeFrame( this );
  s = "TradeFrame: ";
  s.append( sSymbol );
  pTradeFrame->SetTitleBarText( s ); 
  pTradeFrame->Create(theApp.m_pMainWnd);
  pTradeFrame->SetAllowRedraw( false );

  //pChartDaily = new CVuChart(theApp.m_pMainWnd);
  //pChartDaily->ShowWindow(1);

  s = "Intraday ";
  s.append( m_sSymbol );
  pChartIntraDay = new CVuChart(s,theApp.m_pMainWnd);
  //pChartIntraDay->SetTitleBarText( s );
  pChartIntraDay->ShowWindow(1);
  pChartIntraDay->m_chart.SetTitle( s.c_str() );

  pVuPendingOrders = new CVuPendingOrders( theApp.m_pMainWnd );
  pVuPendingOrders->po1.SetOnCancelHandler( MakeDelegate( this, &CTradingLogic::OnCancelAcct1Order ) );
  pVuPendingOrders->po2.SetOnCancelHandler( MakeDelegate( this, &CTradingLogic::OnCancelAcct2Order ) );

  s = "Market Depth: ";
  s.append( m_sSymbol );
  pVuMarketDepth = new CVuMarketDepth( s, theApp.m_pMainWnd);
  //pVuMarketDepth -> SetTitleBarText( s );
  pVuMarketDepth ->ShowWindow( SW_SHOWNORMAL );

  stkSession1->SetOnQuoteLevel1Handler( MakeDelegate( this, &CTradingLogic::OnQuoteLevel1 ) );
  stkSession1->SetOnPrintHandler( MakeDelegate( this, &CTradingLogic::OnPrint ) );
  stkSession1->SetOnLevel2Handler( MakeDelegate( this, &CTradingLogic::OnLevel2 ) );

  char chOpenSide;
  int nOpenPosition;
  double dblOpenCost;

  stkSession1->GetOpenPosition(  chOpenSide, nOpenPosition, dblOpenCost );
  pTradeFrame->HandleAcct1OnOpenPosition( dblOpenCost, chOpenSide, nOpenPosition );
  stkSession2->GetOpenPosition(  chOpenSide, nOpenPosition, dblOpenCost );
  pTradeFrame->HandleAcct2OnOpenPosition( dblOpenCost, chOpenSide, nOpenPosition );

  stkSession1->SetOnOpenPositionHandler( MakeDelegate( pTradeFrame, &CTradeFrame::HandleAcct1OnOpenPosition ) );
  stkSession2->SetOnOpenPositionHandler( MakeDelegate( pTradeFrame, &CTradeFrame::HandleAcct2OnOpenPosition ) );

  //bool bLive = true;
  pQuotes = new Quotes( 100000 );
  pTrades = new Trades( 100000 );

  pChartIntraDay->m_chart.SetBarFactoryWidthSeconds( 60 );
  pChartIntraDay->m_chart.SetWindowWidthSeconds( 90 * 60 ); // 90 minute window
  pChartIntraDay->m_chart.setMajorTickInc( 12 * 60 );
  pChartIntraDay->m_chart.setMinorTickInc( 60 );

  //ASSERT( NULL != theApp.m_pIQFeed );
  m_IQFeedProvider.GetIQFeedProvider()->TimeMessage.Add( MakeDelegate( this, &CTradingLogic::OnIQFeedTimeMessage ) );

  m_state = EState::History;

  pBars = new Bars( 250 );
  phd = new IQFeedHistoryHD( m_IQFeedProvider.GetIQFeedProvider(), pBars );
  phd->SetOnRequestComplete( MakeDelegate( this, &CTradingLogic::OnDailyBarHistoryDone ) );
  pht = new IQFeedHistoryHT( m_IQFeedProvider.GetIQFeedProvider(), pQuotes, pTrades );
  pht->SetOnRequestComplete( MakeDelegate( this, &CTradingLogic::OnTickHistoryDone ) );
  if ( theApp.m_bLive ) {
    phd->LiveRequest( (LPCTSTR) sSymbol, 250 );
    pht->LiveRequest( (LPCTSTR) sSymbol, theApp.m_nDaysAgo );
  }
  else {
    phd->FileRequest( (LPCTSTR) sSymbol, 250 );
    pht->FileRequest( (LPCTSTR) sSymbol, theApp.m_nDaysAgo );
  }

  stateMDUpdate = MDUpdateBoth;  // these two lines depend upon Session being logged in AND connected
  m_refresh.Add( MakeDelegate( this, &CTradingLogic::OnPeriodicRefresh ) );
}

CTradingLogic::~CTradingLogic(void){

  //theApp.m_pIQFeed->TimeMessage.Remove( MakeDelegate( this, &CTradingLogic::OnIQFeedTimeMessage ) );

  m_refresh.Remove( MakeDelegate( this, &CTradingLogic::OnPeriodicRefresh ) );

  delete pVuPendingOrders;
  pVuPendingOrders = NULL;

  delete pVuMarketDepth;
  pVuMarketDepth = NULL;

  //delete pTradeFrame;
  //pTradeFrame = NULL;

  delete pht;
  pht = NULL;
  delete phd;
  phd = NULL;
  delete pBars;
  pBars = NULL;
  delete pQuotes;
  pQuotes=NULL;
  delete pTrades;
  pTrades = NULL;

  //delete pChartDaily;
  delete pChartIntraDay;

//  pTradeFrame->DestroyWindow();
  //delete( pTradeFrame );
  //delete( pProvider );
}

void CTradingLogic::OnIQFeedTimeMessage( IQFTimeMessage *pMsg ) {
}

void CTradingLogic::OnPeriodicRefresh( CGeneratePeriodicRefresh *pMsg ) {
  UpdateMarketDepth();
}

void CTradingLogic::OnDailyBarHistoryDone( IQFeedHistory *pHistory ) {
  if ( pBars->Count() > 0 ) {

    PivotGroup group( pBars );
    for ( PivotGroup::const_iterator iter = group.begin(); iter != group.end(); ++iter ) {
      pTradeFrame->AppendStaticIndicator( iter->first, iter->second.sName.c_str() );
    }
  }
}

void CTradingLogic::OnTickHistoryDone( IQFeedHistory *pHistory ) {

  // this is being processed on a non-gui thread, 
  // need to turn this into tha thread cross over event
  size_t nQuotes = pQuotes->Count();
  size_t nTrades = pTrades->Count();
  ASSERT( nQuotes == nTrades );
  if ( ( nQuotes > 0 ) && ( nTrades > 0 ) ) {
    pQuotes->Flip();
    pTrades->Flip();

    //int n = pTrades->Count();
    size_t i;

    pChartIntraDay->m_chart.SetUpdateChart( false );

    Trade *pTrade;
    Quote *pQuote;
    for ( i = 0; i < nTrades; i++ ) {
      pQuote = pQuotes -> At( i );
      pTradeFrame->NewQuoteLevel1( *pQuote );
      pTrade = pTrades -> At( i );
      OnPrintCommon( *pTrade );
    }
  }

  pChartIntraDay->m_chart.SetUpdateChart( true );
  m_state = EState::RealTime;
  pTradeFrame->SetAllowRedraw( true );
}

void CTradingLogic::OnPrintCommon( const Trade &trade ) {
  if ( !m_bFirstTradeFound ) {
    if ( trade.m_dt.time_of_day() >= time_duration( 9, 30, 0 ) && trade.m_dt.time_of_day() < time_duration( 16, 0 ,0 ) ) {
      m_bFirstTradeFound = true;
      m_dblOpen = trade.m_dblTrade;
      pTradeFrame->AppendStaticIndicator( m_dblOpen, "Open" );
    }
  }
  pTradeFrame->NewPrint( trade );
  pChartIntraDay->m_chart.Add( trade );
}

void CTradingLogic::OnPrint( const Trade &trade ) {
  //pChartIntraDay->m_chart.ClearChart();
  switch ( m_state ) {
    case EState::RealTime:
      pTrades->AppendDatum( trade );  // only for realtime, history filled up trades already
      OnPrintCommon( trade );
      break;
    case EState::History:
      // history is being updated
      // genesis should be on same thread as HistoryDone, so history can get its ticks in before genesis
      // break;  // no break here, falls through
      // if this doesnt' work, then OnPrint needs to be separated into one for history and one for genesis
      // this will probably break as it is being fed by two threads.
      break;
    case EState::Init:
      // neither history or genesis updates
      break;
  }
}

void CTradingLogic::OnQuoteLevel1( const Quote &quote ) {
  switch ( m_state ) {
    case EState::RealTime:
      pQuotes->AppendDatum( quote );
      pTradeFrame->NewQuoteLevel1( quote );
    case EState::History:
      break;
    case EState::Init:
      // neither history or genesis updates
      break;
  }
}

//void CTradingLogic::OnLevel2( char Side, long Shares, double Price, LPSTR MMID ) {
// called as refresh from GT routines, but don't update display at this time
void CTradingLogic::OnLevel2( const MarketDepth &md ) {
  switch ( m_state ) {
    case EState::RealTime:
      switch ( md.m_eSide ) {
        case MarketDepth::Ask: stateMDUpdate = ( MDUpdateBid == stateMDUpdate ) ? MDUpdateBoth : MDUpdateAsk;
          break;
        case MarketDepth::Bid: stateMDUpdate = ( MDUpdateAsk == stateMDUpdate ) ? MDUpdateBoth : MDUpdateBid;
          break;
      }
      break;
    case EState::History:
      break;
    case EState::Init:
      break;
  }
}

void CTradingLogic::UpdateMarketDepth() {
  // states need to be defined elsewhere, as this routine is reached from the periodic time out pulse
  int ix;
  int ixColor;
  long dwComparePrice;
  GTLevel2 *pL2;
  //char szMMID[10];
  GMMID mmid;
  char szTime[ 10 ];

  if ( MDUpdateBid == stateMDUpdate || MDUpdateBoth == stateMDUpdate ) {
    int i = stkSession1->m_level2.GetBidLevel( rShares, rPrices, nLevels );
    pTradeFrame->NewLevel2Bid( i, rShares, rPrices );

    dwComparePrice = 0;
    ixColor = 0;
    int j = stkSession1->m_level2.GetBidCount();
    for ( ix = 0; ix < j; ix++ ) {
      pL2 = stkSession1->m_level2.GetBidItem( ix );
      //copymmid( szMMID, pL2->mmid );
      mmid = pL2->mmid;
      if ( 0 != dwComparePrice && dwComparePrice != pL2->dwComparePrice ) {
        ++ixColor;
        if ( Colour::Black == MDColors[ ixColor ] ) ixColor = 0;
      }
      sprintf( szTime, "%02d:%02d:%02d", pL2->gtime.chHour, pL2->gtime.chMin, pL2->gtime.chSec );
      pVuMarketDepth->m_lcMarketDepthBid.AssignRow( 
        ix, MDColors[ ixColor ], mmid.szMMID, pL2->dwShares, pL2->dblPrice, pL2->nOwnShares, szTime );
    }
    pVuMarketDepth->m_lcMarketDepthBid.BlankRemainingRows();
  }
  if ( MDUpdateAsk == stateMDUpdate || MDUpdateBoth == stateMDUpdate ) {
    int i = stkSession1->m_level2.GetAskLevel( rShares, rPrices, nLevels );
    pTradeFrame->NewLevel2Ask( i, rShares, rPrices );

    dwComparePrice = 0;
    ixColor = 0;
    int j = stkSession1->m_level2.GetAskCount();
    for ( ix = 0; ix < j; ix++ ) {
      pL2 = stkSession1->m_level2.GetAskItem( ix );
      //copymmid( szMMID, pL2->mmid );
      mmid = pL2->mmid;
      if ( 0 != dwComparePrice && dwComparePrice != pL2->dwComparePrice ) {
        ++ixColor;
        if ( Colour::Black == MDColors[ ixColor ] ) ixColor = 0;
      }
      sprintf( szTime, "%02d:%02d:%02d", pL2->gtime.chHour, pL2->gtime.chMin, pL2->gtime.chSec );
      pVuMarketDepth->m_lcMarketDepthAsk.AssignRow( 
        ix, MDColors[ ixColor ], mmid.szMMID, pL2->dwShares, pL2->dblPrice, pL2->nOwnShares, szTime );
    }
    pVuMarketDepth->m_lcMarketDepthAsk.BlankRemainingRows();
  }
  stateMDUpdate = MDUpdateNone;
}


void CTradingLogic::Acct1LmtBuy( long Shares, double dblPrice ) {
  if ( theApp.m_bAllowTrades ) {
    CGTOrderX *pOrder = new CGTOrderX( stkSession1, CGTOrderX::EOrderType::Limit, CGTOrderX::EOrderSide::Buy, Shares );
    //(GTOrder) *pOrder = stkSession1->m_defOrder;
    pOrder->SetOnOrderSentHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::InsertOrder ) );
    pOrder->SetOnUpdateOrderStatusHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::UpdateOrderStatus ) );
    pOrder->SetOnOrderCompletedHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::DeleteOrder ) );
    stkSession1->Bid( *pOrder, Shares, dblPrice, METHOD_ISLD );
    //stkSession1->Bid( Shares, dblPrice, METHOD_ISLD
    //pOrder->dblPrice = dblPrice;
    stkSession1->PlaceOrder(*pOrder);
  }
}

void CTradingLogic::Acct1LmtSell( long Shares, double dblPrice ) {
  if ( theApp.m_bAllowTrades ) {
    CGTOrderX *pOrder = new CGTOrderX( stkSession1, CGTOrderX::EOrderType::Limit, CGTOrderX::EOrderSide::Sell, Shares );
    //(GTOrder) *pOrder = stkSession1->m_defOrder;
    pOrder->dblPrice = dblPrice;
    pOrder->SetOnOrderSentHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::InsertOrder ) );
    pOrder->SetOnUpdateOrderStatusHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::UpdateOrderStatus ) );
    pOrder->SetOnOrderCompletedHandler( MakeDelegate( &pVuPendingOrders->po1, &CCtlPendingOrders::DeleteOrder ) );
    stkSession1->PlaceOrder(*pOrder);
  }
}

void CTradingLogic::Acct2LmtBuy( long Shares, double dblPrice ) {
  if ( theApp.m_bAllowTrades ) {
    CGTOrderX *pOrder = new CGTOrderX( stkSession1, CGTOrderX::EOrderType::Limit, CGTOrderX::EOrderSide::Buy, Shares );
    //(GTOrder) *pOrder = stkSession2->m_defOrder;
    pOrder->dblPrice = dblPrice;
    pOrder->SetOnOrderSentHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::InsertOrder ) );
    pOrder->SetOnUpdateOrderStatusHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::UpdateOrderStatus ) );
    pOrder->SetOnOrderCompletedHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::DeleteOrder ) );
    stkSession2->PlaceOrder(*pOrder);
  }
}

void CTradingLogic::Acct2LmtSell( long Shares, double dblPrice ) {
  if ( theApp.m_bAllowTrades ) {
    CGTOrderX *pOrder = new CGTOrderX( stkSession1, CGTOrderX::EOrderType::Limit, CGTOrderX::EOrderSide::Sell, Shares );
    //(GTOrder) *pOrder = stkSession2->m_defOrder;
    pOrder->dblPrice = dblPrice;
    pOrder->SetOnOrderSentHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::InsertOrder ) );
    pOrder->SetOnUpdateOrderStatusHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::UpdateOrderStatus ) );
    pOrder->SetOnOrderCompletedHandler( MakeDelegate( &pVuPendingOrders->po2, &CCtlPendingOrders::DeleteOrder ) );
    stkSession2->PlaceOrder(*pOrder);
  }
}

void CTradingLogic::OnCancelAcct1Order( DWORD_PTR ptr ) {
  CGTOrderX *pOrder = (CGTOrderX*) ptr;
  if ( theApp.m_bAllowTrades ) {
    stkSession1->CancelOrder( pOrder->GetPending() );
  }
}

void CTradingLogic::OnCancelAcct2Order( DWORD_PTR ptr ) {
  CGTOrderX *pOrder = (CGTOrderX*) ptr;
  if ( theApp.m_bAllowTrades ) {
    stkSession2->CancelOrder( pOrder->GetPending() );
  }
}

void CTradingLogic::Acct1CancelBid() {
  if ( theApp.m_bAllowTrades ) {
  }
}

void CTradingLogic::Acct2CancelBid() {
  if ( theApp.m_bAllowTrades ) {
  }
}

void CTradingLogic::Acct1CancelAsk() {
  if ( theApp.m_bAllowTrades ) {
  }
}

void CTradingLogic::Acct2CancelAsk() {
  if ( theApp.m_bAllowTrades ) {
  }
}
