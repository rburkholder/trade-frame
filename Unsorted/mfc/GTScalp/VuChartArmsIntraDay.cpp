// VuChartArmsIntraDay.cpp : implementation file
//

#include "stdafx.h"
#include "GTScalp.h"
#include "VuChartArmsIntraDay.h"

// CVuChartArmsIntraDay dialog

IMPLEMENT_DYNAMIC(CVuChartArmsIntraDay, CDialog)

CVuChartArmsIntraDay::CVuChartArmsIntraDay(CWnd* pParent /*=NULL*/)
	: CDialog(CVuChartArmsIntraDay::IDD, pParent) {

  BOOL b = Create(IDD, pParent );

}

CVuChartArmsIntraDay::~CVuChartArmsIntraDay() {

  delete pTradesIndu;
  delete pTradesTrin;
  delete pTradesTick;

  delete pQuotesIndu;
  delete pQuotesTrin;
  delete pQuotesTick;

  delete pHistoryIndu;
  delete pHistoryTrin;
  delete pHistoryTick;

  DestroyWindow();
}

BOOL CVuChartArmsIntraDay::OnInitDialog() {
  BOOL b = CDialog::OnInitDialog();

  pTradesIndu = pTradesTrin = pTradesTick = NULL;
  m_bInduHistoryDone = m_bTrinHistoryDone = m_bTickHistoryDone = false;

  return b;
}

void CVuChartArmsIntraDay::StartCharts( bool bLive, unsigned int nDaysAgo ) {

  m_ChartIndu.SetBarFactoryWidthSeconds( 180 );
  m_ChartIndu.SetWindowWidthSeconds( 14 * 30 * 60 ); // 7.0 hours
  m_ChartIndu.setMajorTickInc( 12 * 60 );
  m_ChartIndu.setMinorTickInc( 60 );
  m_ChartIndu.SetTitle( "Indu" );

  m_ChartTrin.SetBarFactoryWidthSeconds( 180 );
  m_ChartTrin.SetWindowWidthSeconds( 14 * 30 * 60 ); // 7.0 hours
  m_ChartTrin.setMajorTickInc( 12 * 60 );
  m_ChartTrin.setMinorTickInc( 60 );
  m_ChartTrin.SetTitle( "Trin" );

  m_ChartTick.SetBarFactoryWidthSeconds( 180 );
  m_ChartTick.SetWindowWidthSeconds( 14 * 30 * 60 ); // 7.0 hours
  m_ChartTick.setMajorTickInc( 12 * 60 );
  m_ChartTick.setMinorTickInc( 60 );
  m_ChartTick.SetTitle( "Tick" );

  pQuotesIndu = new Quotes( 100000 );
  pQuotesTrin = new Quotes( 100000 );
  pQuotesTick = new Quotes( 100000 );

  pTradesIndu = new Trades( 100000 );
  pTradesTrin = new Trades( 100000 );
  pTradesTick = new Trades( 100000 );

  pHistoryIndu = new IQFeedHistoryHT( m_IQFeedProvider.GetIQFeedProvider(), pQuotesIndu, pTradesIndu );
  pHistoryTrin = new IQFeedHistoryHT(m_IQFeedProvider.GetIQFeedProvider(), pQuotesTrin, pTradesTrin );
  pHistoryTick = new IQFeedHistoryHT(m_IQFeedProvider.GetIQFeedProvider(), pQuotesTick, pTradesTick );

  /*
  pHistoryIndu->SetOnRequestComplete( MakeDelegate( this, &CVuChartArmsIntraDay::OnInduHistoryDone ) );
  pHistoryTrin->SetOnRequestComplete( MakeDelegate( this, &CVuChartArmsIntraDay::OnTrinHistoryDone ) );
  pHistoryTick->SetOnRequestComplete( MakeDelegate( this, &CVuChartArmsIntraDay::OnTickHistoryDone ) );

  if ( bLive ) {
    pHistoryIndu->LiveRequest( "INDU.X", nDaysAgo );
    pHistoryTrin->LiveRequest( "TRIN.Z", nDaysAgo );
    pHistoryTick->LiveRequest( "TICK.Z", nDaysAgo );
  }
  else {
    pHistoryIndu->FileRequest( "INDU.X", nDaysAgo );
    pHistoryTrin->FileRequest( "TRIN.Z", nDaysAgo );
    pHistoryTick->FileRequest( "TICK.Z", nDaysAgo );
  }
  */
  HandleRealTime();
}

void CVuChartArmsIntraDay::OnInduHistoryDone( IQFeedHistory *pHistory ) {
  if ( pTradesIndu->Count() > 0 ) {  // what happens if we do have a count of 0?
    pTradesIndu->Flip();
    ASSERT( !m_bInduHistoryDone );
    m_bInduHistoryDone = true;
    ProcessHistory();
  }
}

void CVuChartArmsIntraDay::OnTrinHistoryDone( IQFeedHistory *pHistory ) {
  if ( pTradesTrin->Count() > 0 ) {  // what happens if we do have a count of 0?
    pTradesTrin->Flip();
    ASSERT( !m_bTrinHistoryDone );
    m_bTrinHistoryDone = true;
    ProcessHistory();
  }
}

