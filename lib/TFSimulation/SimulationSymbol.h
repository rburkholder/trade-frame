/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include <boost/detail/atomic_count.hpp>

#include "TFTimeSeries/TimeSeries.h"

#include "TFTrading/Symbol.h"

#include "SimulateOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class SimulationSymbol: public Symbol<SimulationSymbol> {
  friend class SimulationProvider;
public:

  using inherited_t = Symbol<SimulationSymbol>;
  using pInstrument_t = inherited_t::pInstrument_t;
  using pInstrument_cref = inherited_t::pInstrument_cref;
  using trade_t = inherited_t::trade_t;
  using quote_t = inherited_t::quote_t;
  using greek_t = inherited_t::greek_t;

  SimulationSymbol( const std::string& sSymbol,
                    pInstrument_cref pInstrument,
                    const std::string& sGroup ); // base with trades/ quotes/, greeks/
  virtual ~SimulationSymbol();

protected:

  void StartQuoteWatch();
  void StopQuoteWatch();

  void StartDepthWatch();
  void StopDepthWatch();

  void StartTradeWatch();
  void StopTradeWatch();

  void StartGreekWatch();
  void StopGreekWatch();

  void HandleQuoteEvent( const DatedDatum &datum );
  void HandleDepthEvent( const DatedDatum &datum );
  void HandleTradeEvent( const DatedDatum &datum );
  void HandleGreekEvent( const DatedDatum &datum );

  std::string m_sDirectory;

  Quotes m_quotes;
  Trades m_trades;
  MarketDepths m_depths;
  Greeks m_greeks;

  SimulateOrderExecution m_simExec;

private:

};

} // namespace tf
} // namespace ou
