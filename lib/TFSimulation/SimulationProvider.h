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
#include <sstream>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>  // separate thread background merge processing
#include <boost/bind.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <OUCommon/TimeSource.h>
#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Order.h>
#include <TFTimeSeries/MergeDatedDatums.h>

#include "SimulationSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// simulation provider needs to send an open event on each symbol it does
//  will need to be based upon time
// looks like MergeDatedDatums will need an OnOpen event simulated

// 20100821:  todo: provide cache mechanism for multiple runs
//    first time through, use the minheap, 
//    subsequent times through, scan a vector

class SimulationProvider
: public CProviderInterface<SimulationProvider,SimulationSymbol>
{
public:

  typedef boost::shared_ptr<SimulationProvider> pProvider_t;
  typedef CProviderInterface<SimulationProvider,SimulationSymbol> inherited_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef Order::pOrder_t pOrder_t;
  typedef inherited_t::pSymbol_t pSymbol_t;

  SimulationProvider(void);
  virtual ~SimulationProvider(void);
  virtual void Connect( void );
  virtual void Disconnect( void );

  void SetGroupDirectory( const std::string sGroupDirectory );  // eg /basket/20080620
  const std::string &GetGroupDirectory( void ) { return m_sGroupDirectory; };

  void Run( bool bAsync = true );
  void Stop( void );
  void PlaceOrder( pOrder_t pOrder );
  void CancelOrder( pOrder_t pOrder );

  void AddTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler );
  void RemoveTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler );
  void AddQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler );
  void RemoveQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler );

  void EmitStats( std::stringstream& ss );

  typedef FastDelegate0<> OnSimulationThreadStarted_t; // Allows Singleton LocalCommonInstances to be set, called within new thread
  void SetOnSimulationThreadStarted( OnSimulationThreadStarted_t function ) {
    m_OnSimulationThreadStarted = function;
  }
  typedef FastDelegate0<> OnSimulationThreadEnded_t; // Allows Singleton LocalCommonInstances to be reset
  void SetOnSimulationThreadEnded( OnSimulationThreadEnded_t function ) {
    m_OnSimulationThreadEnded = function;
  }

  typedef FastDelegate0<> OnSimulationComplete_t;
  void SetOnSimulationComplete( OnSimulationComplete_t function ) {
    m_OnSimulationComplete = function;
  }

protected:

  pSymbol_t NewCSymbol( SimulationSymbol::pInstrument_t pInstrument );
  void StartQuoteWatch( pSymbol_t pSymbol );
  void StopQuoteWatch( pSymbol_t Symbol );
  void StartTradeWatch( pSymbol_t pSymbol );
  void StopTradeWatch( pSymbol_t pSymbol );
  void StartDepthWatch( pSymbol_t pSymbol );
  void StopDepthWatch( pSymbol_t pSymbol );
  void StartGreekWatch( pSymbol_t pSymbol );
  void StopGreekWatch( pSymbol_t pSymbol );

  std::string m_sGroupDirectory;

  MergeDatedDatums* m_pMerge;

  OnSimulationThreadStarted_t m_OnSimulationThreadStarted;
  OnSimulationThreadEnded_t m_OnSimulationThreadEnded;
  OnSimulationComplete_t m_OnSimulationComplete;

  void Merge( void );  // the background thread

  void HandleExecution( Order::idOrder_t orderId, const CExecution &exec );
  void HandleCommission( Order::idOrder_t orderId, double commission );
  void HandleCancellation( Order::idOrder_t orderId );

  ptime m_dtSimStart;
  ptime m_dtSimStop;
  unsigned long m_nProcessedDatums;

private:

  boost::thread m_threadMerge;

};

} // namespace tf
} // namespace ou
