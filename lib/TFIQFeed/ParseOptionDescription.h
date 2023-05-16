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

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/stl.hpp>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct structParsedOptionDescription {
  std::string& sUnderlying;
  boost::uint8_t& nMonth;
  boost::uint16_t& nYear;
  ou::tf::OptionSide::EOptionSide& eOptionSide;
  double& dblStrike;
  structParsedOptionDescription(
    std::string& sUnderlying_,
    boost::uint8_t& nMonth_, boost::uint16_t& nYear_,
        ou::tf::OptionSide::EOptionSide& eOptionSide_, double& dblStrike_ ):
      sUnderlying( sUnderlying_ ),
      nMonth( nMonth_ ), nYear( nYear_ ), eOptionSide( eOptionSide_ ), dblStrike( dblStrike_ ) {};
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

typedef ou::tf::iqfeed::structParsedOptionDescription adapted_option_t;

BOOST_FUSION_ADAPT_STRUCT(
  adapted_option_t,
  (std::string, sUnderlying)
  (boost::uint8_t&, nMonth)
  (boost::uint16_t&, nYear)
  (ou::tf::OptionSide::EOptionSide&, eOptionSide)
  (double&, dblStrike)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct OptionDescriptionParser: qi::grammar<Iterator, adapted_option_t()> {

  OptionDescriptionParser( void ): OptionDescriptionParser::base_type(start) {

    symMonths.add
      ( "JAN",  1 )
      ( "FEB",  2 )
      ( "MAR",  3 )
      ( "APR",  4 )
      ( "MAY",  5 )
      ( "JUN",  6 )
      ( "JUL",  7 )
      ( "AUG",  8 )
      ( "SEP",  9 )
      ( "OCT", 10 )
      ( "NOV", 11 )
      ( "DEC", 12 )
      ;

    symOptionSide.add
      ( "C", OptionSide::Call )
      ( "P", OptionSide::Put )
      ;

    // define option processing rules
    ruleString %= +(qi::char_ - qi::char_(' '));
    ruleUnderlyingSymbol %= ruleString;
    ruleMonth %= symMonths;
    ruleYear %= qi::uint_;
    ruleStrike %= qi::float_;
    ruleOptionSide %= symOptionSide;
    start %=
           ruleUnderlyingSymbol
         > qi::lit( ' ' ) > ruleMonth
         > qi::lit( ' ' ) > ruleYear
         > qi::lit( ' ' ) > ruleOptionSide
         > qi::lit( ' ' ) > ruleStrike
      ;
  }

  qi::symbols<char, boost::uint8_t> symMonths;
  qi::symbols<char, ou::tf::OptionSide::EOptionSide> symOptionSide;

  qi::rule<Iterator, std::string()> ruleString;
  qi::rule<Iterator, std::string()> ruleUnderlyingSymbol;
  qi::rule<Iterator, boost::uint8_t()> ruleMonth;
  qi::rule<Iterator, boost::uint16_t()> ruleYear;
  qi::rule<Iterator, double()> ruleStrike;
  qi::rule<Iterator, ou::tf::OptionSide::EOptionSide()> ruleOptionSide;
  qi::rule<Iterator, adapted_option_t()> start;

};

} // namespace iqfeed
} // namespace tf
} // namespace ou
