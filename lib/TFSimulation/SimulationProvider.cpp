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

#include <cassert>
#include <stdexcept>

#include <TFHDF5TimeSeries/HDF5DataManager.h>

#include <TFTrading/KeyTypes.h>
#include <TFTrading/OrderManager.h>

#include "MergeDatedDatums.h"
#include "SimulationProvider.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

SimulationProvider::SimulationProvider()
: sim::SimulationInterface<SimulationProvider,SimulationSymbol>()
, m_sHdf5FileName( HDF5DataManager::GetHdf5FileDefault() )
{
  m_sName = "Simulator";
  m_nID = keytypes::EProviderSimulator;

  m_bProvidesQuotes = true;
  m_bProvidesDepths = true;
  m_bProvidesTrades = true;
  m_bProvidesGreeks = true;
  m_bProvidesBrokerInterface = true;
}

SimulationProvider::~SimulationProvider() {
  Reset();
}

void SimulationProvider::SetHdf5FileName( const std::string& sHdf5FileName ) {
  assert( 0 < sHdf5FileName.size() );
  m_sHdf5FileName = sHdf5FileName;
}

void SimulationProvider::SetGroupDirectory( const std::string& sGroupDirectory ) {
  HDF5DataManager dm( HDF5DataManager::RO, m_sHdf5FileName );
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

SimulationProvider::pSymbol_t SimulationProvider::NewCSymbol( pInstrument_t pInstrument ) {
  pSymbol_t pSymbol( new SimulationSymbol( pInstrument->GetInstrumentName( ID() ), pInstrument, m_sGroupDirectory, m_sHdf5FileName ) );
  inherited_t::AddCSymbol( pSymbol );
  return pSymbol;
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

void SimulationProvider::StartDepthByMMWatch( pSymbol_t pSymbol ) {
  pSymbol->StartDepthByMMWatch();
}

void SimulationProvider::StopDepthByMMWatch( pSymbol_t pSymbol ) {
  pSymbol->StopDepthByMMWatch();
}

void SimulationProvider::StartDepthByOrderWatch( pSymbol_t pSymbol ) {
  pSymbol->StartDepthByOrderWatch();
}

void SimulationProvider::StopDepthByOrderWatch( pSymbol_t pSymbol ) {
  pSymbol->StopDepthByOrderWatch();
}

void SimulationProvider::StartGreekWatch( pSymbol_t pSymbol ) {
  pSymbol->StartGreekWatch();
}

void SimulationProvider::StopGreekWatch( pSymbol_t pSymbol ) {
  pSymbol->StopGreekWatch();
}

// root of background simulation thread, thread is started from Run.
void SimulationProvider::Merge() {

  m_pMerge = std::make_unique<MergeDatedDatums>();

  if ( nullptr != m_OnSimulationThreadStarted ) m_OnSimulationThreadStarted();

  // for each of the symbols, add the quote, trade and greek series
  // datums from each series will be merged and emitted in chronological order
  for ( mapSymbols_t::iterator iter = m_mapSymbols.begin();

    iter != m_mapSymbols.end(); ++iter ) {

      pSymbol_t sym( iter->second );

      Quotes& quotes( sym->m_quotes );
      if ( 0 != quotes.Size() ) {
        m_pMerge -> Add(
          quotes,
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleQuoteEvent ) );
      }

      DepthsByMM& depths_mm( sym->m_depths_mm );
      if ( 0 != depths_mm.Size() ) {
        m_pMerge -> Add(
          depths_mm,
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleDepthByMMEvent ) );
      }

      DepthsByOrder& depths_order( sym->m_depths_order );
      if ( 0 != depths_order.Size() ) {
        m_pMerge -> Add(
          depths_order,
          MakeDelegate( iter->second.get(), &SimulationSymbol::HandleDepthByOrderEvent ) );
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
  m_dtSimStart = ou::TimeSource::GlobalInstance().External();

  bool bOldMode = ou::TimeSource::LocalCommonInstance().GetSimulationMode();
  ou::TimeSource::LocalCommonInstance().SetSimulationMode();

  m_pMerge->Run();

  m_nProcessedDatums = m_pMerge->GetCountProcessedDatums();

  m_dtSimStop = ou::TimeSource::LocalCommonInstance().External();

  if ( nullptr != m_OnSimulationComplete ) m_OnSimulationComplete();

  ou::TimeSource::LocalCommonInstance().SetSimulationMode( bOldMode );

  if ( nullptr != m_OnSimulationThreadEnded ) m_OnSimulationThreadEnded();

  m_pMerge.reset();
}

void SimulationProvider::Reset() {

  // todo: what happens when Merge is 'stopped'?
  if ( m_threadMerge.joinable() ) {
    m_threadMerge.join(); // wait for completion
  }

}

void SimulationProvider::Run( bool bAsync ) {

  if ( 0 == m_sGroupDirectory.size() ) throw std::invalid_argument( "Group Directory is empty" );
  if ( 0 == m_mapSymbols.size() ) throw std::invalid_argument( "No Symbols to simulate" );

  if ( nullptr != m_pMerge.get() ) {
    std::cout << "Simulation already in progress" << std::endl;
  }
  else {
    m_threadMerge = std::move( std::thread( std::bind( &SimulationProvider::Merge, this ) ) );

    if ( !bAsync ) {
      m_threadMerge.join();
    }

  }
}

void SimulationProvider::EmitStats( std::stringstream& ss ) {
  boost::posix_time::time_duration dur = m_dtSimStop - m_dtSimStart;
  unsigned long nDuration = dur.total_milliseconds();
  if ( 0 == nDuration ) {
    ss << m_nProcessedDatums << " datums processed";
  }
  else {
    double nDatumsPerSecond = (double)m_nProcessedDatums / (double)nDuration;
  //  ss << m_nProcessedDatums << " datums in " << nDuration << " seconds, " << nDatumsPerSecond << " datums/second." << std::endl;
    ss << m_nProcessedDatums << " datums in " << nDuration << " milliseconds, " << nDatumsPerSecond << " datums/millisecond.";
  }
}

// at some point:  run, stop, pause, resume, reset
void SimulationProvider::Stop() {
  if ( nullptr == m_pMerge.get() ) {
    std::cout << "no simulation to stop" << std::endl;
  }
  else {
    m_pMerge->Stop();
    std::cout << "stopping simulation" << std::endl;
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
