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
 * File:    Ind_HighPass.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: July 14, 2024 13:19:59
 */

#include "Ind_HighPass.hpp"

namespace ou {
namespace tf {
namespace indicator {

HighPass::HighPass( unsigned int period, ou::ChartDataView& cdv, unsigned int ixSlot_ )
: m_cdv( cdv ), ixSlot( ixSlot_ )
, n0( 3 )
, pi( M_PI )
, root_2( std::sqrt( 2.0 ) )
, a1( std::exp( ( -root_2 * pi ) / period ) )
//, b1( 2.0 * a1 * std::cos( root_2 * 180.0 / period ) ) // degree
, b1( 2.0 * a1 * std::cos( root_2 * pi / period ) ) // radians
, c2( b1 )
, c3( -a1 * a1 )
, c1( ( 1.0 + c2 - c3 ) / 4.0 )
{
  m_cdv.Add( ixSlot, &m_ce );
}

HighPass::~HighPass() {
  m_cdv.Remove( ixSlot, &m_ce );
}

void HighPass::Set( ou::Colour::EColour colour, const std::string& sName ) {
  m_ce.SetName( sName );
  m_ce.SetColour( colour );
}

double HighPass::Update( boost::posix_time::ptime dt, double value ) {

  if ( 0 == n0 ) {
    //us0 = ( c1 * value ) + ( c2 * dblLatest ); // ema
    val2 = val1; val1 = val0; val0 = value;
    hp2  = hp1;  hp1  = hp0;
    hp0 =
      c1 * ( val0 - 2.0 * val1 + val2 )
    + c2 * hp1
    + c3 * hp2
    ;
  }
  else {
    assert( n0 > 0 );
    n0--;
    switch ( n0 ) {
      case 2:
        val2 = value;
        hp2 = 0.0;
        break;
      case 1:
        val1 = value;
        hp1 = 0.0;
        break;
      case 0:
        val0 = value;
        hp0 = 0.0;
        break;
    }
  }

  m_ce.Append( dt, hp0 );
  return hp0;
}


} // namespace indicator
} // namespace tf
} // namespace ou

