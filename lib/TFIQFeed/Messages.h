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

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/bind.hpp>
#include <boost/phoenix/operator.hpp>
//#include <boost/spirit/include/phoenix/stl.hpp>

// will need to use the flex field capability where we get only the fields we need
// field offsets are 1 based, in order to easily match up with documentation
// for all the charT =  = unsigned char template parameters, need to turn into a trait
//   trait is shared with IQFeedMessages and Network

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct date_values {
  uint16_t year;
  uint16_t month;
  uint16_t day;
  date_values(): year {}, month {}, day {} {}
};

struct time_values {
  uint16_t hours;
  uint16_t minutes;
  uint16_t seconds;
  time_values(): hours {}, minutes {}, seconds {} {}
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

BOOST_FUSION_ADAPT_STRUCT( // iqfeed format mm/dd/yyyy
  ou::tf::iqfeed::date_values,
  (uint16_t, month)
  (uint16_t, day)
  (uint16_t, year)
  )

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::time_values,
  (uint16_t, hours)
  (uint16_t, minutes)
  (uint16_t, seconds)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

using date = boost::gregorian::date;
using time = boost::posix_time::time_duration;
using ptime = boost::posix_time::ptime;

template <class T, class charT = unsigned char>
class IQFBaseMessage {
public:

  // factor this out of here and Network and turn into trait
  using bufferelement_t = charT;
  using linebuffer_t = typename std::vector<bufferelement_t>;
  using iterator_t = typename linebuffer_t::iterator;  // TODO: need to use const_iterator
  using fielddelimiter_t = std::pair<iterator_t, iterator_t>;
  using ixFields_t = typename linebuffer_t::size_type;

  IQFBaseMessage( void );
  IQFBaseMessage( iterator_t& current, iterator_t& end );

  void Assign( iterator_t& current, iterator_t& end );

  // change to return a fielddelimiter_t
  const std::string Field( ixFields_t ) const;
  double Double( ixFields_t ) const;  // use boost::spirit?
  int Integer( ixFields_t ) const;  // use boost::spirit?
  date Date( ixFields_t ) const;
  time Time( ixFields_t ) const;

  iterator_t FieldBegin( ixFields_t ) const;
  iterator_t FieldEnd( ixFields_t ) const;

protected:

  ~IQFBaseMessage(void);

  std::vector<fielddelimiter_t> m_vFieldDelimiters;

  void Tokenize( iterator_t& begin, iterator_t& end );  // scans for ',' and builds the m_vFieldDelimiters vector

private:

  int parse_int( fielddelimiter_t ) const;
  double parse_double(  fielddelimiter_t ) const;
  date parse_date( fielddelimiter_t ) const; // MM/DD/YYYY
  time parse_time( fielddelimiter_t ) const; // HH:mm:SS

};

//****
class IQFSystemMessage: public IQFBaseMessage<IQFSystemMessage> { // S
public:

  IQFSystemMessage( void );
  IQFSystemMessage( iterator_t& current, iterator_t& end );
  ~IQFSystemMessage(void);

protected:
private:
};

//****
class IQFErrorMessage: public IQFBaseMessage<IQFErrorMessage> { // S
public:

  enum enumFieldIDs {
    ESymbol = 2 // used with SymbolNotFound for extraction
  };

  IQFErrorMessage();
  IQFErrorMessage( iterator_t& current, iterator_t& end );
  ~IQFErrorMessage();

  const std::string SymbolNotFound() const { return Field( ESymbol ); }

protected:
private:
};

//****
class IQFTimeMessage: public IQFBaseMessage<IQFTimeMessage> { // T
public:

  IQFTimeMessage( void );
  IQFTimeMessage( iterator_t& current, iterator_t& end );
  ~IQFTimeMessage(void);

  void Assign( iterator_t& current, iterator_t& end );

  ptime TimeStamp( void ) const { return m_dt; };

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

  const std::string Distributor( void ) const { return Field( NDistributor ); };
  const std::string StoryId( void ) const { return Field( NStoryId ); };
  const std::string SymbolList( void ) const { return Field( NSymbolList ); };
  const std::string DateTime( void ) const { return Field( NDateTime ); };
  const std::string Headline( void ) const {
    std::string sHeadLine;
    sHeadLine.assign( m_vFieldDelimiters[ NHeadLine ].first, m_vFieldDelimiters[ 0 ].second );
    return sHeadLine;
  };

