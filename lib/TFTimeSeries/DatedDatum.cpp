/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <assert.h>

#include "DatedDatum.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

//
// DatedDatum
//

DatedDatum::DatedDatum()
: m_dt( not_a_date_time )
{}

DatedDatum::DatedDatum( const boost::posix_time::ptime dt )
: m_dt( dt )
{}

DatedDatum::DatedDatum( const DatedDatum& datum )
: m_dt( datum.m_dt )
{}

DatedDatum::DatedDatum(const std::string& dt) {
  //m_dt = boost::posix_time::time_from_string(dt);
  assert( dt.length() == 19 );
  const char* s = dt.c_str();
  m_dt = ptime( // convert to lexical_cast ?
    boost::gregorian::date( atoi( s ), atoi( s + 5 ), atoi( s + 8 ) ),
    boost::posix_time::time_duration( atoi( s + 11 ), atoi( s + 14 ), atoi( s + 17 ) ) );
}

DatedDatum::~DatedDatum() {
}

H5::CompType* DatedDatum::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( DatedDatum ) );
  pComp->insertMember( "DateTime", HOFFSET( DatedDatum, m_dt ), H5::PredType::NATIVE_LLONG );
  return pComp;
}

//
// Quote
//

Quote::Quote()
: DatedDatum(), m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 )
{}

Quote::Quote( const ptime dt )
: DatedDatum(dt), m_dblBid( 0 ), m_dblAsk( 0 ), m_nBidSize( 0 ), m_nAskSize( 0 )
{}

Quote::Quote( const Quote& quote )
: DatedDatum( quote.m_dt )
, m_dblBid( quote.m_dblBid ), m_dblAsk( quote.m_dblAsk )
, m_nBidSize( quote.m_nBidSize ), m_nAskSize( quote.m_nAskSize )
{}

Quote::Quote( const ptime dt, price_t dblBid, bidsize_t nBidSize, price_t dblAsk, asksize_t nAskSize )
: DatedDatum( dt )
, m_dblBid( dblBid ), m_dblAsk( dblAsk )
, m_nBidSize( nBidSize ), m_nAskSize( nAskSize )
{}

Quote::Quote( const std::string& dt, const std::string& bid,
              const std::string& bidsize, const std::string& ask, const std::string& asksize )
: DatedDatum( dt )
{
  char* stopchar;
  m_dblBid = strtod( bid.c_str(), &stopchar );
  m_nBidSize = atoi( bidsize.c_str() );
  m_dblAsk = strtod( ask.c_str(), &stopchar );
  m_nAskSize = atoi( asksize.c_str() );
}

Quote::~Quote() {
}

bool Quote::IsValid() const {
  bool bOk( true );
  //bOk &= ( ( 0 == m_nBidSize ) && ( 0.0 == m_dblBid ) ); // NOTE: some options are zero bid
  //bOk &= ( ( 0 != m_nAskSize ) && ( 0.0 != m_dblAsk ) );
  // TODO: what other tests?
  return bOk;
};

bool Quote::IsNonZero() const {
  bool bOk( false );
  bOk |= ( 0.0 < m_dblAsk );
  bOk |= ( 0.0 < m_dblBid ); // can be zero for far out of the money options
  // TODO: maybe make flag to accept for OTM options?
  //bOk &= ( ( 0 == m_nBidSize ) && ( 0.0 == m_dblBid ) ); // NOTE: some options are zero bid
  //bOk &= ( ( 0 != m_nAskSize ) && ( 0.0 != m_dblAsk ) );
  // TODO: what other tests?
  return bOk;
};

H5::CompType* Quote::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Quote ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Bid",     HOFFSET( Quote, m_dblBid ),   H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Ask",     HOFFSET( Quote, m_dblAsk ),   H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "BidSize", HOFFSET( Quote, m_nBidSize ), H5::PredType::NATIVE_INT );
  pComp->insertMember( "AskSize", HOFFSET( Quote, m_nAskSize ), H5::PredType::NATIVE_INT );
  return pComp;
}

