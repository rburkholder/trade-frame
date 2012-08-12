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

class SimulationSymbol: public CSymbol<SimulationSymbol> {
  friend class SimulationProvider;
public:

  typedef CSymbol<SimulationSymbol> inherited_t;
  typedef inherited_t::pInstrument_t pInstrument_t;
  typedef inherited_t::pInstrument_cref pInstrument_cref;
  typedef inherited_t::trade_t trade_t;
  typedef inherited_t::quote_t quote_t;
  typedef inherited_t::greek_t greek_t;
  
  SimulationSymbol( const std::string& sSymbol, 
                     pInstrument_cref pInstrument, 
                     const std::string& sGroup ); // base with trades/ quotes/, greeks/
  ~SimulationSymbol(void);

protected:

  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );
  void StartGreekWatch( void );
  void StopGreekWatch( void );

  void HandleTradeEvent( const DatedDatum &datum );
  void HandleQuoteEvent( const DatedDatum &datum );
  void HandleGreekEvent( const DatedDatum &datum );

  std::string m_sDirectory;

  Quotes m_quotes;
  Trades m_trades;
  Greeks m_greeks;

  SimulateOrderExecution m_simExec;

private:

};

} // namespace tf
} // namespace ou