  fielddelimiter_t Distributor_iter( void ) const {
    BOOST_ASSERT( NDistributor <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NDistributor ];
  }
  fielddelimiter_t StoryId_iter( void ) const {
    BOOST_ASSERT( NStoryId <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NStoryId ];
  }
  fielddelimiter_t SymbolList_iter( void ) const {
    BOOST_ASSERT( NSymbolList <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NSymbolList ];
  }
  fielddelimiter_t DateTime_iter( void ) const {
    BOOST_ASSERT( NDateTime <= m_vFieldDelimiters.size() - 1 );
    return m_vFieldDelimiters[ NDateTime ];
  }
  fielddelimiter_t HeadLine_iter( void ) const {
    BOOST_ASSERT( NHeadLine <= m_vFieldDelimiters.size() - 1 );
    fielddelimiter_t fd( m_vFieldDelimiters[ NHeadLine ].first, m_vFieldDelimiters[ 0 ].second ); // necessary to incorporate included commas, and field 0 has end of buffer marker
    return fd;
  }

protected:
private:
};

//**** IQFTradeCorrectionMessage - https://www.iqfeed.net/dev/index.cfm?section=docs&page=l1tcor
// 'C,SPY,X,,,,619.0500,15000,1296380724,,19,'
// 'C,SPY,I,O,07/16/2025,,619.0500,15000,859256116,198944,24,'
// 'C,GLD,X,,,,309.3200,52000,859326789,,24,'
// 'C,SLV,X,,,,34.4200,250,909457745,,19,'

class IQFTradeCorrectionMessage: public IQFBaseMessage<IQFTradeCorrectionMessage> { // C
public:
  enum enumFieldIDs {
    CSymbol = 2,
    CCorrectionType = 3, // 'I' = trade insert, 'X' = trade delete
    CTradeType = 4, // Single character: 'C', 'E', 'O' as in the Level 1 fields
    CTradeDate = 5, // MM/DD/CCYY, date trade inserted/deleted
    CTradeTime = 6, // HH:MM:SS.ffffff, Time (including microseconds) trade inserted/deleted
    CTradePrice = 7,
    CTradeSize = 8,
    CTickID = 9, // identifier of trade, probably matched update message
    CTradeConditions = 10, // 2 digit hex,
    CTradeMarketCenter = 11 // market center
  };

  IQFTradeCorrectionMessage() {}
  IQFTradeCorrectionMessage( iterator_t& current, iterator_t& end )
  : IQFBaseMessage<IQFTradeCorrectionMessage>( current, end ) {}
  ~IQFTradeCorrectionMessage() {}


protected:
private:
};

//**** IQFFundamentalMessage
class IQFFundamentalMessage: public IQFBaseMessage<IQFFundamentalMessage> { // F
public:

  enum enumFieldIds {
    FSymbol = 2,
    fExchangeID = 3,
    FPriceEarnings = 4,
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
    FCurYrEPS = 15,
    FNxtYrEPS = 16,
    FFiveYearGrowth = 17,
    FFiscalYrEnd = 18,
    FCompanyName = 19,
    FRootOptionSymbols = 20,
    FPctInst = 21,
    FBeta = 22,
    FLeaps = 23,
    FCurAssets = 24,
    FCurLiab = 25,
    FBalShtDate = 26,
    FLongTermDebt = 27,
    FCommonShares = 28,  // 1000's of shares
    FFormatCode = 31,
    FPrecision = 32,
    FSIC = 33,
    FVolatility = 34,
    FSecurityType = 35,
    FListedMarket = 36,
    F52WkHiDate = 37,
    F52WkLoDate = 38,
    FCalYrHiDate = 39,
    FCalYrLoDate = 40,
    FYearEndClose = 41,
    FBondMaturityDate = 42,
    FBondCouponRate = 43,
    FExpirationDate = 44,
    FStrikePrice = 45,
    FNAICS = 46,
    FExchangeRoot = 47,
    FOptionsPremiumMult = 48,     // options
    FOptionsMultipleDeliver = 49, // options
    FSessionOpenTime = 50,  // futures & futures options
    FSessionCloseTime = 51, // futures & futures options
    FBaseCurrency = 52,     // futures & futures options
    FContractSize = 53,     // Deliverable quantitiy of a future or option contract.
    FContractMonths = 54,
    FMinimumTickSize = 55,
    FFirstDeliveryDate = 56,// futures & futures options
    FFinancialInstrumentGlobalIdentifier = 57,
    FSecuritySubType = 58,
    _FLastEntry
  };

