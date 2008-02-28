#include "StdAfx.h"
#include "IQFeedSymbol.h"
//#include "GTScalp.h"

#include <iostream>

CIQFSymbol::CIQFSymbol(const string &symbol) {
  m_sSymbol = symbol;
  m_cnt = 0;
  dblTrade = 0;
  dblChange = 0;
  TradeSize = 0;
  TotalVolume = 0;
  dblBid = 0;
  dblAsk = 0;
  BidSize = 0;
  AskSize = 0;
  dblOpen = 0;
  dblClose = 0;
  cntTrades = 0;
  OpenInterest = 0;
  dblHigh = 0;
  dblLow = 0;

  QStatus = qUnknown;
}

CIQFSymbol::~CIQFSymbol(void) {
}

bool CIQFSymbol::Watch() {
  bool b = false;
  m_cnt++;
  if ( 1 == m_cnt ) {
    b = true;
  }
  return b;
}

bool CIQFSymbol::UnWatch() {
  bool b = false;
  if ( 0 < m_cnt ) {
    m_cnt--;
    if ( 0 == m_cnt ) {
      b = true;
    }
  }
  return b;
}                                               

void CIQFSymbol::HandleFundamentalMessage( CIQFFundamentalMessage *pMsg ) {
  //pMsg->EmitFields();
  sOptionRoots = pMsg->Field( CIQFFundamentalMessage::FRootOptionSymbols );
  AverageVolume = pMsg->Integer( CIQFFundamentalMessage::FAveVolume );
  sCompanyName = pMsg->Field( CIQFFundamentalMessage::FName );
  Precision = pMsg->Integer( CIQFFundamentalMessage::FPrecision );
  dblHistoricalVolatility = pMsg->Double( CIQFFundamentalMessage::FVolatility );
  dblStrikePrice = pMsg->Double( CIQFFundamentalMessage::FStrikePrice );

  //CString s;
  //s.Format( "%s: %s, %s, %d, %d, %0.2f", 
  //  m_sSymbol, sCompanyName, sOptionRoots, AverageVolume, Precision, dblHistoricalVolatility );
  //theApp.pConsoleMessages->WriteLine( s ); 

  OnFundamentalMessage( this );
}

void CIQFSymbol::HandlePricingMessage( CIQFPricingMessage *pMsg ) {
  string sLastTradeTime = pMsg->Field( CIQFPricingMessage::QPLastTradeTime );
  if ( sLastTradeTime.length() > 0 ) {  // can we do 'assume' anything if it is 0?
    char chType = sLastTradeTime[ sLastTradeTime.length() - 1 ];
    switch ( chType ) {
    case 't':
    case 'T':
      dtLastTrade = pMsg->LastTradeTime();
      dblTrade = pMsg->Double( CIQFPricingMessage::QPLast );
      dblChange = pMsg->Double( CIQFPricingMessage::QPChange );
      TotalVolume = pMsg->Integer( CIQFPricingMessage::QPTtlVol );
      TradeSize = pMsg->Integer( CIQFPricingMessage::QPLastVol );
      dblHigh = pMsg->Double( CIQFPricingMessage::QPHigh );
      dblLow = pMsg->Double( CIQFPricingMessage::QPLow );
      dblOpen = pMsg->Double( CIQFPricingMessage::QPOpen );
      dblClose = pMsg->Double( CIQFPricingMessage::QPClose );
      cntTrades = pMsg->Integer( CIQFPricingMessage::QPNumTrades );
      break;
    case 'b':
      dtLastTrade = pMsg->LastTradeTime();
      dblBid = pMsg->Double( CIQFPricingMessage::QPBid );
      BidSize = pMsg->Integer( CIQFPricingMessage::QPBidSize );
      dblAsk = pMsg->Double( CIQFPricingMessage::QPAsk );
      AskSize = pMsg->Integer( CIQFPricingMessage::QPAskSize );
      break;
    case 'a':
      dtLastTrade = pMsg->LastTradeTime();
      dblAsk = pMsg->Double( CIQFPricingMessage::QPAsk );
      AskSize = pMsg->Integer( CIQFPricingMessage::QPAskSize );
      break;
    case 'o':
      break;
    }
  }
  //OpenInterest = pMsg->Integer( CIQFPricingMessage::QPOpenInterest );

  //CString s;
  //s.Format( "%s: %c %0.2f@%d b=%0.2f@%d a=%0.2f@%d #=%d", 
  //  m_sSymbol, chType, dblTrade, TradeSize, dblBid, BidSize, dblAsk, AskSize, cntTrades );
  //theApp.pConsoleMessages->WriteLine( s ); 
}

void CIQFSymbol::HandleSummaryMessage( CIQFSummaryMessage *pMsg ) {
  HandlePricingMessage( pMsg );
  OnSummaryMessage( this );
}

void CIQFSymbol::HandleUpdateMessage( CIQFUpdateMessage *pMsg ) {

  if ( qUnknown == QStatus ) {
    QStatus = ( _T( "Not Found" ) == pMsg->Field( CIQFPricingMessage::QPLast ) ) ? qNotFound : qFound;
    if ( qNotFound == QStatus ) {
      CString s;
      s.Format( "%s not found", m_sSymbol );
      //theApp.pConsoleMessages->WriteLine( s ); 
      cout << s << endl;
    }
  }
  if ( qFound == QStatus ) {
    HandlePricingMessage( pMsg );
    OnUpdateMessage( this );
  }
}

void CIQFSymbol::HandleNewsMessage( CIQFNewsMessage *pMsg ) {
}
