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

#include <string>
#include <vector>
#include <utility>

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

// will need to use the flex field capability where we get only the fields we need
// field offsets are 1 based, in order to easily match up with documentation
// for all the charT =  = unsigned char template parameters, need to turn into a trait
//   trait is shared with IQFeedMessages and Network

namespace ou { // One Unified
namespace tf { // TradeFrame

template <class T, class charT = unsigned char>
class IQFBaseMessage {
public:

  // factor this out of here and CNetwork and turn into trait
  typedef charT bufferelement_t;
  typedef typename std::vector<bufferelement_t> linebuffer_t;
  typedef typename linebuffer_t::iterator iterator_t;
  typedef std::pair<iterator_t, iterator_t> fielddelimiter_t;
  typedef typename linebuffer_t::size_type vector_size_type;

  IQFBaseMessage( void );
  IQFBaseMessage( iterator_t& current, iterator_t& end );
  ~IQFBaseMessage(void);

  void Assign( iterator_t& current, iterator_t& end );

  // change to return a fielddelimiter_t
  const std::string& Field( vector_size_type ); // returns reference to a field (will be sNull or sField );
  double Double( vector_size_type );  // use boost::spirit?
  int Integer( vector_size_type );  // use boost::spirit?

  iterator_t FieldBegin( vector_size_type );
  iterator_t FieldEnd( vector_size_type );

protected:

  std::vector<fielddelimiter_t> m_vFieldDelimiters;

  std::string sNull;  // always the empty string
  std::string sField;  // will hold content of selected field during field request call

  void Tokenize( iterator_t& begin, iterator_t& end );  // scans for ',' and builds the m_vFieldDelimiters vector

private:

};

//****
class IQFSystemMessage: public IQFBaseMessage<IQFSystemMessage> { // S
public:

  IQFSystemMessage( void );
  IQFSystemMessage( iterator_t& current, iterator_t& end );
  ~IQFSystemMessage(void);

private:
};

//****
class IQFTimeMessage: public IQFBaseMessage<IQFTimeMessage> { // T
public:

  IQFTimeMessage( void );
  IQFTimeMessage( iterator_t& current, iterator_t& end );
  ~IQFTimeMessage(void);

  void Assign( iterator_t& current, iterator_t& end );

  ptime& TimeStamp( void ) { return m_dt; };

protected:
  ptime m_dt; 
  // different markets have different hours so should get rid of these two declarations
  boost::posix_time::time_duration m_timeMarketOpen, m_timeMarketClose;
  bool m_bMarketIsOpen;  
  
private:
};

//****
class IQFNewsMessage: public IQFBaseMessage<IQFNewsMessage> { // N
public:

  enum enumFieldIds {
    NDistributor = 2,
    NStoryId,
    NSymbolList,
    NDateTime,
    NHeadLine
  };

  IQFNewsMessage( void );
  IQFNewsMessage( iterator_t& current, iterator_t& end );
  ~IQFNewsMessage(void);

  const std::string& Distributor( void ) { return Field( NDistributor ); };
  const std::string& StoryId( void ) { return Field( NStoryId ); };
  const std::string& SymbolList( void ) { return Field( NSymbolList ); };
  const std::string& DateTime( void ) { return Field( NDateTime ); };
  const std::string& Headline( void ) { 
    m_sHeadLine.assign( m_vFieldDelimiters[ NHeadLine ].first, m_vFieldDelimiters[ 0 ].second );
    return m_sHeadLine;
  };

  fielddelimiter_t Distributor_iter( void ) { 
    BOOST_ASSERT( NDistributor <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NDistributor ];
  }
  fielddelimiter_t StoryId_iter( void ) { 
    BOOST_ASSERT( NStoryId <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NStoryId ];
  }
  fielddelimiter_t SymbolList_iter( void ) { 
    BOOST_ASSERT( NSymbolList <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NSymbolList ];
  }
  fielddelimiter_t DateTime_iter( void ) { 
    BOOST_ASSERT( NDateTime <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NDateTime ];
  }
  fielddelimiter_t HeadLine_iter( void ) { 
    BOOST_ASSERT( NHeadLine <= m_vFieldDelimiters.size() - 1 );
    fielddelimiter_t fd( m_vFieldDelimiters[ NHeadLine ].first, m_vFieldDelimiters[ 0 ].second ); // necessary to incorporate included commas, and field 0 has end of buffer marker
    return fd;
  }


protected:
private:
  std::string m_sHeadLine;
};

//**** IQFFundamentalMessage
class IQFFundamentalMessage: public IQFBaseMessage<IQFFundamentalMessage> { // F
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
    FShortInterest = 18,
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
//    FMarketCenter = 39,
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

  IQFFundamentalMessage( void );
  IQFFundamentalMessage( iterator_t& current, iterator_t& end );
  ~IQFFundamentalMessage(void);

private:
};

//**** IQFPricingMessage ( root for IQFUpdateMessage, IQFSummaryMessage )
template <class T, class charT = unsigned char>
class IQFPricingMessage: public IQFBaseMessage<IQFPricingMessage<T, charT> > { // Q, P
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

  IQFPricingMessage( void );
  IQFPricingMessage( iterator_t& current, iterator_t& end );
  ~IQFPricingMessage(void);

  ptime LastTradeTime( void );
protected:

private:
};


//**** IQFUpdateMessage
class IQFUpdateMessage: public IQFPricingMessage<IQFUpdateMessage> { // Q
public:

  IQFUpdateMessage( void );
  IQFUpdateMessage( iterator_t& current, iterator_t& end );
  ~IQFUpdateMessage(void);

protected:

private:
};

//**** IQFSummaryMessage
class IQFSummaryMessage: public IQFPricingMessage<IQFSummaryMessage> { // P
public:

