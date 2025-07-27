/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    PivotModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 26, 2025 15:44:33
 */

#include <OUCommon/TimeSource.h>

#include <TFIndicators/Pivots.h>

#include "PivotModel.hpp"

PivotModel::PivotModel()
: m_bFirstBarProcessed( false )
{
  m_cePivot.SetName( "Pivots" );
}

PivotModel::~PivotModel() {
}

void PivotModel::OnHistoryBar( const ou::tf::Bar& bar_ ) {

  if ( m_bFirstBarProcessed ) {
    m_dtLastBarUTC = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
    //m_open = std::max<ou::tf::Price::price_t>( m_open, bar_.Open() );
    m_high = std::max<ou::tf::Price::price_t>( m_high, bar_.High() );
    m_low  = std::min<ou::tf::Price::price_t>( m_low , bar_.Low() );
    m_close = bar_.Close();
    m_volume += bar_.Volume();
  }
  else {
    m_bFirstBarProcessed = true;
    m_dtFirstBarUTC = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
    m_open = bar_.Open();
    m_high = bar_.High();
    m_low = bar_.Low();
    m_close = bar_.Close();
    m_volume = bar_.Volume();
  }
  //std::cout << "bar close " << bar.Close() << "@" << bar_.DateTime() << "(est)/" << dtUtc << "(utc)" << std::endl;
}

void PivotModel::OnHistoryDone() {
  // lib/TFBitsNPieces/DailyHistory.cpp used as inspiration

  using PS = ou::tf::PivotSet;
  ou::tf::PivotSet ps;

  ps.CalcPivots( m_high, m_low, m_close );

  m_cePivot.AddMark( ps.GetPivotValue( PS::R2 ), ps.GetPivotColour( PS::R2 ), "r2" );
  m_cePivot.AddMark( ps.GetPivotValue( PS::R1 ), ps.GetPivotColour( PS::R1 ), "r1" );
  m_cePivot.AddMark( ps.GetPivotValue( PS::PV ), ps.GetPivotColour( PS::PV ), "pv" );
  m_cePivot.AddMark( ps.GetPivotValue( PS::S1 ), ps.GetPivotColour( PS::S1 ), "s1" );
  m_cePivot.AddMark( ps.GetPivotValue( PS::S2 ), ps.GetPivotColour( PS::S2 ), "s2" );

  std::cout
    << "pivots"
    <<  " r2=" << ps.GetPivotValue( PS::R2 )
    << ", r1=" << ps.GetPivotValue( PS::R1 )
    << ", pv=" << ps.GetPivotValue( PS::PV )
    << ", s1=" << ps.GetPivotValue( PS::S1 )
    << ", s2=" << ps.GetPivotValue( PS::S2 )
    << std::endl;

}
