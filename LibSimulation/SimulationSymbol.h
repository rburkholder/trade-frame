/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "LibTimeSeries/TimeSeries.h"
#include "LibTrading/Symbol.h"

#include "SimulateOrderExecution.h"

class CSimulationSymbol: public CSymbol {
  friend class CSimulationProvider;
public:
  CSimulationSymbol(const std::string &sSymbol, const std::string &sDirectory );
  virtual ~CSimulationSymbol(void);
protected:
  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );

  //virtual bool AddTradeHandler( CSymbol::tradehandler_t ); 
  //virtual bool RemoveTradeHandler( CSymbol::tradehandler_t );

  void HandleTradeEvent( const CDatedDatum &datum );
  void HandleQuoteEvent( const CDatedDatum &datum );

  std::string m_sDirectory;

  CQuotes m_quotes;
  CTrades m_trades;

  CSimulateOrderExecution m_simExec;
private:
};
