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

#include <hdf5/H5Cpp.h>

#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;

  // http://www.boost.org/doc/html/date_time/posix_time.html#date_time.posix_time.ptime_class
  //ptime m_dt(boost::date_time::special_values::not_a_date_time);

namespace ou { // One Unified
namespace tf { // TradeFrame

// TODO: convert to crtp, remove virtual destructor?

class DatedDatum {
public:

  using volume_t = unsigned long;
  using tradesize_t = volume_t;
  using quotesize_t = volume_t;
  using dt_t = boost::posix_time::ptime;

  using price_t = double;

  DatedDatum();
  DatedDatum( const dt_t dt );
  DatedDatum( const DatedDatum& datum );
  DatedDatum( const std::string& dt ); // YYYY-MM-DD HH:MM:SS
  virtual ~DatedDatum();

  inline bool IsNull() const { return m_dt.is_not_a_date_time(); }

  inline bool operator<( const DatedDatum &rhs ) const { return m_dt < rhs.m_dt; }
  inline bool operator<=( const DatedDatum& rhs ) const { return m_dt <= rhs.m_dt; }
  inline bool operator>( const DatedDatum& rhs ) const { return m_dt > rhs.m_dt; }
  inline bool operator>=( const DatedDatum& rhs ) const { return m_dt >= rhs.m_dt; }
  inline bool operator==( const DatedDatum& rhs ) const { return m_dt == rhs.m_dt; }
  inline bool operator!=( const DatedDatum& rhs ) const { return m_dt != rhs.m_dt; }

  inline const dt_t DateTime() const { return m_dt; }
  inline void DateTime( const dt_t dt ) { m_dt = dt; }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );  // create new one if null
  static uint64_t Signature() { return 9; } // DatedDatum

   // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

protected:
  dt_t m_dt;
private:
};

//
// Quote
//

class Quote: public DatedDatum {
public:

  using bidsize_t = quotesize_t;
  using asksize_t = quotesize_t;

  Quote();
  Quote( const dt_t dt );
  Quote( const Quote& quote );
  Quote( const dt_t dt, double dblBid, bidsize_t nBidSize, double dblAsk, asksize_t nAskSize );
  Quote( const std::string& dt,
    const std::string& bid, const std::string& bidsize,
    const std::string& ask, const std::string& asksize );
  virtual ~Quote();

  inline price_t Bid() const { return m_dblBid; }
  inline price_t Ask() const { return m_dblAsk; }
  inline bidsize_t BidSize() const { return m_nBidSize; }
  inline asksize_t AskSize() const { return m_nAskSize; }

  bool IsValid() const;
  bool IsNonZero() const;
  bool SameBidAsk( const Quote& rhs ) const { return ( m_dblBid == rhs.m_dblBid ) && ( m_dblAsk == rhs.m_dblAsk ); }
  bool CrossedQuote() const { return ( m_dblBid >= m_dblAsk ); }

  inline price_t Midpoint() const { return ( m_dblBid + m_dblAsk ) / 2.0; }
  inline price_t Spread() const { return m_dblAsk - m_dblBid; }
  price_t GeometricMidPoint() const { return std::sqrt( m_dblBid * m_dblAsk ); };  // pg 53, Intro HF Finance
  price_t LogarithmicMidPointA() const { return ( std::log( m_dblBid ) + std::log( m_dblAsk ) ) / 2.0; } // eq 3.4 pg 39, Intro HF Finance
  price_t LogarithmicMidPointB() const { return std::log( std::sqrt( m_dblBid * m_dblAsk ) ); } // eq 3.4 pg 39, Intro HF Finance

  double Imbalance() const;  // Positive (negative) imbalance indicates an order book that is heavier on the bid (ask) side

  bool LeeReady( double, double ) const; // false sell, true buy

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return DatedDatum::Signature() * 10000 + 1133; } // DatedDatum -> Quote

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

  Trade();
  Trade( const dt_t dt );
  Trade( const Trade &trade );
  Trade( const dt_t dt, price_t dblTrade, volume_t nTradeSize );
  Trade( const std::string& dt, const std::string& trade, const std::string& size );
  virtual ~Trade();

