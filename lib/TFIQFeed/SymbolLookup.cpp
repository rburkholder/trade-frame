/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    SymbolLookup.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed
 * Created: Novemeber 2, 2021, 11:09
 */

#include <sstream>

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi/string/symbols.hpp>

#include "SymbolLookup.h"

// TODO: SLM, SST requests, parse and map to callers process

// SLM: Listed Markets  http://www.iqfeed.net/dev/api/docs/ListedMarkets.cfm
// [RequestID (if specified)],LS,[Listed Market ID],[Short Name],[Long Name],[Group ID],[Short Group Name],<CR><LF>

// SST: Security Types  http://www.iqfeed.net/dev/api/docs/SecurityTypes.cfm
// [RequestID (if specified)],LS,[Security Type ID],[Short Name],[Long Name],<LF>

// both end with
// !ENDMSG!,<CR><LF>

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::SymbolLookup::ListedMarket,
  (std::string, sShortName)
  (std::string, sLongName)
  (std::string, idGroup)
  (std::string, sGroupName)
  )

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::SymbolLookup::SecurityType,
  (std::string, sShortName)
  (std::string, sLongName)
  )

namespace qi = boost::spirit::qi;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

using const_iterator_t = ou::Network<SymbolLookup>::linebuffer_t::const_iterator;

template<typename Iterator>
struct ListedMarketParser: qi::grammar<Iterator, SymbolLookup::mapListedMarket_t()> {

  ListedMarketParser(): ListedMarketParser::base_type( start ) {

    id = qi::uint_;
    name %= ( +( qi::char_ - qi::char_(",") ) );

    structure %=
                           name // shart name
      >> qi::lit( ',' ) >> name // long name
      >> qi::lit( ',' ) >> name // group id
      >> qi::lit( ',' ) >> name // short group name
      ;

    pair %= qi::lit( "LM,LS," ) >> id >> qi::lit( ',' ) >> structure >> qi::lit( ',' );

    start %= pair;

  }

  qi::rule<Iterator, uint16_t()> id;
  qi::rule<Iterator, std::string()> name;
  qi::rule<Iterator, SymbolLookup::ListedMarket()> structure;
  qi::rule<Iterator, std::pair<uint16_t,SymbolLookup::ListedMarket>()> pair;
  qi::rule<Iterator, SymbolLookup::mapListedMarket_t()> start;

};

template<typename Iterator>
struct SecurityTypeParser: qi::grammar<Iterator, SymbolLookup::mapSecurityType_t()> {

  SecurityTypeParser(): SecurityTypeParser::base_type( start ) {

    id = qi::int_;
    name %= ( +( qi::char_ - qi::char_(",") ) );

    structure %=
                           name // shart name
      >> qi::lit( ',' ) >> name // long name
      ;

    pair %= qi::lit( "ST,LS," ) >> id >> qi::lit( ',' ) >> structure >> qi::lit( ',' );

    start %= pair;

  }

  qi::rule<Iterator, int()> id;
  qi::rule<Iterator, std::string()> name;
  qi::rule<Iterator, SymbolLookup::SecurityType()> structure;
  qi::rule<Iterator, std::pair<int,SymbolLookup::SecurityType>()> pair;
  qi::rule<Iterator, SymbolLookup::mapSecurityType_t()> start;

};

enum class ECommand { unknown, protocol, lm, st, end_lm, end_st };

template<typename Iterator>
struct CommandParser: qi::grammar<Iterator, ECommand()> {

  CommandParser(): CommandParser::base_type( start ) {

    symbols.add
      ( "LM,LS", ECommand::lm )
      ( "LM,!ENDMSG!", ECommand::end_lm )
      ( "ST,LS", ECommand::st )
      ( "ST,!ENDMSG!", ECommand::end_st )
      ( "S,CURRENT PROTOCOL", ECommand::protocol )
      ;

    start = symbols >> qi::lit( ',' );

  }

  qi::symbols<char, ECommand> symbols;
  qi::rule<Iterator, ECommand()> start;

};

namespace {
  CommandParser<SymbolLookup::linebuffer_t::const_iterator> grammarCommandParser;
  ListedMarketParser<SymbolLookup::linebuffer_t::const_iterator> grammarListedMarketParser;
  SecurityTypeParser<SymbolLookup::linebuffer_t::const_iterator> grammarSecurityTypeParser;
} // anonymous

// http://www.iqfeed.net/dev/api/docs/OptionChainsviaTCPIP.cfm

SymbolLookup::SymbolLookup(
  mapListedMarket_t& mapListedMarket,
  mapSecurityType_t& mapSecurityType,
  fDone_t&& fDone
)
: Network<SymbolLookup>( "127.0.0.1", 9100 ),
  m_fDone( std::move( fDone ) ),
  m_mapListedMarket( mapListedMarket ),
  m_mapSecurityType( mapSecurityType )
{
  assert( m_fDone );
}

