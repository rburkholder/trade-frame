/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    ConfigParser.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/at
 * Created: March 15, 2022 12:56
 */

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <iostream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <boost/fusion/include/std_pair.hpp>

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/object.hpp>
#include <boost/phoenix/operator.hpp>

//https://www.boost.org/doc/libs/1_75_0/libs/spirit/classic/example/fundamental/file_parser.cpp
#include <boost/spirit/include/classic_file_iterator.hpp>

#include "ConfigParser.hpp"

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::config::per_symbol_choices_t,
  (ou::tf::config::per_symbol_choices_t::EFeed, eFeed)
  (size_t, nPriceBins)
  (double, dblPriceUpper)
  (double, dblPriceLower)
  (size_t, nVolumeBins)
  (size_t, nVolumeUpper)
  (size_t, nVolumeLower)
)

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::config::choices_t,
  (size_t, ib_client_id)
  (size_t, nThreads)
  (bool, bStartSimulator)
  (std::string, sGroupDirectory)
  (size_t, nTimeBins)
  (std::string, sTimeUpper)  // TODO: try the conversion to ptime later
  (std::string, sTimeLower)  // TODO: try the conversion to ptime later
  (ou::tf::config::choices_t::mapInstance_t, mapInstance)
)

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator>
struct ChoicesParser: qi::grammar<Iterator, ou::tf::config::choices_t()> {

