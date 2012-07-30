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
  m_pProvider->Disconnect();
  delete m_pStrategy;
  m_pStrategy = 0;
  m_pProvider.reset();
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
  m_pProvider.reset( new ou::tf::CSimulationProvider );
  m_pProvider->SetGroupDirectory( sSourcePath );
  m_pStrategy = new StrategyEquity( m_pProvider, m_pInstrument, m_dtStart );
  m_pStrategy->Init( registrations, pfnLong, pfnShort );
}

void StrategyWrapper::Start( void ) {
  m_pProvider->OnConnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_pProvider->Connect();
}

//void StrategyWrapper::Stop( void ) {
//  m_pProvider->Disconnect();
//}

void StrategyWrapper::HandleProviderConnected( int i ) {

  m_bRunning = true;

//  m_dtEnd = boost::posix_time::ptime( date( 2011, 11, 7 ), time_duration( 17, 45, 0 ) );  // put in time start
  
  m_pProvider->SetOnSimulationComplete( MakeDelegate( this, &StrategyWrapper::HandleSimulationComplete ) );

  m_pProvider->Run( false );  // returns upon completion of simulation

  std::stringstream ss;
  m_pProvider->EmitStats( ss );
  std::cout << ss.str() << std::endl;

  m_pProvider->SetOnSimulationComplete( 0 );

}

void StrategyWrapper::HandleProviderDisconnected( int i ) {
  m_pProvider->OnConnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_bRunning = false;
  m_pStrategy->End();
}

void StrategyWrapper::HandleSimulationComplete( void ) {
  // generate statistics here?
  // any clean up required?
//  m_pProvider->Disconnect();
}
 
double StrategyWrapper::GetPL( void ) {
  return m_pStrategy->GetPL();
}