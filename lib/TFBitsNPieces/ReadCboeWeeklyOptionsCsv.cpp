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
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/bind.hpp>
//#include <boost/phoenix/bind/bind_member_variable.hpp>
#include <boost/phoenix/object/construct.hpp>

#include <boost/phoenix/stl.hpp>
#include <boost/phoenix/stl/container.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>

//https://www.boost.org/doc/libs/1_75_0/libs/spirit/classic/example/fundamental/file_parser.cpp
#include <boost/spirit/include/classic_file_iterator.hpp>

#include "ReadCboeWeeklyOptionsCsv.h"

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::cboe::csv::WeeklyOptions,
  (std::vector<std::string>, vHeader),
  (std::vector<ou::tf::cboe::csv::ExpiryEntry>, vExpiryEntry),
  (std::vector<ou::tf::cboe::csv::SymbolEntry>, vSymbolExchTrade)
  (std::vector<ou::tf::cboe::csv::SymbolEntry>, vSymbolEquity)
)

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace cboe {
namespace csv {

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct WeeklyOptionsParser: qi::grammar<Iterator, WeeklyOptions()> {

  WeeklyOptionsParser(): WeeklyOptionsParser::base_type( start ) {

    rDigit %= qi::char_( "0-9" );
    rNotAQuote %= +( qi::char_ - '"' );

    rQuoted %= qi::lit( '"' ) > rNotAQuote > qi::lit( '"' );

    rHeader %= +( qi::char_ - qi::eol );
    rHeader.name( "header" );

    rDate  %= rDigit > rDigit > qi::char_( '/' )
            > rDigit > rDigit > qi::char_( '/' )
            > rDigit > rDigit;
    //debug( rDate );

    rQuotedDate %= qi::lit( '"' ) > ( rDate | qi::eps ) > qi::lit( '"' );
    //debug( rDateEntry );

    rExpiryName %= rQuoted;
    rExpiryName.name( "expiry name" );
    //debug( rExpiryName );

    rExpiryList %= +( qi::lit( ',' ) > rQuotedDate );
    rExpiryList.name( "expiry list" );
    //debug( rExpiryList );

    rExpiryEntry
      = ( rExpiryName >> rExpiryList )
        [ qi::labels::_val
            = boost::phoenix::construct<ExpiryEntry>( qi::labels::_1, qi::labels::_2 )
        ]
      ;

    rSymbol %= rQuoted;
    rSymbol.name( "symbol" );

    rDescription %= rQuoted;
    rDescription.name( "description" );

    rSymbolEntry
      = ( rSymbol > qi::lit( ',' ) > rDescription )
       [ qi::labels::_val
           = boost::phoenix::construct<SymbolEntry>( qi::labels::_1, qi::labels::_2 )
       ];

    // some background at http://boost-spirit.com/home/2010/01/15/how-do-rules-propagate-attributes/
    // something more to read: http://boost-spirit.com/home/2010/01/21/what-are-rule-bound-semantic-actions/

    using boost::phoenix::bind;
    using boost::phoenix::push_back;

    start
      = rHeader [push_back(bind( &WeeklyOptions::vHeader, qi::labels::_val), qi::labels::_1 ) ] > qi::eol
      > +( rExpiryEntry [push_back(bind( &WeeklyOptions::vExpiryEntry, qi::labels::_val), qi::labels::_1 ) ] > qi::eol )
      > /* qi::eps > */ qi::eol
      > rHeader [push_back(bind( &WeeklyOptions::vHeader, qi::labels::_val), qi::labels::_1 ) ] > qi::eol
      > +( rSymbolEntry [push_back(bind( &WeeklyOptions::vSymbolExchTrade, qi::labels::_val), qi::labels::_1) ] > qi::eol )
      > /* qi::eps > */ qi::eol
      > rHeader [push_back(bind( &WeeklyOptions::vHeader, qi::labels::_val), qi::labels::_1 ) ] > qi::eol
      > +( rSymbolEntry [push_back(bind( &WeeklyOptions::vSymbolEquity, qi::labels::_val), qi::labels::_1 ) ] > qi::eol )
      > ( qi::eol | qi::eps );
  }

  qi::rule<Iterator, char()> rDigit;
  qi::rule<Iterator, std::string()> rNotAQuote;
  qi::rule<Iterator, std::string()> rQuoted;
  qi::rule<Iterator, std::string()> rHeader;
  qi::rule<Iterator, std::string()> rDate;  // TODO: convert to date
  qi::rule<Iterator, std::string()> rQuotedDate;
  qi::rule<Iterator, std::string()> rExpiryName;
  qi::rule<Iterator, std::vector<std::string>()> rExpiryList;
  qi::rule<Iterator, ExpiryEntry()> rExpiryEntry;  // TODO: split out into structure
  qi::rule<Iterator, std::string()> rName;
  qi::rule<Iterator, std::string()> rSymbol;
  qi::rule<Iterator, std::string()> rDescription;
  qi::rule<Iterator, SymbolEntry()> rSymbolEntry;
  qi::rule<Iterator, WeeklyOptions()> start;

};

void ReadCboeWeeklyOptions( fSymbolEntry_t&& fET, fSymbolEntry_t&& fEquity ) {

  WeeklyOptions we;
  ReadCboeWeeklyOptions( we );

  if ( fET ) {
    for ( const SymbolEntry& se: we.vSymbolExchTrade ) {
      fET( se );
    }
  }

  if ( fEquity ) {
    for ( const SymbolEntry& se: we.vSymbolEquity ) {
      fEquity( se );
    }
  }
}

void ReadCboeWeeklyOptions( fSymbolEntry_t&& f ) {

  WeeklyOptions we;
  ReadCboeWeeklyOptions( we );

  if ( f ) {
    for ( const SymbolEntry& se: we.vSymbolExchTrade ) {
      f( se );
    }
  }

  if ( f ) {
    for ( const SymbolEntry& se: we.vSymbolEquity ) {
      f( se );
    }
  }
}

void ReadCboeWeeklyOptions( WeeklyOptions& we ) {

  using file_iterator_t = boost::spirit::classic::file_iterator<char>;

  WeeklyOptionsParser<file_iterator_t> parserWeeklyOptions;

  const static std::string sFileName( "../weeklysmf.csv" );

  file_iterator_t begin( sFileName );

  if ( !begin ) {
    throw std::runtime_error( "Error opening " + sFileName );
  }

  file_iterator_t end = begin.make_end();

  bool b = qi::parse( begin, end, parserWeeklyOptions, we );

}

} // namespace csv
} // namespace cboe
} // namespace tf
} // namespace ou
