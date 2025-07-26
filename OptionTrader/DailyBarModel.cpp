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
 * File:    DailyBarModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 26, 2025 11:40:04
 */

#include <OUCommon/TimeSource.h>

#include "DailyBarModel.hpp"

DailyBarModel::DailyBarModel()
{
  m_cePriceBars.SetName( "Daily Bars" );

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );
  //m_dvChart.Add( EChartSlot::Price, &cem );

}

DailyBarModel::~DailyBarModel() {
}

//void SessionBarModel::Set( pWatch_t& pWatch, ou::ChartEntryMark& cem ) {
//}

void DailyBarModel::OnHistoryBar( const ou::tf::Bar& bar_ ) {
  ptime dtUtc = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
  ou::tf::Bar bar( dtUtc, bar_.Open(), bar_.High(), bar_.Low(), bar_.Close(), bar_.Volume( ) );
  std::cout << "bar close " << bar.Close() << "@" << bar_.DateTime() << "(est)/" << dtUtc << "(utc)" << std::endl;
  HandleBarCompletionPrice( bar );
}

void DailyBarModel::OnHistoryDone() {
}

void DailyBarModel::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_cePriceBars.AppendBar( bar );
  m_ceVolume.Append( bar );
}
