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
 * File:    MsgPriceLevelArrival.cpp
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 16, 2021 20:02
 */

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <string>

 #include <boost/date_time/gregorian/gregorian_types.hpp>
 #include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include "MsgOrderArrival.h"

using date_t = boost::gregorian::date;
using td_t = boost::posix_time::time_duration;

// http://www.iqfeed.net/dev/api/docs/docsBeta/MarketDepthMessages.cfm

struct decoded {
  char chMsgType;
  std::string sSymbolName;
  char chOrderSide;  // 'A' Sell, 'B' Buy
  double dblPrice;
  uint32_t nLevelSize;
  uint32_t nOrderCount;
  uint8_t nPrecision;
  td_t time;
  date_t date;
};

BOOST_FUSION_ADAPT_STRUCT(
  decoded,
  (char, chMsgType)
  (std::string, sSymbolName)
  (char, chOrderSide)
  (double, dblPrice)
  (uint32_t, nLevelSize)
  (uint32_t, nOrderCount)
  (uint8_t, nPrecision)
  (td_t, time)
  (date_t, date)
  )

namespace {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  template<typename Iterator>
  struct parser_decoded: qi::grammar<Iterator, decoded()> {

    parser_decoded(): parser_decoded::base_type( start ) {

      ruleMsgType %=
          qi::char_( '7' ) // Price Level Summary
        | qi::char_( '8' ) // Price Level Update
        ;

      ruleString %= *( qi::char_ - qi::char_( ',' ) );
      ruleUint32 %= qi::uint_;
      ruleUint8  %= qi::ushort_;

      ruleOrderSide %=
          qi::char_( 'A' ) // Sell
        | qi::char_( 'B' ) // Buy)
        ;

      rulePrice %= qi::double_;

      ruleTime %=
          qi::long_ > qi::lit( ':' ) // HH
        > qi::long_ > qi::lit( ':' ) // mm
        > qi::long_ > qi::lit( '.' ) // ss
        > qi::long_                  // fractional
        ;

      ruleDate %=
          qi::long_ > qi::lit( '-' ) // year
        | qi::long_ > qi::lit( '-' ) // month
        | qi::long_                  // day
        ;

      start %=
          ruleMsgType > qi::lit( ',' ) // cMsgType
        > ruleString > qi::lit( ',' ) // sSymbolName
        > ruleOrderSide > qi::lit( ',' ) // ruleOrderSide
        > rulePrice > qi::lit( ',' ) // dblPrice
        > ruleUint32 > qi::lit( ',' ) // nLevelSize
        > ruleUint32 > qi::lit( ',' ) // nOrderCount
        > ruleUint8 > qi::lit( ',' ) // nPrecision
        > ruleTime > qi::lit( ',' ) // time
        > ruleDate > qi::lit( ',' ) // date
        ;

    }

    qi::rule<Iterator, char()> ruleMsgType;
    qi::rule<Iterator, std::string()> ruleString;
    qi::rule<Iterator, uint32_t()> ruleUint32;
    qi::rule<Iterator, char()> ruleOrderSide;
    qi::rule<Iterator, double()> rulePrice;
    qi::rule<Iterator, uint8_t()> ruleUint8;
    qi::rule<Iterator, td_t()> ruleTime;
    qi::rule<Iterator, date_t()> ruleDate;

    qi::rule<Iterator, decoded()> start;

  };
}

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message


} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
