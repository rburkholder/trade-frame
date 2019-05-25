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
 * File:    SpreadCandidate.cpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * 
 * Created on May 25, 2019, 1:25 PM
 */

#include "SpreadCandidate.h"

SpreadCandidate::SpreadCandidate()
: m_nUnDesired {}, m_nDesired {}, m_nConsecutiveSpreadOk {}
{}

SpreadCandidate::SpreadCandidate( const SpreadCandidate&& rhs )
: m_quote( rhs.m_quote ), m_nUnDesired( rhs.m_nUnDesired ), m_nDesired( rhs.m_nDesired ),
  m_nConsecutiveSpreadOk( rhs.m_nConsecutiveSpreadOk ),
  m_pWatch( std::move( rhs.m_pWatch ) )
{}

SpreadCandidate::SpreadCandidate( pWatch_t pWatch )
: m_nUnDesired {}, m_nDesired {}, m_nConsecutiveSpreadOk {}
{
  SetWatch( pWatch );
}

SpreadCandidate::~SpreadCandidate() {
  Clear();
}

void SpreadCandidate::Clear() {
  if ( m_pWatch ) {
    m_pWatch->StopWatch();
    m_pWatch->OnQuote.Remove( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
    m_pWatch.reset();
  }
}

void SpreadCandidate::SetWatch( pWatch_t pWatch ) {
  Clear();
  m_pWatch = pWatch;
  if ( m_pWatch ) {
    m_nDesired = m_nUnDesired = m_nConsecutiveSpreadOk = 0;
    m_pWatch->OnQuote.Add( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
    m_pWatch->StartWatch();
  }
}

SpreadCandidate::pWatch_t SpreadCandidate::GetWatch() { return m_pWatch; }

bool SpreadCandidate::ValidateSpread( size_t nDuration ) {
  bool bOk( false );
  if ( m_pWatch ) {
    if ( m_nUnDesired < m_nDesired ) {
      // TODO: may want to enforce some sort of ratio
      //   will need some statistics on what a good number might be
      m_nConsecutiveSpreadOk++;
    }
    else {
      m_nConsecutiveSpreadOk = 0;
    }
    bOk = ( nDuration <= m_nConsecutiveSpreadOk );
  }
  return bOk;
}

void SpreadCandidate::UpdateQuote( const ou::tf::Quote& quote ) {
  m_quote = quote;
  double spread( m_quote.Spread() );
  if ( ( 0.005 <= spread ) && ( spread < 0.10 ) ) {
    m_nDesired++;
  }
  else {
    m_nUnDesired++;
  }
}

