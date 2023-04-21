/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    InterfaceBookOptionChain.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/12 23:21:16
 */

#pragma once

#include <string>
#include <functional>

#include <boost/date_time/gregorian/greg_date.hpp>

#include <TFTimeSeries/DatedDatum.h>

#include <TFTrading/TradingEnumerations.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

// acquire interface, use interface, dismiss interface

struct InterfaceBookOptionChain {

  using fOnPageEvent_t = std::function<void(boost::gregorian::date)>;

  virtual void Add( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const std::string& sSymbol ) = 0;

  virtual void Set(
    fOnPageEvent_t&& fOnPageChanging // departed
  , fOnPageEvent_t&& fOnPageChanged  // arrival
  ) = 0;

  virtual void MakeRowVisible( boost::gregorian::date, double strike ) = 0;

  // TODO: these updates are time intensive, can they be improved?
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Quote& ) = 0;
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Trade& ) = 0;
  virtual void Update( boost::gregorian::date, double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Greek& ) = 0;

  virtual void Clear(  boost::gregorian::date, double strike ) = 0;

// Order Interface
//   ComboOrder: fill order legs from leg list

// internal:
//   buy/sell call/put leg into leg list
};

} // namespace tf
} // namespace ou