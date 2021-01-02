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
 * File:    LegDescription.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: January 1, 2021, 15:50
 */

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <stdexcept>
#include <sstream>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_numeric.hpp>
#include <boost/spirit/include/qi.hpp>

 #include "LegDescription.h"

 using values_t = LegDescription::values_t;

 BOOST_FUSION_ADAPT_STRUCT(
  values_t,
  (unsigned int, m_ixLeg)
  (LegDescription::Type, m_type),
  (LegDescription::State, m_state),
  (LegDescription::Option, m_option),
  (LegDescription::Side, m_side),
  (LegDescription::Momentum, m_momentum),
  (LegDescription::Algo, m_algo)
  )

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct LegDescriptionParser: qi::grammar<Iterator, values_t()> {

  LegDescriptionParser( void): LegDescriptionParser::base_type( start ) {

    // Todo, case insensitive: https://www.boost.org/doc/libs/1_75_0/libs/spirit/doc/html/spirit/qi/reference/string/symbols.html

    type_.add
      ( "synthlong",  LegDescription::Type::SynthLong )
      ( "synthshort", LegDescription::Type::SynthShort )
      ( "cover",      LegDescription::Type::Cover )
      ( "protect",    LegDescription::Type::Protect )
      ;

    state_.add
      ( "open",    LegDescription::State::Open )
      ( "expired", LegDescription::State::Expired )
      ( "closed",  LegDescription::State::Closed )
      ;

    option_.add
      ( "call", LegDescription::Option::Call )
      ( "put",  LegDescription::Option::Put )
      ;

    side_.add
      ( "long",  LegDescription::Side::Long )
      ( "short", LegDescription::Side::Short )
      ;

    momentum_.add
      ( "rise", LegDescription::Momentum::Rise )
      ( "fall", LegDescription::Momentum::Fall )
      ;

    algo_.add
      ( "collar", LegDescription::Algo::Collar )
      ;

    ix %=       qi::lit( "leg=" ) >> qi::uint_;
    type %=     qi::lit( "type=") >> type_;
    state %=    qi::lit( "state=" ) >> state_;
    option %=   qi::lit( "option=" ) >> option_;
    side %=     qi::lit( "side=" ) >> side_;
    momentum %= qi::lit( "momentum=" ) >> momentum_;
    algo %=     qi::lit( "algo=" ) >> algo_;

    // Todo: allow random order, partial list?
    start %=
      ix       >> qi::lit( ',' ) >>
      type     >> qi::lit( ',' ) >>
      state    >> qi::lit( ',' ) >>
      option   >> qi::lit( ',' ) >>
      side     >> qi::lit( ',' ) >>
      momentum >> qi::lit( ',' ) >>
      algo
      ;

  }

  qi::symbols<char,LegDescription::Type> type_;
  qi::symbols<char,LegDescription::State> state_;
  qi::symbols<char,LegDescription::Option> option_;
  qi::symbols<char,LegDescription::Side> side_;
  qi::symbols<char,LegDescription::Momentum> momentum_;
  qi::symbols<char,LegDescription::Algo> algo_;

  qi::rule<Iterator,unsigned int()> ix;
  qi::rule<Iterator,LegDescription::Type> type;
  qi::rule<Iterator,LegDescription::State> state;
  qi::rule<Iterator,LegDescription::Option> option;
  qi::rule<Iterator,LegDescription::Side> side;
  qi::rule<Iterator,LegDescription::Momentum> momentum;
  qi::rule<Iterator,LegDescription::Algo> algo;

  qi::rule<Iterator, values_t()> start;
};

LegDescription::LegDescription( const std::string& s ) {
  Parse( s );
}

LegDescription::~LegDescription() {
}

const LegDescription::values_t& LegDescription::Decode( const std::string& s ) {
  Parse( s );
  return m_values;
}

const std::string LegDescription::Encode() const {

  std::stringstream ss;

  ss << "leg=" << m_values.m_ixLeg;

  ss << ",type=";
  switch ( m_values.m_type ) {
    case Type::Cover:
      ss << "cover";
      break;
    case Type::Protect:
      ss << "protect";
      break;
    case Type::SynthLong:
      ss << "synthlong";
      break;
    case Type::SynthShort:
      ss << "synthshort";
      break;
  }

  ss << ",state=";
  switch ( m_values.m_state ) {
    case State::Open:
      ss << "open";
      break;
    case State::Closed:
      ss << "closed";
      break;
    case State::Expired:
      ss << "expired";
      break;
  }

  ss << ",option=";
  switch (m_values.m_option ) {
    case Option::Call:
      ss << "call";
      break;
    case Option::Put:
      ss << "put";
      break;
  }

  ss << ",side=";
  switch ( m_values.m_side ) {
    case Side::Long:
      ss << "long";
      break;
    case Side::Short:
      ss << "short";
      break;
  }

  ss << ",momeentum=";
  switch ( m_values.m_momentum ) {
    case Momentum::Rise:
      ss << "rise";
      break;
    case Momentum::Fall:
      ss << "fall";
      break;
  }

  ss << ",algo=";
  switch ( m_values.m_algo ) {
    case Algo::Collar:
      ss << "collar";
      break;
  }

  return ss.str();
}

const LegDescription::values_t& LegDescription::Values() const {
  return m_values;
}

void LegDescription::Assign( const values_t& values ) {
  m_values = values;
}

void LegDescription::Parse( const std::string& s ) {

  if ( s.empty() ) {
    throw std::runtime_error( "LegDescription constructor, zero length string" );
  }

  LegDescriptionParser<std::string::const_iterator> parser;

  std::string::const_iterator begin( s.begin() );
  std::string::const_iterator end( s.end() );

  bool b = qi::parse( begin, end, parser, m_values );
  if ( b && ( begin == end ) ) {}
  else {
    std::runtime_error( "LegDescription has unknown content: " + s );
  }

}
