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

#include <TFTimeSeries/TimeSeries.h>

#include <TFTrading/Symbol.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class SimulationSymbol: public Symbol<SimulationSymbol> {
  friend class SimulationProvider;
public:

  using inherited_t = Symbol<SimulationSymbol>;
  using pInstrument_t = inherited_t::pInstrument_t;
  using pInstrument_cref = inherited_t::pInstrument_cref;

  SimulationSymbol( const std::string& sSymbol
                  , pInstrument_cref pInstrument
                  , const std::string& sGroup
                  , const std::string& sFileName
                  ); // base with trades/ quotes/, greeks/, depths/
  virtual ~SimulationSymbol();

protected:

  void StartQuoteWatch();
  void StopQuoteWatch();

  void StartTradeWatch();
  void StopTradeWatch();

  void StartGreekWatch();
  void StopGreekWatch();

  void StartDepthByMMWatch();
  void StopDepthByMMWatch();

  void StartDepthByOrderWatch();
  void StopDepthByOrderWatch();

  void HandleQuoteEvent( const DatedDatum &datum );
  void HandleTradeEvent( const DatedDatum &datum );
  void HandleGreekEvent( const DatedDatum &datum );

  void HandleDepthByMMEvent( const DatedDatum &datum );
  void HandleDepthByOrderEvent( const DatedDatum &datum );

private:

  std::string m_sFileName;
  std::string m_sDirectory;

  Quotes m_quotes;
  Trades m_trades;
  DepthsByMM m_depths_mm;
  DepthsByOrder m_depths_order;
  Greeks m_greeks;

};

} // namespace tf
} // namespace ou
