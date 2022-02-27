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
#include <boost/spirit/include/qi_symbols.hpp>

#include "OptionChainQuery.h"

struct PreRoll {
  enum class ECmd { CFU, CFO, CEO };
  enum class EExtra { LC, ENDMSG, BADSYM, ERROR };
  ECmd cmd;
  std::string sSymbol;
  EExtra extra;
};

BOOST_FUSION_ADAPT_STRUCT(
  PreRoll,
  (PreRoll::ECmd, cmd)
  (std::string, sSymbol)
  (PreRoll::EExtra, extra)
)

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::OptionChainQuery::OptionChain,
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
      ;

    extra.add
      ( "LC", PreRoll::EExtra::LC )
      ( "n", PreRoll::EExtra::BADSYM )
      ( "E", PreRoll::EExtra::ERROR )
      ( "!ENDMSG!", PreRoll::EExtra::ENDMSG )
      ;

    ruleCmd = cmd;
    ruleExtra = extra;
    ruleSymbol = +( qi::char_ - qi::char_( ',' ) );

    start
      %= ruleCmd >> qi::lit( '-' ) >> ruleSymbol >> qi::lit( ',' ) >> ruleExtra >> qi::lit( ',' )
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

using OptionChain = OptionChainQuery::OptionChain;

template<typename Iterator>
struct OptionChainParser: qi::grammar<Iterator, OptionChain()> {

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
  qi::rule<Iterator, OptionChain()> start;

};

// ==

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
      break;
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
                case PreRoll::ECmd::CEO:
                case PreRoll::ECmd::CFO:
                  {

                    OptionChain chain;

                    //std::cout << "EState::reply" << std::endl;
                    OptionChainParser<const_iterator_t> grammarOptionChain;
                    //const_iterator_t bgn = (*buffer).begin();

                    //std::string buf( iter, end );
                    //std::cout << "buf: '" << buf << "'" << std::endl;

                    bOk = parse( iter, end, grammarOptionChain, chain );

                    if ( bOk ) {
                      //std::cout << "bOk true" << std::endl;
                      assert( 0 < preroll.sSymbol.size() );
                      chain.sSymbol = std::move( preroll.sSymbol );
                    }
                    else {
                      std::cout
                        << "OptionChainQuery::OnNetworkLineBuffer parse error: "
                        << end - iter << ","
                        << preroll.sSymbol
                        << "'," << chain.vSymbol.size()
                        //<< "'," << chain.vCall.size()
                        //<< "," << chain.vPut.size()
                        << std::endl;
                      const_iterator_t bgn = (*buffer).begin();
                      const std::string sContent( bgn, end ); // debug statement
                      std::cout << sContent << std::endl;
                      // TODO: match the error message
                      //   but we are parsing errors at this point
                    }

                    std::scoped_lock<std::mutex> lock( m_mutexMapRequest );

                    if ( 0 < chain.sSymbol.size() ) {
                      mapRequest_t::const_iterator iter = m_mapRequest.find( chain.sSymbol );
                      if ( m_mapRequest.end() == iter ) {
                        std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find key " << chain.sSymbol << std::endl;
                      }
                      else {
                        iter->second( chain );
                      }
                    }

                    mapRequest_t::const_iterator iter = m_mapRequest.find( chain.sSymbol );
                    if ( m_mapRequest.end() == iter ) {
                      const_iterator_t bgn = (*buffer).begin();
                      const std::string sContent( bgn, end ); // debug statement
                      std::cout << "OptionChainQuery::OnNetworkLineBuffer error: can't find ending key '" << chain.sSymbol << "' in '" << sContent << "'" << std::endl;
                    }
                    else {
                      m_mapRequest.erase( iter );  // may need to do this on endmsg only
                    }

                  }
                  break;
              }
              break;
            case PreRoll::EExtra::ENDMSG:
              break;
            case PreRoll::EExtra::BADSYM:
              std::cout << "OptionChainQuery::OnNetworkLineBuffer badsym: " << preroll.sSymbol << std::endl;
              break;
            case PreRoll::EExtra::ERROR:
              std::cout << "OptionChainQuery::OnNetworkLineBuffer error: " << std::string( (*buffer).begin(), (*buffer).end() ) << std::endl;
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
    fOptionChain_t&& fOptionChain
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
  m_mapRequest.emplace( mapRequest_t::value_type( sSymbol, std::move( fOptionChain ) ) );
  m_state = EState::response;
  this->Send( ss.str().c_str() );
}

void OptionChainQuery::QueryFuturesOptionChain(
    const std::string& sSymbol,
    const std::string& sSide,
    const std::string& sMonthCodes,
    const std::string& sYears,
    const std::string& sNearMonths,
    fOptionChain_t&& fOptionChain
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
    << "\n";
  std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
  m_mapRequest.emplace( mapRequest_t::value_type( sSymbol, std::move( fOptionChain ) ) );
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
  fOptionChain_t&& fOptionChain
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
    << "\n";
  //std::cout << "request: '" << ss.str() << "'" << std::endl;
  std::scoped_lock<std::mutex> lock( m_mutexMapRequest );
  m_mapRequest.emplace( mapRequest_t::value_type( sSymbol, std::move( fOptionChain ) ) );
  m_state = EState::response;
  this->Send( ss.str().c_str() );
}


} // namespace iqfeed
} // namespace tf
} // namespace ou
