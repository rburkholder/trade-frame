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
// looks like CMergeDatedDatums will need an OnOpen event simulated

// 20100821:  todo: provide cache mechanism for multiple runs
//    first time through, use the minheap, 
//    subsequent times through, scan a vector

class CSimulationProvider
: public CProviderInterface<CSimulationProvider,CSimulationSymbol>
{
public:

  typedef boost::shared_ptr<CSimulationProvider> pProvider_t;
  typedef CProviderInterface<CSimulationProvider,CSimulationSymbol> inherited_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef COrder::pOrder_t pOrder_t;
  typedef inherited_t::pSymbol_t pSymbol_t;
  typedef CSimulateOrderExecution::enumExecuteAgainst enumExecuteAgainst;

  CSimulationProvider(void);
  virtual ~CSimulationProvider(void);
  virtual void Connect( void );
  virtual void Disconnect( void );

  void SetGroupDirectory( const std::string sGroupDirectory );  // eg /basket/20080620
  const std::string &GetGroupDirectory( void ) { return m_sGroupDirectory; };
//  void SetExecuteAgainst( enumExecuteAgainst ea ) { m_ea = ea; };
//  enumExecuteAgainst GetExecuteAgainst( void ) { return m_ea; };

  void Run( void );
  void Stop( void );
  void PlaceOrder( pOrder_t pOrder );
  void CancelOrder( pOrder_t pOrder );

  void AddTradeHandler( pInstrument_cref pInstrument, CSimulationSymbol::tradehandler_t handler );
  void RemoveTradeHandler( pInstrument_cref pInstrument, CSimulationSymbol::tradehandler_t handler );
  void AddQuoteHandler( pInstrument_cref pInstrument, CSimulationSymbol::quotehandler_t handler );
  void RemoveQuoteHandler( pInstrument_cref pInstrument, CSimulationSymbol::quotehandler_t handler );

  void EmitStats( std::stringstream& ss );

  typedef FastDelegate0<> OnSimulationComplete_t;
  void SetOnSimulationComplete( OnSimulationComplete_t function ) {
    m_OnSimulationComplete = function;
  }

protected:

//  enumExecuteAgainst m_ea;

  pSymbol_t NewCSymbol( CSimulationSymbol::pInstrument_t pInstrument );
  void StartQuoteWatch( pSymbol_t pSymbol );
  void StopQuoteWatch( pSymbol_t Symbol );
  void StartTradeWatch( pSymbol_t pSymbol );
  void StopTradeWatch( pSymbol_t pSymbol );
  void StartDepthWatch( pSymbol_t pSymbol );
  void StopDepthWatch( pSymbol_t pSymbol );
  void StartGreekWatch( pSymbol_t pSymbol );
  void StopGreekWatch( pSymbol_t pSymbol );

  std::string m_sGroupDirectory;

  CMergeDatedDatums *m_pMerge;

  OnSimulationComplete_t m_OnSimulationComplete;

  void Merge( void );  // the background thread

  void HandleExecution( COrder::idOrder_t orderId, const CExecution &exec );
  void HandleCommission( COrder::idOrder_t orderId, double commission );
  void HandleCancellation( COrder::idOrder_t orderId );

  ptime m_dtSimStart;
  ptime m_dtSimStop;
  unsigned long m_nProcessedDatums;

private:

  boost::thread m_threadMerge;

};

} // namespace tf
} // namespace ou