//
// Trade
//

Trade::Trade()
: DatedDatum(), m_dblPrice( 0 ), m_nTradeSize( 0 )
{}

Trade::Trade( const ptime dt )
: DatedDatum(dt), m_dblPrice( 0 ), m_nTradeSize( 0 )
{}

Trade::Trade( const Trade& trade )
: DatedDatum( trade.m_dt ), m_dblPrice( trade.m_dblPrice ), m_nTradeSize( trade.m_nTradeSize )
{}

Trade::Trade( const ptime dt, price_t dblTrade, volume_t nTradeSize )
: DatedDatum( dt ), m_dblPrice( dblTrade ), m_nTradeSize( nTradeSize )
{}

Trade::Trade( const std::string& dt, const std::string& trade, const std::string& size )
: DatedDatum( dt )
{
  char* stopchar;
  m_dblPrice = strtod( trade.c_str(), &stopchar );
  m_nTradeSize = atoi( size.c_str() );
}

Trade::~Trade() {
}

H5::CompType* Trade::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Trade ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Price", HOFFSET( Trade, m_dblPrice ),   H5::PredType::NATIVE_DOUBLE );  // 2012/07/15 kills all previous data files
  pComp->insertMember( "Size",  HOFFSET( Trade, m_nTradeSize ), H5::PredType::NATIVE_INT );
  return pComp;
}

// Bar

Bar::Bar()
: DatedDatum(), m_dblOpen( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), m_dblClose( 0 ), m_nVolume( 0 )
{}

Bar::Bar( const ptime dt)
: DatedDatum( dt ), m_dblOpen( 0 ), m_dblHigh( 0 ), m_dblLow( 0 ), m_dblClose( 0 ), m_nVolume( 0 )
{}

Bar::Bar(const Bar& bar): DatedDatum( bar.m_dt ),
  m_dblOpen( bar.m_dblOpen ), m_dblHigh( bar.m_dblHigh ), m_dblLow( bar.m_dblLow ), m_dblClose( bar.m_dblClose ), m_nVolume( bar.m_nVolume ) {
}

Bar::Bar( const boost::posix_time::ptime dt, price_t dblOpen, price_t dblHigh, price_t dblLow, price_t dblClose, volume_t nVolume )
: DatedDatum( dt )
, m_dblOpen( dblOpen ), m_dblHigh( dblHigh ), m_dblLow( dblLow ), m_dblClose( dblClose ), m_nVolume( nVolume )
{}

Bar::Bar( const std::string& dt, const std::string& open,
          const std::string& high, const std::string& low,
          const std::string& close, const std::string& volume)
: DatedDatum( dt )
{
  char* stopchar;
  m_dblOpen = strtod( open.c_str(), &stopchar );
  m_dblHigh = strtod( high.c_str(), &stopchar );
  m_dblLow = strtod( low.c_str(), &stopchar );
  m_dblClose = strtod( close.c_str(), &stopchar );
  m_nVolume = atoi( volume.c_str() );
}

void Bar::Accumulate( const Bar& bar ) {
  m_dblHigh = std::max<ou::tf::Price::price_t>( m_dblHigh, bar.High() );
  m_dblLow = std::min<ou::tf::Price::price_t>( m_dblLow, bar.Low() );
  m_dblClose = bar.Close();
  m_nVolume += bar.Volume();
}

Bar::~Bar() {
}

H5::CompType* Bar::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Bar ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Open",   HOFFSET( Bar, m_dblOpen ),  H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "High",   HOFFSET( Bar, m_dblHigh ),  H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Low",    HOFFSET( Bar, m_dblLow ),   H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Close",  HOFFSET( Bar, m_dblClose ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Volume", HOFFSET( Bar, m_nVolume ),  H5::PredType::NATIVE_INT );
  return pComp;
}

