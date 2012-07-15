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
#include "StrategyWrapper.h"

StrategyWrapper::StrategyWrapper(void)
  : m_bRunning( false ), m_pStrategy( 0 )
{
}

StrategyWrapper::~StrategyWrapper(void) {
}

void StrategyWrapper::Start( pInstrument_t pInstrument, const std::string& sSourcePath ) {
  m_pProvider.reset( new ou::tf::CSimulationProvider );
  m_pProvider->OnConnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Add( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_pProvider->Connect();
}

void StrategyWrapper::Stop( void ) {
  m_pProvider->Disconnect();
}

void StrategyWrapper::HandleProviderConnected( int i ) {
  m_bRunning = true;
  m_pStrategy = new StrategyEquity( m_pProvider );
  m_pStrategy->Start();

//  m_dtEnd = boost::posix_time::ptime( date( 2011, 11, 7 ), time_duration( 17, 45, 0 ) );  // put in time start
  
  m_pProvider->SetOnSimulationComplete( MakeDelegate( this, &StrategyWrapper::HandleSimulationComplete ) );

//  Activate();

  m_pProvider->Run();

}

void StrategyWrapper::HandleProviderDisconnected( int i ) {
  m_bRunning = false;
  m_pStrategy->Stop();
  delete m_pStrategy;
  m_pStrategy = 0;
  m_pProvider->OnConnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderConnected ) );
  m_pProvider->OnDisconnected.Remove( MakeDelegate( this, &StrategyWrapper::HandleProviderDisconnected ) );
  m_pProvider.reset();
}

void StrategyWrapper::HandleSimulationComplete( void ) {
  // generate statistics here?
  // any clean up required?
}
