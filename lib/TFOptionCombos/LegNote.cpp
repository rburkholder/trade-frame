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
 * File:    LegNote.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: January 1, 2021, 15:50
 */

#include <sstream>
#include <stdexcept>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_numeric.hpp>

#include <boost/phoenix/core.hpp>

#include "LegNote.h"

namespace {
  static const char* Name_Type[] = {
      "SynthLong"
    , "SynthShort"
    , "Cover"
    , "Protect"
  };
}

using values_t = ou::tf::option::LegNote::values_t;

 BOOST_FUSION_ADAPT_STRUCT(
  values_t,
  (ou::tf::option::LegNote::Type, m_type),
  (ou::tf::option::LegNote::State, m_state),
  (ou::tf::option::LegNote::Option, m_option),
  (ou::tf::option::LegNote::Side, m_side),
  (ou::tf::option::LegNote::Momentum, m_momentum),
  (ou::tf::option::LegNote::Algo, m_algo),
  (bool, m_lock)
  (double,m_iv)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

namespace qi = boost::spirit::qi;

template<typename Iterator>
struct LegNoteParser: qi::grammar<Iterator, values_t()> {

  LegNoteParser( void): LegNoteParser::base_type( start ) {

    // Todo, case insensitive: https://www.boost.org/doc/libs/1_75_0/libs/spirit/doc/html/spirit/qi/reference/string/symbols.html

    type_.add
      ( "synthlong",  LegNote::Type::SynthLong )
      ( "synthshort", LegNote::Type::SynthShort )
      ( "cover",      LegNote::Type::Cover )
      ( "protect",    LegNote::Type::Protect )
      ( "dpgp",       LegNote::Type::DltaPlsGmPls )
      ( "dpgm",       LegNote::Type::DltaPlsGmMns )
      ( "dmgp",       LegNote::Type::DltaMnsGmPls )
      ( "dmgm",       LegNote::Type::DltaMnsGmMns )
      ;

    state_.add
      ( "open",    LegNote::State::Open )
      ( "expired", LegNote::State::Expired )
      ( "closed",  LegNote::State::Closed )
      ;

    option_.add
      ( "call", LegNote::Option::Call )
      ( "put",  LegNote::Option::Put )
      ;

    side_.add
      ( "long",  LegNote::Side::Long )
      ( "short", LegNote::Side::Short )
      ;

    momentum_.add
      ( "rise", LegNote::Momentum::Rise )
      ( "fall", LegNote::Momentum::Fall )
      ;

    algo_.add
      ( "collar", LegNote::Algo::Collar )
      ;

    type =     qi::lit( "type=") >> type_;
    state =    qi::lit( "state=" ) >> state_;
    option =   qi::lit( "option=" ) >> option_;
    side =     qi::lit( "side=" ) >> side_;
    momentum = qi::lit( "momentum=" ) >> momentum_;
    algo =     qi::lit( "algo=" ) >> algo_;
    lock =     qi::lit( "lock=" ) >> qi::bool_;
    iv =       qi::lit( "iv=" ) >> qi::double_;

    // Todo: allow random order, partial list?
    start =
      type     >> qi::lit( ',' ) >>
      state    >> qi::lit( ',' ) >>
      option   >> qi::lit( ',' ) >>
      side     >> qi::lit( ',' ) >>
      momentum >> qi::lit( ',' ) >>
      algo
      >> -( qi::lit( ',' ) >> lock )
               >> qi::eps
      >> -( qi::lit( ',' ) >> iv )
               >> qi::eps
      ;

  }

  qi::symbols<char,LegNote::Type> type_;
  qi::symbols<char,LegNote::State> state_;
  qi::symbols<char,LegNote::Option> option_;
  qi::symbols<char,LegNote::Side> side_;
  qi::symbols<char,LegNote::Momentum> momentum_;
  qi::symbols<char,LegNote::Algo> algo_;

  qi::rule<Iterator,LegNote::Type> type;
  qi::rule<Iterator,LegNote::State> state;
  qi::rule<Iterator,LegNote::Option> option;
  qi::rule<Iterator,LegNote::Side> side;
  qi::rule<Iterator,LegNote::Momentum> momentum;
  qi::rule<Iterator,LegNote::Algo> algo;
  qi::rule<Iterator,bool()> lock;
  qi::rule<Iterator,double()> iv;

  qi::rule<Iterator, values_t()> start;
};

LegNote::LegNote()
: m_bValid( false ) {}

LegNote::LegNote( const values_t& values )
: m_bValid( true ), m_values( values )
{}

LegNote::LegNote( const std::string& s )
: m_bValid( false )
{
  Parse( s );
}

LegNote::LegNote( const LegNote&& rhs )
: m_bValid( rhs.m_bValid ), m_values( std::move( rhs.m_values ) )
{}

LegNote& LegNote::operator=( const LegNote&& rhs ) {
  if ( this != &rhs ) {
    m_bValid = std::move( rhs.m_bValid );
    m_values = std::move( rhs.m_values );
  }
  return *this;
}

LegNote::~LegNote() {
}

const LegNote::values_t& LegNote::Decode( const std::string& s ) {
  Parse( s );
  return m_values;
}

const std::string LegNote::Encode() const {
  // TODO: raise error if not valid

  std::stringstream ss;

  ss << "type=";
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
    case Type::DltaPlsGmPls:
      ss << "dpgp";
      break;
    case Type::DltaPlsGmMns:
      ss << "dpgm";
      break;
    case Type::DltaMnsGmPls:
      ss << "dmgp";
      break;
    case Type::DltaMnsGmMns:
      ss << "dmgm";
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

  ss << ",momentum=";
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

  ss << ",lock=" << ( m_values.m_lock ? "true" : "false" );

  ss << ",iv=" << m_values.m_iv;

  return ss.str();
}

const LegNote::values_t& LegNote::Values() const {
  // TODO: raise error if not valid
  return m_values;
}

void LegNote::Assign( const values_t& values ) {
  m_values = values;
  m_bValid = true;
}

void LegNote::Parse( const std::string& s ) {

  m_bValid = false;

  if ( s.empty() ) {
    throw std::runtime_error( "LegNote constructor, zero length string" );
  }

  LegNoteParser<std::string::const_iterator> parser;

  std::string::const_iterator begin( s.begin() );
  std::string::const_iterator end( s.end() );

  size_t start_diff = end - begin;

  bool b = qi::parse( begin, end, parser, m_values );
  if ( b && ( begin == end ) ) {}
  else {
    size_t end_diff = end - begin;
    std::stringstream ss;
    ss << "LegNote has unknown content: " << start_diff << "," << end_diff << "," << s;
    std::string e = ss.str();
    throw std::runtime_error( e );
  }

  m_bValid = true;

}

std::string LegNote::LU( Type type ) {
  return std::string( Name_Type[ (int)type ] );
}

} // namespace option
} // namespace tf
} // namespace ou
