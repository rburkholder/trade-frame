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
 * File:    Ind_SuprSmth.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 13:59:27
 */

#include "Ind_SuprSmth.hpp"

namespace ou {
namespace tf {
namespace indicator {

SuperSmoother::SuperSmoother( unsigned int period, ou::ChartDataView& cdv, unsigned int ixSlot_ )
: m_cdv( cdv ), ixSlot( ixSlot_ )
, n0( 3 )
, pi( M_PI )
, root_2( std::sqrt( 2.0 ) )
, a1( std::exp( ( -root_2 * pi ) / period ) )
//, b1( 2.0 * a1 * std::cos( root_2 * 180.0 / period ) ) // degree
, b1( 2.0 * a1 * std::cos( root_2 * pi / period ) ) // radians
, c2( b1 )
, c3( -a1 * a1 )
, c1( 1.0 - c2 - c3 )
{
  m_cdv.Add( ixSlot, &m_ce );
}

SuperSmoother::~SuperSmoother() {
  m_cdv.Remove( ixSlot, &m_ce );
}

void SuperSmoother::Set( ou::Colour::EColour colour, const std::string& sName ) {
  m_ce.SetName( sName );
  m_ce.SetColour( colour );
}

double SuperSmoother::Update( boost::posix_time::ptime dt, double value ) {

  if ( 0 == n0 ) {
    val1 = val0; val0 = value;
    ss2  = ss1;  ss1  = ss0;
    ss0 =
      c1 * ( val0 + val1 ) * 0.5
    + c2 * ss1
    + c3 * ss2
    ;
  }
  else {
    assert( n0 > 0 );
    n0--;
    switch ( n0 ) {
      case 2:
               ss2 = ss0 = value;
        break;
      case 1:
        val1 = ss1 = ss0 = value;
        break;
      case 0:
        val0 = ss0 = value;
        break;
    }
  }

  m_ce.Append( dt, ss0 );
  return ss0;
}


} // namespace indicator
} // namespace tf
} // namespace ou

