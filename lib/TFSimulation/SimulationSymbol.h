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

class CSimulationSymbol: public CSymbol<CSimulationSymbol> {
  friend class CSimulationProvider;
public:

  typedef CSymbol<CSimulationSymbol> inherited_t;
  typedef inherited_t::pInstrument_t pInstrument_t;
  typedef inherited_t::pInstrument_cref pInstrument_cref;
  typedef inherited_t::trade_t trade_t;
  typedef inherited_t::quote_t quote_t;
  typedef inherited_t::greek_t greek_t;
  
  CSimulationSymbol( const std::string& sSymbol, 
                     pInstrument_cref pInstrument, 
                     const std::string& sGroup ); // base with trades/ quotes/, greeks/
  ~CSimulationSymbol(void);

protected:

  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );
  void StartGreekWatch( void );
  void StopGreekWatch( void );

  void HandleTradeEvent( const CDatedDatum &datum );
  void HandleQuoteEvent( const CDatedDatum &datum );
  void HandleGreekEvent( const CDatedDatum &datum );

  std::string m_sDirectory;

  CQuotes m_quotes;
  CTrades m_trades;
  CGreeks m_greeks;

  CSimulateOrderExecution m_simExec;

private:

};

} // namespace tf
} // namespace ou
