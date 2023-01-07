/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    MovingAverage.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFBitsNPieces
 * Created: 2022/11/29 11:57:41
 */

#include "MovingAverage.hpp"

namespace ou {
namespace tf {

MovingAverage::MovingAverage( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
: m_ema( quotes, nPeriods, tdPeriod )
{
  m_ceMA.SetName( sName );
  m_ceMA.SetColour( colour );
  m_ema.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdate ) );
}

MovingAverage::MovingAverage( MovingAverage&& rhs )
: m_ema(  std::move( rhs.m_ema ) )
, m_ceMA( std::move( rhs.m_ceMA ) )
{
  m_ema.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdate ) );
}

MovingAverage::~MovingAverage() {
  m_ema.OnUpdate.Remove( MakeDelegate( this, &MovingAverage::HandleUpdate ) );
}

void MovingAverage::AddToView( ou::ChartDataView& cdv, size_t slot ) {
  cdv.Add( slot, &m_ceMA );
}

void MovingAverage::HandleUpdate( const ou::tf::Price& price ) {
  m_ceMA.Append( price );
}

} // namespace ou
} // namespace tf

