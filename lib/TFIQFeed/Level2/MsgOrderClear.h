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
 * File:    MsgOrderClear.h
 * Author:  raymond@burkholder.net
 * Project: TFIQFeed/Level2
 * Created on September 28, 2022 21:19:16
 */

#pragma once

#include <string>

//#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
namespace l2 { // level 2 data
namespace msg { // message
namespace OrderClear {

//using td_t = boost::posix_time::time_duration;

struct decoded {
  char chMsgType;
  std::string sSymbolName;
  uint64_t nOrderId;
  union MMID {
    uint32_t id;
    char rch[ 4 ];
    MMID(): id {} {}
  } mmid;
  char chOrderSide;  // 'A' Sell, 'B' Buy
  uint32_t nQuantity;
  decoded(): nOrderId {}, nQuantity {} {}
  decoded( const std::string& sSymbolName_, char chOrderSide_ )
  : chMsgType( 'C' ), sSymbolName( sSymbolName_ ), chOrderSide( chOrderSide_ )
  , nOrderId {},nQuantity {} {}
};

} // namespace OrderClear
} // namespace msg
} // namespace l2
} // namesapce iqfeed
} // namespace tf
} // namespace ou
