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
 * Project: IndicatorTrading
 * Created: 2022/11/29 11:57:41
 */

#include "MovingAverage.hpp"

MovingAverage::MovingAverage( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
: m_ema( quotes, nPeriods, tdPeriod )
{
  m_ceMA.SetName( sName );
  m_ceMA.SetColour( colour );
}

MovingAverage::MovingAverage( MovingAverage&& rhs )
: m_ema(  std::move( rhs.m_ema ) )
, m_ceMA( std::move( rhs.m_ceMA ) )
{}

void MovingAverage::AddToView( ou::ChartDataView& cdv, size_t slot ) {
  cdv.Add( slot, &m_ceMA );
}

void MovingAverage::Update( ptime dt ) {
  m_ceMA.Append( dt, m_ema.GetEMA() );
}
