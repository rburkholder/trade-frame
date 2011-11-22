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

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <HDF5/H5Cpp.h>

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

  // http://www.boost.org/doc/html/date_time/posix_time.html#date_time.posix_time.ptime_class
  //ptime m_dt(boost::date_time::special_values::not_a_date_time);

namespace ou { // One Unified
namespace tf { // TradeFrame

class CDatedDatum {
public:

  typedef long volume_t;
  typedef volume_t tradesize_t;
  typedef volume_t quotesize_t;

  typedef double price_t;

  CDatedDatum( void );
  CDatedDatum( const ptime &dt );
  CDatedDatum( const CDatedDatum &datum );
  CDatedDatum( const std::string &dt ); // YYYY-MM-DD HH:MM:SS
  virtual ~CDatedDatum( void );

  bool IsNull( void ) const { return m_dt.is_not_a_date_time(); };

  bool operator<( const CDatedDatum &datum ) const { return m_dt < datum.m_dt; };
  bool operator<=( const CDatedDatum& datum ) const { return m_dt <= datum.m_dt; };
  bool operator>( const CDatedDatum& datum ) const { return m_dt > datum.m_dt; };
  bool operator>=( const CDatedDatum& datum ) const { return m_dt >= datum.m_dt; };
  bool operator==( const CDatedDatum& datum ) const { return m_dt == datum.m_dt; };
  bool operator!=( const CDatedDatum& datum ) const { return m_dt != datum.m_dt; };

  const ptime& DateTime( void ) const { return m_dt; };
  void DateTime( const ptime& dt ) { m_dt = dt; };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );  // create new one if null

protected:
  ptime m_dt;
private:
};

//
// CQuote
//

class CQuote: public CDatedDatum {
public:

  typedef quotesize_t bidsize_t;
  typedef quotesize_t asksize_t;

  CQuote( void );
  CQuote( const ptime &dt );
  CQuote( const CQuote &quote );
  CQuote( const ptime &dt, double dblBid, bidsize_t nBidSize, double dblAsk, asksize_t nAskSize );
  CQuote( const std::string &dt, 
    const std::string &bid, const std::string &bidsize, 
    const std::string &ask, const std::string &asksize );
  virtual ~CQuote( void );

  price_t Bid( void ) const { return m_dblBid; };
  price_t Ask( void ) const { return m_dblAsk; };
  bidsize_t BidSize( void ) const { return m_nBidSize; };
  asksize_t AskSize( void ) const { return m_nAskSize; };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );

protected:
private:
  price_t m_dblBid;
  price_t m_dblAsk;
  bidsize_t m_nBidSize;
  asksize_t m_nAskSize;
};

//
// CTrade
//

class CTrade: public CDatedDatum {
public:

  CTrade( void );
  CTrade( const ptime &dt );
  CTrade( const CTrade &trade );
  CTrade( const ptime &dt, price_t dblTrade, volume_t nTradeSize );
  CTrade( const std::string &dt, const std::string &trade, const std::string &size );
  ~CTrade( void );

  price_t Trade( void ) const { return m_dblTrade; };
  volume_t Volume( void ) const { return m_nTradeSize; };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );

protected:
private:
  price_t m_dblTrade;
  volume_t m_nTradeSize;
};

//
// CBar
//

class CBar: public CDatedDatum {
public:

  CBar( void );
  CBar( const ptime& dt );
  CBar( const CBar& bar );
  CBar( const ptime& dt, price_t dblOpen, price_t dblHigh, price_t dblLow, price_t dblClose, volume_t nVolume );
  CBar( const std::string& dt, const std::string& open, const std::string& high, 
    const std::string& low, const std::string& close, const std::string& volume );
  ~CBar (void );

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

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );

protected:
private:
  double m_dblOpen;
  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;
  volume_t m_nVolume;
};

// 
// CMarketDepth
//

class CMarketDepth: public CDatedDatum {
public:

  typedef unsigned long MMID_t;
  enum ESide : char { Bid, Ask, None };

  CMarketDepth( void );
  CMarketDepth( const ptime &dt );
  CMarketDepth( const CMarketDepth &md );
  CMarketDepth( const ptime &dt, char chSide, quotesize_t nShares, price_t dblPrice, MMID_t mmid );
  CMarketDepth( const std::string &dt, char chSide, const std::string &shares, 
    const std::string &price, const std::string &mmid );
   ~CMarketDepth(void);

  MMID_t MMID( void ) const { return m_uMMID.mmid; };
  price_t Price( void ) const { return m_dblPrice; };
  volume_t Volume( void ) const { return m_nShares; };

  ESide m_eSide; 
  const char& MMIDStr( void ) const { return *m_uMMID.rch; };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );

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
// CGreek
//

class CGreek: public CDatedDatum {
public:

  CGreek( void );
  CGreek( const ptime &dt );
  CGreek( const CGreek& greeks );
  CGreek( const ptime& dt, double dblImpliedVolatility, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho );
  ~CGreek( void );

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
// CPrice
//

class CPrice: public CDatedDatum {
public:

  CPrice( void );
  CPrice( const ptime &dt );
  CPrice( const CPrice &price );
  CPrice( const ptime &dt, price_t dblPrice );
  CPrice( const std::string &dt, const std::string &price );
  ~CPrice( void );

  price_t Price( void ) const { return m_dblPrice; };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );

protected:
private:
  price_t m_dblPrice;
};

} // namespace tf
} // namespace ou