  IQFFundamentalMessage( void );
  IQFFundamentalMessage( iterator_t& current, iterator_t& end );
  ~IQFFundamentalMessage(void);

protected:
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

  using iterator_t = typename IQFBaseMessage<IQFPricingMessage<T, charT> >::iterator_t;
  using fielddelimiter_t = typename IQFBaseMessage<IQFPricingMessage<T, charT> >::fielddelimiter_t;

  IQFPricingMessage( void );
  IQFPricingMessage( iterator_t& current, iterator_t& end );

  ptime LastTradeTime( void ) const;

protected:
  ~IQFPricingMessage(void);
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

protected:
private:
};

// *******

//**** IQFDynamicFeedMessage ( root for IQFDynamicFeedSummaryMessage, IQFDynamicFeedUpdateMessage)
template <class T, class charT = unsigned char>
class IQFDynamicFeedMessage: public IQFBaseMessage<IQFDynamicFeedMessage<T, charT> > { // Q, P
public:

  enum enumFieldIds {
    DFSymbol = 2,
    DFTtlVol = 3,
    DFBid = 4,
    DFAsk = 5,
    DFBidSize = 6,
    DFAskSize = 7,
    DFNumTrades = 8,
    DFMostRecentTrade = 9,
    DFMostRecentTradeSize = 10,
    DFMostRecentTradeTime = 11,
    DFMostRecentTradeConditions = 12,
    DFMostRecentTradeMarketCenter = 13,
    DFMessageContents = 14,
    DFMostRecentTradeAggressor = 15,
    DFOpenInterest = 16,
    _DFLastEntry
  };

  using iterator_t = typename IQFBaseMessage<IQFDynamicFeedMessage<T, charT> >::iterator_t;
  using fielddelimiter_t = typename IQFBaseMessage<IQFDynamicFeedMessage<T, charT> >::fielddelimiter_t;

  static const std::string selector;

  IQFDynamicFeedMessage( void )
  : IQFBaseMessage<IQFDynamicFeedMessage<T, charT> >() {}
  IQFDynamicFeedMessage( iterator_t& current, iterator_t& end )
  : IQFBaseMessage<IQFDynamicFeedMessage<T, charT> >( current, end ) {}

protected:
  ~IQFDynamicFeedMessage(void){}
private:
};

template <class T, class charT>
const std::string IQFDynamicFeedMessage<T, charT>::selector( "Symbol,Total Volume,Bid,Ask,Bid Size,Ask Size,Number of Trades Today,Most Recent Trade,Most Recent Trade Size,Most Recent Trade Time,Most Recent Trade Conditions,Most Recent Trade Market Center,Message Contents,Most Recent Trade Aggressor,Open Interest" );
//                                     S,SELECT UPDATE FIELDS,Symbol,Total Volume,Bid,Ask,Bid Size,Ask Size,Number of Trades Today,Most Recent Trade,Most Recent Trade Size,Most Recent Trade Time,Most Recent Trade Conditions,Most Recent Trade Market Center,Message Contents,Most Recent Trade Aggressor
// S,SET PROTOCOL,6.1
// rTST$Y
// http://www.iqfeed.net/dev/api/docs/Level1UpdateSummaryMessage.cfm

//**** IQFDynamicFeedSummaryMessage
class IQFDynamicFeedSummaryMessage: public IQFDynamicFeedMessage<IQFDynamicFeedSummaryMessage> { // P
public:

  IQFDynamicFeedSummaryMessage( void )
  : IQFDynamicFeedMessage<IQFDynamicFeedSummaryMessage>() {}
  IQFDynamicFeedSummaryMessage( iterator_t& current, iterator_t& end )
  : IQFDynamicFeedMessage<IQFDynamicFeedSummaryMessage>( current, end ) {}
  ~IQFDynamicFeedSummaryMessage(void){}

protected:
private:
};

//**** IQFDynamicFeedUpdateMessage
class IQFDynamicFeedUpdateMessage: public IQFDynamicFeedMessage<IQFDynamicFeedUpdateMessage> { // Q
public:

