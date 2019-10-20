/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/* 
 * File:    Instance.cpp
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on October 6, 2019, 9:12 PM
 */

#include "Instance.h"

Instance::Instance( pPosition_t pPosition )
: m_bIntervalHasTrades( false )
{
  assert( pPosition );
  m_pPosition = pPosition;
  
  m_pWatch = pPosition->GetWatch();
  m_pWatch->RecordSeries( false );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Instance::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Instance::HandleTrade ) );
  m_pWatch->StartWatch();
}

Instance::~Instance() {
  if ( m_pWatch ) m_pWatch->StopWatch();
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Instance::HandleTrade ) );
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Instance::HandleQuote ) );
  m_pWatch.reset(); // TODO: need to wait for queue to flush
}

void Instance::HandleQuote( const ou::tf::Quote& quote ) {
  m_spread = quote.Ask() - quote.Bid();
}

void Instance::HandleTrade( const ou::tf::Trade& trade ) {
  if ( !m_bIntervalHasTrades ) {
    m_volume = 0;
    m_dblOpen = m_dblClose = trade.Price();
    m_bIntervalHasTrades = true;
  }
  else {
    m_dblClose = trade.Price();
  }
  m_volume += trade.Volume();
}

void Instance::Evaluate( fEvaluate_t&& fEvaluate ) {
  if ( m_bIntervalHasTrades ) {
    if ( 0.10 > m_spread ) {
      fEvaluate( m_volume * ( ( m_dblOpen + m_dblClose ) / 2.0 ), m_pPosition );
    }
  }
  m_bIntervalHasTrades = false;
}