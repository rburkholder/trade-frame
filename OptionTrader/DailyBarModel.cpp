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

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include "OUCommon/Colour.h"
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
  m_barsHistory.Append( bar_ );
  //std::cout << "bar close " << bar.Close() << "@" << bar_.DateTime() << "(est)/" << dtUtc << "(utc)" << std::endl;
  HandleBarCompletionPrice( bar );
}

void DailyBarModel::OnHistoryDone() {

  double dblAvg200 {};
  double dblAvg100 {};
  double dblAvg50 {};
  double dblAvg21 {};
  double dblAvg7 {};

  double dblAvgRange200 {};
  double dblAvgRange100 {};
  double dblAvgRange50 {};
  double dblAvgRange21 {};
  double dblAvgRange7 {};

  int ix( 1 );

  //BOOST_LOG_TRIVIAL(info) << "history size " << m_barsHistory.Size();
  m_barsHistory.ForEachReverse(
    [this,&ix
      , &dblAvg200,&dblAvg100,&dblAvg50,&dblAvg21,&dblAvg7
      , &dblAvgRange200,&dblAvgRange100,&dblAvgRange50,&dblAvgRange21,&dblAvgRange7
    ]( const ou::tf::Bar& bar ){
    if ( 200 >= ix ) {
      std::string sIx = boost::lexical_cast<std::string>( ix );
      m_ceStatistics.AddMark( bar.High(), ou::Colour::LightSalmon, "hi-" + sIx );
      m_ceStatistics.AddMark( bar.Low(),  ou::Colour::LightSalmon, "lo-" + sIx );
    }

    const ou::tf::DatedDatum::price_t close( bar.Close() );
    const ou::tf::DatedDatum::price_t  diff( bar.High() - bar.Low() );

    if ( 200 >= ix ) {
      dblAvg200      += close / 200.0;
      dblAvgRange200 += diff  / 200.0;
    }
    if ( 100 >= ix ) {
      dblAvg100      += close / 100.0;
      dblAvgRange100 += diff  / 100.0;
    }
    if ( 50 >= ix ) {
      dblAvg50       += close / 50;
      dblAvgRange50  += diff  / 50;
    }
    if ( 21 >= ix ) {
      dblAvg21       += close / 21;
      dblAvgRange21  += diff  / 21;
    }
    if ( 7 >= ix ) {
      dblAvg7        += close / 7;
      dblAvgRange7   += diff  / 7;
    }
    ix++;
  });

  BOOST_LOG_TRIVIAL(info)
    << "sma"
    << " 7 day=" << dblAvg7
    << ", 21 day=" << dblAvg21
    << ", 50 day=" << dblAvg50
    << ", 100 day=" << dblAvg100
    << ", 200 day=" << dblAvg200
    ;

  m_ceStatistics.AddMark(   dblAvg7, ou::Colour::DarkOrchid,   "7 day" );
  m_ceStatistics.AddMark(  dblAvg21, ou::Colour::DarkOrchid,  "21 day" );
  m_ceStatistics.AddMark(  dblAvg50, ou::Colour::DarkOrchid,  "50 day" );
  m_ceStatistics.AddMark( dblAvg100, ou::Colour::DarkOrchid, "100 day" );
  m_ceStatistics.AddMark( dblAvg200, ou::Colour::DarkOrchid, "200 day" );

  BOOST_LOG_TRIVIAL(info)
    << "range"
    <<    " 7 day=" << dblAvgRange7
    <<  ", 21 day=" << dblAvgRange21
    <<  ", 50 day=" << dblAvgRange50
    << ", 100 day=" << dblAvgRange100
    << ", 200 day=" << dblAvgRange200
    ;

  }

void DailyBarModel::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_cePriceBars.AppendBar( bar );
  m_ceVolume.Append( bar );
}
