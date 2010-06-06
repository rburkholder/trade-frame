/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include <assert.h>

#include "DatedDatum.h"

//using namespace H5;

//
// CDatedDatum
//

CDatedDatum::CDatedDatum(void) :
  m_dt( not_a_date_time ) { 
}

  CDatedDatum::CDatedDatum(const boost::posix_time::ptime &dt):
  m_dt( dt ) {
}

CDatedDatum::CDatedDatum(const CDatedDatum &datum) :
  m_dt( datum.m_dt ) {
}

CDatedDatum::CDatedDatum(const std::string &dt) {
  //m_dt = boost::posix_time::time_from_string(dt);
  assert( dt.length() == 19 );
  const char *s = dt.c_str();
  m_dt = ptime( 
    boost::gregorian::date( atoi( s ), atoi( s + 5 ), atoi( s + 8 ) ), 
    boost::posix_time::time_duration( atoi( s + 11 ), atoi( s + 14 ), atoi( s + 17 ) ) );
}

CDatedDatum::~CDatedDatum(void) {
}

H5::CompType *CDatedDatum::DefineDataType( H5::CompType *pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( CDatedDatum ) );
  pComp->insertMember( "DateTime", HOFFSET( CDatedDatum, m_dt ), H5::PredType::NATIVE_LLONG );
  return pComp;
}

//
// CQuote
//

CQuote::CQuote(void): CDatedDatum(), m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 ) {
}

CQuote::CQuote(const ptime &dt): CDatedDatum(dt), m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 ) {
}

CQuote::CQuote(const CQuote &quote): 
  CDatedDatum( quote.m_dt ), 
    m_dblBid( quote.m_dblBid ), m_dblAsk( quote.m_dblAsk ), 
    m_nBidSize( quote.m_nBidSize ), m_nAskSize( quote.m_nAskSize )
{
}

CQuote::CQuote( const ptime &dt, price_t dblBid, bidsize_t nBidSize, price_t dblAsk, asksize_t nAskSize ) :
CDatedDatum( dt ), 
    m_dblBid( dblBid ), m_dblAsk( dblAsk ), 
    m_nBidSize( nBidSize ), m_nAskSize( nAskSize ) {
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

H5::CompType *CQuote::DefineDataType( H5::CompType *pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( CQuote ) );
  CDatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Bid", HOFFSET( CQuote, m_dblBid ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Ask", HOFFSET( CQuote, m_dblAsk ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "BidSize", HOFFSET( CQuote, m_nBidSize ), H5::PredType::NATIVE_INT );
  pComp->insertMember( "AskSize", HOFFSET( CQuote, m_nAskSize ), H5::PredType::NATIVE_INT );
  return pComp;
}

//
// CTrade
//

CTrade::CTrade(void): CDatedDatum(), m_dblTrade( 0 ), m_nTradeSize( 0 ) {
}

CTrade::CTrade(const ptime &dt): CDatedDatum(dt), m_dblTrade( 0 ), m_nTradeSize( 0 ) {
}

CTrade::CTrade(const CTrade &trade): CDatedDatum( trade.m_dt ), 
    m_dblTrade( trade.m_dblTrade ), m_nTradeSize( trade.m_nTradeSize ) {
}

CTrade::CTrade( const ptime &dt, price_t dblTrade, volume_t nTradeSize ) :
CDatedDatum( dt ), m_dblTrade( dblTrade ), m_nTradeSize( nTradeSize ) {
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

H5::CompType *CTrade::DefineDataType( H5::CompType *pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( CTrade ) );
  CDatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Trade", HOFFSET( CTrade, m_dblTrade ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "TradeSize", HOFFSET( CTrade, m_nTradeSize ), H5::PredType::NATIVE_INT );
  return pComp;
}

// CBar

CBar::CBar(void): CDatedDatum(), m_dblOpen( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), m_dblClose( 0 ), m_nVolume( 0 ) {
}

CBar::CBar( const ptime &dt): CDatedDatum(dt), m_dblOpen( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), m_dblClose( 0 ), m_nVolume( 0 ) {
}

CBar::CBar(const CBar &bar): CDatedDatum( bar.m_dt ), 
  m_dblOpen( bar.m_dblOpen ), m_dblHigh( bar.m_dblHigh ), m_dblLow( bar.m_dblLow ), m_dblClose( bar.m_dblClose ), m_nVolume( bar.m_nVolume ) {
}

CBar::CBar(const boost::posix_time::ptime &dt, price_t dblOpen, price_t dblHigh, price_t dblLow, price_t dblClose, volume_t nVolume):
CDatedDatum( dt ), 
  m_dblOpen( dblOpen ), m_dblHigh( dblHigh ), m_dblLow( dblLow ), m_dblClose( dblClose ), m_nVolume( nVolume ) {
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

H5::CompType *CBar::DefineDataType( H5::CompType *pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( CBar ) );
  CDatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Open", HOFFSET( CBar, m_dblOpen ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "High", HOFFSET( CBar, m_dblHigh ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Low", HOFFSET( CBar, m_dblLow ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Close", HOFFSET( CBar, m_dblClose ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Volume", HOFFSET( CBar, m_nVolume ), H5::PredType::NATIVE_INT );
  return pComp;
}

//
// CMarketDepth
//

CMarketDepth::CMarketDepth(): CDatedDatum(), m_eSide( None ), m_nShares( 0 ), m_dblPrice( 0 ) {
  //m_szMMID[ 0 ] = 0;
}

CMarketDepth::CMarketDepth(const ptime &dt): CDatedDatum(dt), m_eSide( None ), m_nShares( 0 ), m_dblPrice( 0 ) {
  //m_szMMID[ 0 ] = 0;
}

CMarketDepth::CMarketDepth(const CMarketDepth &md): CDatedDatum( md.m_dt ), 
  m_eSide( md.m_eSide ), m_nShares( md.m_nShares ), m_dblPrice( md.m_dblPrice ), m_uMMID( md.m_uMMID ) {
  //strcpy_s( m_szMMID, 10, md.m_szMMID );
}

CMarketDepth::CMarketDepth(const boost::posix_time::ptime &dt, char chSide, volume_t nShares, price_t dblPrice, MMID_t mmid):
    CDatedDatum( dt ), m_eSide( None ), m_nShares( nShares ), m_dblPrice( dblPrice ), m_uMMID( mmid ) {
  if ( 'S' == chSide ) m_eSide = Ask;
  if ( 'B' == chSide ) m_eSide = Bid;
  //copymmid( m_szMMID, mmid );
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
  strcpy_s( m_uMMID.rch, 5, mmid.c_str());
  //m_mmid = mmid.c_str();
}


CMarketDepth::~CMarketDepth() {
  CDatedDatum::~CDatedDatum();
}

H5::CompType *CMarketDepth::DefineDataType( H5::CompType *pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( CMarketDepth ) );
  CDatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Shares", HOFFSET( CMarketDepth, m_nShares ), H5::PredType::NATIVE_LONG );
  pComp->insertMember( "Price", HOFFSET( CMarketDepth, m_dblPrice ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Side", HOFFSET( CMarketDepth, m_eSide ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID0", HOFFSET( CMarketDepth, m_uMMID.rch[0] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID1", HOFFSET( CMarketDepth, m_uMMID.rch[1] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID2", HOFFSET( CMarketDepth, m_uMMID.rch[2] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID3", HOFFSET( CMarketDepth, m_uMMID.rch[3] ), H5::PredType::NATIVE_CHAR );  
  return pComp; 
}

