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
 * File:    Dispatcher.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 17, 2021 10:56
 */

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include "Dispatcher.h"

BOOST_FUSION_ADAPT_STRUCT(
  ou::tf::iqfeed::l2::SystemStatus,
  (ou::tf::iqfeed::l2::SystemStatus::ECmd, cmd)
  (std::string, param_a)
  (char, param_b)
)

namespace qi = boost::spirit::qi;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data

template<typename Iterator>
struct SystemStatusParser: qi::grammar<Iterator, ou::tf::iqfeed::l2::SystemStatus()> {

  using ECmd = ou::tf::iqfeed::l2::SystemStatus::ECmd;

  SystemStatusParser(): SystemStatusParser::base_type( ruleStart ) {

    cmd.add
      ( "SERVER CONNECTED", ECmd::ServerConnected )
      ( "CURRENT PROTOCOL", ECmd::CurrentProtocol )
      ( "CLEAR DEPTH", ECmd::ClearDepth )
      ( "SERVER DISCONNECTED", ECmd::ServerDisconnected )
      ;

    ruleCmd = cmd;
    ruleVersion = +( qi::char_( "0-9" ) | qi::char_( '.' ) );
    ruleSymbol = +( qi::char_( "0-9" ) | qi::char_( "A-Z" ) | qi::char_( '@' ) );
    ruleSide = qi::char_( 'A' ) | qi::char_( 'B' );
    ruleStart
      %=
         qi::lit( 'S' )
      >> qi::lit( ',' ) > ruleCmd
      >> -( ( qi::lit( ',' ) > ruleVersion )
          | ( qi::lit( ',' ) > ruleSymbol > qi::lit( ',' ) > ruleSide )
          )
      >> -qi::lit( ',' )
      //>> qi::eol
      ;

  }

  qi::symbols<char, ECmd> cmd;

  qi::rule<Iterator, ECmd()> ruleCmd;
  qi::rule<Iterator, std::string()> ruleVersion;
  qi::rule<Iterator, std::string()> ruleSymbol;
  qi::rule<Iterator, char()> ruleSide;
  qi::rule<Iterator, ou::tf::iqfeed::l2::SystemStatus()> ruleStart;
};

bool ParseSystemStatus( const std::string& src, SystemStatus& status ) {
  SystemStatusParser<std::string::const_iterator> grammarSystemStatus;
  bool bOk = parse( src.begin(), src.end(), grammarSystemStatus, status );
  return bOk;
}

} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
