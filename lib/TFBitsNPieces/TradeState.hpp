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
 * Project: TFBitsNPieces
 * Created: November 5, 2024 21:37
 */

#pragma once

#include <string>

#include <boost/describe/enum.hpp>

#define ETRADESTATE \
    Init,  /* initiaize state in current market */                             \
    Search,  /* looking for long or short enter */                             \
    EntrySubmittedUp, /* order has been submitted, waiting for confirmation */ \
    EntrySubmittedDn, /* order has been submitted, waiting for confirmation */ \
    ExitSignalUp,  /* position exists, looking for exit */                     \
    ExitSignalDn,  /* position exists, looking for exit */                     \
    ExitSubmitted, /* wait for exit to complete */                             \
    Cancelling,                                                                \
    Cancelled,                                                                 \
    NoTrade, /* from the config file, no trading, might be a future */         \
    EndOfDayCancel,                                                            \
    EndOfDayNeutral,                                                           \
    Done /* no more action */

class ETradeState {
public:

  enum E {
    ETRADESTATE
    };

  BOOST_DESCRIBE_NESTED_ENUM(
    E,
      ETRADESTATE
    )

  ETradeState(): m_state( E::Init ) {}

  E Set( E state );
  E operator=( E state ) { Set( state ); return m_state; }

   // __PRETTY_FUNCTION__ __LINE__
  E Set( E state, const char* func, const int line );
  E Set( E state, const std::string&, const char* func, const int line );

  E Get() const { return m_state; }
  E operator()() const { return m_state; };

protected:
private:
  E m_state;
};

