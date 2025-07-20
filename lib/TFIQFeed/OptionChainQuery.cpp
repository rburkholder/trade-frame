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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include "OptionChainQuery.h"

struct PreRoll {
  enum class ECmd { CFU, CFO, CEO, S };
  enum class EExtra { LC, ENDMSG, BADSYM, ERROR, V62 };
  ECmd cmd;
  std::string sSymbol;
  EExtra extra;
};

using FuturesList = ou::tf::iqfeed::OptionChainQuery::FuturesList;
using OptionList = ou::tf::iqfeed::OptionChainQuery::OptionList;

BOOST_FUSION_ADAPT_STRUCT(
  PreRoll,
  (PreRoll::ECmd, cmd)
  (std::string, sSymbol)
  (PreRoll::EExtra, extra)
)

BOOST_FUSION_ADAPT_STRUCT(
  FuturesList,
  //(std::string, sKey)
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vSymbol)
  //(ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vCall)
  //(ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vPut)
  )

BOOST_FUSION_ADAPT_STRUCT(
  OptionList,
  //(std::string, sKey)
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vSymbol) // calls first
  (ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vSymbol) // puts appended
  //(ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vCall)
  //(ou::tf::iqfeed::OptionChainQuery::vSymbol_t, vPut)
  )

namespace qi = boost::spirit::qi;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

// ==

template<typename Iterator>
struct PreRollParser: qi::grammar<Iterator, PreRoll()> {

  PreRollParser(): PreRollParser::base_type( start ) {

    cmd.add
      ( "CFU", PreRoll::ECmd::CFU )
      ( "CFO", PreRoll::ECmd::CFO )
      ( "CEO", PreRoll::ECmd::CEO )
      ( "S",   PreRoll::ECmd::S   )
      ;

    extra.add
      ( "LC", PreRoll::EExtra::LC )
      ( "n", PreRoll::EExtra::BADSYM )
      ( "E", PreRoll::EExtra::ERROR )
      ( "!ENDMSG!", PreRoll::EExtra::ENDMSG )
      ( "6.2", PreRoll::EExtra::V62 )
      ;

    ruleCmd = cmd;
    ruleExtra = extra;
    ruleSymbol = +( qi::char_ - qi::char_( ',' ) );

    start
      %=
        ( ruleCmd >> qi::lit( '-' ) >> ruleSymbol >> qi::lit( ',' ) >> ruleExtra >> qi::lit( ',' ) )
      | ( ruleCmd >> qi::lit( ',' ) >> ruleSymbol >> qi::lit( ',' ) >> ruleExtra )
      ;
  }

  qi::symbols<char,PreRoll::ECmd> cmd;
  qi::symbols<char,PreRoll::EExtra> extra;

  qi::rule<Iterator, PreRoll::ECmd()> ruleCmd;
  qi::rule<Iterator, std::string()> ruleSymbol;
  qi::rule<Iterator, PreRoll::EExtra()> ruleExtra;
  qi::rule<Iterator, PreRoll()> start;
};

// ===

template<typename Iterator>
struct FutureChainParser: qi::grammar<Iterator, FuturesList()> {

  FutureChainParser(): FutureChainParser::base_type( start ) {

    symbol %= (+(qi::char_ - qi::char_(",:"))) >> qi::lit(',');
    symbols %= +symbol;
    start
      %= symbols
      //>> qi::eoi
      ;

  }

  qi::rule<Iterator, std::string()> symbol;
  qi::rule<Iterator, OptionChainQuery::vSymbol_t()> symbols;
  qi::rule<Iterator, FuturesList()> start;

};

// ===

template<typename Iterator>
struct OptionChainParser: qi::grammar<Iterator, OptionList()> {

  OptionChainParser(): OptionChainParser::base_type( start ) {

    symbol %= (+(qi::char_ - qi::char_(",:"))) >> qi::lit(',');
    calls %= +symbol;
    puts %= +symbol;
    start
      %= calls
      >> -(qi::lit(',')) >> qi::lit(':') >> qi::lit(',')
      >> puts
      //>> qi::eoi
      ;

  }

  qi::rule<Iterator, std::string()> symbol;
  qi::rule<Iterator, OptionChainQuery::vSymbol_t()> calls;
  qi::rule<Iterator, OptionChainQuery::vSymbol_t()> puts;
  qi::rule<Iterator, OptionList()> start;

};

// ==

// http://www.iqfeed.net/dev/api/docs/OptionChainsviaTCPIP.cfm

