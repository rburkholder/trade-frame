/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    Collector.cpp
 * Author:  raymond@burkholder.net
 * Project: ImpliedVolatility
 * Created: January 31, 2023  20:37:13
 */

#include "Collector.hpp"

Collector::Collector( pWatch_t pWatch )
: m_pWatch( std::move( pWatch ) )
{
  m_pWatch->OnQuote.Add( MakeDelegate( this, &Collector::HandleUnderlyingQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &Collector::HandleUnderlyingTrade ) );
}

Collector::~Collector() {
  m_pWatch->OnQuote.Remove( MakeDelegate( this, &Collector::HandleUnderlyingQuote ) );
  m_pWatch->OnTrade.Remove( MakeDelegate( this, &Collector::HandleUnderlyingTrade ) );
}

void Collector::HandleUnderlyingQuote( const ou::tf::Quote& ) {
}

void Collector::HandleUnderlyingTrade( const ou::tf::Trade& ) {
}
