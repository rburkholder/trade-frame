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
  (std::string, sKey)
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vOption)
  //(ou::tf::iqfeed::OptionChainQuery::vCall_t, vCall)
  //(ou::tf::iqfeed::OptionChainQuery::vPut_t, vPut)
  )

namespace qi = boost::spirit::qi;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace {

  using const_iterator_t = ou::Network<OptionChainQuery>::linebuffer_t::const_iterator;

  qi::rule<const_iterator_t> ruleErrorInvalidSymbol { qi::lit( "E,Invalid symbol" ) };

}

using OptionChain = OptionChainQuery::OptionChain;

template<typename Iterator>
struct FutureOptionChainParser: qi::grammar<Iterator, OptionChain()> {

  FutureOptionChainParser(): FutureOptionChainParser::base_type( start ) {

    symbol %= (+(qi::char_ - qi::char_(",:"))) >> qi::lit(',');
    //calls %= +symbol;
    //puts %= +symbol;
    options %= +symbol;
    //start %= symbol >> calls >> qi::lit(':') >> qi::lit(',') >> puts >> qi::eoi;
    start %= symbol >> options >> qi::lit(':') >> qi::lit(',') >> options >> qi::eoi;

  }

  qi::rule<Iterator, std::string()> symbol;
  //qi::rule<Iterator, OptionChainQuery::vCall_t()> calls;
  //qi::rule<Iterator, OptionChainQuery::vPut_t()> puts;
  qi::rule<Iterator, OptionChainQuery::vSymbol_t()> options;
  qi::rule<Iterator, OptionChain()> start;

};

template<typename Iterator>
struct EndMessageParser: qi::grammar<Iterator, std::string()> {

  EndMessageParser(): EndMessageParser::base_type( start ) {

    // TODO: need to process !ENDMSG!
    // TODO: need to process error message

    start %= (+(qi::char_ - qi::char_(","))) >> qi::lit( ",!ENDMSG!," );

  }

  qi::rule<Iterator, std::string()> start;

};

// http://www.iqfeed.net/dev/api/docs/OptionChainsviaTCPIP.cfm

OptionChainQuery::OptionChainQuery(
  fConnected_t&& fConnected
)
: Network<OptionChainQuery>( "127.0.0.1", 9100 ),
  m_fConnected( std::move( fConnected ) ),
  m_state( EState::quiescent )
{
  assert( m_fConnected );
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

// TODO: will need to perform a pre-parse to obtain the key
//   then lookup in map for state
//   then full parse for appropriate result

void OptionChainQuery::OnNetworkLineBuffer( linebuffer_t* buffer ) {

  using const_iterator_t = linebuffer_t::const_iterator;

  const_iterator_t end = (*buffer).end();

  OptionChain chain;
  bool bOk;

  std::lock_guard<std::mutex> lock( m_mutexMapRequest );

  switch ( m_state ) {
    case EState::quiescent:
      //std::cout << "EState::quiescent" << std::endl;
      break;
    case EState::reply:
      {
        //std::cout << "EState::reply" << std::endl;
        FutureOptionChainParser<const_iterator_t> grammarFutureOptionChain;
        const_iterator_t bgn = (*buffer).begin();
        bOk = parse( bgn, end, grammarFutureOptionChain, chain );

        if ( bOk ) {
          //std::cout << "bOk true" << std::endl;
          assert( 0 < chain.sKey.size() );
        }
        else {
          std::cout
            << "OptionChainQuery::OnNetworkLineBuffer parse error: "
            << chain.sKey
            << "'," << chain.vOption.size()
            //<< "'," << chain.vCall.size()
            //<< "," << chain.vPut.size()
            << std::endl;
          const_iterator_t bgn = (*buffer).begin();
          const std::string sContent( bgn, end ); // debug statement
          std::cout << sContent << std::endl;
          // TODO: match the error message
          //   but we are parsing errors at this point
        }

        if ( 0 < chain.sKey.size() ) {
          mapRequest_t::const_iterator iter = m_mapRequest.find( chain.sKey );
          if ( m_mapRequest.end() == iter ) {
            std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find key " << chain.sKey << std::endl;
          }
          else {
            iter->second( chain );
          }
        }

        //if ( parse( bgn, end, ruleErrorInvalidSymbol ) ) {}

        m_state = EState::done;
      }
      break;
    case EState::done:
      {
        //std::cout << "EState::done" << std::endl;
        const_iterator_t bgn = (*buffer).begin();
        std::string sKey;
        EndMessageParser<const_iterator_t> grammarEndMsg;
        if ( parse( bgn, end, grammarEndMsg, sKey ) ) { // 'QGCZ21,!ENDMSG!,'
          //std::cout << "parsed for key: " << sKey << std::endl;
          mapRequest_t::const_iterator iter = m_mapRequest.find( sKey );
          if ( m_mapRequest.end() == iter ) {
            const_iterator_t bgn = (*buffer).begin();
            const std::string sContent( bgn, end ); // debug statement
            std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find ending key '" << sKey << "' in '" << sContent << "'" << std::endl;
          }
          else {
            m_mapRequest.erase( iter );  // may need to do this on endmsg only
          }
        }
        else {
          const_iterator_t bgn = (*buffer).begin();
          const std::string sContent( bgn, end ); // debug statement
          std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't parse: '" << sContent << "'" << std::endl;
        }

        m_state = EState::quiescent;
      }
      break;
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
    << ( 0 == sRequestId.size() ? sSymbol : sRequestId )
    << "\n";
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryFutureOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    const std::string& sRequestId,
    fOptionChain_t&& fOptionChain
    ) {
  assert( 0 < sSymbol.size() );
  const std::string sMapKey(0 == sRequestId.size() ? sSymbol : sRequestId );
  std::stringstream ss;
  ss
    << "CFO,"
    << sSymbol << ","
    << sSide << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << sMapKey
    << "\n";
  std::lock_guard<std::mutex> lock( m_mutexMapRequest );
  m_mapRequest.emplace( mapRequest_t::value_type( sMapKey, std::move( fOptionChain ) ) );
  m_state = EState::reply;
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
    << ( 0 == sRequestId.size() ? sSymbol : sRequestId )
    << "\n";
  this->Send( ss.str().c_str() );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
