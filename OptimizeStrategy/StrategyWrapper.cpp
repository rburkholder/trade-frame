/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"

// this is anticipated to be started in a new thread in order to handle multiple simultaneous simulations

#include "StrategyWrapper.h"

StrategyWrapper::StrategyWrapper(void)
  : m_bRunning( false ), m_pStrategy( 0 )
{
}

StrategyWrapper::~StrategyWrapper(void) {
  m_pSimulator->Disconnect();
  delete m_pStrategy;
  m_pStrategy = 0;
  m_pSimulator.reset();
}

void StrategyWrapper::Init( 
  StrategyEquity::registrations_t& registrations, 
  pInstrument_t pInstrument, 
  const boost::gregorian::date& dateStart, 
  const std::string& sSourcePath, 
  fdEvaluate_t pfnLong, fdEvaluate_t pfnShort ) 
{
  m_dtStart = dateStart;
  m_pInstrument = pInstrument;
  m_pSimulator.reset( new ou::tf::SimulationProvider );
  m_pSimulator->SetGroupDirectory( sSourcePath );
  m_pSimulator->SetOnSimulationThreadStarted( MakeDelegate( this, &StrategyWrapper::HandleSimulationThreadStart ) );
  m_pSimulator->SetOnSimulationThreadEnded( MakeDelegate( this, &StrategyWrapper::HandleSimulationThreadEnd ) );
  m_pStrategy = new StrategyEquity( m_pSimulator, m_pInstrument, m_dtStart );
  m_pStrategy->Init( registrations, pfnLong, pfnShort );
}

void StrategyWrapper::Start( void ) {
  m_pSimulator->OnConnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pSimulator->OnDisconnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_pSimulator->Connect();
}

//void StrategyWrapper::Stop( void ) {
//  m_pSimulator->Disconnect();
//}

void StrategyWrapper::HandleProviderConnected( int i ) {

  m_bRunning = true;

//  m_dtEnd = boost::posix_time::ptime( date( 2011, 11, 7 ), time_duration( 17, 45, 0 ) );  // put in time start
  
  m_pSimulator->SetOnSimulationComplete( MakeDelegate( this, &StrategyWrapper::HandleSimulationComplete ) );

  m_pSimulator->Run( false );  // returns upon completion of simulation

//  std::stringstream ss;
//  m_pSimulator->EmitStats( ss );
//  std::cout << ss.str() << std::endl;

  m_pSimulator->SetOnSimulationComplete( 0 );

}

void StrategyWrapper::HandleProviderDisconnected( int i ) {
  m_pSimulator->OnConnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pSimulator->OnDisconnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_bRunning = false;
  m_pStrategy->End();
}

void StrategyWrapper::HandleSimulationThreadStart( void ) {
  ou::TimeSource::SetLocalCommonInstance( new ou::TimeSource );
  ou::tf::OrderManager::SetLocalCommonInstance( new ou::tf::OrderManager );
}

void StrategyWrapper::HandleSimulationThreadEnd( void ) {
  ou::TimeSource::SetLocalCommonInstance( 0 );
  ou::tf::OrderManager::SetLocalCommonInstance( 0 );
}

void StrategyWrapper::HandleSimulationComplete( void ) {
  // generate statistics here?
  // any clean up required?
//  m_pSimulator->Disconnect();
}
 
double StrategyWrapper::GetPL( std::stringstream& ss ) {
  return m_pStrategy->GetPL( ss );
}