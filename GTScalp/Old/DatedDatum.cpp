#include "StdAfx.h"
#include "DatedDatum.h"

// CDatedDatum

CDatedDatum::CDatedDatum(void) { 
  m_dt = not_a_date_time;
}

CDatedDatum::CDatedDatum(boost::posix_time::ptime dt) {
  m_dt = dt;
}

CDatedDatum::CDatedDatum(const CDatedDatum &datum) {
  m_dt = datum.m_dt;
}

CDatedDatum::CDatedDatum(const std::string &dt) {
  //m_dt = boost::posix_time::time_from_string(dt);
  ASSERT( dt.length() == 19 );
  const char *s = dt.c_str();
  m_dt = ptime( 
    boost::gregorian::date( atoi( s ), atoi( s + 5 ), atoi( s + 8 ) ), 
    boost::posix_time::time_duration( atoi( s + 11 ), atoi( s + 14 ), atoi( s + 17 ) ) );
}

CDatedDatum::~CDatedDatum(void) {
}

bool operator<( const CDatedDatum &datum1, const CDatedDatum &datum2 ) { return datum1.m_dt < datum2.m_dt; };

// CQuote

CQuote::CQuote(void): CDatedDatum() {
  m_dblBid =0;
  m_dblAsk = 0;
  m_nBidSize = 0;
  m_nAskSize = 0;
}

CQuote::CQuote(ptime dt): CDatedDatum(dt) {
  m_dblBid =0;
  m_dblAsk = 0;
  m_nBidSize = 0;
  m_nAskSize = 0;
}

CQuote::CQuote(const CQuote &quote): CDatedDatum( quote ) {
  m_dblBid = quote.m_dblBid;
  m_nBidSize = quote.m_nBidSize;
  m_dblAsk = quote.m_dblAsk;
  m_nAskSize = quote.m_nAskSize;
}

CQuote::CQuote( ptime dt, double dblBid, int nBidSize, double dblAsk, int nAskSize ) :
CDatedDatum( dt ) {
  m_dblBid = dblBid;
  m_nBidSize = nBidSize;
  m_dblAsk = dblAsk;
  m_nAskSize = nAskSize;
}

CQuote::CQuote( const std::string &dt, const std::string &bid, 
               const std::string &bidsize, const std::string &ask, const std::string &asksize ) :
CDatedDatum( dt ) {
  char *stopchar;
  m_dblBid = strtod( bid.c_str(), &stopchar );
  m_nBidSize = atoi( bidsize.c_str() );
  m_dblAsk = strtod( ask.c_str(), &stopchar );
  m_nAskSize = atoi( asksize.c_str() );
}

CQuote::~CQuote(void) {
  CDatedDatum::~CDatedDatum();
}

bool operator<( const CQuote &quote1, const CQuote &quote2 ) { return quote1.m_dt < quote2.m_dt; };

// CTrade

CTrade::CTrade(void): CDatedDatum() {
  m_dblTrade = 0;
  m_nTradeSize = 0;
}

CTrade::CTrade(ptime dt): CDatedDatum(dt) {
  m_dblTrade = 0;
  m_nTradeSize = 0;
}

CTrade::CTrade(const CTrade &trade): CDatedDatum( trade ) {
  m_dblTrade = trade.m_dblTrade;
  m_nTradeSize = trade.m_nTradeSize;
}

CTrade::CTrade( ptime dt, double dblTrade, int nTradeSize ) :
CDatedDatum( dt ) {
  m_dblTrade = dblTrade;
  m_nTradeSize = nTradeSize;
}

CTrade::CTrade( const std::string &dt, const std::string &trade, const std::string &size ) :
CDatedDatum( dt ) {
  char *stopchar;
  m_dblTrade = strtod( trade.c_str(), &stopchar );
  m_nTradeSize = atoi( size.c_str() );
}

CTrade::~CTrade(void) {
  CDatedDatum::~CDatedDatum();
}

bool operator<( const CTrade &trade1, const CTrade &trade2 ) { return trade1.m_dt < trade2.m_dt; };

// CBar

