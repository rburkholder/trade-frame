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

#include "stdafx.h"

#include <stdexcept>
#include <cassert>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFTrading/KeyTypes.h>

#include "SimulationProvider.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

SimulationProvider::SimulationProvider(void)
: ProviderInterface<SimulationProvider,SimulationSymbol>(), 
  m_pMerge( 0 )
{
  m_sName = "Simulator";
  m_nID = keytypes::EProviderSimulator;

  m_bProvidesQuotes = true;
  m_bProvidesTrades = true;
  m_bProvidesGreeks = true;
  m_pProvidesBrokerInterface = true;
}

SimulationProvider::~SimulationProvider(void) {

  if ( 0 != m_pMerge ) {
    delete m_pMerge;
    m_pMerge = NULL;
  }
}

void SimulationProvider::SetGroupDirectory( const std::string sGroupDirectory ) {
  HDF5DataManager dm( HDF5DataManager::RO );
  std::string s;
  if( !dm.GroupExists( sGroupDirectory ) ) 
    throw std::invalid_argument( "Could not find: " + sGroupDirectory );
  s = sGroupDirectory + "/trades";
  if( !dm.GroupExists( s ) ) 
    throw std::invalid_argument( "Could not find: " + s );
  s = sGroupDirectory + "/quotes";
  if( !dm.GroupExists( s ) ) 
    throw std::invalid_argument( "Could not find: " + s );
  m_sGroupDirectory = sGroupDirectory;
}

void SimulationProvider::Connect() {
  if ( !m_bConnected ) {
    OnConnecting( 0 );
    m_bConnected = true;
    ProviderInterface::Connect();
    OnConnected( 0 );
  }
}

void SimulationProvider::Disconnect() {
  if ( m_bConnected ) {
    OnDisconnecting( 0 );
    m_bConnected = false;
    ProviderInterface::Disconnect();
    OnDisconnected( 0 );
  }
}

SimulationProvider::pSymbol_t SimulationProvider::NewCSymbol( SimulationSymbol::pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new SimulationSymbol(pInstrument->GetInstrumentName(), pInstrument, m_sGroupDirectory) );
  pSymbol->m_simExec.SetOnOrderFill( MakeDelegate( this, &SimulationProvider::HandleExecution ) );
  pSymbol->m_simExec.SetOnCommission( MakeDelegate( this, &SimulationProvider::HandleCommission ) );
  pSymbol->m_simExec.SetOnOrderCancelled( MakeDelegate( this, &SimulationProvider::HandleCancellation ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
}

void SimulationProvider::AddQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler ) {
  inherited_t::AddQuoteHandler( pInstrument, handler );
  inherited_t::m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  assert( m_mapSymbols.end() != iter );
  pSymbol_t pSymSymbol( iter->second );
  if ( 1 == iter->second->GetQuoteHandlerCount() ) {
    inherited_t::AddQuoteHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &SimulateOrderExecution::NewQuote ) );
  }
}

void SimulationProvider::RemoveQuoteHandler( pInstrument_cref pInstrument, SimulationSymbol::quotehandler_t handler ) {
  inherited_t::RemoveQuoteHandler( pInstrument, handler );
  inherited_t::m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( false );  // this shouldn't occur
  }
  else {
    if ( 1 == iter->second->GetQuoteHandlerCount() ) {
      pSymbol_t pSymSymbol( iter->second );
      inherited_t::RemoveQuoteHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &SimulateOrderExecution::NewQuote ) );
    }
  }
}

void SimulationProvider::AddTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler ) {
  inherited_t::AddTradeHandler( pInstrument, handler );
  inherited_t::m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  assert( m_mapSymbols.end() != iter );
  pSymbol_t pSymSymbol( iter->second );
  if ( 1 == iter->second->GetTradeHandlerCount() ) {
    inherited_t::AddTradeHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &SimulateOrderExecution::NewTrade ) );
  }
}

void SimulationProvider::RemoveTradeHandler( pInstrument_cref pInstrument, SimulationSymbol::tradehandler_t handler ) {
  inherited_t::RemoveTradeHandler( pInstrument, handler );
  inherited_t::m_mapSymbols_t::iterator iter;
  iter = m_mapSymbols.find( pInstrument->GetInstrumentName( m_nID ) );
  if ( m_mapSymbols.end() == iter ) {
    assert( false );  // this shouldn't occur
  }
  else {
    if ( 1 == iter->second->GetTradeHandlerCount() ) {
      pSymbol_t pSymSymbol( iter->second );
      inherited_t::RemoveTradeHandler( pInstrument, MakeDelegate( &pSymSymbol->m_simExec, &SimulateOrderExecution::NewTrade ) );
    }
  }
}

// these need to open the data file, load the data, and prepare to simulate
void SimulationProvider::StartQuoteWatch( pSymbol_t pSymbol ) {
  pSymbol->StartQuoteWatch();
}

void SimulationProvider::StopQuoteWatch( pSymbol_t pSymbol ) {
  pSymbol->StopQuoteWatch();
}

void SimulationProvider::StartTradeWatch( pSymbol_t pSymbol ) {
  pSymbol->StartTradeWatch();
}

void SimulationProvider::StopTradeWatch( pSymbol_t pSymbol ) {
  pSymbol->StopTradeWatch();
}

void SimulationProvider::StartDepthWatch( pSymbol_t pSymbol ) {
  pSymbol->StartDepthWatch();
}

