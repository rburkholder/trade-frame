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

#include <boost/thread.hpp>  // separate thread background merge processing
#include <boost/bind.hpp>

#include <LibCommon/TimeSource.h>
#include <LibTrading/ProviderInterface.h>
#include <LibTrading/Order.h>

#include "SimulationSymbol.h"
#include "CrossThreadMerge.h"

// simulation provider needs to send an open event on each symbol it does
//  will need to be based upon time
// looks like CMergeDatedDatums will need an OnOpen event simulated

class CSimulationProvider
: public CProviderInterface<CSimulationProvider,CSimulationSymbol>
{
public:

  typedef CProviderInterface<CSimulationProvider,CSimulationSymbol> ProviderInterface_t;
  typedef CInstrument::pInstrument_t pInstrument_t;
  typedef CInstrument::pInstrument_ref pInstrument_ref;
  typedef COrder::pOrder_t pOrder_t;
  //typedef COrder::pOrder_ref pOrder_ref;

  CSimulationProvider(void);
  virtual ~CSimulationProvider(void);
  virtual void Connect( void );
  virtual void Disconnect( void );
  void SetGroupDirectory( const std::string sGroupDirectory );  // eg /basket/20080620
  const std::string &GetGroupDirectory( void ) { return m_sGroupDirectory; };
  void Run( void );
  void Stop( void );
  void PlaceOrder( pOrder_t pOrder );
  void CancelOrder( pOrder_t pOrder );
  void AddTradeHandler( const std::string &sSymbol, CSimulationSymbol::tradehandler_t handler );
  void RemoveTradeHandler( const std::string &sSymbol, CSimulationSymbol::tradehandler_t handler );
protected:
  CSimulationSymbol *NewCSymbol( CSimulationSymbol::pInstrument_t pInstrument );
  void StartQuoteWatch( CSimulationSymbol *pSymbol );
  void StopQuoteWatch( CSimulationSymbol *pSymbol );
  void StartTradeWatch( CSimulationSymbol *pSymbol );
  void StopTradeWatch( CSimulationSymbol *pSymbol );
  void StartDepthWatch( CSimulationSymbol *pSymbol );
  void StopDepthWatch( CSimulationSymbol *pSymbol );

  std::string m_sGroupDirectory;

//  CWinThread *m_pMergeThread;
  CCrossThreadMerge *m_pMerge;

  //static UINT __cdecl Merge( LPVOID lpParam );
  void Merge( void );  // the background process

  void HandleExecution( COrder::orderid_t orderId, const CExecution &exec );

  ptime m_dtSimStart;
  ptime m_dtSimStop;

private:

  boost::thread m_threadMerge;

};
