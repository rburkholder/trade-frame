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

#include <boost/lexical_cast.hpp>

#include <boost/fusion/include/std_pair.hpp>
#include <boost/spirit/include/qi.hpp>
//#include <boost/spirit/include/qi/string/symbols.hpp>

#include "SymbolLookup.h"

// TODO: in addition to numerations, use keyword look up to obtain the numeric value for symbol lookups
//    perform a validation of enum in map

// SLM: Listed Markets  http://www.iqfeed.net/dev/api/docs/ListedMarkets.cfm
// [RequestID (if specified)],LS,[Listed Market ID],[Short Name],[Long Name],[Group ID],[Short Group Name],<CR><LF>

// SST: Security Types  http://www.iqfeed.net/dev/api/docs/SecurityTypes.cfm
// [RequestID (if specified)],LS,[Security Type ID],[Short Name],[Long Name],<LF>

// both end with
// !ENDMSG!,<CR><LF>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct SymbolByFilter {
  key_t idListedMarket;
  key_t idSecurityType;
  std::string sShortName;
  std::string sLongName;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou


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

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::SymbolLookup::TradeCondition,
  (std::string, sShortName)
  (std::string, sLongName)
)

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::SymbolByFilter,
  (std::string, sShortName)
  (key_t, idListedMarket)
  (key_t, idSecurityType)
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
                           name // short name
      >> qi::lit( ',' ) >> name // long name
      >> qi::lit( ',' ) >> name // group id
      >> qi::lit( ',' ) >> name // short group name
      ;

    pair %= qi::lit( "LM,LS," ) >> id >> qi::lit( ',' ) >> structure >> qi::lit( ',' );

    start %= pair;

  }

  qi::rule<Iterator, key_t()> id;
  qi::rule<Iterator, std::string()> name;
  qi::rule<Iterator, SymbolLookup::ListedMarket()> structure;
  qi::rule<Iterator, std::pair<key_t,SymbolLookup::ListedMarket>()> pair;
  qi::rule<Iterator, SymbolLookup::mapListedMarket_t()> start;

};

template<typename Iterator>
struct SecurityTypeParser: qi::grammar<Iterator, SymbolLookup::mapSecurityType_t()> {

  SecurityTypeParser(): SecurityTypeParser::base_type( start ) {

    id = qi::uint_;
    name %= ( +( qi::char_ - qi::char_(",") ) );

    structure %=
                           name // short name
      >> qi::lit( ',' ) >> name // long name
      ;

    pair %= qi::lit( "ST,LS," ) >> id >> qi::lit( ',' ) >> structure >> qi::lit( ',' );

    start %= pair;

  }

  qi::rule<Iterator, key_t()> id;
  qi::rule<Iterator, std::string()> name;
  qi::rule<Iterator, SymbolLookup::SecurityType()> structure;
  qi::rule<Iterator, std::pair<key_t,SymbolLookup::SecurityType>()> pair;
  qi::rule<Iterator, SymbolLookup::mapSecurityType_t()> start;

};

template<typename Iterator>
struct TradeConditionParser: qi::grammar<Iterator, SymbolLookup::mapTradeCondition_t()> {

  TradeConditionParser(): TradeConditionParser::base_type( start ) {

    id = qi::uint_;
    name %= ( +( qi::char_ - qi::char_(",") ) );

    structure %=
                           name // short name
      >> qi::lit( ',' ) >> name // long name
      ;

    pair %= qi::lit( "TC,LS," ) >> id >> qi::lit( ',' ) >> structure >> qi::lit( ',' );

    start %= pair;

  }

  qi::rule<Iterator, key_t()> id;
  qi::rule<Iterator, std::string()> name;
  qi::rule<Iterator, SymbolLookup::TradeCondition()> structure;
  qi::rule<Iterator, std::pair<key_t,SymbolLookup::TradeCondition>()> pair;
  qi::rule<Iterator, SymbolLookup::mapTradeCondition_t()> start;

};

template<typename Iterator>
struct SymbolByFilterParser: qi::grammar<Iterator, ou::tf::iqfeed::SymbolByFilter()> {

  SymbolByFilterParser(): SymbolByFilterParser::base_type( start ) {

    id = qi::uint_;
    nameShort %= ( +( qi::char_ - qi::char_(",") ) );
    nameLong  %= ( +( qi::char_ - qi::eol ) );

    start %=
      qi::lit( "BF,LS," )
      >>                   nameShort // symbol
      >> qi::lit( ',' ) >> id // idListedMarket
      >> qi::lit( ',' ) >> id // idSecurityType
      >> qi::lit( ',' ) >> nameLong // long name
      //>> qi::lit( ',' )
      ;

  }

  qi::rule<Iterator, key_t()> id;
  qi::rule<Iterator, std::string()> nameShort;
  qi::rule<Iterator, std::string()> nameLong;
  qi::rule<Iterator, ou::tf::iqfeed::SymbolByFilter()> start;

};

enum class ECommand { 
  unknown, protocol
, lm, lm_end // listed markets
, st, st_end // security types
, tc, tc_end // trade conditions
, bf, bf_end, bf_nodata // symbols by filter
};

template<typename Iterator>
struct CommandParser: qi::grammar<Iterator, ECommand()> {

