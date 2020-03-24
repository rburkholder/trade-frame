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

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <math.h>

#include <hdf5/H5Cpp.h>

#include <boost/cstdint.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;

  // http://www.boost.org/doc/html/date_time/posix_time.html#date_time.posix_time.ptime_class
  //ptime m_dt(boost::date_time::special_values::not_a_date_time);

namespace ou { // One Unified
namespace tf { // TradeFrame

class DatedDatum {
public:

  typedef unsigned long volume_t;
  typedef volume_t tradesize_t;
  typedef volume_t quotesize_t;
  typedef boost::posix_time::ptime dt_t;

  typedef double price_t;

  DatedDatum( void );
  DatedDatum( const ptime& dt );
  DatedDatum( const DatedDatum& datum );
  DatedDatum( const std::string& dt ); // YYYY-MM-DD HH:MM:SS
  virtual ~DatedDatum( void );

  bool IsNull( void ) const { return m_dt.is_not_a_date_time(); };

  bool operator<( const DatedDatum &datum ) const { return m_dt < datum.m_dt; };
  bool operator<=( const DatedDatum& datum ) const { return m_dt <= datum.m_dt; };
  bool operator>( const DatedDatum& datum ) const { return m_dt > datum.m_dt; };
  bool operator>=( const DatedDatum& datum ) const { return m_dt >= datum.m_dt; };
  bool operator==( const DatedDatum& datum ) const { return m_dt == datum.m_dt; };
  bool operator!=( const DatedDatum& datum ) const { return m_dt != datum.m_dt; };

  const ptime& DateTime( void ) const { return m_dt; };
  void DateTime( const ptime& dt ) { m_dt = dt; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );  // create new one if null
  static boost::uint64_t Signature( void ) { return 9; };

protected:
  dt_t m_dt;
private:
};

//
// Quote
//

class Quote: public DatedDatum {
public:

  typedef quotesize_t bidsize_t;
  typedef quotesize_t asksize_t;

  Quote( void );
  Quote( const ptime& dt );
  Quote( const Quote& quote );
  Quote( const ptime& dt, double dblBid, bidsize_t nBidSize, double dblAsk, asksize_t nAskSize );
  Quote( const std::string& dt,
    const std::string& bid, const std::string& bidsize,
    const std::string& ask, const std::string& asksize );
  virtual ~Quote( void );

  price_t Bid( void ) const { return m_dblBid; };
  price_t Ask( void ) const { return m_dblAsk; };
  bidsize_t BidSize( void ) const { return m_nBidSize; };
  asksize_t AskSize( void ) const { return m_nAskSize; };

  bool IsValid() const;
  bool IsNonZero() const;
  bool SameBidAsk( const Quote& rhs ) const { return ( m_dblBid == rhs.m_dblBid ) && ( m_dblAsk == rhs.m_dblAsk ) ; }
  bool CrossedQuote( void ) const { return ( m_dblBid >= m_dblAsk ); };
  price_t Midpoint( void ) const { return ( m_dblBid + m_dblAsk ) / 2.0; };
  price_t Spread( void ) const { return m_dblAsk - m_dblBid; };
  price_t GeometricMidPoint( void ) const { return std::sqrt( m_dblBid * m_dblAsk ); };  // pg 53, Intro HF Finance
  price_t LogarithmicMidPointA( void ) const { return ( std::log( m_dblBid ) + std::log( m_dblAsk ) ) / 2.0; }; // eq 3.4 pg 39, Intro HF Finance
  price_t LogarithmicMidPointB( void ) const { return std::log( std::sqrt( m_dblBid * m_dblAsk ) ); }; // eq 3.4 pg 39, Intro HF Finance

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 10000 + 1133; };

protected:
private:
  price_t m_dblBid;
  price_t m_dblAsk;
  bidsize_t m_nBidSize;
  asksize_t m_nAskSize;
};

//
// Trade
//

class Trade: public DatedDatum {
public:

  Trade( void );
  Trade( const ptime &dt );
  Trade( const Trade &trade );
  Trade( const ptime& dt, price_t dblTrade, volume_t nTradeSize );
  Trade( const std::string& dt, const std::string& trade, const std::string& size );
  ~Trade( void );

  price_t Price( void ) const { return m_dblPrice; };  // 20120715 was Trace, may cause problems in other areas.
  volume_t Volume( void ) const { return m_nTradeSize; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 100 + 13; };

protected:
private:
  price_t m_dblPrice;
  volume_t m_nTradeSize;
};

//
// Bar
//

class Bar: public DatedDatum {
public:

  Bar( void );
  Bar( const ptime& dt );
  Bar( const Bar& bar );
  Bar( const ptime& dt, price_t dblOpen, price_t dblHigh, price_t dblLow, price_t dblClose, volume_t nVolume );
  Bar( const std::string& dt, const std::string& open, const std::string& high,
    const std::string& low, const std::string& close, const std::string& volume );
  ~Bar (void );

  price_t Open( void ) const { return m_dblOpen; };
  price_t High( void ) const { return m_dblHigh; };
  price_t Low( void ) const { return m_dblLow; };
  price_t Close( void ) const { return m_dblClose; };
  volume_t Volume( void ) const { return m_nVolume; };

  void Open( price_t price ) { m_dblOpen = price; };
  void High( price_t price ) { m_dblHigh = price; };
  void Low( price_t price ) { m_dblLow = price; };
  void Close( price_t price ) { m_dblClose = price; };
  void Volume( volume_t vol ) { m_nVolume = vol; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 100000 + 11113; };

protected:
private:
  double m_dblOpen;
  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;
  volume_t m_nVolume;
};

//
// MarketDepth
//

class MarketDepth: public DatedDatum {
public:

