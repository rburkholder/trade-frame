/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <algorithm>

#include "Process.h"

#include <LibIndicators/Pivots.h>

CProcess::CProcess(void)
:
  m_tws( "U215226" ),
  m_bIBConnected( false ), m_bIQFeedConnected( false ),
//  m_reqId( 0 ),
  m_sSymbolName( "GLD" )
{
  m_tws.OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnIBConnected ) );
  m_tws.OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnIBDisconnected ) );

  m_iqfeed.OnConnected.Add( MakeDelegate( this, &CProcess::HandleOnIQFeedConnected ) );
  m_iqfeed.OnDisconnected.Add( MakeDelegate( this, &CProcess::HandleOnIQFeedDisconnected ) );
}

CProcess::~CProcess(void)
{
}

void CProcess::IBConnect( void ) {
  if ( !m_bIBConnected ) {
    
    m_tws.Connect();
    m_bIBConnected = true;
  }
}

void CProcess::IBDisconnect( void ) {
  if ( m_bIBConnected ) {
    m_tws.Disconnect();
    m_bIBConnected = false;
  }
}

void CProcess::IQFeedConnect( void ) {
  if ( !m_bIQFeedConnected ) {
    
    m_iqfeed.Connect();
    m_bIQFeedConnected = true;
  }
}

void CProcess::IQFeedDisconnect( void ) {
  if ( m_bIQFeedConnected ) {
    m_iqfeed.Disconnect();
    m_bIQFeedConnected = false;
  }
}

void CProcess::HandleOnIBConnected(int e) {
  // obtain strike list of underlying instrument
  Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "OPT";
  contract.symbol = m_sSymbolName;
  contract.expiry = "201006";
//  contract.strike = 120.0;
  contract.right = "CALL";
  m_tws.SetOnContractDetailsHandler( MakeDelegate( this, &CProcess::HandleStrikeListing ) );
  m_tws.SetOnContractDetailsDoneHandler( MakeDelegate( this, &CProcess::HandleStrikeListingDone ) );
  m_tws.RequestContractDetails( contract );
//  contract.right = "PUT";
//  m_tws.RequestContractDetails( ++m_reqId, contract );
}

void CProcess::HandleStrikeListing( const ContractDetails& details ) {
  m_vCrossOverPoints.push_back( details.summary.strike );
}

void CProcess::HandleStrikeListingDone(  ) {
  m_ss.str( "" );
  m_ss << "#strikes: " << m_vCrossOverPoints.size() << std::endl;
  OutputDebugString( m_ss.str().c_str() );
}

void CProcess::HandleOnIBDisconnected(int e) {
}

void CProcess::HandleOnIQFeedConnected(int e) {
  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void CProcess::HandleOnIQFeedDisconnected(int e) {
}

void CProcess::OnHistoryConnected( void ) {
  CIQFeedHistoryQuery<CProcess>::RetrieveNEndOfDays( m_sSymbolName, 1 );
}

void CProcess::OnHistorySummaryData( structSummary* pDP ) {
  m_Bar.Open( pDP->Open );
  m_Bar.Close( pDP->Close );
  m_Bar.High( pDP->High );
  m_Bar.Low( pDP->Low );
  m_Bar.Volume( pDP->PeriodVolume );
  m_Bar.DateTime( pDP->DateTime );
  ReQueueSummary( pDP );
}

void CProcess::OnHistoryRequestDone( void ) {
  CIQFeedHistoryQuery<CProcess>::Disconnect();  
  CPivotSet pivots;
  pivots.CalcPivots( m_sSymbolName, m_Bar.High(), m_Bar.Low(), m_Bar.Close() );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R3 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R2 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::R1 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::PV ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S1 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S2 ) );
  m_vCrossOverPoints.push_back( pivots.GetPivotValue( CPivotSet::S3 ) );
}

void CProcess::StartTrading( void ) {
  std::sort( m_vCrossOverPoints.begin(), m_vCrossOverPoints.end() );
}

void CProcess::StopTrading( void ) {
}