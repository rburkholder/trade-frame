/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;

#include "MarketSymbol.h"

typedef ou::tf::iqfeed::MarketSymbol::TableRowDef trd_t;
typedef ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc_t;

BOOST_FUSION_ADAPT_STRUCT(
  trd_t,
  (std::string, sSymbol)
  (std::string, sDescription)
  (std::string, sExchange)
  (std::string, sListedMarket)
  (sc_t, sc)
  (boost::uint32_t, nSIC)
  (boost::uint32_t, nNAICS)
  (bool, bFrontMonth)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;

void doout( const std::string& s );

template<typename Iterator>
struct MktSymbolLineParser: qi::grammar<Iterator, trd_t()> {
  MktSymbolLineParser( void ): MktSymbolLineParser::base_type(start) {
    
    symTypes.add 
      ( "BONDS", sc_t::Bonds )
      ( "CALC", sc_t::Calc )
      ( "EQUITY", sc_t::Equity )
      ( "FOPTION", sc_t::FOption )
      ( "FOREX", sc_t::Forex )
      ( "FORWARD", sc_t::Forward )
      ( "FUTURE", sc_t::Future )
      ( "ICSPREAD", sc_t::ICSpread )
      ( "IEOPTION", sc_t::IEOption )
      ( "INDEX", sc_t::Index )
      ( "MKTSTATS", sc_t::MktStats )
      ( "MONEY", sc_t::Money )
      ( "MUTUAL", sc_t::Mutual )
      ( "PRECMTL", sc_t::PrecMtl )
      ( "SPOT", sc_t::Spot )
      ( "SPREAD", sc_t::Spread )
      ( "STRATSPREAD", sc_t::StratSpread )
      ( "SWAPS", sc_t::Swaps )
      ( "TREASURIES", sc_t::Treasuries )
      ;

    rNotATab      %= qi::lexeme[ +( qi::char_ - '\t' ) ];

    //rDefaultSymType %= ( qi::eps[ qi::_val = sc_t::Unknown, qi::_pass = false ] >> qi::omit[ rNotATab ] );  // proper syntax
    rDefaultSymType %= ( qi::eps[ qi::_val = sc_t::Unknown ] >> qi::omit[ rNotATab ] );

    rSymbol       %= rNotATab;
    rDescription  %= rNotATab;
    rExchange     %= rNotATab;
    rListedMarket %= rNotATab;
    rSymbolClassifier %= ( symTypes | rDefaultSymType ); 
    rSic          %= qi::lexeme[ qi::uint_ ];
    rFrontMonth   %= ( qi::char_( 'Y' )[ qi::_val = qi::true_ ] | qi::eps );
    rNaics        %= qi::lexeme[ qi::uint_ ];

    start %=               rSymbol
      > qi::lit( '\t' ) > -rDescription 
      > qi::lit( '\t' ) >  rExchange   
      > qi::lit( '\t' ) >  rListedMarket 
      > qi::lit( '\t' ) >  rSymbolClassifier
      > qi::lit( '\t' ) > -rSic 
      > qi::lit( '\t' ) > -rFrontMonth 
      > qi::lit( '\t' ) > -rNaics 
      > ( qi::eol | qi::eps );
/*
    qi::on_error<qi::fail>(
      start,
      std::cout 
      << phoenix::val( "unknown symbol type: " ) 
      << qi::_4 
      << "," 
      << phoenix::construct<std::string>( qi::_1, qi::_3 ) 
      << std::endl
      );
   */
  }

  qi::symbols<char, sc_t> symTypes;

  qi::rule<Iterator, sc_t()> rDefaultSymType;
  qi::rule<Iterator, std::string()> rNotATab;
  qi::rule<Iterator, std::string()> rSymbol;
  qi::rule<Iterator, std::string()> rDescription;
  qi::rule<Iterator, std::string()> rExchange;
  qi::rule<Iterator, std::string()> rListedMarket;
  qi::rule<Iterator, sc_t()> rSymbolClassifier;
  qi::rule<Iterator, boost::uint32_t()> rSic;
  qi::rule<Iterator, bool> rFrontMonth;
  qi::rule<Iterator, boost::uint32_t()> rNaics;
  qi::rule<Iterator, trd_t()> start;
};


} // namespace iqfeed
} // namespace tf
} // namespace ou