void CVuChartArmsIntraDay::OnTickHistoryDone( IQFeedHistory *pHistory ) {
  if ( pTradesTick->Count() > 0 ) {  // what happens if we do have a count of 0?
    pTradesTick->Flip();
    ASSERT( !m_bTickHistoryDone );
    m_bTickHistoryDone = true;
    ProcessHistory();
  }
}

void CVuChartArmsIntraDay::ProcessHistory() {
  if ( m_bInduHistoryDone && m_bTrinHistoryDone && m_bTickHistoryDone ) {

//    merge.Add( (CTimeSeries<CDatedDatum> *) pTradesIndu, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeIndu ) );
//    merge.Add( (CTimeSeries<CDatedDatum> *) pTradesTrin, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTrin ) );
//    merge.Add( (CTimeSeries<CDatedDatum> *) pTradesTick, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTick ) );
//    merge.Add( pTradesIndu, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeIndu ) );
//    merge.Add( pTradesTrin, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTrin ) );
//    merge.Add( pTradesTick, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTick ) );

    merge.Run();

  // start real time data here
    HandleRealTime();
  }

}

void CVuChartArmsIntraDay::HandleRealTime() {
  if ( theApp.m_bLive ) {
//    CIQFeedSymbol *pSym;

    char symIndu[] = "INDU.X";
    m_IQFeedProvider.GetIQFeedProvider()->AddTradeHandler( symIndu, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeIndu ) );
    //pSym = theApp.m_pIQFeed->Attach( symIndu );
    //pSym->OnUpdateMessage.Add( MakeDelegate( this, &CVuChartArmsIntraDay::HandleInduUpdate ) );
    //theApp.m_pIQFeed->Watch( symIndu );

    char symTrin[] = "TRIN.Z";
    m_IQFeedProvider.GetIQFeedProvider()->AddTradeHandler( symTrin, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTrin ) );
    //pSym = theApp.m_pIQFeed->Attach( symTrin );
    //pSym->OnUpdateMessage.Add( MakeDelegate( this, &CVuChartArmsIntraDay::HandleTrinUpdate ) );
    //theApp.m_pIQFeed->Watch( symTrin );

    char symTick[] = "TICK.Z";
    m_IQFeedProvider.GetIQFeedProvider()->AddTradeHandler( symTick, MakeDelegate( this, &CVuChartArmsIntraDay::ProcessMergeTick ) );
    //pSym = theApp.m_pIQFeed->Attach( symTick );
    //pSym->OnUpdateMessage.Add( MakeDelegate( this, &CVuChartArmsIntraDay::HandleTickUpdate ) );
    //theApp.m_pIQFeed->Watch( symTick );
  }
}

//void CVuChartArmsIntraDay::HandleInduUpdate( CIQFSymbol *pSym ) {
//  Trade trade( pSym->m_dtLastTrade, pSym->m_dblTrade, pSym->m_nTradeSize );
//  ProcessMergeIndu( trade );
//}

//void CVuChartArmsIntraDay::HandleTrinUpdate( CIQFSymbol *pSym ) {
//  Trade trade( pSym->m_dtLastTrade, pSym->m_dblTrade, pSym->m_nTradeSize );
//  ProcessMergeTrin( trade );
//}

//void CVuChartArmsIntraDay::HandleTickUpdate( CIQFSymbol *pSym ) {
//  Trade trade( pSym->m_dtLastTrade, pSym->m_dblTrade, pSym->m_nTradeSize );
//  ProcessMergeTick( trade );
//}

void CVuChartArmsIntraDay::ProcessMergeIndu( const Trade &trade ) {
  //Trade *pTrade = (Trade *) pDatum;
  m_ChartArmsIntraDay.ProcessIndu( trade );
  m_ChartIndu.Add( trade );
}

void CVuChartArmsIntraDay::ProcessMergeTrin( const Trade &trade ) {
  //Trade *pTrade = (Trade *) pDatum;
  m_ChartArmsIntraDay.ProcessTrin( trade );
  m_ChartTrin.Add( trade );
}

void CVuChartArmsIntraDay::ProcessMergeTick( const Trade &trade ) {
  //Trade *pTrade = (Trade *) pDatum;
  m_ChartArmsIntraDay.ProcessTick( trade );
  m_ChartTick.Add( trade );
}

void CVuChartArmsIntraDay::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_CHARTINDUARMS, m_ChartArmsIntraDay);
  DDX_Control(pDX, IDC_CHARTINDU, m_ChartIndu);
  DDX_Control(pDX, IDC_CHARTARMS, m_ChartTrin);
  DDX_Control(pDX, IDC_CHARTTICK, m_ChartTick);
//  DDX_Control(pDX, IDC_ARMSINTRADAYCHART, m_ChartArmsIntraDay);
  //IDC_CHARTINDUARMS
}


BEGIN_MESSAGE_MAP(CVuChartArmsIntraDay, CDialog)
END_MESSAGE_MAP()


// CVuChartArmsIntraDay message handlers

void CVuChartArmsIntraDay::OnStnClickedTick() {
  // TODO: Add your control notification handler code here
}
