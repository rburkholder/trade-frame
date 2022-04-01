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
 * File:    MsgOrderDelete.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on October 16, 2021 22:12
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
namespace OrderDelete {

//using date_t = boost::gregorian::date;
//using td_t = boost::posix_time::time_duration;

//  '5,TSLA,,CHXE,B,,2022-04-01,'
struct decoded {
  char chMsgType;
  std::string sSymbolName;
  uint64_t nOrderId;
  std::string sMarketMaker;
  char chOrderSide;  // 'A' Sell, 'B' Buy
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

} // namespace OrderDelete
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou

namespace msg_delete_t = ou::tf::iqfeed::l2::msg::OrderDelete;

BOOST_FUSION_ADAPT_STRUCT(
  msg_delete_t::decoded,
  (char, chMsgType)
  (std::string, sSymbolName)
  (uint64_t, nOrderId)
  (std::string, sMarketMaker)
  (char, chOrderSide)
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
namespace OrderDelete {

  namespace qi = boost::spirit::qi;
  namespace ascii = boost::spirit::ascii;

  namespace msg_t = ou::tf::iqfeed::l2::msg::OrderDelete;

  template<typename Iterator>
  struct parser_decoded: qi::grammar<Iterator, decoded()> {

    parser_decoded(): parser_decoded::base_type( start ) {

      ruleMsgType %=
          qi::char_( '5' ) // Order Delete
        ;

      ruleUint32 %= qi::ulong_;
      ruleUint64 %= qi::ulong_long;
      ruleString %= *( qi::char_ - qi::char_( ',' ) );

      ruleOrderSide %=
          qi::char_( 'A' ) // Sell
        | qi::char_( 'B' ) // Buy)
        ;
/*
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
*/
      start %=
           ruleMsgType >> qi::lit( ',' ) // cMsgType
        >> ruleString >> qi::lit( ',' ) // sSymbolName
        >> -ruleUint64 >> qi::lit( ',' ) // nOrderId
        >> -ruleString >> qi::lit( ',' ) // market maker for nasdaq LII
        >> ruleOrderSide >> qi::lit( ',' ) // ruleOrderSide
        >> ruleUint32 >> qi::lit( ':' ) // hours
        >> ruleUint32 >> qi::lit( ':' ) // minutes
        >> ruleUint32 >> qi::lit( '.' ) // seconds
        >> ruleUint32 >> qi::lit( ',' ) // fractional
        //>> -ruleTime >> qi::lit( ',')
        >> ruleUint32 >> qi::lit( '-' ) // year
        >> ruleUint32 >> qi::lit( '-' ) // month
        >> ruleUint32 // day
        >> qi::lit( ',' )
        ;

    }

    qi::rule<Iterator, char()> ruleMsgType;
    qi::rule<Iterator, char()> ruleOrderSide;
    qi::rule<Iterator, uint32_t()> ruleUint32;
    qi::rule<Iterator, uint64_t()> ruleUint64;
    qi::rule<Iterator, std::string()> ruleString;
    //qi::rule<Iterator, td_t()> ruleTime;
    //qi::rule<Iterator, date_t()> ruleDate;

    qi::rule<Iterator, decoded()> start;

  };

  template <typename T>
  bool Decode( decoded& out, T begin, T end ) {

    static parser_decoded<T> parser;

    // '5,TSLA,,CHXE,B,,2022-04-01,' nasdaq LII
    // "5,@ESZ21,648907593934,,A,20:32:47.333543,2021-10-24,"
    bool bOk = parse( begin, end, parser, out );

    return bOk;
  }

} // namespace OrderDelete
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
