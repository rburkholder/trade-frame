#include "StdAfx.h"
#include "GTScalp.h"
#include "TradingLogic.h"
#include "GTOrderX.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

EColor MDColors[] = { 
  LightYellow, LightYellow, PaleGoldenrod, Wheat, Goldenrod, DarkGoldenrod,
  DarkKhaki, DarkOrange, Orange, OrangeRed, Red, Crimson, Black };  // Black is not used, simply an end of list indicator

CTradingLogic::CTradingLogic( CString sSymbol ) {
  
  string s;
  m_bFirstTradeFound = false;

  //m_bAllowTrades = false;  // allows trades to be executed by broker
  m_state = EState::Init;

  m_sSymbol = sSymbol;
  stkSession1 = (CGTStockX*) theApp.m_session1.CreateStock( m_sSymbol );
  stkSession2 = (CGTStockX*) theApp.m_session2.CreateStock( m_sSymbol );
  stkSession2 ->CancelLevel2();
  stkSession2 ->CancelQuote();
  stkSession2 ->CancelChart();

  pTradeFrame = new CTradeFrame( this );
  pTradeFrame->Create(theApp.m_pMainWnd);
  pTradeFrame->SetAllowRedraw( false );

  //pChartDaily = new CVuChart(theApp.m_pMainWnd);
  //pChartDaily->ShowWindow(1);

  pChartIntraDay = new CVuChart(theApp.m_pMainWnd);
  pChartIntraDay->ShowWindow(1);
  s = "Intraday ";
  s.append( m_sSymbol );
  pChartIntraDay->m_chart.SetTitle( s );

  pVuPendingOrders = new CVuPendingOrders( theApp.m_pMainWnd );
  pVuPendingOrders->po1.SetOnCancelHandler( MakeDelegate( this, &CTradingLogic::OnCancelAcct1Order ) );
  pVuPendingOrders->po2.SetOnCancelHandler( MakeDelegate( this, &CTradingLogic::OnCancelAcct2Order ) );

  pVuMarketDepth = new CVuMarketDepth( theApp.m_pMainWnd );
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
  pQuotes = new CQuotes( 100000 );
  pTrades = new CTrades( 100000 );

  pChartIntraDay->m_chart.SetBarFactoryWidthSeconds( 60 );
  pChartIntraDay->m_chart.SetWindowWidthSeconds( 90 * 60 ); // 90 minute window
  pChartIntraDay->m_chart.setMajorTickInc( 12 * 60 );
  pChartIntraDay->m_chart.setMinorTickInc( 60 );

  ASSERT( NULL != theApp.m_pIQFeed );
  theApp.m_pIQFeed->TimeMessage.Add( MakeDelegate( this, &CTradingLogic::OnIQFeedTimeMessage ) );

  pPivot1Day = NULL;
  pPivot3Day = NULL;
  pPivotWeek = NULL;
  pPivotMonth = NULL;
  pPivot20Days = NULL;

  m_state = EState::History;

  pBars = new CBars( 250 );
  phd = new IQFeedHistoryHD( pBars );
  phd->SetOnRequestComplete( MakeDelegate( this, &CTradingLogic::OnDailyBarHistoryDone ) );
  pht = new IQFeedHistoryHT( pQuotes, pTrades );
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
  theApp.m_pRefresh->OnRefresh.Add( MakeDelegate( this, &CTradingLogic::OnPeriodicRefresh ) );
}

