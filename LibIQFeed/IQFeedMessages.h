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

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;

#include <string>
#include <vector>
#include <utility>

// will need to use the flex field capability where we get only the fields we need
// field offsets are 1 based, in order to easily match up with documentation
// for all the charT =  = unsigned char template parameters, need to turn into a trait
//   trait is shared with IQFeedMessages and Network

template <class T, class charT = unsigned char>
class CIQFBaseMessage {
public:

  // factor this out of here and CNetwork and turn into trait
  typedef charT bufferelement_t;
  typedef typename std::vector<bufferelement_t> linebuffer_t;
  typedef typename linebuffer_t::iterator iterator_t;

  CIQFBaseMessage( iterator_t& current, iterator_t& end );
  ~CIQFBaseMessage(void);

  //void EmitFields( void );
  //void EmitLine( void );  // turn into stream operator
  const std::string &Field( unsigned short ); // returns reference to a field (will be sNull or sField );
  double Double( unsigned short );  // use boost::spirit?
  int Integer( unsigned short );  // use boost::spirit?

protected:

  typedef std::pair<iterator_t, iterator_t> fielddelimiter_t;
  std::vector<fielddelimiter_t> m_vFieldDelimiters;

  std::string sNull;  // always the empty string
  std::string sField;  // will hold content of selected field during field request call

  void Tokenize( iterator_t& begin, iterator_t& end );  // scans for ',' and builds the m_vFieldDelimiters vector

private:

};

//****
class CIQFSystemMessage: public CIQFBaseMessage<CIQFSystemMessage> { // S
public:

  CIQFSystemMessage( iterator_t& current, iterator_t& end );
  ~CIQFSystemMessage(void);

private:
};

//****
class CIQFTimeMessage: public CIQFBaseMessage<CIQFTimeMessage> { // T
public:

  CIQFTimeMessage( iterator_t& current, iterator_t& end );
  ~CIQFTimeMessage(void);

  ptime& TimeStamp( void ) { return m_dt; };

protected:
  ptime m_dt; 
  // different markets have different hours so should get rid of these two declarations
  boost::posix_time::time_duration m_timeMarketOpen, m_timeMarketClose;
  bool m_bMarketIsOpen;  
  
private:
};

//****
class CIQFNewsMessage: public CIQFBaseMessage<CIQFNewsMessage> { // N
public:

  enum enumFieldIds {
    NDistributor = 2,
    NStoryId,
    NSymbolList,
    NDateTime,
    NHeadline
  };

  std::string m_sDistributor;
  std::string m_sStoryId;
  std::string m_sSymbolList;
  std::string m_sDateTime;
  std::string m_sHeadline;

  CIQFNewsMessage( iterator_t& current, iterator_t& end );
  ~CIQFNewsMessage(void);

protected:
private:
};

//**** CIQFFundamentalMessage
class CIQFFundamentalMessage: public CIQFBaseMessage<CIQFFundamentalMessage> { // F
public:

  enum enumFieldIds {
    FSymbol = 2,
    FPE = 4,
    FAveVolume = 5,
    F52WkHi = 6,
    F52WkLo = 7,
    FCalYrHi = 8,
    FCalYrLo = 9,
    FDivYld = 10,
    FDivAmt = 11,
    FDivRate = 12,
    FDivPayDate = 13,
    FDivExDate = 14,
    FCurYrEPS = 20,
    FNxtYrEPS = 21,
    FFiscalYrEnd = 23,
    FName = 25,
    FRootOptionSymbols = 26,
    FPctInst = 27,
    FBeta = 28,
    FLeaps = 29,
    FCurAssets = 30,
    FCurLiab = 31,
    FBalShtDate = 32,
    FLongTermDebt = 33,
    FCommonShares = 34,
    FMarketCenter = 39,
    FFormatCode = 40,
    FPrecision = 41,
    FSIC = 42,
    FVolatility = 43,
    FSecurityType = 44,
    FListedMarket = 45,
    F52WkHiDate = 46,
    F52WkLoDate = 47,
    FCalYrHiDate = 48,
    FCalYrLoDate = 49,
    FYearEndClose = 50,
    FBondMaturityDate = 51,
    FBondCouponRate = 52,
    FExpirationDate = 53,
    FStrikePrice = 54,
    _FLastEntry
  };

  CIQFFundamentalMessage( iterator_t& current, iterator_t& end );
  ~CIQFFundamentalMessage(void);

private:
};

//**** CIQFPricingMessage ( root for CIQFUpdateMessage, CIQFSummaryMessage )
template <class T, class charT = unsigned char>
class CIQFPricingMessage: public CIQFBaseMessage<CIQFPricingMessage<T, charT> > { // Q, P
public:

  enum enumFieldIds {
    QPSymbol = 2,
    QPLast = 4,
    QPChange = 5,
    QPPctChange = 6,
    QPTtlVol = 7,
    QPLastVol = 8,
    QPHigh = 9,
    QPLow = 10,
    QPBid = 11,
    QPAsk = 12,
    QPBidSize = 13,
    QPAskSize = 14,
    QPTick = 15,
    QPBidTick = 16,
    QPTradeRange = 17,
    QPLastTradeTime = 18,
    QPOpenInterest = 19,
    QPOpen = 20,
    QPClose = 21,
    QPSpread = 22,
    QPSettle = 24,
    QPDelay = 25,
    QPNav = 28,
    QPMnyMktAveMaturity = 29,
    QPMnyMkt7DayYld = 30,
    QPLastTradeDate = 31,
    QPExtTradeLast = 33,
    QPNav2 = 36,
    QPExtTradeChng = 37,
    QPExtTradeDif = 38,
    QPPE = 39,
    QPPctOff30AveVol = 40,
    QPBidChange = 41,
    QPAskChange = 42,
    QPChangeFromOpen = 43,
    QPMarketOpen = 44,
    QPVolatility = 45,
    QPMarketCap = 46,
    QPDisplayCode = 47,
    QPPrecision = 48,
    QPDaysToExpiration = 49,
    QPPrevDayVol = 50,
    QPNumTrades = 56,
    QPFxBidTime = 57,
    QPFxAskTime = 58,
    QPVWAP = 59,
    QPTickId = 60,
    QPFinStatus = 61,
    QPSettleDate = 62,
    _QPLastEntry
  };

