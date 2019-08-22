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

#include "Capture.h"

Capture::Capture()
: m_nSequence {}
, m_bQuoteReady( false )
, m_bTradeReady( false )
{
}

Capture::~Capture() {
  if ( m_pWatch ) m_pWatch->StopWatch();
  m_bf.SetOnBarComplete( nullptr );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Capture::HandleTrade ) );
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Capture::HandleQuote ) );
  m_pWatch.reset(); // TODO: need to wait for queue to flush
}

void Capture::Assign(
  ou::tf::BarFactory::duration_t duration,
  pWatch_t pWatch,
  fBarComplete_t&& fBarComplete
) {
  assert( nullptr != fBarComplete );
  m_fBarComplete = std::move( fBarComplete );
  m_bf.SetBarWidth( duration );
  m_pWatch = pWatch;
  m_bf.SetOnBarComplete( MakeDelegate( this, &Capture::HandleBarComplete ) );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Capture::HandleQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Capture::HandleTrade ) );
  m_pWatch->StartWatch();
}

void Capture::Pull( bool& bQuoteReady, ou::tf::Quote& quote, bool& bTradeReady, ou::tf::Trade& trade ) {
  {
    m_spinlock.lock();
    bQuoteReady = m_bQuoteReady;
    if ( m_bQuoteReady ) {
      quote = m_quote;
      m_bQuoteReady = false;
    }
    m_spinlock.unlock();
  }
  {
    m_spinlock.lock();
    bTradeReady = m_bTradeReady;
    if ( m_bTradeReady ) {
      trade = m_trade;
      m_bTradeReady = false;
    }
    m_spinlock.unlock();
  }
}

void Capture::HandleQuote( const ou::tf::Quote& quote ) {
  m_spinlock.lock();
  m_quote = quote;
  m_bQuoteReady = true;
  m_spinlock.unlock();
}

void Capture::HandleTrade( const ou::tf::Trade& trade ) {
  m_spinlock.lock();
  m_trade = trade;
  m_bTradeReady = true;
//  if ( nullptr != m_fBarComplete ) {
//    m_bf.Add( trade );
//  }
  m_spinlock.unlock();
}

void Capture::HandleBarComplete( const ou::tf::Bar& bar ) {
  m_nSequence++;
  if ( nullptr != m_fBarComplete ) {
    m_fBarComplete(
      m_pWatch->GetInstrument()->GetInstrumentName(),
      m_nSequence,
      bar,
      m_pWatch->LastQuote(),
      m_pWatch->LastTrade()
      );
  }
}