  typedef unsigned long MMID_t;
  enum ESide : char { Bid, Ask, None };

  MarketDepth( void );
  MarketDepth( const ptime& dt );
  MarketDepth( const MarketDepth& md );
  MarketDepth( const ptime& dt, char chSide, quotesize_t nShares, price_t dblPrice, MMID_t mmid );
  MarketDepth( const std::string& dt, char chSide, const std::string& shares,
    const std::string& price, const std::string& mmid );
   ~MarketDepth(void);

  MMID_t MMID( void ) const { return m_uMMID.mmid; };
  price_t Price( void ) const { return m_dblPrice; };
  volume_t Volume( void ) const { return m_nShares; };

  ESide m_eSide;
  const char& MMIDStr( void ) const { return *m_uMMID.rch; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 10000000 + 3188888; };

protected:
  union unionMMID {
    MMID_t mmid;
    char rch[5];
    unionMMID( void ) { mmid = 0; rch[4] = 0; };
    unionMMID( MMID_t id ) : mmid( id ) { rch[4] = 0; };
    unionMMID( const unionMMID &u ) : mmid( u.mmid ) { rch[4] = 0; };
  } m_uMMID;
private:
  volume_t m_nShares;
  price_t m_dblPrice;
};

//
// Greek
//

class Greek: public DatedDatum {
public:

  struct greeks_t {
    double delta;
    double gamma;
    double theta;
    double vega;
    double rho;
    greeks_t( void ) : delta( 0.0 ), gamma( 0.0 ), theta( 0.0 ), vega( 0.0 ), rho( 0.0 ) {};
  };

  Greek( void );
  Greek( const ptime& dt );
  Greek( const Greek& greeks );
  Greek( const ptime& dt, double dblImpliedVolatility, const greeks_t& greeks );
  Greek( const ptime& dt, double dblImpliedVolatility, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho );
  ~Greek( void );

  double ImpliedVolatility( void ) const { return m_dblImpliedVolatility; };
  double Delta( void ) const { return m_dblDelta; };
  double Gamma( void ) const { return m_dblGamma; };
  double Theta( void ) const { return m_dblTheta; };
  double Vega( void ) const { return m_dblVega; };
  double Rho( void ) const { return m_dblRho; };

  void ImpliedVolatility( double dblImpliedVolatility ) { m_dblImpliedVolatility = dblImpliedVolatility; };
  void Delta( double dblDelta ) { m_dblDelta = dblDelta; };
  void Gamma( double dblGamma ) { m_dblGamma = dblGamma; };
  void Theta( double dblTheta ) { m_dblTheta = dblTheta; };
  void Vega( double dblVega ) { m_dblVega = dblVega; };
  void Rho( double dblRho ) { m_dblRho = dblRho; };

  void Assign( const ptime& dt, double dblImplVol, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho ) {
    m_dt = dt;
    m_dblImpliedVolatility = dblImplVol;
    m_dblDelta = dblDelta;
    m_dblGamma =  dblGamma;
    m_dblTheta = dblTheta;
    m_dblVega = dblVega;
    m_dblRho = dblRho;
  };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 1000000 + 111111; };

protected:

private:
  double m_dblImpliedVolatility;
  double m_dblDelta;  // sensitivity to underlying's price changes
  double m_dblGamma;  // measure of delta's sensitivity to underlying's price changes
  double m_dblTheta;  // measure of option value's sensitivity to volatility
  double m_dblVega;   // measure of options value's sensivity to passage of time
  double m_dblRho;    // measure of option value's sensivity to interest rates

};

//
// Price
//

class Price: public DatedDatum {
public:

  Price( void );
  Price( const ptime& dt );
  Price( const Price& price );
  Price( const ptime& dt, price_t dblPrice );
  Price( const std::string &dt, const std::string& price );
  ~Price( void );

  price_t Value( void ) const { return m_dblPrice; };  // 20120715 was Price, is going to cause some problems in some code somewhere as is now class name

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return DatedDatum::Signature() * 10 + 1; };

protected:
private:
  price_t m_dblPrice;
};

//
// PriceIV
// pg 458 Option Pricing Formulas suggests this structure can be used with 12.2.4 Implied Forward Volatility
//
class PriceIV: public Price {
public:
  PriceIV( void );
  PriceIV( const ptime& dt );
  PriceIV( const PriceIV& rhs );
  PriceIV( const ptime& dtSampled, price_t dblPrice, double dblIVCall, double dblIVPut );
  ~PriceIV( void ) {};

  double IVCall( void ) const { return m_dblIVCall; };
  double IVPut( void ) const { return m_dblIVPut; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return Price::Signature() * 1000 + 412; };

protected:
private:
  double m_dblIVCall;
  double m_dblIVPut;
};


//
// PriceIVExpiry
// TODO: PriceIVExpiry inherits from PriceIV?
//

class PriceIVExpiry: public Price {
public:
  PriceIVExpiry( void );
  PriceIVExpiry( const ptime& dt );
  PriceIVExpiry( const PriceIVExpiry& rhs );
  PriceIVExpiry( const ptime& dtSampled, price_t dblPrice, const ptime& dtExpiry, double dblIVCall, double dblIVPut );
  ~PriceIVExpiry( void ) {};

  double IVCall( void ) const { return m_dblIVCall; };
  double IVPut( void ) const { return m_dblIVPut; };
  ptime Expiry( void ) const { return m_dtExpiry; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static boost::uint64_t Signature( void ) { return Price::Signature() * 1000 + 411; };

protected:
private:
  ptime m_dtExpiry;
  double m_dblIVCall;
  double m_dblIVPut;
};

} // namespace tf
} // namespace ou