  CommandParser(): CommandParser::base_type( start ) {

    symbols.add
      ( "LM,LS", ECommand::lm )
      ( "LM,!ENDMSG!", ECommand::lm_end )
      ( "ST,LS", ECommand::st )
      ( "ST,!ENDMSG!", ECommand::st_end )
      ( "TC,LS", ECommand::tc )
      ( "TC,!ENDMSG!", ECommand::tc_end )
      ( "BF,LS", ECommand::bf )
      ( "BF,!ENDMSG!", ECommand::bf_end )
      ( "BF,E,!NO_DATA!", ECommand::bf_nodata)
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
  TradeConditionParser<SymbolLookup::linebuffer_t::const_iterator> grammarTradeConditionParser;
  SymbolByFilterParser<SymbolLookup::linebuffer_t::const_iterator> grammarSymbolByFilterParser;
} // anonymous

SymbolLookup::SymbolLookup(
  mapListedMarket_t& mapListedMarket,
  mapSecurityType_t& mapSecurityType,
  mapTradeCondition_t& mapTradeCondition,
  fDone_t&& fDone
)
: Network<SymbolLookup>( "127.0.0.1", 9100 )
, m_kwmListedMarket( 0, 200 )
, m_kwmSecurityType( 0, 60 )
, m_kwmTradeCondition( 0, 180 )
, m_mapListedMarket( mapListedMarket )
, m_mapSecurityType( mapSecurityType )
, m_mapTradeCondition( mapTradeCondition )
, m_fDoneConnection( std::move( fDone ) )
{
  assert( m_fDoneConnection );
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
    case ECommand::tc:
      {
        bgn = (*buffer).begin();
        bOk = parse( bgn, end, grammarTradeConditionParser, m_mapTradeCondition );
      }
      break;
    case ECommand::bf:
      {
        SymbolByFilter sbf;
        bgn = (*buffer).begin();
        bOk = parse( bgn, end, grammarSymbolByFilterParser, sbf );
        if ( bOk ) {
          setIdSecurityType_t::const_iterator iter = m_setIdSecurityType.find( sbf.idSecurityType );
          if ( m_setIdSecurityType.end() != iter ) {
            m_fSymbol( sbf.sShortName, sbf.idListedMarket );
          }
        }
        else {
          std::string line( bgn, end );
          std::cout << "SymbolByFilter error: " << line << std::endl;
        }
      }
      break;
    case ECommand::protocol:
      Send( "SLM,LM\n" );
      break;
    case ECommand::lm_end:
      Send( "SST,ST\n" );
      break;
    case ECommand::st_end:
      Send( "STC,TC\n" );
      break;
    case ECommand::tc_end:
      MapSecurityTypes();
      BuildKeyWords();
      m_fDoneConnection();
      m_fDoneConnection = nullptr;
      break;
    case ECommand::bf_end:
    case ECommand::bf_nodata:
      m_fDoneSymbolList();
      m_fDoneSymbolList = nullptr;
      m_fSymbol = nullptr;
      m_setIdSecurityType.clear();
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

void SymbolLookup::BuildKeyWords() {
  for ( const mapListedMarket_t::value_type& vt: m_mapListedMarket ) {
    try {
      m_kwmListedMarket.AddPattern( vt.second.sShortName, vt.first );
    }
    catch ( std::domain_error& e ) {
      std::cout << "m_mapListedMarket duplicate: " << e.what() << std::endl;
    }
  }
  for ( const mapSecurityType_t::value_type& vt: m_mapSecurityType ) {
    m_kwmSecurityType.AddPattern( vt.second.sShortName, vt.first );
  }
  for ( const mapTradeCondition_t::value_type& vt: m_mapTradeCondition ) {
    try {
      m_kwmTradeCondition.AddPattern( vt.second.sShortName, vt.first );
    }
    catch ( std::domain_error& e ) {
      std::cout << "m_mapTradeCondition duplicate: " << e.what() << std::endl;
    }
    
  }
}

void SymbolLookup::SymbolList( 
  const setNames_t& setExchangeFilter, const setNames_t& setSecurityTypeFilter,
  fSymbol_t&& fSymbol, fDone_t&& fDone
) {

  assert( nullptr == m_fSymbol );
  m_fSymbol = std::move( fSymbol );
  assert( m_fSymbol );

  assert( nullptr == m_fDoneSymbolList );
  m_fDoneSymbolList = std::move( fDone ); // over-writes what should already be completed
  assert( m_fDoneSymbolList );
  
  m_setIdSecurityType.clear(); // build filter for during retrieval
  for ( const setNames_t::value_type& name: setSecurityTypeFilter ) {
    key_t id = m_kwmSecurityType.FindMatch( name );
    assert( 0 != id );
    m_setIdSecurityType.emplace( id );
  }

  bool bAddSpace( false );
  std::string sExchangeList;

  for ( const setNames_t::value_type& name: setExchangeFilter ) {
    if ( bAddSpace ) {
      sExchangeList += ' ';
    }
    else bAddSpace = true;
    
    key_t id = m_kwmListedMarket.FindMatch( name );
    assert( 0 != id );
    sExchangeList += boost::lexical_cast<std::string>( id );
  }
  
  std::string send( "SBF,s,*,e," + sExchangeList + ",BF" );
  std::cout << "sending '" << send << "'" << std::endl;
  Send( send + "\n" );  
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
