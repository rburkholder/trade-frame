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

#include <sstream>

#include <boost/phoenix/core.hpp>

#include <boost/fusion/include/vector.hpp>

#include <boost/spirit/include/qi.hpp>

#include "Messages.h"

// News Message: This does appear to be an error in the documentation.
// "SNT:AAPL::1:20070901:;"
// Enter the date with a range, such as "20070901-20070910" to get the data you are looking for.

// https://www.boost.org/doc/libs/master/libs/spirit/example/qi/parse_date.cpp
// define custom transformation
namespace boost { namespace spirit { namespace traits {
  template<>
  struct transform_attribute<
      boost::gregorian::date, fusion::vector<unsigned short, unsigned short, unsigned short>
    , qi::domain
    >
  {
      typedef fusion::vector<unsigned short, unsigned short, unsigned short> date_parts;

      typedef date_parts type;

      static date_parts pre( boost::gregorian::date ) {
        return date_parts();
      }

      static void post( boost::gregorian::date& date, date_parts const& v )
      {
        date = boost::gregorian::date( // mm/dd/yyyy
              fusion::at_c<2>(v)  // year
            , fusion::at_c<0>(v)  // month
            , fusion::at_c<1>(v)) // day
            ;
      }

      static void fail( boost::gregorian::date& ) {}
  };
}}}

// https://www.boost.org/doc/libs/master/libs/spirit/example/qi/parse_date.cpp
// define custom transformation
namespace boost { namespace spirit { namespace traits {
  template<>
  struct transform_attribute<
      boost::posix_time::time_duration, fusion::vector<unsigned short, unsigned short, unsigned short>
    , qi::domain
    >
  {
      typedef fusion::vector<unsigned short, unsigned short, unsigned short> time_parts;

      typedef time_parts type;

      static time_parts pre( boost::posix_time::time_duration ) {
        return time_parts();
      }

      static void post( boost::posix_time::time_duration& td, time_parts const& v )
      {
        td = boost::posix_time::time_duration(
              fusion::at_c<0>(v)
            , fusion::at_c<1>(v)
            , fusion::at_c<2>(v))
            ;
      }

      static void fail( boost::posix_time::time_duration& ) {}
  };
}}}

namespace {

namespace qi = boost::spirit::qi;

struct date_time {
  boost::gregorian::date date;
  boost::posix_time::time_duration time;
};

template<typename Iterator>
struct ParserDate: qi::grammar<Iterator, boost::gregorian::date()> {
  ParserDate(): ParserDate::base_type( ruleDate ) {

    ruleValue %= qi::ushort_;
    ruleDate %= ruleValue >> qi::lit( '/' )
             >> ruleValue >> qi::lit( '/' )
             >> ruleValue
             ;
  }

  typedef boost::fusion::vector<unsigned short, unsigned short, unsigned short> date_parts;

  qi::rule<Iterator, unsigned short()> ruleValue;
  qi::rule<Iterator, date_parts()> ruleDate;
};

template<typename Iterator>
struct ParserTime: qi::grammar<Iterator, boost::posix_time::time_duration()> {
  ParserTime(): ParserTime::base_type( ruleTime ) {

    ruleValue %= qi::ushort_;
    ruleTime %= ruleValue >> qi::lit( ':' )
             >> ruleValue >> qi::lit( ':' )
             >> ruleValue
             ;

  }

  typedef boost::fusion::vector<unsigned short, unsigned short, unsigned short> time_parts;

  qi::rule<Iterator, unsigned short()> ruleValue;
  qi::rule<Iterator, time_parts()> ruleTime;
};

} // namespace

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

//**** IQFBaseMessage

// templated

//**** IQFSystemMessage

IQFSystemMessage::IQFSystemMessage( void )
: IQFBaseMessage<IQFSystemMessage>()
{
}

IQFSystemMessage::IQFSystemMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFSystemMessage>( current, end )
{
}

IQFSystemMessage::~IQFSystemMessage() {
}

//**** IQFErrorMessage

IQFErrorMessage::IQFErrorMessage( void )
: IQFBaseMessage<IQFErrorMessage>()
{
}

IQFErrorMessage::IQFErrorMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFErrorMessage>( current, end )
{
}

IQFErrorMessage::~IQFErrorMessage() {
}

//**** IQFNewsMessage

IQFNewsMessage::IQFNewsMessage( void )
: IQFBaseMessage<IQFNewsMessage>()
{
}

IQFNewsMessage::IQFNewsMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFNewsMessage>()
{
}

IQFNewsMessage::~IQFNewsMessage() {
}

//**** IQFFundamentalMessage
// resize the vector to accept with out resizing so often?

IQFFundamentalMessage::IQFFundamentalMessage( void )
: IQFBaseMessage<IQFFundamentalMessage>()
{
}

IQFFundamentalMessage::IQFFundamentalMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFFundamentalMessage>( current, end )
{
}

IQFFundamentalMessage::~IQFFundamentalMessage() {
}

//**** IQFUpdateMessage

IQFUpdateMessage::IQFUpdateMessage( void )
: IQFPricingMessage<IQFUpdateMessage>()
{
}

IQFUpdateMessage::IQFUpdateMessage( iterator_t& current, iterator_t& end )
: IQFPricingMessage<IQFUpdateMessage>( current, end )
{
}

IQFUpdateMessage::~IQFUpdateMessage() {
}

//**** IQFSummaryMessage

IQFSummaryMessage::IQFSummaryMessage( void )
: IQFPricingMessage<IQFSummaryMessage>()
{
}

IQFSummaryMessage::IQFSummaryMessage( iterator_t& current, iterator_t& end )
: IQFPricingMessage<IQFSummaryMessage>( current, end )
{
}

IQFSummaryMessage::~IQFSummaryMessage() {
}

//**** IQFTimeMessage

IQFTimeMessage::IQFTimeMessage( void )
: IQFBaseMessage<IQFTimeMessage>(),
  m_bMarketIsOpen( false ),
  m_timeMarketOpen( time( 9, 30, 0 ) ), m_timeMarketClose( time( 16, 0, 0 ) )
{
}

IQFTimeMessage::IQFTimeMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFTimeMessage>(),
  m_bMarketIsOpen( false ),
  m_timeMarketOpen( time( 9, 30, 0 ) ), m_timeMarketClose( time( 16, 0, 0 ) )
{
  Assign( current, end );
}

IQFTimeMessage::~IQFTimeMessage() {
}

void IQFTimeMessage::Assign(iterator_t &current, iterator_t &end) {
  IQFBaseMessage<IQFTimeMessage>::Assign( current, end );
  std::stringstream ss( Field( 2 ) );
  boost::posix_time::time_input_facet *input_facet;
  input_facet = new boost::posix_time::time_input_facet();  // input facet stuff needs to be with ss.imbue, can't be reused
  input_facet->format( "%Y%m%d %H:%M:%S" );
  ss.imbue( std::locale( ss.getloc(), input_facet ) );
  ss >> m_dt;
  m_bMarketIsOpen = ( ( m_dt.time_of_day() >= m_timeMarketOpen ) && ( m_dt.time_of_day() < m_timeMarketClose ) );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
