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

#include <TFTrading/Order.h>

#include "SimulationSymbol.hpp"
#include "SimulationInterface.hpp"

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
: public sim::SimulationInterface<SimulationProvider,SimulationSymbol>
{
  friend sim::SimulationInterface<SimulationProvider,SimulationSymbol>;
public:

  using inherited_t = sim::SimulationInterface<SimulationProvider,SimulationSymbol>;

  using pSymbol_t = inherited_t::pSymbol_t;

  using pOrder_t = Order::pOrder_t;
  using pInstrument_t = Instrument::pInstrument_t;
  using pInstrument_cref = Instrument::pInstrument_cref;
  using pProvider_t = std::shared_ptr<SimulationProvider>;

  SimulationProvider();
  virtual ~SimulationProvider();

  static pProvider_t Factory() {
    return std::make_shared<SimulationProvider>();
  }

  static pProvider_t Cast( pProvider_t pProvider ) {
    return std::dynamic_pointer_cast<SimulationProvider>( pProvider );
  }

  virtual void Connect();
  void Reset();
  virtual void Disconnect();

  void SetHdf5FileName( const std::string& );
  const std::string& GetGdf5FileName() const { return m_sHdf5FileName; }

  void SetGroupDirectory( const std::string& );  // eg /basket/20080620
  const std::string& GetGroupDirectory() const { return m_sGroupDirectory; }

  void Run( bool bAsync = true );
  void Stop();

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

  void EmitStats( std::stringstream& ss );

protected:

  std::string m_sHdf5FileName;
  std::string m_sGroupDirectory;

  ptime m_dtSimStart;
  ptime m_dtSimStop;
  unsigned long m_nProcessedDatums;

  pSymbol_t virtual NewCSymbol( pInstrument_t pInstrument );

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

  using pMerge_t = std::unique_ptr<MergeDatedDatums>;
  pMerge_t m_pMerge;

  std::thread m_threadMerge;

};

} // namespace tf
} // namespace ou
