/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/12

// This parser is used for OPTION and FOPTION symbol parsing

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_repeat.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct structParsedOptionSymbol1 {
  std::string sText;
  std::string sDigits;
  std::string sCode;
  double dblStrike;
  structParsedOptionSymbol1( void ) : dblStrike( 0.0 ) {};
};

struct structParsedOptionSymbol2 {
  boost::uint16_t nYear;
  boost::uint8_t nDay;
  structParsedOptionSymbol2( void ) : nYear( 0 ), nDay( 0 ) {};
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

typedef ou::tf::iqfeed::structParsedOptionSymbol1 pos1_t;

BOOST_FUSION_ADAPT_STRUCT(
  pos1_t,
  (std::string, sText)
  (std::string, sDigits)
  (std::string, sCode)
  (double, dblStrike)
  )

typedef ou::tf::iqfeed::structParsedOptionSymbol2 pos2_t;

BOOST_FUSION_ADAPT_STRUCT(
  pos2_t,
  (boost::uint16_t, nYear)
  (boost::uint8_t, nDay)
  )


namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ascii = boost::spirit::ascii;

typedef qi::uint_parser<unsigned, 10, 2, 2> uint2_p;

template<typename Iterator>
struct OptionSymbolParser1: qi::grammar<Iterator, pos1_t()> {

  OptionSymbolParser1( void ): OptionSymbolParser1::base_type(start) {

    // define option processing rules
    ruleText %= +qi::char_( "A-Z" );
    ruleDigits %= +qi::char_( "0-9" );
    ruleCode %= qi::char_( "A-X" );
    ruleStrike %= qi::float_;

    start %= ruleText >> ruleDigits >> ruleCode >> ruleStrike;
  }

  qi::rule<Iterator, std::string()> ruleText;  // symbol
  qi::rule<Iterator, std::string()> ruleDigits;  // yydd
  qi::rule<Iterator, std::string()> ruleCode;  // month and call/put
  qi::rule<Iterator, double()> ruleStrike; // strike

  qi::rule<Iterator, pos1_t()> start;

};

template<typename Iterator>
struct OptionSymbolParser2: qi::grammar<Iterator, pos2_t()> {

  OptionSymbolParser2( void ): OptionSymbolParser2::base_type(start) {

    // define option processing rules
    ruleYear %= uint2_p();
    ruleDay  %= uint2_p();

    start %= ruleYear >> ruleDay;
  }

  qi::rule<Iterator, boost::uint16_t()> ruleYear;
  qi::rule<Iterator, boost::uint8_t()> ruleDay;

  qi::rule<Iterator, pos2_t()> start;

};

template<typename Iterator>
struct FOptionSymbolParser1: qi::grammar<Iterator, pos1_t()> {

  FOptionSymbolParser1( void ): FOptionSymbolParser1::base_type(start) {

    // define option processing rules
    ruleText %= +qi::char_( "@A-Z" );
    ruleDigits %= +qi::char_( "0-9" );
    ruleCode %= qi::char_( "A-X" );
    ruleStrike %= qi::float_;

    start %= ruleText >> ruleDigits >> ruleCode >> ruleStrike;
  }

  qi::rule<Iterator, std::string()> ruleText;  // symbol
  qi::rule<Iterator, std::string()> ruleDigits;  // yydd
  qi::rule<Iterator, std::string()> ruleCode;  // month and call/put
  qi::rule<Iterator, double()> ruleStrike; // strike

  qi::rule<Iterator, pos1_t()> start;

};



} // namespace iqfeed
} // namespace tf
} // namespace ou
