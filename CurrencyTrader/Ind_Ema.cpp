/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    Ind_Ema.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 11:35:57
 */

#include "Ind_Ema.hpp"

namespace ou {
namespace tf {
namespace indicator {

Ema::Ema( unsigned int n, ou::ChartDataView& cdv, unsigned int ixSlot_ )
: bBootStrapped( false ), dblLatest {}, m_cdv( cdv ), ixSlot( ixSlot_ )
, c1( 2.0 / ( n + 1 ) )  // ema: smaller - used on arriving value
, c2( 1.0 - c1 )         // ema: 1 - c1 (larger), used on prior ema
{
  m_cdv.Add( ixSlot, &m_ce );
}

Ema::~Ema() {
  m_cdv.Remove( ixSlot, &m_ce );
}

void Ema::Set( ou::Colour::EColour colour, const std::string& sName ) {
  m_ce.SetName( sName );
  m_ce.SetColour( colour );
}

double Ema::Update( boost::posix_time::ptime dt, double value ) {

  if ( bBootStrapped ) {
    dblLatest = ( c1 * value ) + ( c2 * dblLatest );
  }
  else {
    bBootStrapped = true;
    dblLatest = value;
  }

  m_ce.Append( dt, dblLatest );
  return dblLatest;
}

} // namespace indicator
} // namespace tf
} // namespace ou