  IQFDynamicFeedUpdateMessage( void )
  : IQFDynamicFeedMessage<IQFDynamicFeedUpdateMessage>() {}
  IQFDynamicFeedUpdateMessage( iterator_t& current, iterator_t& end )
  : IQFDynamicFeedMessage<IQFDynamicFeedUpdateMessage>( current, end ) {}
  ~IQFDynamicFeedUpdateMessage(void){}

protected:
private:
};

// *******

//**** IQFDynamicInfoBaseMessage ( root for IQFDynamicInfoSummaryMessage, IQFDynamicInfoUpdateMessage)
template <class T, class charT = unsigned char>
class IQFDynamicInfoMessage: public IQFBaseMessage<IQFDynamicInfoMessage<T, charT> > { // Q, P
public:

  enum enumFieldIds {
    DISymbol = 2,
    DIOpenInterest = 3,
    DIOpen = 4,
    DISettle = 5,
    DIDelay = 6,
    DIRestrictedCode = 7,
    DINetAssetValue = 8,
    DIAverageMaturity = 9,
    DI7DayYield = 10,
    DIPriceEarningsRatio = 11,
    DIMarketCapitalization = 12,
    DIFractionDisplayCode = 13,
    DIDecimalPrecision = 14,
    DIDaysToExpiration = 15,
    DIPreviousDayVolume = 16,
    DIFinancialStatusIndicator = 17,
    DISettlementDate = 18,
    DIAvailableRegions = 19,
    _DILastEntry
  };

  using iterator_t = typename IQFBaseMessage<IQFDynamicInfoMessage<T, charT> >::iterator_t;
  using fielddelimiter_t = typename IQFBaseMessage<IQFDynamicInfoMessage<T, charT> >::fielddelimiter_t;

  static const std::string selector;

  IQFDynamicInfoMessage();
  IQFDynamicInfoMessage( iterator_t& current, iterator_t& end );
  ~IQFDynamicInfoMessage();

protected:
private:
};

template <class T, class charT>
const std::string IQFDynamicInfoMessage<T, charT>::selector( "Symbol,Open Interest,Open,Settle,Delay,Restricted Code,Net Asset Value,Average Maturity,7 Day Yield,Price-Earnings Ratio,Market Capitalization,Fraction Display Code,Decimal Precision,Days to Expiration,Previous Day Volume,Financial Status Indicator,Settlement Date,Available Regions" );
//                                     S,SELECT UPDATE FIELDS,Symbol,Open Interest,Open,Settle,Delay,Restricted Code,Net Asset Value,Average Maturity,7 Day Yield,Price-Earnings Ratio,Market Capitalization,Fraction Display Code,Decimal Precision,Days to Expiration,Previous Day Volume,Financial Status Indicator,Settlement Date,Available Regions

//**** IQFDynamicInfoSummaryMessage
class IQFDynamicInfoSummaryMessage: public IQFDynamicInfoMessage<IQFDynamicInfoSummaryMessage> { // P
public:

  IQFDynamicInfoSummaryMessage();
  IQFDynamicInfoSummaryMessage( iterator_t& current, iterator_t& end );
  ~IQFDynamicInfoSummaryMessage();

protected:
private:
};

//**** IQFDynamicInfoUpdateMessage
class IQFDynamicInfoUpdateMessage: public IQFDynamicInfoMessage<IQFDynamicInfoUpdateMessage> { // Q
public:

  IQFDynamicInfoUpdateMessage();
  IQFDynamicInfoUpdateMessage( iterator_t& current, iterator_t& end );
  ~IQFDynamicInfoUpdateMessage();

protected:
private:
};

// *******

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
const std::string IQFBaseMessage<T, charT>::Field( ixFields_t fld ) const {
  std::string sField;
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    sField.assign( fielddelimiter.first, fielddelimiter.second );
  }
  return sField;
}

template <class T, class charT>
double IQFBaseMessage<T, charT>::Double( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  double value {};
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    value = parse_double( fielddelimiter );
  }

  return value;
}

template <class T, class charT>
int IQFBaseMessage<T, charT>::Integer( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  int value {};
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    value = parse_int( fielddelimiter );
  }

  return value;
}

template <class T, class charT>
date IQFBaseMessage<T, charT>::Date( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  date d( 9999, 9, 9 );
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    auto diff = fielddelimiter.second - fielddelimiter.first ;
    if ( 10 == diff ) {
      d = parse_date( fielddelimiter );
    }
  }
  return d;
}