  inline price_t Price() const { return m_dblPrice; }  // 20120715 was Trace, may cause problems in other areas.
  inline volume_t Volume() const { return m_nTradeSize; }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return DatedDatum::Signature() * 100 + 13; }  // DatedDatum -> Trade

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

  Bar();
  Bar( const dt_t dt );
  Bar( const Bar& bar );
  Bar( const dt_t dt, price_t dblOpen, price_t dblHigh, price_t dblLow, price_t dblClose, volume_t nVolume );
  Bar( const std::string& dt, const std::string& open, const std::string& high,
    const std::string& low, const std::string& close, const std::string& volume );
  virtual ~Bar();

  inline price_t Open() const { return m_dblOpen; }
  inline price_t High() const { return m_dblHigh; }
  inline price_t Low() const { return m_dblLow; }
  inline price_t Close() const { return m_dblClose; }
  inline volume_t Volume() const { return m_nVolume; }

  inline void Open( price_t price ) { m_dblOpen = price; }
  inline void High( price_t price ) { m_dblHigh = price; }
  inline void Low( price_t price ) { m_dblLow = price; }
  inline void Close( price_t price ) { m_dblClose = price; }
  inline void Volume( volume_t vol ) { m_nVolume = vol; }

  void Accumulate( const Bar& );

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return DatedDatum::Signature() * 100000 + 11113; } // DatedDatum -> Bar

protected:
private:
  price_t m_dblOpen;
  price_t m_dblHigh;
  price_t m_dblLow;
  price_t m_dblClose;
  volume_t m_nVolume;
};

//
// Depth (common structure to DepthByMM, DepthByOrder)
//

class Depth: public DatedDatum {
public:

  Depth();
  Depth( const dt_t );
  Depth( const Depth& );
  explicit Depth( const dt_t, price_t, volume_t ); // quicky temp build
  explicit Depth( const dt_t, char chSide, price_t, volume_t ); // quicky temp build
  explicit Depth( const dt_t, char chMsgType, char chSide, price_t, quotesize_t );
  virtual ~Depth();

  inline char MsgType() const { return m_chMsgType; }
  inline char Side() const { return m_chSide; }
  inline volume_t Volume() const { return m_nShares; }
  inline price_t Price() const { return m_dblPrice; }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() {
    return DatedDatum::Signature() * 10000 + 8813; } // DatedDatum -> Depth
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

protected:
private:
  price_t m_dblPrice;
  volume_t m_nShares;
  char m_chMsgType; // 6 is summary, 3 is add, 4 is update
  char m_chSide; // simplifies insertion into MarketDepth handlers
};

//
// DepthByMM (nasdaq equity only)
//

class DepthByMM: public Depth {
public:

  using MMID_t = uint32_t;

  DepthByMM();
  DepthByMM( const dt_t );
  DepthByMM( const DepthByMM& );
  explicit DepthByMM( const dt_t, char chMsgType, char chSide, volume_t nShares, price_t dblPrice, char* pch );
  explicit DepthByMM( const dt_t, char chMsgType, char chSide, volume_t nShares, price_t dblPrice, MMID_t mmid );
  virtual ~DepthByMM();

  static MMID_t Cast( const char* rchMMID ) {
    unionMMID ummid( rchMMID );
    return ummid.mmid;
  }

  static std::string Cast( MMID_t mmid ) {
    unionMMID ummid( mmid );
    std::string s( ummid.rch, 4 );
    return s;
  }

  inline MMID_t MMID() const { return m_uMMID.mmid; }
  std::string MMIDStr() const { return std::string( m_uMMID.rch, 4 ); }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() {
    return DatedDatum::Signature() * 10000 + 8888; } // Depth -> DepthByMM
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

protected:
private:

  union unionMMID {
    MMID_t mmid;
    char rch[4];
    unionMMID() { mmid = 0; }
    unionMMID( MMID_t id ): mmid( id ) {}
    unionMMID( const unionMMID &u ): mmid( u.mmid ) {}
    unionMMID( const char* pch ) {
      char* p = rch;
      for ( int ix = 0; ix < 4; ix++ ) {
        *p = *pch;
        p++; pch++;
      }
    }
    unionMMID( const std::string& s ) {
      assert( 4 == s.size() );
      rch[0] = s[0];
      rch[1] = s[1];
      rch[2] = s[2];
      rch[3] = s[3];
    }
  } m_uMMID;
};

//
// DepthByOrder (futures)
//

class DepthByOrder: public Depth {
public:

  using idorder_t = uint64_t;

  DepthByOrder();
  DepthByOrder( const dt_t );
  DepthByOrder( const DepthByOrder& );
  explicit DepthByOrder( const dt_t, const dt_t dtMarket, idorder_t, uint64_t nPriority, char chMsgType, char chSide, price_t dblPrice = 0.0, volume_t nShares = 0 );
  virtual ~DepthByOrder();