//
// Depth (common structure to DepthByMM, DepthByOrder)
//

Depth::Depth()
: DatedDatum(), m_dblPrice {}, m_nShares {}, m_chMsgType( '0' ), m_chSide( ' ' )
{}

Depth::Depth( const dt_t dt )
: DatedDatum( dt ), m_dblPrice {}, m_nShares {}, m_chMsgType( '0' ), m_chSide( ' ' )
{}

Depth::Depth( const Depth& depth )
: DatedDatum( depth.m_dt )
, m_dblPrice( depth.m_dblPrice ), m_nShares( depth.m_nShares)
, m_chMsgType( depth.m_chMsgType ), m_chSide( depth.m_chSide )
{}

Depth::Depth( const dt_t dt, price_t dblPrice, quotesize_t nShares )
: DatedDatum( dt ), m_dblPrice( dblPrice ), m_nShares( nShares ), m_chMsgType( '0' ), m_chSide( '0' )
{}

Depth::Depth( const dt_t dt, char chSide, price_t dblPrice, quotesize_t nShares )
: DatedDatum( dt ), m_dblPrice( dblPrice ), m_nShares( nShares ), m_chMsgType( '0' ), m_chSide( chSide )
{}

Depth::Depth( const dt_t dt, char chMsgType, char chSide, price_t dblPrice, quotesize_t nShares )
: DatedDatum( dt ), m_dblPrice( dblPrice ), m_nShares( nShares ), m_chMsgType( chMsgType ), m_chSide( chSide )
{}

Depth::~Depth() {}

H5::CompType* Depth::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Depth ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "MsgType",  HOFFSET( Depth, m_chMsgType ),    H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "Side",     HOFFSET( Depth, m_chSide ),       H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "Price",    HOFFSET( Depth, m_dblPrice ),     H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Shares",   HOFFSET( Depth, m_nShares ),      H5::PredType::NATIVE_LONG );
  return pComp;
}

//
// DepthByMM
//

DepthByMM::DepthByMM(): Depth() {}

DepthByMM::DepthByMM( const ptime dt ): Depth( dt ) {}

DepthByMM::DepthByMM( const DepthByMM& depth )
: Depth( depth ), m_uMMID( depth.m_uMMID )
{}

DepthByMM::DepthByMM( const boost::posix_time::ptime dt, char chMsgType, char chSide, volume_t nShares, price_t dblPrice, char* pch )
: Depth( dt, dblPrice, nShares, chMsgType, chSide ), m_uMMID( pch )
{}

DepthByMM::DepthByMM( const boost::posix_time::ptime dt, char chMsgType, char chSide, volume_t nShares, price_t dblPrice, MMID_t mmid )
: Depth( dt, chMsgType, chSide, dblPrice, nShares ), m_uMMID( mmid )
{}

DepthByMM::~DepthByMM() {}

H5::CompType* DepthByMM::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( DepthByMM ) );
  Depth::DefineDataType( pComp );
  pComp->insertMember( "MMID0",    HOFFSET( DepthByMM, m_uMMID.rch[0] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID1",    HOFFSET( DepthByMM, m_uMMID.rch[1] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID2",    HOFFSET( DepthByMM, m_uMMID.rch[2] ), H5::PredType::NATIVE_CHAR );
  pComp->insertMember( "MMID3",    HOFFSET( DepthByMM, m_uMMID.rch[3] ), H5::PredType::NATIVE_CHAR );
  return pComp;
}

//
// DepthByOrder
//

DepthByOrder::DepthByOrder()
: Depth()
, m_dtMarket( boost::posix_time::not_a_date_time )
, m_nOrderID {}, m_nPriority {}
{}

DepthByOrder::DepthByOrder( const ptime dt )
: Depth( dt )
, m_dtMarket( boost::posix_time::not_a_date_time )
, m_nOrderID {}, m_nPriority {}
{}

