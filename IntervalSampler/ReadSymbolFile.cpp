/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    ReadSymbolFile.cpp
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 6, 2019, 1:43 PM
 */

#include <vector>
#include <fstream>
#include <exception>
#include <string>

#define BOOST_SPIRIT_USE_PHOENIX_V3
//#define BOOST_SPIRIT_DEBUG
//#define BOOST_SPIRIT_DEBUG_OUT std::cout

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

#include "ReadSymbolFile.h"

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

using vSymbol_t = ReadSymbolFile::vSymbol_t;

template<typename Iterator>
struct decode_symbol_list: qi::grammar<Iterator, vSymbol_t()> {
  decode_symbol_list(): decode_symbol_list::base_type( ruleVector ) {

    ruleCharAlpha = qi::char_( "A-Z" );
    ruleCharNum   = qi::char_( "0-9" );
    ruleCharInitial = ruleCharAlpha | ruleCharNum;
    ruleCharOther   = ruleCharInitial | qi::char_( '.' );

//    ruleInterval = +ruleCharNum;
    ruleName = ruleCharInitial >> *ruleCharOther;

    //ruleSep = qi::lit( ' ' ) | qi::lit( ',' ) | qi::lit( '\t' ) | qi::lit( '\n' );

    //ruleItem = ruleSep >> ruleName;

    //ruleVector = ruleInterval >> +( qi::eol >> ruleItem ) >> -qi::eol;
    //ruleVector = ruleInterval >> +((qi::lit( ' ' ) | qi::lit( ',' ) | qi::lit( '\t' ) | qi::lit( '\n' )) >> ruleName) >> -qi::eol;
    ruleVector = ruleName
      >> +( +(
        qi::lit( ' ' )
      | qi::lit( ',' )
      | qi::lit( '\t' )
      | qi::lit( '\r' )
      | qi::lit( '\n' )
      )
      >> ruleName ) >> -qi::eol;

//    BOOST_SPIRIT_DEBUG_NODE( ruleNameChar );
//    BOOST_SPIRIT_DEBUG_NODE( ruleName );
//    BOOST_SPIRIT_DEBUG_NODE( rulePair );
//    BOOST_SPIRIT_DEBUG_NODE( ruleMap );

  }

  qi::rule<Iterator, char()> ruleCharAlpha;
  qi::rule<Iterator, char()> ruleCharNum;
  qi::rule<Iterator, char()> ruleCharInitial;
  qi::rule<Iterator, char()> ruleCharOther;
  //qi::rule<Iterator, char()> ruleSep;

//  qi::rule<Iterator, std::string()> ruleInterval;
  qi::rule<Iterator, std::string()> ruleName;
  //qi::rule<Iterator, std::string()> ruleItem;

  qi::rule<Iterator, vSymbol_t()> ruleVector;
};

ReadSymbolFile::ReadSymbolFile( const std::string& sFileName, vSymbol_t& vSymbol) {
  
  std::ifstream in(sFileName);
  in.unsetf(std::ios::skipws);

  if ( in.eof() ) {
    throw std::runtime_error( sFileName + " is empty" );
  }

  // http://boost-spirit.com/home/2010/01/05/stream-based-parsing-made-easy/
  boost::spirit::istream_iterator begin( in );
  boost::spirit::istream_iterator end;

  assert( !in.eof() );

  decode_symbol_list<boost::spirit::istream_iterator> parser;

  bool bResult = qi::parse( begin, end, parser, vSymbol );
  std::cerr << "symbol diff: " << vSymbol.size() << std::endl;

  if (!bResult) {
    throw std::runtime_error( "can't decode symbol list: " + sFileName );
  }

//  for ( vSymbol_t::value_type vt: vSymbol ) {
//    std::cout << "symbol: '" << vt << "'" << std::endl;
//  }

  std::cout << "symbol list contains " << vSymbol.size() << " symbols" << std::endl;
}

ReadSymbolFile::~ReadSymbolFile( ) {
}

