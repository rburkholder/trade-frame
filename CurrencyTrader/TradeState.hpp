/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    TradeState.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: November 5, 2024 21:37
 */

#pragma once

#include <string>

class TradeState {
public:

  enum class ETradeState {
    Init,  // initiaize state in current market
    Search,  // looking for long or short enter
    EntrySubmittedUp, // order has been submitted, waiting for confirmation
    EntrySubmittedDn, // order has been submitted, waiting for confirmation
    ExitSignalUp,  // position exists, looking for exit
    ExitSignalDn,  // position exists, looking for exit
    ExitSubmitted, // wait for exit to complete
    Cancelling,
    Cancelled,
    NoTrade, // from the config file, no trading, might be a future
    EndOfDayCancel,
    EndOfDayNeutral,
    Done // no more action
    };

  TradeState(): m_stateTrade( ETradeState::Init ) {}

   // __PRETTY_FUNCTION__ __LINE__
  ETradeState Set( ETradeState state );
  ETradeState Set( ETradeState state, const std::string& func, const std::string& line );
  ETradeState Get() const { return m_stateTrade; }

protected:
private:
  ETradeState m_stateTrade;
};