DepthByOrder::DepthByOrder( const DepthByOrder& depth )
: Depth( depth )
, m_dtMarket( depth.m_dtMarket )
, m_nOrderID( depth.m_nOrderID ), m_nPriority( depth.m_nPriority )
{}

DepthByOrder::DepthByOrder( const dt_t dt, const dt_t dtMarket, idorder_t nOrderID, uint64_t nPriority, char chMsgType, char chSide, price_t dblPrice, volume_t nShares)
: Depth( dt, chMsgType, chSide, dblPrice, nShares )
, m_dtMarket( dtMarket )
, m_nOrderID( nOrderID ), m_nPriority( nPriority )
{}

DepthByOrder::~DepthByOrder() {}

H5::CompType* DepthByOrder::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( DepthByOrder ) );
  Depth::DefineDataType( pComp );
  pComp->insertMember( "MarketDT", HOFFSET( DepthByOrder, m_dtMarket ),   H5::PredType::NATIVE_LLONG );
  pComp->insertMember( "OrderId",  HOFFSET( DepthByOrder, m_nOrderID ),   H5::PredType::NATIVE_UINT64 );
  pComp->insertMember( "Priority", HOFFSET( DepthByOrder, m_nPriority ),  H5::PredType::NATIVE_UINT64 );
  return pComp;
}

//
// Greek
//

Greek::Greek()
: DatedDatum(), m_dblImpliedVolatility( 0 ), m_dblDelta( 0 ), m_dblGamma( 0 ), m_dblTheta( 0 ), m_dblVega( 0 ), m_dblRho( 0 )
{}

Greek::Greek( const ptime dt )
: DatedDatum(dt), m_dblImpliedVolatility( 0 ), m_dblDelta( 0 ), m_dblGamma( 0 ), m_dblTheta( 0 ), m_dblVega( 0 ), m_dblRho( 0 )
{}

Greek::Greek( const Greek& greeks )
: DatedDatum( greeks.m_dt )
, m_dblImpliedVolatility( greeks.m_dblImpliedVolatility )
, m_dblDelta( greeks.m_dblDelta ), m_dblGamma( greeks.m_dblGamma ), m_dblTheta( greeks.m_dblTheta ), m_dblVega( greeks.m_dblVega ), m_dblRho( greeks.m_dblRho )
{}

Greek::Greek( const ptime dt, double dblImpliedVolatility, const greeks_t& greeks )
: DatedDatum( dt )
, m_dblImpliedVolatility( dblImpliedVolatility )
, m_dblDelta( greeks.delta ), m_dblGamma( greeks.gamma ), m_dblTheta( greeks.theta ), m_dblVega( greeks.vega ), m_dblRho( greeks.rho )
{}

Greek::Greek( const boost::posix_time::ptime dt, double dblImpliedVolatility, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho )
: DatedDatum( dt )
, m_dblImpliedVolatility( dblImpliedVolatility )
, m_dblDelta( dblDelta ), m_dblGamma( dblGamma ), m_dblTheta( dblTheta ), m_dblVega( dblVega ), m_dblRho( dblRho )
{}

Greek::~Greek() {
}

H5::CompType* Greek::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Greek ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "ImplVol", HOFFSET( Greek, m_dblImpliedVolatility ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Delta",   HOFFSET( Greek, m_dblDelta ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Gamma",   HOFFSET( Greek, m_dblGamma ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Theta",   HOFFSET( Greek, m_dblTheta ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Vega",    HOFFSET( Greek, m_dblVega ),  H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "Rho",     HOFFSET( Greek, m_dblRho ),   H5::PredType::NATIVE_DOUBLE );
  return pComp;
}

//
// Price
//

Price::Price()
: DatedDatum(), m_dblPrice( 0 )
{}

Price::Price( const ptime dt )
: DatedDatum( dt ), m_dblPrice( 0 )
{}