OptionChainQuery::OptionChainQuery(
  fConnected_t&& fConnected
)
: Network<OptionChainQuery>( "127.0.0.1", 9100 )
, m_fConnected( std::move( fConnected ) )
, m_state( EState::quiescent )
{
  assert( m_fConnected );
}

OptionChainQuery::~OptionChainQuery() {
}

void OptionChainQuery::Connect() {
  ou::Network<OptionChainQuery>::Connect();
}

void OptionChainQuery::OnNetworkConnected() {
  //std::cout << "OnHistoryConnected" << std::endl;
  this->Send( "S,SET PROTOCOL,6.2\n" );
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

// 2021/10/29 - amusing response to a query via the new servers:
// "@ESZ21,grep: /data/online/data/commodities/options/underlying/@ESZ21: No such file or directory"

void OptionChainQuery::OnNetworkLineBuffer( linebuffer_t* buffer ) {

  using const_iterator_t = linebuffer_t::const_iterator;

  const_iterator_t iter = (*buffer).begin();
  const_iterator_t end = (*buffer).end();

  //std::string s( iter, end );
  //std::cout << "chain response: " << s << std::endl;

  bool bOk;

  switch ( m_state ) {
    case EState::quiescent:
      //std::cout << "EState::quiescent" << std::endl;
      // fall through, simply process all responses
    case EState::response:
      {

        PreRoll preroll;
        PreRollParser<const_iterator_t> grammarPreRoll;
        iter = (*buffer).begin();
        bOk = parse( iter, end, grammarPreRoll,preroll );

        if ( bOk ) {
          switch ( preroll.extra ) {
            case PreRoll::EExtra::LC:
              switch ( preroll.cmd ) {
                case PreRoll::ECmd::CFU:
                  {

                    FuturesList list;

                    //std::cout << "EState::reply" << std::endl;
                    FutureChainParser<const_iterator_t> grammarFutureChain;
                    //const_iterator_t bgn = (*buffer).begin();

                    //std::string buf( iter, end );
                    //std::cout << "buf: '" << buf << "'" << std::endl;

                    bOk = parse( iter, end, grammarFutureChain, list );

                    assert( 0 < preroll.sSymbol.size() );

                    if ( bOk ) {
                      assert( 0 < preroll.sSymbol.size() );
                    }
                    else {
                      std::cout
                        << "OptionChainQuery::OnNetworkLineBuffer CFU parse error: "
                        << end - iter << ","
                        << preroll.sSymbol
                        << "'," << list.vSymbol.size()
                        << std::endl;
                    }

                    bool bProcess( false );
                    mapFutures_t::const_iterator citer;
                    {
                      std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
                      citer = m_mapFutures.find( preroll.sSymbol );
                      if ( m_mapFutures.end() == citer ) {
                      }
                      else {
                        bProcess = true;
                      }
                    }

                    if ( bProcess ) {
                      citer->second( list ); // this needs to be outside of lock
                      std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
                      m_mapFutures.erase( citer );
                    }
                    else {
                      std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find CFU key " << preroll.sSymbol << std::endl;
                    }
                  }
                  break;
                case PreRoll::ECmd::CEO:
                case PreRoll::ECmd::CFO:
                  {

                    OptionList list;

                    //std::cout << "EState::reply" << std::endl;
                    OptionChainParser<const_iterator_t> grammarOptionChain;
                    //const_iterator_t bgn = (*buffer).begin();

                    //std::string buf( iter, end );
                    //std::cout << "buf: '" << buf << "'" << std::endl;

                    bOk = parse( iter, end, grammarOptionChain, list );

                    assert( 0 < preroll.sSymbol.size() );

                    if ( bOk ) {
                      assert( 0 < preroll.sSymbol.size() );
                      list.sUnderlying = std::move( preroll.sSymbol );
                    }
                    else {
                      std::cout
                        << "OptionChainQuery::OnNetworkLineBuffer CEO/CFO parse error: "
                        << end - iter << ","
                        << preroll.sSymbol
                        << "'," << list.vSymbol.size()
                        << std::endl;
                    }

                    bool bProcess( false );
                    mapOptions_t::const_iterator citer;
                    {
                      std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
                      citer = m_mapOptions.find( list.sUnderlying );
                      if ( m_mapOptions.end() == citer ) {
                      }
                      else {
                        bProcess = true;
                      }
                    }

                    if ( bProcess ) {
                      citer->second( list );  // this needs to be outside of lock
                      std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
                      m_mapOptions.erase( citer );
                    }
                    else {
                      std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find CEO key " << list.sUnderlying << std::endl;
                    }

                  }
                  break;
                case PreRoll::ECmd::S:
                  // ignore the response, simply an echo of version, maybe validate sometime
                  break;
              }
              break;
            case PreRoll::EExtra::V62:
              m_state = EState::quiescent;
              break;
            case PreRoll::EExtra::BADSYM:
              // TODO: remove from m_mapRequest
              std::cout << "OptionChainQuery::OnNetworkLineBuffer badsym: " << preroll.sSymbol << std::endl;
              m_state = EState::quiescent;
              break;
            case PreRoll::EExtra::ERROR:
              // TODO: remove from m_mapRequest
              std::cout << "OptionChainQuery::OnNetworkLineBuffer error: " << std::string( (*buffer).begin(), (*buffer).end() ) << std::endl;
              m_state = EState::quiescent;
              break;
            case PreRoll::EExtra::ENDMSG:
              m_state = EState::quiescent;
              break;
          }
        }
        else {
          std::cout << "OptionChainQuery::OnNetworkLineBuffer error: unknown response: " << std::string( (*buffer).begin(), (*buffer).end() ) << std::endl;
        }

      }
      break;
    case EState::done:
      {
        m_state = EState::quiescent;
      }
      break;
  }

  GiveBackBuffer( buffer );
}

void OptionChainQuery::QueryFuturesChain(
    const std::string& sSymbol,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    fFuturesList_t&& fFuturesList
) {
  assert( 0 < sSymbol.size() );
  assert( std::string::npos == sSymbol.find( ',' ) );
  std::stringstream ss;
  ss
    << "CFU,"
    << sSymbol << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << "CFU-" << sSymbol
    << "\n";
  std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
  assert( m_mapFutures.end() == m_mapFutures.find( sSymbol ) );
  m_mapFutures.emplace( mapFutures_t::value_type( sSymbol, std::move( fFuturesList ) ) );
  m_state = EState::response;
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryFuturesOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    fOptionList_t&& fOptionList
) {
  assert( 0 < sSymbol.size() );
  assert( std::string::npos == sSymbol.find( ',' ) );
  std::stringstream ss;
  ss
    << "CFO,"
    << sSymbol << ","
    << sSide << ","
    << sMonthCodes << ","
    << sYears << ","
    << sNearMonths << ","
    << "CFO-" << sSymbol
    ;
  std::cout << "request: '" << ss.str() << "'" << std::endl; // for diagnostics
  ss << "\n";
  std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
  assert( m_mapOptions.end() == m_mapOptions.find( sSymbol ) );
  m_mapOptions.emplace( mapOptions_t::value_type( sSymbol, std::move( fOptionList ) ) );
  m_state = EState::response;
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryEquityOptionChain(
  const std::string& sSymbol,
  const std::string& sSide,
  const std::string& sMonthCodes, // see above
  const std::string& sNearMonths, // number of near contracts to display: values 0 through 4
  const std::string& sFilterType, // Optional - "0" (default) = no filter or "1" = filter on a strike range or "2" = filter on the number of In/Out Of The Money contracts
  const std::string& sFilterOne,  // Ignored if [Filter Type] is "0". If [Filter Type] = "1" then beginning strike price or if [Filter Type] = "2" then the number of contracts in the money
  const std::string& sFilterTwo,  // Ignored if [Filter Type] is "0". If [Filter Type] = "1" then ending strike price or if [Filter Type] = "2" then the number of contracts out of the money   // suggest 12
  fOptionList_t&& fOptionList
) {
  assert( 0 < sSymbol.size() );
  assert( std::string::npos == sSymbol.find( ',' ) );
  std::stringstream ss;
  ss
    << "CEO,"
    << sSymbol << ","
    << sSide << ","
    << sMonthCodes << ","
    << sNearMonths << ","
    << sFilterType << ","
    << sFilterOne << ","
    << sFilterTwo << ","
    << "CEO-" << sSymbol << ","
    << "1" // 0 = default, exclude non-standard options, 1 = include
    ;
  std::cout << "request: '" << ss.str() << "'" << std::endl; // for diagnostics
  ss << "\n";
  std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
  assert( m_mapOptions.end() == m_mapOptions.find( sSymbol ) );
  m_mapOptions.emplace( mapOptions_t::value_type( sSymbol, std::move( fOptionList ) ) );
  m_state = EState::response;
  this->Send( ss.str().c_str() );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