SymbolLookup::~SymbolLookup() {
}

void SymbolLookup::Connect() {
  ou::Network<SymbolLookup>::Connect();
}

void SymbolLookup::OnNetworkConnected() {

  //std::cout << "SymbolLookup::OnNetworkConnected" << std::endl;

  Send( "S,SET PROTOCOL,6.2\n" );

};

void SymbolLookup::Disconnect() {
  ou::Network<SymbolLookup>::Disconnect();
}

void SymbolLookup::OnNetworkDisconnected() {
}

void SymbolLookup::OnNetworkError( size_t e ) {
}

void SymbolLookup::OnNetworkSendDone() {
}

void SymbolLookup::OnNetworkLineBuffer( linebuffer_t* buffer ) {

  using const_iterator_t = linebuffer_t::const_iterator;

  const_iterator_t bgn( (*buffer).begin() );
  const_iterator_t end( (*buffer).end() );

  //std::string line( bgn, end );
  //std::cout << "SymbolLookup line: " << line << std::endl;

  bool bOk;

  ECommand command( ECommand::unknown );
  bOk = parse( bgn, end, grammarCommandParser, command );

  switch ( command ) {
    case ECommand::unknown:
      {
        std::string s( (*buffer).begin(), (*buffer).end() );
        std::cout << "SymbolLookup unknown line: '" << s << "'" << std::endl;
      }
      break;
    case ECommand::lm:
      {
        bgn = (*buffer).begin();
        bOk = parse( bgn, end, grammarListedMarketParser, m_mapListedMarket );
      }
      break;
    case ECommand::st:
      {
        bgn = (*buffer).begin();
        bOk = parse( bgn, end, grammarSecurityTypeParser, m_mapSecurityType );
      }
      break;
    case ECommand::protocol:
      Send( "SLM,LM\n" );
      break;
    case ECommand::end_lm:
      Send( "SST,ST\n" );
      break;
    case ECommand::end_st:
      MapSecurityTypes();
      m_fDone();
      break;
  }

  GiveBackBuffer( buffer );
}

void SymbolLookup::MapSecurityTypes() {

  qi::symbols<char, ESecurityType> types;
  types.add
    ("EQUITY", ESecurityType::Equity )
    ("IEOPTION", ESecurityType::IEOption )
    ("MUTUAL", ESecurityType::Mutual )
    ("MONEY", ESecurityType::Money )
    ("BONDS", ESecurityType::Bonds )
    ("INDEX", ESecurityType::Index )
    ("MKTSTATS", ESecurityType::MktStats )
    ("FUTURE", ESecurityType::Future )
    ("FOPTION", ESecurityType::FOption )
    ("SPREAD", ESecurityType::Spread )
    ("SPOT", ESecurityType::Spot )
    ("FORWARD", ESecurityType::Forward )
    ("CALC", ESecurityType::Calc )
    ("STRIP", ESecurityType::Unknown )
    ("FOREX", ESecurityType::Forex )
    ("ARGUS", ESecurityType::Unknown )
    ("PRECMTL", ESecurityType::PrecMtl )
    ("RACKS", ESecurityType::Unknown )
    ("RFSPOT", ESecurityType::Unknown )
    ("ICSPREAD", ESecurityType::ICSpread )
    ("STRATSPREAD", ESecurityType::StratSpread )
    ("TREASURIES", ESecurityType::Treasuries )
    ("SWAPS", ESecurityType::Swaps )
    ("MKTRPT", ESecurityType::MktRpt )
    ("SNL_NG", ESecurityType::Unknown )
    ("SNL_ELEC", ESecurityType::Unknown )
    ("NP_CAPACITY", ESecurityType::Unknown )
    ("NP_FLOW", ESecurityType::Unknown )
    ("NP_POWER", ESecurityType::Unknown )
    ("COMM3", ESecurityType::Unknown )
    ("JACOBSEN", ESecurityType::Unknown )
    ("ISO", ESecurityType::Unknown )
    ("FAST_RACKS", ESecurityType::Unknown )
    ("COMBINED_FUTURE", ESecurityType::Unknown )
    ("COMBINED_FOPTION", ESecurityType::Unknown )
    ("ARGUSFC", ESecurityType::Unknown )
    ("PETROCHEMWIRE", ESecurityType::Unknown )
    ("FOPTION_IV", ESecurityType::Unknown )
    ("GENERICRPT", ESecurityType::Unknown )
    ("DAILY_FUTURE", ESecurityType::Unknown )
  ;

  for ( mapSecurityType_t::value_type& vt: m_mapSecurityType ) {
    parse( vt.second.sShortName.begin(), vt.second.sShortName.end(), types, vt.second.eSecurityType );
  }
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