CBar::CBar(void): CDatedDatum() {
  m_dblOpen = 0;
  m_dblHigh = 0;
  m_dblLow = 0;
  m_dblClose = 0;
  m_nVolume = 0;
}

CBar::CBar(ptime dt): CDatedDatum(dt) {
  m_dblOpen = 0;
  m_dblHigh = 0;
  m_dblLow = 0;
  m_dblClose = 0;
  m_nVolume = 0;
}

CBar::CBar(const CBar &bar): CDatedDatum( bar ) {
  m_dblOpen = bar.m_dblOpen;
  m_dblHigh = bar.m_dblHigh;
  m_dblLow = bar.m_dblLow;
  m_dblClose = bar.m_dblClose;
  m_nVolume = bar.m_nVolume;
}

CBar::CBar(boost::posix_time::ptime dt, double dblOpen, double dblHigh, double dblLow, double dblClose, int nVolume):
CDatedDatum( dt ) {
  m_dblOpen = dblOpen;
  m_dblHigh = dblHigh;
  m_dblLow = dblLow;
  m_dblClose = dblClose;
  m_nVolume = nVolume;
}

CBar::CBar(const std::string &dt, const std::string &open, 
           const std::string &high, const std::string &low, 
           const std::string &close, const std::string &volume):
CDatedDatum( dt ) {
  char *stopchar;
  m_dblOpen = strtod( open.c_str(), &stopchar );
  m_dblHigh = strtod( high.c_str(), &stopchar );
  m_dblLow = strtod( low.c_str(), &stopchar );
  m_dblClose = strtod( close.c_str(), &stopchar );
  m_nVolume = atoi( volume.c_str() );
}

CBar::~CBar(void) {
  CDatedDatum::~CDatedDatum();
}

bool operator<( const CBar &bar1, const CBar &bar2 ) { return bar1.m_dt < bar2.m_dt; };

// CMarketDepth

CMarketDepth::CMarketDepth(): CDatedDatum() {
  m_eSide = None;
  m_nShares = 0;
  m_dblPrice = 0;
  //m_szMMID[ 0 ] = 0;
}

CMarketDepth::CMarketDepth(ptime dt): CDatedDatum(dt) {
  m_eSide = None;
  m_nShares = 0;
  m_dblPrice = 0;
  //m_szMMID[ 0 ] = 0;
}

CMarketDepth::CMarketDepth(const CMarketDepth &md): CDatedDatum( md ) {
  m_eSide = md.m_eSide;
  m_nShares = md.m_nShares;
  m_dblPrice = md.m_dblPrice;
  //strcpy_s( m_szMMID, 10, md.m_szMMID );
  m_mmid = md.m_mmid;
}

CMarketDepth::CMarketDepth(boost::posix_time::ptime dt, char chSide, long nShares, double dblPrice, MMID mmid):
CDatedDatum( dt ) {
  m_eSide = None;
  if ( 'S' == chSide ) m_eSide = Ask;
  if ( 'B' == chSide ) m_eSide = Bid;
  m_nShares = nShares;
  m_dblPrice = dblPrice;
  //copymmid( m_szMMID, mmid );
  m_mmid = mmid;
  //strcpy_s( m_szMMID, 10, MMID );
}

CMarketDepth::CMarketDepth(const std::string &dt, char chSide, const std::string &shares, 
                           const std::string &price, const std::string &mmid) :
CDatedDatum( dt ) {
  char *stopchar;
  m_eSide = None;
  if ( 'S' == chSide ) m_eSide = Ask;
  if ( 'B' == chSide ) m_eSide = Bid;
  m_nShares = atoi( shares.c_str() );
  m_dblPrice = strtod( price.c_str(), &stopchar );
  //strcpy_s( m_szMMID, 6, mmid.c_str());
  m_mmid = mmid.c_str();
}


CMarketDepth::~CMarketDepth() {
  CDatedDatum::~CDatedDatum();
}

bool operator<( const CMarketDepth &md1, const CMarketDepth &md2 ) { return md1.m_dt < md2.m_dt; };