  inline idorder_t OrderID() const { return m_nOrderID; }
  inline uint64_t Priority() const { return m_nPriority; }
  inline ptime MarketTimeStamp() const { return m_dtMarket; }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() {
    return DatedDatum::Signature() * 1000 + 944; } // Depth -> DepthByOrder
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

protected:
private:
  dt_t m_dtMarket; // market supplied datetime
  idorder_t m_nOrderID;
  uint64_t m_nPriority;
  // NOTE: probably won't add precision from iqfeed message, seems reduundantly supplied information
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
    greeks_t() : delta( 0.0 ), gamma( 0.0 ), theta( 0.0 ), vega( 0.0 ), rho( 0.0 ) {}
  };

  Greek();
  Greek( const dt_t dt );
  Greek( const Greek& greeks );
  Greek( const dt_t dt, double dblImpliedVolatility, const greeks_t& greeks );
  Greek( const dt_t dt, double dblImpliedVolatility, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho );
  virtual ~Greek();

  inline double ImpliedVolatility() const { return m_dblImpliedVolatility; }
  inline double Delta() const { return m_dblDelta; }
  inline double Gamma() const { return m_dblGamma; }
  inline double Theta() const { return m_dblTheta; }
  inline double Vega() const { return m_dblVega; }
  inline double Rho() const { return m_dblRho; }

  inline void ImpliedVolatility( double dblImpliedVolatility ) { m_dblImpliedVolatility = dblImpliedVolatility; }
  inline void Delta( double dblDelta ) { m_dblDelta = dblDelta; }
  inline void Gamma( double dblGamma ) { m_dblGamma = dblGamma; }
  inline void Theta( double dblTheta ) { m_dblTheta = dblTheta; }
  inline void Vega( double dblVega ) { m_dblVega = dblVega; }
  inline void Rho( double dblRho ) { m_dblRho = dblRho; }

  void Assign( const dt_t dt, double dblImplVol, double dblDelta, double dblGamma, double dblTheta, double dblVega, double dblRho ) {
    m_dt = dt;
    m_dblImpliedVolatility = dblImplVol;
    m_dblDelta = dblDelta;
    m_dblGamma =  dblGamma;
    m_dblTheta = dblTheta;
    m_dblVega = dblVega;
    m_dblRho = dblRho;
  };

  static H5::CompType* DefineDataType( H5::CompType *pType = NULL );
  static uint64_t Signature() { return DatedDatum::Signature() * 1000000 + 111111; } // DatedDatum > Greek

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

  Price();
  Price( const dt_t dt );
  Price( const Price& price );
  Price( const dt_t dt, price_t dblPrice );
  Price( const std::string &dt, const std::string& price );
  virtual ~Price();

  inline price_t Value() const { return m_dblPrice; };  // 20120715 was Price, is going to cause some problems in some code somewhere as is now class name

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return DatedDatum::Signature() * 10 + 1; } // DatedDatum > Price
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

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
  PriceIV();
  PriceIV( const dt_t dt );
  PriceIV( const PriceIV& rhs );
  PriceIV( const dt_t dtSampled, price_t dblPrice, double dblIVCall, double dblIVPut );
  virtual ~PriceIV() {};

  inline double IVCall() const { return m_dblIVCall; }
  inline double IVPut() const { return m_dblIVPut; }

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return Price::Signature() * 100 + 11; }; // Price -> PriceIV
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

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
  PriceIVExpiry();
  PriceIVExpiry( const dt_t dt );
  PriceIVExpiry( const PriceIVExpiry& rhs );
  PriceIVExpiry( const dt_t dtSampled, price_t dblPrice, const dt_t& dtExpiry, double dblIVCall, double dblIVPut );
  virtual ~PriceIVExpiry() {};

  inline double IVCall() const { return m_dblIVCall; };
  inline double IVPut() const { return m_dblIVPut; };
  inline dt_t Expiry() const { return m_dtExpiry; };

  static H5::CompType* DefineDataType( H5::CompType* pType = NULL );
  static uint64_t Signature() { return Price::Signature() * 1000 + 411; } // Price -> PriceIVExpiry
  // Signature() left to right reading: 9=datetime, 8=char, 1=double, 2=16 3=32, 4=64

protected:
private:
  dt_t m_dtExpiry;
  double m_dblIVCall;
  double m_dblIVPut;
};

} // namespace tf
} // namespace ou

