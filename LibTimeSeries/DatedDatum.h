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

#include "HDF5/H5Cpp.h"

#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;

class CDatedDatum {
public:
  CDatedDatum(void);
  CDatedDatum( const ptime &dt );
  CDatedDatum( const CDatedDatum &datum );
  CDatedDatum( const std::string &dt ); // YYYY-MM-DD HH:MM:SS
  virtual ~CDatedDatum(void);
  static H5::CompType *DefineDataType( H5::CompType *pType = NULL );  // create new one if null

  ptime m_dt;
  bool IsNull( void ) { return m_dt.is_not_a_date_time(); };

  bool operator<( const CDatedDatum &datum ) const { return m_dt < datum.m_dt; };
//  bool operator<( CDatedDatum *datum ) { return m_dt < datum->m_dt; };
  bool operator==( const CDatedDatum &datum ) const { return m_dt == datum.m_dt; };
  bool operator!=( const CDatedDatum &datum ) const { return m_dt != datum.m_dt; };

  // http://www.boost.org/doc/html/date_time/posix_time.html#date_time.posix_time.ptime_class
  //ptime m_dt(boost::date_time::special_values::not_a_date_time);

  typedef long tradesize_t;
  typedef tradesize_t volume_t;
  typedef long quotesize_t;
  typedef quotesize_t bidsize_t;
  typedef quotesize_t asksize_t;
protected:
private:
};

//bool operator<( const CDatedDatum &datum1, const CDatedDatum &datum2 ) { return datum1.m_dt < datum2.m_dt; };

class CQuote: public CDatedDatum {
public:
  CQuote(void);
  CQuote( const ptime &dt );
  CQuote( const CQuote &quote );
  CQuote( const ptime &dt, double dblBid, bidsize_t nBidSize, double dblAsk, asksize_t nAskSize );
  CQuote( const std::string &dt, const std::string &bid, 
    const std::string &bidsize, const std::string &ask, const std::string &asksize );
  virtual ~CQuote(void);
  static H5::CompType *DefineDataType( H5::CompType *pType = NULL );

//  bool operator<( const CQuote &quote ) const { return m_dt < quote.m_dt; };

  double m_dblBid;
  double m_dblAsk;
  bidsize_t m_nBidSize;
  asksize_t m_nAskSize;

protected:
private:
};

//bool operator<( const CQuote &quote1, const CQuote &quote2 );

class CTrade: public CDatedDatum {
public:
  CTrade(void);
  CTrade( const ptime &dt );
  CTrade( const CTrade &trade );
  CTrade( const ptime &dt, double dblTrade, tradesize_t nTradeSize );
  CTrade( const std::string &dt, const std::string &trade, const std::string &size );
  virtual ~CTrade(void);
  static H5::CompType *DefineDataType( H5::CompType *pType = NULL );

//  bool operator<( const CTrade &trade ) const { return m_dt < trade.m_dt; };

  double m_dblTrade;
  int m_nTradeSize;

protected:
private:
};

//bool operator<( const CTrade &trade1, const CTrade &trade2 );

class CBar: public CDatedDatum {
public:
  CBar(void);
  CBar( const ptime &dt );
  CBar( const CBar &bar );
  CBar( const ptime &dt, double dblOpen, double dblHigh, double dblLow, double dblClose, volume_t nVolume );
  CBar( const std::string &dt, const std::string &open, const std::string &high, 
    const std::string &low, const std::string &close, const std::string &volume );
  virtual ~CBar(void);
  static H5::CompType *DefineDataType( H5::CompType *pType = NULL );

//  bool operator<( const CBar &bar ) const { return m_dt < bar.m_dt; };

  double m_dblOpen;
  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;
  volume_t m_nVolume;

protected:
private:
};

//bool operator<( const CBar &bar1, const CBar &bar2 );

class CMarketDepth: public CDatedDatum {
public:
  typedef unsigned long MMID;

  CMarketDepth(void);
  CMarketDepth( const ptime &dt );
  CMarketDepth( const CMarketDepth &md );
  CMarketDepth( const ptime &dt, char chSide, quotesize_t nShares, double dblPrice, MMID mmid );
  CMarketDepth( const std::string &dt, char chSide, const std::string &shares, 
    const std::string &price, const std::string &mmid );
  virtual ~CMarketDepth(void);
  static H5::CompType *DefineDataType( H5::CompType *pType = NULL );

//  bool operator<( const CMarketDepth &md ) const { return m_dt < md.m_dt; };

  enum ESide : char { Bid, Ask, None };

  long m_nShares;
  double m_dblPrice;
  //char m_szMMID[ 6 ];
  //GMMID m_mmid;
  //char m_mmid[5];
  ESide m_eSide; 
  MMID GetMMID( void ) { return m_uMMID.mmid; };
  const char &GetMMIDStr( void ) { return *m_uMMID.rch; };
protected:
  union unionMMID {
    MMID mmid;
    char rch[5];
    unionMMID( void ) { mmid = 0; rch[4] = 0; };
    unionMMID( MMID id ) : mmid( id ) { rch[4] = 0; };
    unionMMID( const unionMMID &u ) : mmid( u.mmid ) { rch[4] = 0; };
  } m_uMMID;
private:
};

//bool operator<( const CMarketDepth &md1, const CMarketDepth &md2 );


