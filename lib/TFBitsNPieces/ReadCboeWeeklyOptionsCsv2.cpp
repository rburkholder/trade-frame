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

#include <stdexcept>

#include <boost/spirit/include/qi.hpp>

#include <boost/phoenix/core.hpp>

//https://www.boost.org/doc/libs/1_75_0/libs/spirit/classic/example/fundamental/file_parser.cpp
#include <boost/spirit/include/classic_file_iterator.hpp>

#include "ReadCboeWeeklyOptionsCsv.h"

using UnderlyingInfo_t = ou::tf::cboe::csv::UnderlyingInfo;

BOOST_FUSION_ADAPT_STRUCT(
  UnderlyingInfo_t,
  (std::string, sDescription),
  (std::string, sSymbol)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {
namespace csv {

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct WeeklyOptionsParser: qi::grammar<Iterator, UnderlyingInfo_t()> {

  WeeklyOptionsParser( void ): WeeklyOptionsParser::base_type( start ) {

    rNotAQuote %= +( qi::char_ - '"' );
    rNotAComma %= +( qi::char_ - ( ',' | qi::eol ) );

    rDescription %= rNotAQuote;
    rSymbol      %= rNotAQuote;

    rTitle1 %= rNotAComma;
    rTitle2 %= rNotAComma;

    rHeader %=
        rTitle1 > qi::lit( ',' ) > rTitle2
      ;

    rData %=
        qi::lit( '"' ) > rDescription > qi::lit( '"' )
      > qi::lit( ',' )
      > qi::lit( '"' ) > rSymbol > qi::lit( '"' )
      ;

    start %= ( rData | rHeader ) > ( qi::eol | qi::eps );
  }

  qi::rule<Iterator, UnderlyingInfo_t()> start;
  qi::rule<Iterator, UnderlyingInfo_t()> rHeader;
  qi::rule<Iterator, UnderlyingInfo_t()> rData;
  qi::rule<Iterator, std::string()> rNotAQuote;
  qi::rule<Iterator, std::string()> rNotAComma;
  qi::rule<Iterator, std::string()> rDescription;
  qi::rule<Iterator, std::string()> rSymbol;
  qi::rule<Iterator, std::string()> rTitle1;
  qi::rule<Iterator, std::string()> rTitle2;

};

// provide legacy means of access
void ReadCboeWeeklyOptions( vUnderlyinginfo_t& vui ) {
  ReadCboeWeeklyOptions(
    [&vui](const UnderlyingInfo& ui){
      vui.push_back( std::move( ui ) );
  } );
}

// newer means of access
void ReadCboeWeeklyOptions( fUnderlyingInfo_t&& fUnderlyingInfo ) {

  UnderlyingInfo ui;

  using file_iterator_t = boost::spirit::classic::file_iterator<char>;

  WeeklyOptionsParser<file_iterator_t> parserWeeklyOptions;

  const static std::string sFileName( "../cboesymboldirweeklys.csv" );

  file_iterator_t begin( sFileName );

  if ( !begin ) {
    throw std::runtime_error( "Error opening " + sFileName );
  }

  file_iterator_t end = begin.make_end();

  unsigned int cntLine{};

  while ( begin != end ) {
    bool b = qi::parse( begin, end, parserWeeklyOptions, ui );
    cntLine++;
    if ( 1 < cntLine ) { // skip header line
      fUnderlyingInfo( ui );
      //std::cout << ui.sSymbol << "," << ui.sDescription << std::endl;
    }
    ui.clear();
  }

  std::cout << "lines processed: " << cntLine << std::endl;

}

} // namespace csv
} // namespace cboe
} // namespace tf
} // namespace ou

