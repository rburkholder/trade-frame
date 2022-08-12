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
 * File:      Server.hpp
 * Author:    raymond@burkholder.net
 * Project:   TableTrader
 * Created:   2022/08/02 09:58:23
 */

#ifndef SERVER_H
#define SERVER_H

#include <Wt/WServer.h>

#include "Config.hpp"

class Server_impl;

class Server: public Wt::WServer {
public:

  Server(
    int argc, char *argv[],
    const config::Choices&,
    const std::string &wtConfigurationFile=std::string()
    );
  virtual ~Server();

  bool ValidateLogin( const std::string& sUserName, const std::string& sPassWord );

  using fAddCandidateFutures_t = std::function<void(const std::string&)>;
  void AddCandidateFutures( fAddCandidateFutures_t&& );

  using fUpdateUnderlyingInfo_t = std::function<void(const std::string&, const std::string&)>; // name, multiplier
  using fUpdateUnderlyingPrice_t = std::function<void(const std::string&)>; // price
  using fUpdateOptionExpiries_t = std::function<void(const std::string&)>;
  using fUpdateOptionExpiriesDone_t = std::function<void()>;

  using fPopulateStrike_t = std::function<void(const std::string&)>;
  using fPopulateStrikeDone_t = std::function<void()>;

  using fPopulateOption_t = std::function<void(const std::string&)>; // ticker
  using fUpdateAllocated_t = std::function<void(const std::string&, const std::string&)>; // total allocation, option allocation
  using fRealTime_t = std::function<void(
    const std::string& oi,
    const std::string& bid, const std::string& ask,
    const std::string& vol, const std::string& num,
    const std::string& pnl)>;
  using fFill_t = std::function<void(const std::string&)>; // #filled@price

  void SessionAttach( const std::string& sSessionId );
  void SessionDetach( const std::string& sSessionId );

  void Start(
    const std::string& sSessionId, const std::string& sUnderlyingFuture,
    fUpdateUnderlyingInfo_t&&,
    fUpdateUnderlyingPrice_t&&,
    fUpdateOptionExpiries_t&&,
    fUpdateOptionExpiriesDone_t&&
    );

  void ChangeInvestment( const std::string& );

  void TriggerUpdates( const std::string& sSessionId );

  void PrepareStrikeSelection(
    const std::string& sDate,
    fPopulateStrike_t&&,
    fPopulateStrikeDone_t&&
    );

  static double FormatDouble( const std::string sValue, std::string sMessage );
  std::string FormatStrike( double ) const;

  enum class EOptionType { call, put };
  enum class EOrderSide { buy , sell };

  void AddStrike(
    const std::string& sSessionId,
    EOptionType, EOrderSide,
    const std::string&, // type, strike
    fPopulateOption_t&&, fUpdateAllocated_t&&, fRealTime_t&&,
    fFill_t&& entry, fFill_t&& exit
    );
  void DelStrike( const std::string& ); // strike

  void ChangeAllocation( const std::string& sStrike, const std::string& sPercent );

  enum class EOrderType { market, limit_manual, limit_ask, limit_bid, scale };
  std::string SetOrderType(
    EOrderType,
    const std::string& sStrike,
    const std::string& sLimitPrice,
    const std::string& sInitialQuantity,
    const std::string& sIncrementQuantity,
    const std::string& sIncrementPrice
  );

  bool PlaceOrders();
  void CancelAll();
  void CloseAll();

protected:
private:
  const config::Choices& m_choices;

  std::unique_ptr<Server_impl> m_implServer;

  fUpdateUnderlyingInfo_t m_fUpdateUnderlyingInfo;
  fUpdateUnderlyingPrice_t m_fUpdateUnderlyingPrice;
  fUpdateOptionExpiries_t m_fUpdateOptionExpiries;
  fUpdateOptionExpiriesDone_t m_fUpdateOptionExpiriesDone;

};

#endif /* SERVER_H */

