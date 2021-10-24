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

// slight modifications to the original ParseOptionDescripion
// to handle different names in description:  full month, full option side

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include "MarketSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

struct structParsedFOptionDescription {
  std::string& sUnderlying;
  boost::uint8_t& nMonth;
  boost::uint16_t& nYear;
  ou::tf::OptionSide::enumOptionSide& eOptionSide;
  double& dblStrike;
  structParsedFOptionDescription(
    std::string& sUnderlying_,
    boost::uint8_t& nMonth_, boost::uint16_t& nYear_,
        ou::tf::OptionSide::enumOptionSide& eOptionSide_, double& dblStrike_ ):
      sUnderlying( sUnderlying_ ),
      nMonth( nMonth_ ), nYear( nYear_ ), eOptionSide( eOptionSide_ ), dblStrike( dblStrike_ ) {};
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

using adapted_foption_t = ou::tf::iqfeed::structParsedFOptionDescription;

BOOST_FUSION_ADAPT_STRUCT(
  adapted_foption_t,
  (std::string, sUnderlying)
  (boost::uint8_t&, nMonth)
  (boost::uint16_t&, nYear)
  (ou::tf::OptionSide::enumOptionSide&, eOptionSide)
  (double&, dblStrike)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct FOptionDescriptionParser: qi::grammar<Iterator, adapted_foption_t()> {

  FOptionDescriptionParser(): FOptionDescriptionParser::base_type(start) {

    symMonths.add
      ( "JANUARY",  1 )
      ( "FEBRUARY",  2 )
      ( "MARCH",  3 )
      ( "APRIL",  4 )
      ( "MAY",  5 )
      ( "JUNE",  6 )
      ( "JULY",  7 )
      ( "AUGUST",  8 )
      ( "SEPTEMBER",  9 )
      ( "OCTOBER", 10 )
      ( "NOVEMBER", 11 )
      ( "DECEMBER", 12 )
      ;

    symOptionSide.add
      ( "CALL", OptionSide::Call )
      ( "PUT", OptionSide::Put )
      ;

    // define option processing rules
    ruleMonth %= symMonths;
    ruleNoSpaceString = +(qi::char_ - qi::char_(' '));
    ruleNotAMonth = ruleNoSpaceString - symMonths;
    ruleUnderlyingSymbol %= +(*qi::char_(' ') >> ruleNotAMonth);
    //ruleUnderlyingSymbol %= +(-qi::lit(' ') >> ((+(qi::char_ - qi::char_(' '))) - symMonths));
    ruleYear %= qi::uint_;
    ruleStrike %= qi::float_;
    ruleOptionSide %= symOptionSide;
    start %=
           ruleUnderlyingSymbol
        >> qi::lit( ' ' ) > ruleMonth
         > qi::lit( ' ' ) > ruleYear
         > qi::lit( ' ' ) > ruleOptionSide
         > qi::lit( ' ' ) > ruleStrike
      ;
  }

  qi::symbols<char, boost::uint8_t> symMonths;
  qi::symbols<char, ou::tf::OptionSide::enumOptionSide> symOptionSide;

  qi::rule<Iterator, std::string()> ruleNoSpaceString;
  qi::rule<Iterator, std::string()> ruleNotAMonth;
  qi::rule<Iterator, std::string()> ruleUnderlyingSymbol;
  qi::rule<Iterator, boost::uint8_t()> ruleMonth;
  qi::rule<Iterator, boost::uint16_t()> ruleYear;
  qi::rule<Iterator, double()> ruleStrike;
  qi::rule<Iterator, ou::tf::OptionSide::enumOptionSide()> ruleOptionSide;
  qi::rule<Iterator, adapted_foption_t()> start;

};

} // namespace iqfeed
} // namespace tf
} // namespace ou