  ChoicesParser(): ChoicesParser::base_type( start ) {

    luBool.add
      ( "1", true )
      ( "0", false )
      ( "on", true )
      ( "off", false )
      ( "true", true )
      ( "false", false )
      ;

    luFeed.add
      ( "l1", ou::tf::config::per_symbol_choices_t::L1 )
      ( "l2M", ou::tf::config::per_symbol_choices_t::L2M )
      ( "l2O", ou::tf::config::per_symbol_choices_t::L2O )
      ( "l1", ou::tf::config::per_symbol_choices_t::L1 )
      ( "l2m", ou::tf::config::per_symbol_choices_t::L2M )
      ( "l2o", ou::tf::config::per_symbol_choices_t::L2O )
      ;

    //ruleSeparator = *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ');

    ruleDateTime =
         qi::char_("0-9") >> qi::char_("0-9") >> qi::char_("0-9") >> qi::char_("0-9") // YYYY
      >> qi::char_("0-9") >> qi::char_("0-9") // MM
      >> qi::char_("0-9") >> qi::char_("0-9") // DD
      >> qi::char_("T")
      >> qi::char_("0-9") >> qi::char_("0-9") // HH
      >> qi::char_("0-9") >> qi::char_("0-9") // MM
      >> qi::char_("0-9") >> qi::char_("0-9") // SS
      ;

    ruleIbClientId
      %= qi::lit("ib_client_id")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;
    ruleThreads
      %= qi::lit("threads")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;
    ruleStartSimulator
      %= qi::lit("sim_start")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> luBool
      >> *qi::lit(' ') >> qi::eol;
    ruleGroupDirectory
      %= qi::lit("group_directory")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> +( qi::char_("a-zA-Z0-9/") | qi::char_( '-' ) | qi::char_(':') | qi::char_('.') )
      >> *qi::lit(' ') >> qi::eol;


    ruleTimeBins
      %= qi::lit("time_bins" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;

    ruleTimeUpper
      %= qi::lit("time_upper")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> ruleDateTime
      >> *qi::lit(' ') >> qi::eol;
    ruleTimeLower
      %= qi::lit("time_lower")
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> ruleDateTime
      >> *qi::lit(' ') >> qi::eol;

    ruleSymbol
      %= qi::lit('[')
      >> qi::char_("A-Z@") >> *qi::char_("A-Z-#")
      >> qi::lit(']')
      >> *qi::lit(' ') >> qi::eol;

    ruleFeed
      %= qi::lit( "feed" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> luFeed
      >> *qi::lit(' ') >> qi::eol;

    rulePriceBins
      %= qi::lit( "price_bins" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;
    rulePriceUpper
      = qi::lit( "price_upper" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::double_
      >> *qi::lit(' ') >> qi::eol;
    rulePriceLower
      %= qi::lit( "price_lower" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::double_
      >> *qi::lit(' ') >> qi::eol;

    ruleVolumeBins
      %= qi::lit( "volume_bins" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;
    ruleVolumeUpper
      %= qi::lit( "volume_upper" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;
    ruleVolumeLower
      %= qi::lit( "volume_lower" )
      >> *qi::lit(' ') >> qi::lit('=') >> *qi::lit(' ')
      >> boost::spirit::uint_
      >> *qi::lit(' ') >> qi::eol;

    ruleSymbolChoices
      %= -ruleFeed
      >>  rulePriceBins
      >>  rulePriceUpper
      >>  rulePriceLower
      >>  ruleVolumeBins
      >>  ruleVolumeUpper
      >>  ruleVolumeLower
      ;

    ruleMapEntry
      %= ruleSymbol >> ruleSymbolChoices
      ;

    ruleMap %= +ruleMapEntry;

    start
      %= ruleIbClientId
      >> ruleThreads
      >> ruleStartSimulator
      >> -ruleGroupDirectory
      >> ruleTimeBins
      >> ruleTimeUpper
      >> ruleTimeLower
      >> +ruleMapEntry
      ;

    start.name( "parser start" );

    qi::on_error<qi::fail>( // doesn't seem to emit
        start
      , std::cout
          << phoenix::val( "error, expecting " )
          << qi::labels::_4
          << phoenix::val(" here: \"")
          << phoenix::construct<std::string>( qi::labels::_3, qi::labels::_2 )
          << phoenix::val("\"")
          << std::endl
    );

  }

  qi::symbols<char, bool> luBool;
  qi::symbols<char, ou::tf::config::per_symbol_choices_t::EFeed> luFeed;

  qi::rule<Iterator, size_t()> ruleIbClientId;
  qi::rule<Iterator, size_t()> ruleThreads;
  qi::rule<Iterator, bool()> ruleStartSimulator;
  //qi::rule<Iterator, ()> ruleSeparator;
  qi::rule<Iterator, std::string()> ruleGroupDirectory;
  qi::rule<Iterator, size_t()> ruleTimeBins;
  qi::rule<Iterator, std::string()> ruleDateTime;
  qi::rule<Iterator, std::string()> ruleTimeUpper;
  qi::rule<Iterator, std::string()> ruleTimeLower;
  qi::rule<Iterator, std::string()> ruleSymbol;
  qi::rule<Iterator, ou::tf::config::per_symbol_choices_t::EFeed()> ruleFeed;
  qi::rule<Iterator, size_t()> rulePriceBins;
  qi::rule<Iterator, double()> rulePriceUpper;
  qi::rule<Iterator, double()> rulePriceLower;
  qi::rule<Iterator, size_t()> ruleVolumeBins;
  qi::rule<Iterator, size_t()> ruleVolumeUpper;
  qi::rule<Iterator, size_t()> ruleVolumeLower;
  qi::rule<Iterator, ou::tf::config::per_symbol_choices_t()> ruleSymbolChoices;
  qi::rule<Iterator, std::pair<std::string,ou::tf::config::per_symbol_choices_t>()> ruleMapEntry;
  qi::rule<Iterator, ou::tf::config::choices_t::mapInstance_t()> ruleMap;
  qi::rule<Iterator, ou::tf::config::choices_t()> start;

 };


namespace ou {
namespace tf {
namespace config {

bool Load( const std::string& sFileName, choices_t& choices ) {

  using file_iterator_t = boost::spirit::classic::file_iterator<char>;

  ChoicesParser<file_iterator_t> grammarConfigChoices;

  file_iterator_t begin( sFileName );

  if ( !begin ) {
    std::cout << "ou::tf::config::Load could not open " << sFileName << std::endl;
    return false;
  }

  file_iterator_t end = begin.make_end();

  bool b = qi::parse( begin, end, grammarConfigChoices, choices );

  if ( b ) {
    choices.Update();
    if ( 0 == choices.nThreads ) {
      std::cout << "ou::tf::config::Load thread count set to 1" << std::endl;
      choices.nThreads = 1;
    }
    if ( 0 == choices.nTimeBins ) {
      std::cout << "ou::tf::config::Load time: nbins is 0" << std::endl;
      b = false;
    }
    if ( choices.dblTimeLower >= choices.dblTimeUpper ) {
      std::cout << "ou::tf::config::Load time: lower is >= upper" << std::endl;
      b = false;
    }
    for ( const choices_t::mapInstance_t::value_type& vt: choices.mapInstance ) {
      const auto& [sSymbol, per] = vt;

      if ( 0 == per.nPriceBins ) {
        std::cout << "ou::tf::config::Load price: nbins is 0" << std::endl;
        b = false;
      }
      if ( per.dblPriceLower >= per.dblPriceUpper ) {
        std::cout << "ou::tf::config::Load " << sSymbol << " price: lower is >= upper"  << std::endl;
        b = false;
      }

      if ( 0 == per.nVolumeBins ) {
        std::cout << "ou::tf::config::Load volume: nbins is 0" << std::endl;
        b = false;
      }
      if ( per.nVolumeLower >= per.nVolumeUpper ) {
        std::cout << "ou::tf::config::Load " << sSymbol << " volume: lower is >= upper"  << std::endl;
        b = false;
      }

      if ( choices.bStartSimulator ) {
        if ( 0 == choices.sGroupDirectory.size() ) {
          std::cout << "ou::tf::config::Load simulator: group_directory is required" << std::endl;
          b = false;
        }
      }
    }
  }
  else {
    std::cout << "ou::tf::config::Load could not parse " << sFileName << std::endl;
  }

  return b;

}

} // namespace config
} // namespace tf
} // namespace ou