Price::Price( const Price& price)
: DatedDatum( price.m_dt ), m_dblPrice( price.m_dblPrice )
{}

Price::Price( const ptime dt, price_t dblPrice )
: DatedDatum( dt ), m_dblPrice( dblPrice )
{}

Price::Price( const std::string& dt, const std::string& price ) :
DatedDatum( dt ) {
  char* stopchar;
  m_dblPrice = strtod( price.c_str(), &stopchar );
}

Price::~Price() {
}

H5::CompType* Price::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( Price ) );
  DatedDatum::DefineDataType( pComp );
  pComp->insertMember( "Price", HOFFSET( Price, m_dblPrice ), H5::PredType::NATIVE_DOUBLE );
  return pComp;
}

//
// PriceIV
//

PriceIV::PriceIV()
: Price(), m_dblIVCall( 0.0 ), m_dblIVPut( 0.0 ) {};

PriceIV::PriceIV( const ptime dt )
: Price( dt ),  m_dblIVCall( 0.0 ), m_dblIVPut( 0.0 )
{}

PriceIV::PriceIV( const PriceIV& rhs )
: Price( rhs.m_dt, rhs.Value() ), m_dblIVCall( rhs.m_dblIVCall ), m_dblIVPut( rhs.m_dblIVPut )
{}

PriceIV::PriceIV( const ptime dtSampled, price_t dblPrice, double dblIVCall, double dblIVPut )
: Price( dtSampled, dblPrice ), m_dblIVCall( dblIVCall ), m_dblIVPut( dblIVPut )
{}

H5::CompType* PriceIV::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( PriceIV ) );
  Price::DefineDataType( pComp ); // include inherited structures
  pComp->insertMember( "CallIV", HOFFSET( PriceIV, m_dblIVCall ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "PutIV",  HOFFSET( PriceIV, m_dblIVPut ),  H5::PredType::NATIVE_DOUBLE );
  return pComp;
}

//
// PriceIVExpiry
//

PriceIVExpiry::PriceIVExpiry()
: Price(), m_dtExpiry( not_a_date_time), m_dblIVCall( 0.0 ), m_dblIVPut( 0.0 ) {}

PriceIVExpiry::PriceIVExpiry( const ptime dt )
: Price( dt ), m_dtExpiry( not_a_date_time), m_dblIVCall( 0.0 ), m_dblIVPut( 0.0 )
{}

PriceIVExpiry::PriceIVExpiry( const PriceIVExpiry& rhs )
: Price( rhs.m_dt, rhs.Value() ), m_dtExpiry( rhs.m_dtExpiry ), m_dblIVCall( rhs.m_dblIVCall ), m_dblIVPut( rhs.m_dblIVPut )
{}

PriceIVExpiry::PriceIVExpiry( const ptime dtSampled, price_t dblPrice, const ptime& dtExpiry, double dblIVCall, double dblIVPut )
: Price( dtSampled, dblPrice ), m_dtExpiry( dtExpiry ), m_dblIVCall( dblIVCall ), m_dblIVPut( dblIVPut )
{}

H5::CompType* PriceIVExpiry::DefineDataType( H5::CompType* pComp ) {
  if ( NULL == pComp ) pComp = new H5::CompType( sizeof( PriceIVExpiry ) );
  Price::DefineDataType( pComp ); // include inherited structures
  pComp->insertMember( "Expiry", HOFFSET( PriceIVExpiry, m_dtExpiry ),  H5::PredType::NATIVE_LLONG );
  pComp->insertMember( "CallIV", HOFFSET( PriceIVExpiry, m_dblIVCall ), H5::PredType::NATIVE_DOUBLE );
  pComp->insertMember( "PutIV",  HOFFSET( PriceIVExpiry, m_dblIVPut ),  H5::PredType::NATIVE_DOUBLE );
  return pComp;
}

} // namespace tf
} // namespace ou
