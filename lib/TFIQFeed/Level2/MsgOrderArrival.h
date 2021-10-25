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
 * File:    MsgOrderArrival.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 16, 2021 20:02
 */

#pragma once

#define BOOST_SPIRIT_USE_PHOENIX_V3 1

#include <string>

//#include <boost/date_time/gregorian/gregorian_types.hpp>
//#include <boost/date_time/posix_time/posix_time_types.hpp>

#include <boost/spirit/include/qi.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
//#include <boost/spirit/include/phoenix_bind.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message
namespace OrderArrival {

//using date_t = boost::gregorian::date;
//using time_t = boost::posix_time::time_duration;

struct decoded {
  char chMsgType;
  std::string sSymbolName;
  uint64_t nOrderId;
  std::string sMarketMaker;
  char chOrderSide;  // 'A' Sell, 'B' Buy
  double dblPrice;
  uint32_t nQuantity;
  uint64_t nPriority;
  uint8_t nPrecision;
  //time_t time;
  int32_t hours;
  int32_t minutes;
  int32_t seconds;
  int32_t fractional;
  //date_t date;
  int32_t year;
  int32_t month;
  int32_t day;
};

} // namespace OrderArrival
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou

namespace arrival_msg_t = ou::tf::iqfeed::l2::msg::OrderArrival;

BOOST_FUSION_ADAPT_STRUCT(
  arrival_msg_t::decoded,
  (char, chMsgType)
  (std::string, sSymbolName)
  (uint64_t, nOrderId)
  (std::string, sMarketMaker)
  (char, chOrderSide)
  (double, dblPrice)
  (uint32_t, nQuantity)
  (uint64_t, nPriority)
  (uint8_t, nPrecision)
  //(msg_t::time_t,time)
  (int32_t, hours)
  (int32_t, minutes)
  (int32_t, seconds)
  (int32_t, fractional)
  //(msg_t::date_t,date)
  (int32_t, year)
  (int32_t, month)
  (int32_t, day)
  )

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message
namespace OrderArrival {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  template<typename Iterator>
  struct parser_decoded: qi::grammar<Iterator, decoded()> {

    parser_decoded(): parser_decoded::base_type( start ) {

      ruleMsgType %=
          qi::char_( '6' ) // Order Summary
        | qi::char_( '3' ) // Order Add
        | qi::char_( '4' ) // Order Update
        | qi::char_( '0' ) // Price Level Order
        ;

      ruleUint8  %= qi::ushort_;
      ruleUint32 %= qi::ulong_;
      ruleUint64 %= qi::ulong_long;
      ruleString %= *( qi::char_ - qi::char_( ',' ) );

      ruleOrderSide %=
          qi::char_( 'A' ) // Sell
        | qi::char_( 'B' ) // Buy)
        ;

      rulePrice %= qi::double_;
/*
      ruleTime %=
        (
           qi::long_ >> qi::lit( ':' ) // HH
        >> qi::long_ >> qi::lit( ':' ) // mm
        >> qi::long_ >> qi::lit( '.' ) // ss
        >> qi::long_                  // fractional
        )
        // NOTE: unable to make this assignment work, even with:
        [ qi::_val = time_t( qi::_1, qi::_2, qi::_3, qi::_4 ) ]
        ;

      ruleDate %=
           qi::long_ >> qi::lit( '-' ) // year
        >> qi::long_ >> qi::lit( '-' ) // month
        >> qi::long_                  // day
        ;

      start0 %=
           ruleMsgType > qi::lit( ',' ) // cMsgType
        >> ruleString > qi::lit( ',' ) // sSymbolName
        >> ruleUint64 > qi::lit( ',' ) // nOrderId
        >> ruleString > qi::lit( ',' ) // sMarketMaker
        >> ruleOrderSide > qi::lit( ',' ) // ruleOrderSide
        >> rulePrice > qi::lit( ',' ) // dblPrice
        >> ruleUint32 > qi::lit( ',' ) // nQuantity
        >> ruleUint64 > qi::lit( ',' ) // nPriority
        >> ruleUint8 > qi::lit( ',' ) // nPrecision
        >> ruleTime > qi::lit( ',' ) // time
        >> ruleDate > qi::lit( ',' ) // date
        ;
*/
      start %=
           ruleMsgType >> qi::lit( ',' ) // cMsgType
        >> ruleString >> qi::lit( ',' ) // sSymbolName
        >> ruleUint64 >> qi::lit( ',' ) // nOrderId
        >> ruleString >> qi::lit( ',' ) // sMarketMaker
        >> ruleOrderSide >> qi::lit( ',' ) // ruleOrderSide
        >> rulePrice >> qi::lit( ',' ) // dblPrice
        >> ruleUint32 >> qi::lit( ',' ) // nQuantity
        >> ruleUint64 >> qi::lit( ',' ) // nPriority
        >> ruleUint8 >> qi::lit( ',' ) // nPrecision
        >> ruleUint32 >> qi::lit( ':' ) // hours
        >> ruleUint32 >> qi::lit( ':' ) // minutes
        >> ruleUint32 >> qi::lit( '.' ) // seconds
        >> ruleUint32 >> qi::lit( ',' ) // fractional
        >> ruleUint32 >> qi::lit( '-' ) // year
        >> ruleUint32 >> qi::lit( '-' ) // month
        >> ruleUint32 // day
        ;

    }

    qi::rule<Iterator, char()> ruleMsgType;
    qi::rule<Iterator, char()> ruleOrderSide;
    qi::rule<Iterator, uint8_t()> ruleUint8;
    qi::rule<Iterator, uint32_t()> ruleUint32;
    qi::rule<Iterator, uint64_t()> ruleUint64;
    qi::rule<Iterator, double()> rulePrice;
    qi::rule<Iterator, std::string()> ruleString;
    //qi::rule<Iterator, time_t()> ruleTime;
    //qi::rule<Iterator, date_t()> ruleDate;

    //qi::rule<Iterator, msg_t::decoded()> start0;
    qi::rule<Iterator, decoded()> start;

  };

  template <typename T>
  bool Decode( decoded& out, T begin, T end ) {

    static parser_decoded<T> parser;

    bool bOk = parse( begin, end, parser, out );

    return bOk;
  }

} // namespace OrderArrival
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
