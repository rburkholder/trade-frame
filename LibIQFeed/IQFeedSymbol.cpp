#include "StdAfx.h"
#include "IQFeedSymbol.h"

#include <iostream>

CIQFeedSymbol::CIQFeedSymbol(const string &sSymbol) 
: CSymbol( sSymbol ),
m_cnt( 0 ), m_dblTrade( 0 ), m_dblChange( 0 ), m_nTradeSize( 0 ), m_nTotalVolume( ),
m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 ), 
m_dblOpen( 0 ), m_dblClose( 0 ), m_cntTrades( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), 
m_nOpenInterest( 0 ), m_QStatus( qUnknown ),
m_bQuoteTradeWatchInProgress( false ), m_bDepthWatchInProgress( false )
{
}

CIQFeedSymbol::~CIQFeedSymbol(void) {
}

void CIQFeedSymbol::HandleFundamentalMessage( CIQFFundamentalMessage *pMsg ) {
  m_sOptionRoots = pMsg->Field( CIQFFundamentalMessage::FRootOptionSymbols );
  m_AverageVolume = pMsg->Integer( CIQFFundamentalMessage::FAveVolume );
  m_sCompanyName = pMsg->Field( CIQFFundamentalMessage::FName );
  m_Precision = pMsg->Integer( CIQFFundamentalMessage::FPrecision );
  m_dblHistoricalVolatility = pMsg->Double( CIQFFundamentalMessage::FVolatility );
  m_dblStrikePrice = pMsg->Double( CIQFFundamentalMessage::FStrikePrice );

  OnFundamentalMessage( this );
}

void CIQFeedSymbol::DecodePricingMessage( CIQFPricingMessage *pMsg ) {
  m_bNewTrade = m_bNewQuote = m_bNewOpen = false;
  string sLastTradeTime = pMsg->Field( CIQFPricingMessage::QPLastTradeTime );
  if ( sLastTradeTime.length() > 0 ) {  // can we do 'assume' anything if it is 0?
    ptime dtLastTrade;
    double dblOpen, dblBid, dblAsk;
    int nBidSize, nAskSize;
    char chType = sLastTradeTime[ sLastTradeTime.length() - 1 ];
    m_dtLastTrade = pMsg->LastTradeTime();
    switch ( chType ) {
    case 't':
    case 'T':
      m_dblTrade = pMsg->Double( CIQFPricingMessage::QPLast );
      m_dblChange = pMsg->Double( CIQFPricingMessage::QPChange );
      m_nTotalVolume = pMsg->Integer( CIQFPricingMessage::QPTtlVol );
      m_nTradeSize = pMsg->Integer( CIQFPricingMessage::QPLastVol );
      m_dblHigh = pMsg->Double( CIQFPricingMessage::QPHigh );
      m_dblLow = pMsg->Double( CIQFPricingMessage::QPLow );
      m_dblClose = pMsg->Double( CIQFPricingMessage::QPClose );
      m_cntTrades = pMsg->Integer( CIQFPricingMessage::QPNumTrades );
      m_bNewTrade = true;

      dblOpen = pMsg->Double( CIQFPricingMessage::QPOpen );
      if ( ( m_dblOpen != dblOpen ) && ( 0 != dblOpen ) ) { 
        m_dblOpen = dblOpen; 
        m_bNewOpen = true; 
        std::cout << "IQF new open: " << m_sSymbolName<< "=" << m_dblOpen << std::endl;
      };

      // fall through to processing bid / ask
    case 'b':
    case 'a':
      dblBid = pMsg->Double( CIQFPricingMessage::QPBid );
      if ( m_dblBid != dblBid ) { m_dblBid = dblBid; m_bNewQuote = true; }
      nBidSize = pMsg->Integer( CIQFPricingMessage::QPBidSize );
      if ( m_nBidSize != nBidSize ) { m_nBidSize = nBidSize; m_bNewQuote = true; }
      dblAsk = pMsg->Double( CIQFPricingMessage::QPAsk );
      if ( m_dblAsk != dblAsk ) { m_dblAsk = dblAsk; m_bNewQuote = true; }
      nAskSize = pMsg->Integer( CIQFPricingMessage::QPAskSize );
      if ( m_nAskSize != nAskSize ) { m_nAskSize = nAskSize; m_bNewQuote = true; }
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

void CIQFeedSymbol::HandleSummaryMessage( CIQFSummaryMessage *pMsg ) {
  DecodePricingMessage( pMsg );
  OnSummaryMessage( this );
}

void CIQFeedSymbol::HandleUpdateMessage( CIQFUpdateMessage *pMsg ) {

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
    DecodePricingMessage( pMsg );
    OnUpdateMessage( this );
    ptime dt( microsec_clock::local_time() );
    // quote needs to be sent before the trade
    if ( m_bNewQuote ) {
      CQuote quote( dt, m_dblBid, m_nBidSize, m_dblAsk, m_nAskSize );
      CSymbol::m_OnQuote( quote );
    }
    if ( m_bNewTrade ) {
      CTrade trade( dt, m_dblTrade, m_nTradeSize );
      CSymbol::m_OnTrade( trade );
      if ( m_bNewOpen ) {
        CSymbol::m_OnOpen( trade );
      }
    }
  }
}

void CIQFeedSymbol::HandleNewsMessage( CIQFNewsMessage *pMsg ) {
}
