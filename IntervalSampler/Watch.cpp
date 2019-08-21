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
 * File:    Watch.cpp
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on August 21, 2019, 11:10 AM
 */

#include "Watch.h"

Watch::Watch()
: m_nSequence {}
, m_bQuoteReady( false )
, m_bTradeReady( false )
{
}

Watch::~Watch() {
  if ( m_pWatch ) m_pWatch->StopWatch();
  m_bf.SetOnBarComplete( nullptr );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Watch::HandleTrade ) );
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Watch::HandleQuote ) );
  m_pWatch.reset(); // TODO: need to wait for queue to flush
}

void Watch::Assign(
  ou::tf::BarFactory::duration_t duration,
  pWatch_t pWatch,
  fBarComplete_t&& fBarComplete
) {
  assert( nullptr != fBarComplete );
  m_fBarComplete = std::move( fBarComplete );
  m_bf.SetBarWidth( duration );
  m_pWatch = pWatch;
  m_bf.SetOnBarComplete( MakeDelegate( this, &Watch::HandleBarComplete ) );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Watch::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Watch::HandleTrade ) );
  m_pWatch->StartWatch();
}

void Watch::HandleQuote( const ou::tf::Quote& quote ) {
  //m_spinlock.lock(); // released on exit
  m_quote = quote;
  m_bQuoteReady = true;
}

void Watch::HandleTrade( const ou::tf::Trade& trade ) {
  //m_spinlock.lock(); // released on exit
  m_trade = trade;
  m_bTradeReady = true;
  //m_bf.Add( trade );
}

// disabled for now
void Watch::HandleBarComplete( const ou::tf::Bar& bar ) {
  m_nSequence++;
  m_fBarComplete(
    m_pWatch->GetInstrument()->GetInstrumentName(),
    m_nSequence,
    bar,
    m_pWatch->LastQuote(),
    m_pWatch->LastTrade()
    );
}
