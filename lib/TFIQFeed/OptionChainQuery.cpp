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

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <sstream>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include "OptionChainQuery.h"

// TODO:  use as template:  std::unique_ptr<DailyHistory> m_pHistory;

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::OptionChainQuery::OptionChain,
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vCall)
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vPut)
  )

namespace qi = boost::spirit::qi;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace {

  using const_iterator_t = ou::Network<OptionChainQuery>::linebuffer_t::const_iterator;

  qi::rule<const_iterator_t> ruleEndMsg { qi::lit( "!ENDMSG!" ) };
  qi::rule<const_iterator_t> ruleErrorInvalidSymbol { qi::lit( "E,Invalid symbol" ) };

}

using vSymbol_t = OptionChainQuery::vSymbol_t;
using OptionChain = OptionChainQuery::OptionChain;

template<typename Iterator>
struct FutureOptionChainParser: qi::grammar<Iterator, OptionChain()> {

  FutureOptionChainParser(): FutureOptionChainParser::base_type( start ) {

    // TODO: need to process !ENDMSG!
    // TODO: need to process error message

    symbol %= (+(qi::char_ - qi::char_(','))) >> qi::lit(',');
    options %= +symbol;
    start %= options >> qi::lit(':') >> options >> qi::eol;

  }

  qi::rule<Iterator, std::string()> symbol;
  qi::rule<Iterator, vSymbol_t()> options;
  qi::rule<Iterator, OptionChain()> start;

};

// http://www.iqfeed.net/dev/api/docs/OptionChainsviaTCPIP.cfm

OptionChainQuery::OptionChainQuery(
  fConnected_t&& fConnected, fOptionChain_t&& fOptionChain, fDone_t&& fDone
)
: Network<OptionChainQuery>( "127.0.0.1", 9100 ),
  m_fConnected( std::move( fConnected ) ),
  m_fOptionChain( std::move( fOptionChain ) ),
  m_fDone( std::move( fDone ) )
{
  assert( m_fConnected );
  assert( m_fOptionChain );
  assert( m_fDone );
}

OptionChainQuery::~OptionChainQuery(void) {
}

void OptionChainQuery::Connect() {
  ou::Network<OptionChainQuery>::Connect();
}

void OptionChainQuery::OnNetworkConnected() {
  //std::cout << "OnHistoryConnected" << std::endl;
  m_fConnected();
};

void OptionChainQuery::Disconnect() {
  ou::Network<OptionChainQuery>::Disconnect();
}

void OptionChainQuery::OnNetworkDisconnected() {

}

void OptionChainQuery::OnNetworkError( size_t e ) {

}

void OptionChainQuery::OnNetworkSendDone() {

}

void OptionChainQuery::OnNetworkLineBuffer( linebuffer_t* buffer ) {

  using const_iterator_t = linebuffer_t::const_iterator;

  const_iterator_t end = (*buffer).end();

  FutureOptionChainParser<const_iterator_t> grammarFutureOptionChain;

  OptionChain chain;
  bool bOk;

  {
    const_iterator_t bgn = (*buffer).begin();
    bOk = parse( bgn, end, grammarFutureOptionChain, chain );
  }

  if ( bOk ) {
    m_fOptionChain( chain );
  }
  else {
    const_iterator_t bgn = (*buffer).begin();
    if ( parse( bgn, end, ruleEndMsg ) ) {
      m_fDone( true );
    }
    else {
      m_fDone( false );
      //if ( parse( bgn, end, ruleErrorInvalidSymbol ) ) {
    }
  }

  GiveBackBuffer( buffer );
}

void OptionChainQuery::QueryFutureChain(
    const std::string& sSymbol,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  ss
    << "CFU,"
    << sSymbol << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryFutureOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  //boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
  ss
    << "CFO,"
    << sSymbol << ","
    << sSide << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryEquityOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sNearMonths,
    const std::string& sFilterType,
    const std::string& sFilterOne,
    const std::string& sFilterTwo,
    const std::string& sRequestId
    ) {
  std::stringstream ss;
  //boost::this_thread::sleep( boost::posix_time::milliseconds( m_nMillisecondsToSleep ) );
  ss
    << "CEO,"
    << sSymbol << ","
    << sSide << ","
    << sMonthCodes << ","
    << sNearMonths << ","
    << "1" << "," // exclude binary
    << sFilterType << ","
    << sFilterOne << ","
    << sFilterTwo << ","
    << sRequestId
    << "\n";
  this->Send( ss.str().c_str() );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