  CIQFPricingMessage( iterator_t& current, iterator_t& end );
  ~CIQFPricingMessage(void);

  ptime LastTradeTime( void );
protected:

private:
};


//**** CIQFUpdateMessage
class CIQFUpdateMessage: public CIQFPricingMessage<CIQFUpdateMessage> { // Q
public:

  CIQFUpdateMessage( iterator_t& current, iterator_t& end );
  ~CIQFUpdateMessage(void);

protected:

private:
};

//**** CIQFSummaryMessage
class CIQFSummaryMessage: public CIQFPricingMessage<CIQFSummaryMessage> { // P
public:

  CIQFSummaryMessage( iterator_t& current, iterator_t& end );
  ~CIQFSummaryMessage(void);

private:
};




template <class T, class charT>
CIQFBaseMessage<T, charT>::CIQFBaseMessage( iterator_t& current, iterator_t& end )
{
  Tokenize( current, end );
}

template <class T, class charT>
CIQFBaseMessage<T, charT>::~CIQFBaseMessage(void) {
}

template <class T, class charT>
void CIQFBaseMessage<T, charT>::Tokenize( iterator_t& current, iterator_t& end ) {
  // used in IQFeedLookupPort::Parse

  m_vFieldDelimiters.push_back( fielddelimiter_t( current, end ) );  // prime entry 0 with something to get to index 1

  iterator_t begin = current;
  while ( current != end ) {
    if ( ',' == *current ) { // first character shouldn't be ','
      m_vFieldDelimiters.push_back( fielddelimiter_t( begin, current ) );
      ++current;
      begin = current;
    }
  }
}

//template <class T, class charT>
//void CIQFBaseMessage<class T, class charT>::EmitFields( void ) {

//  CString s;
//  for ( int i = 1; i <= cntFieldsFound; i++ ) {
//    if ( 0 != rSize[ i ] ) {
//      s.Format( "%s%d %s", Field( 1 ), i, Field( i ) );
//      //s.Format( "%s%d %s",   m_sMsg.substr( rOffset[ 1 ], rSize[ 1 ] ), i, m_sMsg.substr( rOffset[ i ], rSize[ i ] ) );
//      cout << s << endl;
//    }
//  }
//}

//template <class T, class charT>
//void CIQFBaseMessage<class T, class charT>::EmitLine( void ) {
//  std::cout << m_pStr << std::endl;
//}

template <class T, class charT>
const std::string& CIQFBaseMessage<T, charT>::Field( unsigned short fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first == fielddelimiter.second ) return sNull;
  sField.assign( fielddelimiter.first, fielddelimiter.second );
  return sField;
}

template <class T, class charT>
double CIQFBaseMessage<T, charT>::Double( unsigned short fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  std::string& str = Field( fld );
  if ( 0 == str.size() ) return 0.0;
  return atof( str.c_str() );
}

template <class T, class charT>
int CIQFBaseMessage<T, charT>::Integer( unsigned short fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  std::string& str = Field( fld );
  if ( 0 == str.size() ) return 0;
  return atoi( str.c_str() );
}

//**** CIQFPricingMessage
// resize the vector to accept with out resizing so often?

template <class T, class charT>
CIQFPricingMessage<T, charT>::CIQFPricingMessage( iterator_t& current, iterator_t& end ) 
: CIQFBaseMessage<CIQFPricingMessage>( current, end )
{
}

template <class T, class charT>
CIQFPricingMessage<T, charT>::~CIQFPricingMessage() {
}

template <class T, class charT>
ptime CIQFPricingMessage<T, charT>::LastTradeTime( void ) {

  fielddelimiter_t date = m_vFieldDelimiters[ QPLastTradeDate ];
  fielddelimiter_t time = m_vFieldDelimiters[ QPLastTradeTime ];

  if ( ( ( date.second - date.first ) == 10 ) && ( ( time.second - time.first ) >= 8 ) ) {
    char szDateTime[ 20 ];
    szDateTime[  0 ] = *(date + 6); // yyyy
    szDateTime[  1 ] = *(date + 7);
    szDateTime[  2 ] = *(date + 8);
    szDateTime[  3 ] = *(date + 9);

    szDateTime[  5 ] = *(date + 0); // mm
    szDateTime[  6 ] = *(date + 1);

    szDateTime[  8 ] = *(date + 3); // dd
    szDateTime[  9 ] = *(date + 4);

    szDateTime[ 11 ] = *(time + 0); // hh:mm:ss
    szDateTime[ 12 ] = *(time + 1);
    szDateTime[ 13 ] = *(time + 2);
    szDateTime[ 14 ] = *(time + 3);
    szDateTime[ 15 ] = *(time + 4);
    szDateTime[ 16 ] = *(time + 5);
    szDateTime[ 17 ] = *(time + 6);
    szDateTime[ 18 ] = *(time + 7);

    szDateTime[ 4 ] = szDateTime[ 7 ] = '-';
    szDateTime[ 10 ] = ' ';
    szDateTime[ 19 ] = 0;

    return boost::posix_time::time_from_string(szDateTime);
  }
  else {
    return boost::posix_time::ptime(boost::date_time::special_values::min_date_time );
  }
}


