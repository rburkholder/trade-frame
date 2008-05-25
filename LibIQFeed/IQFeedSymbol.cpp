#include "StdAfx.h"
#include "IQFeedSymbol.h"

#include <iostream>

CIQFSymbol::CIQFSymbol(const string &sSymbol) 
: CSymbol( sSymbol ),
m_cnt( 0 ), m_dblTrade( 0 ), m_dblChange( 0 ), m_nTradeSize( 0 ), m_nTotalVolume( ),
m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 ), 
m_dblOpen( 0 ), m_dblClose( 0 ), m_cntTrades( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), 
m_nOpenInterest( 0 ), m_QStatus( qUnknown ),
m_bQuoteTradeWatchInProgress( false ), m_bDepthWatchInProgress( false )
{
}

CIQFSymbol::~CIQFSymbol(void) {
}

/*
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
*/

void CIQFSymbol::HandleFundamentalMessage( CIQFFundamentalMessage *pMsg ) {
  //pMsg->EmitFields();
  m_sOptionRoots = pMsg->Field( CIQFFundamentalMessage::FRootOptionSymbols );
  m_AverageVolume = pMsg->Integer( CIQFFundamentalMessage::FAveVolume );
  m_sCompanyName = pMsg->Field( CIQFFundamentalMessage::FName );
  m_Precision = pMsg->Integer( CIQFFundamentalMessage::FPrecision );
  m_dblHistoricalVolatility = pMsg->Double( CIQFFundamentalMessage::FVolatility );
  m_dblStrikePrice = pMsg->Double( CIQFFundamentalMessage::FStrikePrice );

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
      m_dtLastTrade = pMsg->LastTradeTime();
      m_dblTrade = pMsg->Double( CIQFPricingMessage::QPLast );
      m_dblChange = pMsg->Double( CIQFPricingMessage::QPChange );
      m_nTotalVolume = pMsg->Integer( CIQFPricingMessage::QPTtlVol );
      m_nTradeSize = pMsg->Integer( CIQFPricingMessage::QPLastVol );
      m_dblHigh = pMsg->Double( CIQFPricingMessage::QPHigh );
      m_dblLow = pMsg->Double( CIQFPricingMessage::QPLow );
      m_dblOpen = pMsg->Double( CIQFPricingMessage::QPOpen );
      m_dblClose = pMsg->Double( CIQFPricingMessage::QPClose );
      m_cntTrades = pMsg->Integer( CIQFPricingMessage::QPNumTrades );
      break;
    case 'b':
      m_dtLastTrade = pMsg->LastTradeTime();
      m_dblBid = pMsg->Double( CIQFPricingMessage::QPBid );
      m_nBidSize = pMsg->Integer( CIQFPricingMessage::QPBidSize );
      m_dblAsk = pMsg->Double( CIQFPricingMessage::QPAsk );
      m_nAskSize = pMsg->Integer( CIQFPricingMessage::QPAskSize );
      break;
    case 'a':
      m_dtLastTrade = pMsg->LastTradeTime();
      m_dblAsk = pMsg->Double( CIQFPricingMessage::QPAsk );
      m_nAskSize = pMsg->Integer( CIQFPricingMessage::QPAskSize );
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

  if ( qUnknown == m_QStatus ) {
    m_QStatus = ( _T( "Not Found" ) == pMsg->Field( CIQFPricingMessage::QPLast ) ) ? qNotFound : qFound;
    if ( qNotFound == m_QStatus ) {
      CString s;
      s.Format( "%s not found", m_sSymbolName );
      //theApp.pConsoleMessages->WriteLine( s ); 
      cout << s << endl;
    }
  }
  if ( qFound == m_QStatus ) {
    HandlePricingMessage( pMsg );
    OnUpdateMessage( this );
  }
}

void CIQFSymbol::HandleNewsMessage( CIQFNewsMessage *pMsg ) {
}
