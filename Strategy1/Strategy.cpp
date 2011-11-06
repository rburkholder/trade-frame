/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

//#include <TFHDF5TimeSeries/HDF5DataManager.h>
//#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include <TFTrading/InstrumentManager.h>

#include "Strategy.h"

Strategy::Strategy(void) 
  : m_sim( new ou::tf::CSimulationProvider() ),
  m_sma1min( &m_quotes, 60 ), m_sma2min( &m_quotes, 120 ), m_sma3min( &m_quotes, 180 ),
  m_sma5min( &m_quotes, 300 ), m_sma15min( &m_quotes, 1800 )
{
  ou::tf::CProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

  m_sim->OnConnected.Add( MakeDelegate( this, &Strategy::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleSimulatorDisConnected ) );

  m_pExecutionProvider = m_sim;
  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &Strategy::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleOnExecDisconnected ) );

  m_pDataProvider = m_sim;
  m_pDataProvider->OnConnected.Add( MakeDelegate( this, &Strategy::HandleOnData1Connected ) );
  m_pDataProvider->OnDisconnected.Add( MakeDelegate( this, &Strategy::HandleOnData1Disconnected ) );

  ou::tf::CInstrumentManager& mgr( ou::tf::CInstrumentManager::Instance() );
  m_pTestInstrument = mgr.Exists( "+GCZ11" ) ? mgr.Get( "+GCZ11" ) : mgr.ConstructFuture( "+GCZ11", "SMART", 2011, 12 );

  m_sim->SetGroupDirectory( "/semiauto/2011-Sep-23 19:17:48.252497/" );
  m_sim->SetExecuteAgainst( ou::tf::CSimulateOrderExecution::EAQuotes );
  
  m_sim->AddQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->AddTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );

}

Strategy::~Strategy(void) {

  m_sim->RemoveQuoteHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleQuote ) );
  m_sim->RemoveTradeHandler( m_pTestInstrument, MakeDelegate( this, &Strategy::HandleTrade ) );

  m_pDataProvider->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleOnData1Connected ) );
  m_pDataProvider->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleOnData1Disconnected ) );

  m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleOnExecDisconnected ) );

  m_sim->OnConnected.Remove( MakeDelegate( this, &Strategy::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Remove( MakeDelegate( this, &Strategy::HandleSimulatorDisConnected ) );

  ou::tf::CProviderManager::Instance().Release( "sim01" );

}

void Strategy::HandleSimulatorConnected( int ) {
  m_sim->Run();
}

void Strategy::HandleSimulatorDisConnected( int ) {
}

void Strategy::HandleOnExecConnected( int ) {
}

void Strategy::HandleOnExecDisconnected( int ) {
}

void Strategy::HandleOnData1Connected( int ) {
}

void Strategy::HandleOnData1Disconnected( int ) {
}

void Strategy::Start( const std::string& sSymbolPath ) {
  m_sim->Connect();
}

void Strategy::HandleQuote( const ou::tf::CQuote& quote ) {
}

void Strategy::HandleTrade( const ou::tf::CTrade& trade ) {
}