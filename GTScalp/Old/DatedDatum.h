#pragma once

#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\gmmid.h"

#include "boost/date_time/posix_time/posix_time.hpp"
//#include "boost/date_time/gregorian/gregorian_types.hpp" 
//#include "boost/archive/text_oarchive.hpp"
//#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/binary_oarchive.hpp"
#include "boost/archive/binary_iarchive.hpp"
#include <boost/serialization/version.hpp>
#include <boost/date_time/posix_time/time_serialize.hpp>

// http://www.boost.org/libs/serialization/doc/tutorial.html#examples
// http://www.boost.org/libs/serialization/doc/index.html

// http://www.boost.org/doc/html/date_time/serialization.html
//   date_time is not binary serializeable

/*
namespace archive {

enum archive_flags {
    no_header = 1,          // suppress archive header info
    no_codecvt = 2,         // suppress alteration of codecvt facet
    no_xml_tag_checking = 4 // suppress checking of xml tags - igored on saving
};
*/

using namespace boost::posix_time;
using namespace boost::gregorian;
//using namespace boost::date_time;

class CDatedDatum {
  friend class boost::serialization::access;  // serialize should be private to force proper hierarchy
public:
  CDatedDatum(void);
  CDatedDatum( ptime dt );
  CDatedDatum( const CDatedDatum &datum );
  CDatedDatum( const std::string &dt ); // YYYY-MM-DD HH:MM:SS
  virtual ~CDatedDatum(void);



  ptime m_dt;
      char a;
      long long l;
  bool IsNull( void ) { return m_dt.is_not_a_date_time(); };

  bool operator<( const CDatedDatum &datum ) { return m_dt < datum.m_dt; };
  bool operator<( CDatedDatum *datum ) { return m_dt < datum->m_dt; };
  bool operator==( const CDatedDatum &datum ) { return m_dt == datum.m_dt; };
  bool operator!=( const CDatedDatum &datum ) { return m_dt != datum.m_dt; };

  // http://www.boost.org/doc/html/date_time/posix_time.html#date_time.posix_time.ptime_class
  //ptime m_dt(boost::date_time::special_values::not_a_date_time);

protected:
long long convert( ptime pp ) {
  long long l;
  size_t t = sizeof( pp );
  memcpy( &l, &pp, 8 );
  return l;
};

private:
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      a = '=';
      ar & a;
      l = convert( m_dt );
      
      ar & l;
    }
};

bool operator<( const CDatedDatum &datum1, const CDatedDatum &datum2 );

BOOST_CLASS_VERSION(CDatedDatum, 1)

class CQuote: public CDatedDatum {
  friend class boost::serialization::access;
public:
  CQuote(void);
  CQuote( ptime dt );
  CQuote( const CQuote &quote );
  CQuote( ptime dt, double dblBid, int nBidSize, double dblAsk, int nAskSize );
  CQuote( const std::string &dt, const std::string &bid, 
    const std::string &bidsize, const std::string &ask, const std::string &asksize );
  virtual ~CQuote(void);

  double m_dblBid;
  double m_dblAsk;
  int m_nBidSize;
  int m_nAskSize;

protected:
private:
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<CDatedDatum>(*this);
      ar & m_dblBid;
      ar & m_dblAsk;
      ar & m_nBidSize;
      ar & m_nAskSize;
    }
};

bool operator<( const CQuote &quote1, const CQuote &quote2 );

BOOST_CLASS_VERSION(CQuote, 1)

class CTrade: public CDatedDatum {
  friend class boost::serialization::access;
public:
  CTrade(void);
  CTrade( ptime dt );
  CTrade( const CTrade &trade );
  CTrade( ptime dt, double dblTrade, int nTradeSize );
  CTrade( const std::string &dt, const std::string &trade, const std::string &size );
  virtual ~CTrade(void);

  double m_dblTrade;
  int m_nTradeSize;

protected:
private:
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<CDatedDatum>(*this);
      ar & m_dblTrade;
      ar & m_nTradeSize;
    }
};

bool operator<( const CTrade &trade1, const CTrade &trade2 );

BOOST_CLASS_VERSION(CTrade, 1)

class CBar: public CDatedDatum {
  friend class boost::serialization::access;
public:
  CBar(void);
  CBar( ptime dt );
  CBar( const CBar &bar );
  CBar( ptime dt, double dblOpen, double dblHigh, double dblLow, double dblClose, int nVolume );
  CBar( const std::string &dt, const std::string &open, const std::string &high, 
    const std::string &low, const std::string &close, const std::string &volume );
  virtual ~CBar(void);

  double m_dblOpen;
  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;
  int m_nVolume;

protected:
private:
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<CDatedDatum>(*this);
      ar & m_dblOpen;
      ar & m_dblHigh;
      ar & m_dblLow;
      ar & m_dblClose;
      ar & m_nVolume;
    }
};

bool operator<( const CBar &bar1, const CBar &bar2 );

BOOST_CLASS_VERSION(CBar, 1)

class CMarketDepth: public CDatedDatum {
  friend class boost::serialization::access;
public:
  CMarketDepth(void);
  CMarketDepth( ptime dt );
  CMarketDepth( const CMarketDepth &md );
  CMarketDepth( ptime dt, char chSide, long nShares, double dblPrice, MMID mmid );
  CMarketDepth( const std::string &dt, char chSide, const std::string &shares, 
    const std::string &price, const std::string &mmid );
  virtual ~CMarketDepth(void);

  enum ESide { Bid, Ask, None };

  long m_nShares;
  double m_dblPrice;
  //char m_szMMID[ 6 ];
  GMMID m_mmid;
  ESide m_eSide;
protected:
private:
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version) {
      ar & boost::serialization::base_object<CDatedDatum>(*this);
      ar & m_nShares;
      ar & m_dblPrice;
      ar & m_mmid;
      ar & m_eSide;
    }
};

bool operator<( const CMarketDepth &md1, const CMarketDepth &md2 );

BOOST_CLASS_VERSION(CMarketDepth, 1)

