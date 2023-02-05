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

// ====

MovingAverage::MovingAverage( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
: m_ema( quotes, nPeriods, tdPeriod )
, m_stats( quotes, nPeriods, tdPeriod )
//, m_maLast {}
{
  m_ceMA.SetName( sName );
  m_ceMA.SetColour( colour );

  m_ema.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdateEma ) );

  m_ceStdDeviation.SetName( sName + " SD" );
  m_ceStdDeviation.SetColour( colour );

//  m_ceStdDevUpper.SetColour( colour );
//  m_ceStdDevLower.SetColour( colour );


  m_stats.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdateStats ) );

}

MovingAverage::MovingAverage( MovingAverage&& rhs )
: m_ema(  std::move( rhs.m_ema ) )
, m_ceMA( std::move( rhs.m_ceMA ) )
, m_stats( std::move( rhs.m_stats ) )
, m_ceStdDeviation( std::move( rhs.m_ceStdDeviation ) )
//, m_ceStdDevUpper( std::move( rhs.m_ceStdDevUpper ) )
//, m_ceStdDevLower( std::move( rhs.m_ceStdDevLower ) )
//, m_maLast( rhs.m_maLast )
{
  m_ema.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdateEma ) );
  m_stats.OnUpdate.Add( MakeDelegate( this, &MovingAverage::HandleUpdateStats ) );
}

MovingAverage::~MovingAverage() {
  m_stats.OnUpdate.Remove( MakeDelegate( this, &MovingAverage::HandleUpdateStats ) );
  m_ema.OnUpdate.Remove( MakeDelegate( this, &MovingAverage::HandleUpdateEma ) );
}

void MovingAverage::AddToView( ou::ChartDataView& cdv, size_t slotMA ) {
  cdv.Add( slotMA, &m_ceMA );
  //cdv.Add( slotSD, &m_ceStdDeviation );
}

void MovingAverage::AddToView( ou::ChartDataView& cdv, size_t slotMA, size_t slotSD ) {
  cdv.Add( slotMA, &m_ceMA );
  //cdv.Add( slotMA, &m_ceStdDevUpper );
  cdv.Add( slotSD, &m_ceStdDeviation );
  //cdv.Add( slotMA, &m_ceStdDevLower );
}

void MovingAverage::HandleUpdateEma( const ou::tf::Price& price ) {
  //m_maLast = price.Value();
  m_ceMA.Append( price );
}

void MovingAverage::HandleUpdateStats( const ou::tf::TSSWStatsMidQuote::Results& results ) {
  //double ema( m_ema.GetEMA() );
  //double mid( m_quotes.last().Midpoint() );
  //double sdx2( 2.0 * results.stats.sd );
  //m_ceStdDevUpper.Append( results.dt, m_maLast + sdx2 );
  m_ceStdDeviation.Append( results.dt, results.stats.sd );
  //m_ceStdDevLower.Append( results.dt, m_maLast - sdx2 );
}

// ====

MovingAverageSlope::MovingAverageSlope( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
: MovingAverage( quotes, nPeriods, tdPeriod, colour, sName )
, m_stats( m_ema, boost::posix_time::time_duration( 0, 0, 3 ) )
, m_dblLast {}
{
  m_ceSlope.SetName( sName + " Slope" );
  m_ceSlope.SetColour( colour );

  m_stats.OnUpdate.Add( MakeDelegate( this, &MovingAverageSlope::HandleUpdate ) );
}

MovingAverageSlope::MovingAverageSlope( MovingAverageSlope&& rhs )
: MovingAverage( std::move( rhs ) )
//, m_stats( std::move( rhs.m_stats ) ) // don't do this, attaches to incorrect m_ema
, m_stats( m_ema, boost::posix_time::time_duration( 0, 0, 3 ) )
, m_ceSlope( std::move( rhs.m_ceSlope ) )
, m_dblLast( rhs.m_dblLast )
{
  m_stats.OnUpdate.Add( MakeDelegate( this, &MovingAverageSlope::HandleUpdate ) );
}

MovingAverageSlope::~MovingAverageSlope() {
  m_stats.OnUpdate.Remove( MakeDelegate( this, &MovingAverageSlope::HandleUpdate ) );
}

void MovingAverageSlope::AddToView( ou::ChartDataView& cdv, size_t slotMA, size_t slotSD, size_t slotSlope ) {
  MovingAverage::AddToView( cdv, slotMA, slotSD );
  cdv.Add( slotSlope, &m_ceSlope );
}

void MovingAverageSlope::HandleUpdate( const ou::tf::TSSWStatsPrice::Results& results ) {
  static const double limit( 0.0003 );
  double slope = results.stats.b1;
  if ( limit < slope ) {
    slope = limit;
  }
  else {
    if ( -limit > slope ) {
      slope = -limit;
    }
  }
  m_ceSlope.Append( results.dt, slope );
  m_dblLast = slope;

}

// ====

} // namespace ou
} // namespace tf