void SimulationProvider::StopDepthWatch( pSymbol_t pSymbol ) {
  pSymbol->StopDepthWatch();
}

void SimulationProvider::StartGreekWatch( pSymbol_t pSymbol ) {
  pSymbol->StartGreekWatch();
}

void SimulationProvider::StopGreekWatch( pSymbol_t pSymbol ) {
  pSymbol->StopGreekWatch();
}

// root of background simulation thread, thread is started from Run.
void SimulationProvider::Merge( void ) {

  if ( 0 != m_OnSimulationThreadStarted ) m_OnSimulationThreadStarted();

  // for each of the symbols, add the quote, trade and greek series
  // datums from each series will be merged and emitted in chronological order
  for ( m_mapSymbols_t::iterator iter = m_mapSymbols.begin();

    iter != m_mapSymbols.end(); ++iter ) {

      pSymbol_t sym( iter->second );

      Quotes& quotes( sym->m_quotes );
      if ( 0 != quotes.Size() ) {
        m_pMerge -> Add( 
          quotes, 
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleQuoteEvent ) );
      }

      Trades& trades( sym->m_trades );
      if ( 0 != trades.Size() ) {
        m_pMerge -> Add( 
          trades, 
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleTradeEvent ) );
      }

      Greeks& greeks( sym->m_greeks );
      if ( 0 != greeks.Size() ) {
        m_pMerge -> Add(
          greeks,
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleGreekEvent ) );
      }

  }

  m_nProcessedDatums = 0;
  m_dtSimStart = ou::TimeSource::Instance().External();

  bool bOldMode = ou::TimeSource::LocalCommonInstance().GetSimulationMode();
  ou::TimeSource::LocalCommonInstance().SetSimulationMode();

  m_pMerge -> Run();

  m_nProcessedDatums = m_pMerge->GetCountProcessedDatums();
  m_dtSimStop = ou::TimeSource::LocalCommonInstance().External();

  if ( 0 != m_OnSimulationComplete ) m_OnSimulationComplete();

  ou::TimeSource::LocalCommonInstance().SetSimulationMode( bOldMode );

  if ( 0 != m_OnSimulationThreadEnded ) m_OnSimulationThreadEnded();
}

void SimulationProvider::Run( bool bAsync ) {
  if ( 0 == m_sGroupDirectory.size() ) throw std::invalid_argument( "Group Directory is empty" );
  if ( 0 == m_mapSymbols.size() ) throw std::invalid_argument( "No Symbols to simulate" );

  if ( 0 != m_pMerge ) {
    std::cout << "Simulation already in progress" << std::endl;
  }
  else {
    m_pMerge = new MergeDatedDatums();
    boost::thread sim( boost::bind( &SimulationProvider::Merge, this ) );

    if ( !bAsync ) {
      sim.join();
    }

  }
}

void SimulationProvider::EmitStats( std::stringstream& ss ) {
  boost::posix_time::time_duration dur = m_dtSimStop - m_dtSimStart;
  unsigned long nDuration = dur.total_seconds();
  unsigned long nDatumsPerSecond = m_nProcessedDatums / nDuration;
//  ss << m_nProcessedDatums << " datums in " << nDuration << " seconds, " << nDatumsPerSecond << " datums/second." << std::endl;
    ss << m_nProcessedDatums << " datums in " << nDuration << " seconds, " << nDatumsPerSecond << " datums/second.";
}

// at some point:  run, stop, pause, resume, reset
void SimulationProvider::Stop() {
  if ( NULL == m_pMerge ) {
    std::cout << "no simulation to stop" << std::endl;
  }
  else {
    m_pMerge->Stop();
    std::cout << "stopping simulation" << std::endl;
  }
}

void SimulationProvider::PlaceOrder( pOrder_t pOrder ) {
  inherited_t::PlaceOrder( pOrder ); // any underlying initialization
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pOrder->GetInstrument()->GetInstrumentName() );
  if ( m_mapSymbols.end() == iter ) {
    std::cout << "Can't place order, can't find symbol: " << pOrder->GetInstrument()->GetInstrumentName( m_nID ) << std::endl;
  }
  else {
    iter->second->m_simExec.SubmitOrder( pOrder );
  }
}

void SimulationProvider::CancelOrder( pOrder_t pOrder ) {
  inherited_t::CancelOrder( pOrder );
  m_mapSymbols_t::iterator iter = m_mapSymbols.find( pOrder->GetInstrument()->GetInstrumentName() );
  if ( m_mapSymbols.end() == iter ) {
    std::cout << "Can't cancel order, can't find symbol: " << pOrder->GetInstrument()->GetInstrumentName( m_nID ) << std::endl;
  }
  else {
    iter->second->m_simExec.CancelOrder( pOrder->GetOrderId() );
  }
}

void SimulationProvider::HandleExecution( Order::idOrder_t orderId, const Execution &exec ) {
  OrderManager::LocalCommonInstance().ReportExecution( orderId, exec );
}

void SimulationProvider::HandleCommission( Order::idOrder_t orderId, double commission ) {
  OrderManager::LocalCommonInstance().ReportCommission( orderId, commission );
}

void SimulationProvider::HandleCancellation( Order::idOrder_t orderId ) {
  OrderManager::LocalCommonInstance().ReportCancellation( orderId );
}


} // namespace tf
} // namespace ou