template <class T, class charT>
time IQFBaseMessage<T, charT>::Time( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );

  time t( boost::posix_time::not_a_date_time );
  fielddelimiter_t fielddelimiter = m_vFieldDelimiters[ fld ];
  if ( fielddelimiter.first != fielddelimiter.second ) {
    if ( 8 == ( fielddelimiter.second - fielddelimiter.first ) ) {
      t = parse_time( fielddelimiter );
    }
  }
  return t;
}

template <class T, class charT>
typename IQFBaseMessage<T, charT>::iterator_t IQFBaseMessage<T, charT>::FieldBegin( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  return m_vFieldDelimiters[ fld ].first;
}

template <class T, class charT>
typename IQFBaseMessage<T, charT>::iterator_t IQFBaseMessage<T, charT>::FieldEnd( ixFields_t fld ) const {
  BOOST_ASSERT( 0 != fld );
  BOOST_ASSERT( fld <= m_vFieldDelimiters.size() - 1 );
  return m_vFieldDelimiters[ fld ].second;
}

namespace qi = boost::spirit::qi;

template <class T, class charT>
int IQFBaseMessage<T, charT>::parse_int( fielddelimiter_t fd ) const {

  using namespace boost::phoenix::arg_names;

  using boost::spirit::qi::_1;
  using boost::phoenix::ref;
  using namespace boost::spirit::qi;

  int value {};

  bool b = qi::parse( fd.first, fd.second, int_[ref( value ) = _1] );

  return value;
}

template <class T, class charT>
double IQFBaseMessage<T, charT>::parse_double( fielddelimiter_t fd ) const {

  using namespace boost::phoenix::arg_names;

  using boost::spirit::qi::_1;
  using boost::phoenix::ref;
  using namespace boost::spirit::qi;

  double value {};

  bool b = qi::parse( fd.first, fd.second, double_[ref( value ) = _1] );
  return value;
}

template<typename Iterator>
struct DateParser: qi::grammar<Iterator, date_values()> {

  DateParser(): DateParser::base_type( start ) {

    value %= qi::uint_;
    start %= value >> qi::lit( '/') >> value >> qi::lit( '/' ) >> value; // mm/dd/yyyy

  }

  qi::rule<Iterator, uint16_t()> value;
  qi::rule<Iterator, date_values()> start;
};

template <class T, class charT>
date IQFBaseMessage<T, charT>::parse_date( fielddelimiter_t fd ) const {

  date_values values;
  DateParser<iterator_t> grammarDateParser;
  date value( boost::posix_time::not_a_date_time );

  bool bOk = parse( fd.first, fd.second, grammarDateParser, values );

  try {
    if ( bOk ) value = date( values.year, values.month, values.day );
  }
  catch (...) {
    std::string s( fd.first, fd.second );
    std::cout << "IQFBaseMessage<T, charT>::parse_date ill formed date: " + s << std::endl;
  }

  return value;
}

template<typename Iterator>
struct TimeParser: qi::grammar<Iterator, time_values()> {

  TimeParser(): TimeParser::base_type( start ) {

    value %= qi::uint_;
    start %= value >> qi::lit( ':') >> value >> qi::lit( ':' ) >> value; // hh:mm:ss

  }

  qi::rule<Iterator, uint16_t()> value;
  qi::rule<Iterator, time_values()> start;
};

template <class T, class charT>
time IQFBaseMessage<T, charT>::parse_time( fielddelimiter_t fd ) const {

  time_values values {};
  TimeParser<iterator_t> grammarTimeParser;
  time value( boost::posix_time::not_a_date_time );

  bool bOk = parse( fd.first, fd.second, grammarTimeParser, values );
  value = time( values.hours, values.minutes, values.seconds );

  return value;
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
ptime IQFPricingMessage<T, charT>::LastTradeTime( void ) const {

  // use CurrencyTrader/Config.cpp as Boost::Spirit mechanism to build ptime

  // TODO: test that the delimiters are available (ie message might be truncated?)
  fielddelimiter_t date = this->m_vFieldDelimiters[ QPLastTradeDate ];
  fielddelimiter_t time = this->m_vFieldDelimiters[ QPLastTradeTime ];

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

} // namespace iqfeed
} // namespace tf
} // namespace ou