  IQFSummaryMessage( void );
  IQFSummaryMessage( iterator_t& current, iterator_t& end );
  ~IQFSummaryMessage(void);

private:
};


template <class T, class charT>
IQFBaseMessage<T, charT>::IQFBaseMessage( void )
{
}

template <class T, class charT>
IQFBaseMessage<T, charT>::IQFBaseMessage( iterator_t& current, iterator_t& end )
{
  Tokenize( current, end );
}

template <class T, class charT>
IQFBaseMessage<T, charT>::~IQFBaseMessage(void) {
}

template <class T, class charT>
void IQFBaseMessage<T, charT>::Assign( iterator_t& current, iterator_t& end ) {
  Tokenize( current, end );
}

template <class T, class charT>
void IQFBaseMessage<T, charT>::Tokenize( iterator_t& current, iterator_t& end ) {
  // used in IQFeedLookupPort::Parse

  m_vFieldDelimiters.clear();
  m_vFieldDelimiters.push_back( fielddelimiter_t( current, end ) );  // prime entry 0 with something to get to index 1

  iterator_t begin = current;
  while ( current != end ) {
    if ( ',' == *current ) { // first character shouldn't be ','
      m_vFieldDelimiters.push_back( fielddelimiter_t( begin, current ) );
      ++current;
      begin = current;
    }
    else {
      ++current;
    }
  }
  // always push what ever is remaining, empty string or not
  m_vFieldDelimiters.push_back( fielddelimiter_t( begin, current ) );
}

template <class T, class charT>
const std::string& IQFBaseMessage<T, charT>::Field( vector_size_type fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first == fielddelimiter.second ) return sNull;
  else sField.assign( fielddelimiter.first, fielddelimiter.second );
  return sField;
}

template <class T, class charT>
double IQFBaseMessage<T, charT>::Double( vector_size_type fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  double dest = 0;
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    namespace qi = boost::spirit::qi;
	  using namespace boost::phoenix::arg_names;

    using boost::spirit::qi::_1;
	  using boost::phoenix::ref;
	  using namespace boost::spirit::qi;

    bool b = qi::parse( fielddelimiter.first, fielddelimiter.second, double_[ref(dest) = _1] );
  }

  return dest;
}

template <class T, class charT>
int IQFBaseMessage<T, charT>::Integer( vector_size_type fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  int dest = 0;
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    namespace qi = boost::spirit::qi;
	  using namespace boost::phoenix::arg_names;

    using boost::spirit::qi::_1;
	  using boost::phoenix::ref;
	  using namespace boost::spirit::qi;

    bool b = qi::parse( fielddelimiter.first, fielddelimiter.second, int_[ref(dest) = _1] );
  }

  return dest;
}

template <class T, class charT>
typename IQFBaseMessage<T, charT>::iterator_t IQFBaseMessage<T, charT>::FieldBegin( vector_size_type fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  return m_vFieldDelimiters[ fld ].first;
}

template <class T, class charT>
typename IQFBaseMessage<T, charT>::iterator_t IQFBaseMessage<T, charT>::FieldEnd( vector_size_type fld ) {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  return m_vFieldDelimiters[ fld ].second;
}

//**** IQFPricingMessage
// resize the vector to accept with out resizing so often?

template <class T, class charT>
IQFPricingMessage<T, charT>::IQFPricingMessage( void ) 
: IQFBaseMessage<IQFPricingMessage<T> >()
{
}

template <class T, class charT>
IQFPricingMessage<T, charT>::IQFPricingMessage( iterator_t& current, iterator_t& end ) 
: IQFBaseMessage<IQFPricingMessage>( current, end )
{
}

template <class T, class charT>
IQFPricingMessage<T, charT>::~IQFPricingMessage() {
}

template <class T, class charT>
ptime IQFPricingMessage<T, charT>::LastTradeTime( void ) {

  fielddelimiter_t date = m_vFieldDelimiters[ QPLastTradeDate ];
  fielddelimiter_t time = m_vFieldDelimiters[ QPLastTradeTime ];

  if ( ( ( date.second - date.first ) == 10 ) && ( ( time.second - time.first ) >= 8 ) ) {
    char szDateTime[ 20 ];
    szDateTime[  0 ] = *(date.first + 6); // yyyy
    szDateTime[  1 ] = *(date.first + 7);
    szDateTime[  2 ] = *(date.first + 8);
    szDateTime[  3 ] = *(date.first + 9);

    szDateTime[  5 ] = *(date.first + 0); // mm
    szDateTime[  6 ] = *(date.first + 1);

    szDateTime[  8 ] = *(date.first + 3); // dd
    szDateTime[  9 ] = *(date.first + 4);

    szDateTime[ 11 ] = *(time.first + 0); // hh:mm:ss
    szDateTime[ 12 ] = *(time.first + 1);
    szDateTime[ 13 ] = *(time.first + 2);
    szDateTime[ 14 ] = *(time.first + 3);
    szDateTime[ 15 ] = *(time.first + 4);
    szDateTime[ 16 ] = *(time.first + 5);
    szDateTime[ 17 ] = *(time.first + 6);
    szDateTime[ 18 ] = *(time.first + 7);

    szDateTime[ 4 ] = szDateTime[ 7 ] = '-';
    szDateTime[ 10 ] = ' ';
    szDateTime[ 19 ] = 0;

    return boost::posix_time::time_from_string(szDateTime);
  }
  else {
    return boost::posix_time::ptime(boost::date_time::special_values::min_date_time );
  }
}

} // namespace tf
} // namespace ou

