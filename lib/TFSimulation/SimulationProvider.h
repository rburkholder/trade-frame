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

#include <thread>
#include <string>
#include <sstream>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <OUCommon/TimeSource.h>

#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Order.h>

#include "SimulationSymbol.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class MergeDatedDatums;

// simulation provider needs to send an open event on each symbol it does
//  will need to be based upon time
// looks like MergeDatedDatums will need an OnOpen event simulated

// 20100821:  todo: provide cache mechanism for multiple runs
//    first time through, use the minheap,
//    subsequent times through, scan a vector

class SimulationProvider
: public ProviderInterface<SimulationProvider,SimulationSymbol>
{
public:

  using pProvider_t = std::shared_ptr<SimulationProvider>;
  using inherited_t = ProviderInterface<SimulationProvider,SimulationSymbol>;
  using pInstrument_t = Instrument::pInstrument_t;
  using pInstrument_cref = Instrument::pInstrument_cref;
  using pOrder_t = Order::pOrder_t;
  using pSymbol_t = inherited_t::pSymbol_t;

  SimulationProvider();
  virtual ~SimulationProvider();

  static pProvider_t Factory() {
    return std::make_shared<SimulationProvider>();
  }

  static pProvider_t Cast( inherited_t::pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<SimulationProvider>( pProvider );
  }

  virtual void Connect();
  virtual void Disconnect();

  void SetGroupDirectory( const std::string sGroupDirectory );  // eg /basket/20080620
  const std::string& GetGroupDirectory() const { return m_sGroupDirectory; };

  void Run( bool bAsync = true );
  void Stop();
  void PlaceOrder( pOrder_t pOrder );
  void CancelOrder( pOrder_t pOrder );

  void AddQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler );
  void RemoveQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler );
  void AddTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler );
  void RemoveTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler );

  void AddDepthByMMHandler( pInstrument_cref pInstrument, SimulationSymbol::depthbymmhandler_t handler );
  void RemoveDepthByMMHandler( pInstrument_cref pInstrument, SimulationSymbol::depthbymmhandler_t handler );
  void AddDepthByOrderHandler( pInstrument_cref pInstrument, SimulationSymbol::depthbyorderhandler_t handler );
  void RemoveDepthByOrderHandler( pInstrument_cref pInstrument, SimulationSymbol::depthbyorderhandler_t handler );

  void EmitStats( std::stringstream& ss );

  using OnSimulationThreadStarted_t = FastDelegate0<>; // Allows Singleton LocalCommonInstances to be set, called within new thread
  void SetOnSimulationThreadStarted( OnSimulationThreadStarted_t function ) {
    m_OnSimulationThreadStarted = function;
  }
  using OnSimulationThreadEnded_t = FastDelegate0<>; // Allows Singleton LocalCommonInstances to be reset
  void SetOnSimulationThreadEnded( OnSimulationThreadEnded_t function ) {
    m_OnSimulationThreadEnded = function;
  }

  using OnSimulationComplete_t = FastDelegate0<>;
  void SetOnSimulationComplete( OnSimulationComplete_t function ) {
    m_OnSimulationComplete = function;
  }

  void SetCommission( const std::string& sSymbol, double commission );

protected:

  std::string m_sGroupDirectory;

  ptime m_dtSimStart;
  ptime m_dtSimStop;
  unsigned long m_nProcessedDatums;

  MergeDatedDatums* m_pMerge;

  pSymbol_t NewCSymbol( SimulationSymbol::pInstrument_t pInstrument );

  void StartQuoteWatch( pSymbol_t pSymbol );
  void StopQuoteWatch( pSymbol_t Symbol );
  void StartTradeWatch( pSymbol_t pSymbol );
  void StopTradeWatch( pSymbol_t pSymbol );
  void StartDepthByMMWatch( pSymbol_t pSymbol );
  void StopDepthByMMWatch( pSymbol_t pSymbol );
  void StartDepthByOrderWatch( pSymbol_t pSymbol );
  void StopDepthByOrderWatch( pSymbol_t pSymbol );
  void StartGreekWatch( pSymbol_t pSymbol );
  void StopGreekWatch( pSymbol_t pSymbol );

  OnSimulationThreadStarted_t m_OnSimulationThreadStarted;
  OnSimulationThreadEnded_t m_OnSimulationThreadEnded;
  OnSimulationComplete_t m_OnSimulationComplete;

  void Merge();  // the background thread

  void HandleExecution( Order::idOrder_t orderId, const Execution &exec );
  void HandleCommission( Order::idOrder_t orderId, double commission );
  void HandleCancellation( Order::idOrder_t orderId );

private:

  std::thread m_threadMerge;

};

} // namespace tf
} // namespace ou