CTradingLogic::~CTradingLogic(void){

  //theApp.m_pIQFeed->TimeMessage.Remove( MakeDelegate( this, &CTradingLogic::OnIQFeedTimeMessage ) );

  theApp.m_pRefresh->OnRefresh.Remove( MakeDelegate( this, &CTradingLogic::OnPeriodicRefresh ) );

  if ( NULL != pPivot1Day ) delete pPivot1Day;
  if ( NULL != pPivot3Day ) delete pPivot3Day;
  if ( NULL != pPivotWeek ) delete pPivotWeek;
  if ( NULL != pPivotMonth ) delete pPivotMonth;
  if ( NULL != pPivot20Days ) delete pPivot20Days;

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

void CTradingLogic::OnIQFeedTimeMessage( CIQFTimeMessage *pMsg ) {
}

void CTradingLogic::OnPeriodicRefresh( CGeneratePeriodicRefresh *pMsg ) {
  UpdateMarketDepth();
}

void CTradingLogic::OnDailyBarHistoryDone( IQFeedHistory *pHistory ) {
  if ( pBars->Count() > 0 ) {

    //pBars->Flip();  // probably not needed as already performed in IQFeedRetrieveHistory::OnPortMessage
    CBar *pBar = pBars->Last();

    //stringstream ss;
    date dtThisDay = pBar->m_dt.date();
    //ss << "This Day: " << dtThisDay;
    date dtPrevDay = ( boost::date_time::Monday == dtThisDay.day_of_week() ) 
      ? dtThisDay - days( 3 )
      : dtThisDay - days( 1 );
    //ss << ", Prev Day: " << dtPrevDay;
    date dtPrevMonday = dtThisDay + days( boost::date_time::Monday - dtThisDay.day_of_week() ) - days( 7 );
    //ss << ", Prev Monday: " << dtPrevMonday;
    date dtPrevWeekFriday = dtPrevMonday + days( 4 );
    //ss << ", Prev Friday: " << dtPrevWeekFriday;
    date dtPrevMonthEnd = dtThisDay - days( dtThisDay.day().as_number() );
    //ss << ", Prev Month End: " << dtPrevMonthEnd;
    date dtPrevMonth = dtPrevMonthEnd - days( dtPrevMonthEnd.day().as_number() - 1 );
    //ss << ", Prev Month: " << dtPrevMonth;
    date dtMonthAgo = dtThisDay - days( 30 );
    //ss << ", Month ago: " << dtMonthAgo;
    date dtSixMonthsAgo = dtThisDay - months( 6 );
    //ss << ", Six Months ago: " << dtSixMonthsAgo;
    date dt200DaysAgo = dtThisDay - days( 42 * 7 - 2 );
    //ss << ", 200 Days ago: " << dt200DaysAgo;

    pPivot1Day = new CPivotSet( "pv1Dy", pBar->m_dblHigh, pBar->m_dblLow, pBar->m_dblClose );

    double day3hi = pBar->m_dblHigh;
    double day3lo = pBar->m_dblLow;
    double day3cl = pBar->m_dblClose;

    if ( pBars->Count() >= 3 ) {
      pBar = pBars->Ago( 1 );
      day3hi = max( day3hi, pBar->m_dblHigh );
      day3lo = min( day3lo, pBar->m_dblLow );
      pBar = pBars->Ago( 2 );
      day3hi = max( day3hi, pBar->m_dblHigh );
      day3lo = min( day3lo, pBar->m_dblLow );
    }
    pPivot3Day = new CPivotSet( "pv3Dy", day3hi, day3lo, day3cl );

    CBars *pBarsForPeriod;

    if ( pBars->Count() >= 10 ) {
      pBarsForPeriod = pBars->Subset( ptime( dtPrevMonday ), 5 );
      pPivotWeek = new CPivotSet( "pvWk", pBarsForPeriod );
      delete pBarsForPeriod;
    }
    if ( pBars->Count() >= 42 ) {
      pBarsForPeriod = pBars->Subset( ptime( dtPrevMonth ), 20 );
      pPivotMonth = new CPivotSet( "pvMn", pBarsForPeriod );
      delete pBarsForPeriod;
    }
    if ( pBars->Count() >= 20 ) {
      pBarsForPeriod = pBars->Subset( ptime( dtMonthAgo ) );
      pPivot20Days = new CPivotSet( "pv20D", pBarsForPeriod );
      delete pBarsForPeriod;
    }

    CPivotSet *pivots[] = { pPivot1Day, pPivot3Day, pPivotWeek, pPivotMonth, pPivot20Days, NULL };
    int i = 0;
    CPivotSet *pPivot;
    pPivot = pivots[ i ];
    char sz[ 10 ];
    while ( NULL != pPivot ) {
      
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "R3" );
      pTradeFrame->AppendStaticIndicator( pPivot->R3(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "R23" );
      pTradeFrame->AppendStaticIndicator( pPivot->R23(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "R2" );
      pTradeFrame->AppendStaticIndicator( pPivot->R2(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "R12" );
      pTradeFrame->AppendStaticIndicator( pPivot->R12(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "R1" );
      pTradeFrame->AppendStaticIndicator( pPivot->R1(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "PVR1" );
      pTradeFrame->AppendStaticIndicator( pPivot->PVR1(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "PV" );
      pTradeFrame->AppendStaticIndicator( pPivot->PV(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "PVS1" );
      pTradeFrame->AppendStaticIndicator( pPivot->PVS1(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "S1" );
      pTradeFrame->AppendStaticIndicator( pPivot->S1(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "S12" );
      pTradeFrame->AppendStaticIndicator( pPivot->S12(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "S2" );
      pTradeFrame->AppendStaticIndicator( pPivot->S2(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "S23" );
      pTradeFrame->AppendStaticIndicator( pPivot->S23(), sz );
      *sz = 0;
      strcat_s( sz, 10, pPivot->Name().c_str() );
      strcat_s( sz, 10, "S3" );
      pTradeFrame->AppendStaticIndicator( pPivot->S3(), sz );

      ++i;
      pPivot = pivots[ i ];
    }

    //int n = pBars->Count();
    //int i;

    /*
    pChartDaily->m_chart.ClearChart();
    pChartDaily->m_chart.SetUpdateChart( false );
    pChartDaily->m_chart.setMajorTickInc( 30 * 86400  );
    pChartDaily->m_chart.setMinorTickInc(  86400  );

    for ( i = 0; i < n; i++ ) {
      pBar = pBars -> At( i );
      pChartDaily->m_chart.Add( pBar );
    }
    pChartDaily->m_chart.SetUpdateChart( true );
    pChartDaily->m_chart.UpdateChart();
    */
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

    CTrade *pTrade;
    CQuote *pQuote;
    for ( i = 0; i < nTrades; i++ ) {
      pQuote = pQuotes -> At( i );
      pTradeFrame->NewQuoteLevel1( *pQuote );
      pTrade = pTrades -> At( i );
      OnPrintCommon( *pTrade );
    }
  }

  pChartIntraDay->m_chart.SetUpdateChart( true );
  //pChartIntraDay->m_chart.UpdateChart();
  m_state = EState::RealTime;
  pTradeFrame->SetAllowRedraw( true );
}

void CTradingLogic::OnPrintCommon( const CTrade &trade ) {
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

void CTradingLogic::OnPrint( const CTrade &trade ) {
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

void CTradingLogic::OnQuoteLevel1( const CQuote &quote ) {
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
void CTradingLogic::OnLevel2( const CMarketDepth &md ) {
  switch ( m_state ) {
    case EState::RealTime:
      switch ( md.m_eSide ) {
        case CMarketDepth::Ask: stateMDUpdate = ( MDUpdateBid == stateMDUpdate ) ? MDUpdateBoth : MDUpdateAsk;
          break;
        case CMarketDepth::Bid: stateMDUpdate = ( MDUpdateAsk == stateMDUpdate ) ? MDUpdateBoth : MDUpdateBid;
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
        if ( Black == MDColors[ ixColor ] ) ixColor = 0;
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
        if ( Black == MDColors[ ixColor ] ) ixColor = 0;
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
