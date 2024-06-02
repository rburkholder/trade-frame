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
 * File:    PanelOrderButtons_structs.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: March 9, 2022 09:35
 */

#pragma once

#include <string>
#include <cstdint>

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelOrderButtons_Order {

  enum class EOrderMethod { Buy=1, Sell=2, Cancel=3 };
  enum class EPositionEntryMethod { Market=1, LimitOnly=2, LimitTimeOut=3, Stoch=4 };
  enum class EPositionExitProfitMethod { Relative=1, Absolute=2, Stoch=3 };
  enum class EPositionExitStopMethod { TrailingAbsolute=1, TrailingPercent=2, Stop=3 };
  enum class EInstrument {
    Underlying=100,
    SynthLong=31, CallItm=11, PutOtm=22,
    SynthShort=32, CallOtm=12, PutItm=21
    };

  std::string m_sQuanStock;
  std::string m_sQuanFuture;
  std::string m_sQuanOption;

  bool m_bPositionEntryEnable;
  std::string m_sPositionEntryValue;
  EPositionEntryMethod m_ePositionEntryMethod;

  bool m_bPositionExitProfitEnable;
  std::string m_sPositionExitProfitValue;
  EPositionExitProfitMethod m_ePositionExitProfitMethod;

  bool m_bPositionExitStopEnable;
  std::string m_sPositionExitStopValue;
  EPositionExitStopMethod m_ePositionExitStopMethod;

  EInstrument m_eInstrument;

  bool m_bStochastic1;
  bool m_bStochastic2;
  bool m_bStochastic3;

  PanelOrderButtons_Order()
  : m_bPositionEntryEnable( true ), m_ePositionEntryMethod( EPositionEntryMethod::Market )
  , m_bPositionExitProfitEnable( false ), m_ePositionExitProfitMethod( EPositionExitProfitMethod::Relative )
  , m_bPositionExitStopEnable( false ), m_ePositionExitStopMethod( EPositionExitStopMethod::TrailingAbsolute )
  , m_eInstrument( EInstrument::Underlying )
  , m_bStochastic1( false ), m_bStochastic2( false ), m_bStochastic3( false )
  , m_sQuanStock( "100" ), m_sQuanFuture( "1" ), m_sQuanOption( "1" ) // needs to match prefilled gui fields
  {}

  double PositionEntryValue() const;
  double PositionExitProfitValue() const;
  double PositionExitStopValue() const;

  std::uint32_t QuanStock() const;
  std::uint32_t QuanFuture() const;
  std::uint32_t QuanOption() const;
};

struct PanelOrderButtons_MarketData {

  // to facilitate cross thread update, strings are moved in Update

  std::string m_sBase; //  // expiry only?  with popup of full iqf & custom?
  std::string m_sBaseAsk;
  std::string m_sBaseBid;

  bool m_bOptionPresent;

  std::string m_sCall1; // expiry & strike only?  with popup of full iqf & custom?
  std::string m_sCall1Ask;
  std::string m_sCall1Bid;

  std::string m_sPut1; // expiry & strike only?  with popup of full iqf & custom?
  std::string m_sPut1Ask;
  std::string m_sPut1Bid;

  std::string m_sCall2; // expiry & strike only?  with popup of full iqf & custom?
  std::string m_sCall2Ask;
  std::string m_sCall2Bid;

  std::string m_sPut2; // expiry & strike only?  with popup of full iqf & custom?
  std::string m_sPut2Ask;
  std::string m_sPut2Bid;

  PanelOrderButtons_MarketData(): m_bOptionPresent( false ) {}

};

struct PanelOrderButtons_PositionData {

  // to facilitate cross thread update, strings are moved in Update

  std::string m_sSymbol;
  std::string m_sQuantity;
  std::string m_sProfitLoss;

};

} // namespace tf
} // namespace ou
