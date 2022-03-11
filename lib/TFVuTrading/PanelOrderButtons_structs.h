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

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelOrderButtons_Order {

  enum class EPositionEntryMethod { Market=1, Limit=2, Stoch=3 };
  enum class EPositionExitProfitMethod { Relative=1, Absolute=2, Stoch=3 };
  enum class EPositionExitStopMethod { TrailingAbsolute=1, TrailingPercent=2, Stop=3 };
  enum class EInstrument { Underlying=1, Call1=11, Put1=12, Call2=21, Put2=22 };

  bool m_bCockForCursor;

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
  : m_bCockForCursor( false )
  , m_bPositionEntryEnable( true ), m_ePositionEntryMethod( EPositionEntryMethod::Market )
  , m_bPositionExitProfitEnable( false ), m_ePositionExitProfitMethod( EPositionExitProfitMethod::Relative )
  , m_bPositionExitStopEnable( false ), m_ePositionExitStopMethod( EPositionExitStopMethod::TrailingAbsolute )
  , m_eInstrument( EInstrument::Underlying )
  , m_bStochastic1( false ), m_bStochastic2( false ), m_bStochastic3( false )
  {}

  double PositionEntryValue() const;
  double PositionExitProfitValue() const;
  double PositionExitStopValue() const;
};

struct PanelOrderButtons_MarketData {

  std::string m_sBase;
  std::string m_sBaseAsk;
  std::string m_sBaseBid;

  std::string m_sCall1;
  std::string m_sCall1Ask;
  std::string m_sCall1Bid;

  std::string m_sPut1;
  std::string m_sPut1Ask;
  std::string m_sPut1Bid;

  std::string m_sCall2;
  std::string m_sCall2Ask;
  std::string m_sCall2Bid;

  std::string m_sPut2;
  std::string m_sPut2Ask;
  std::string m_sPut2Bid;

};

} // namespace tf
} // namespace ou
