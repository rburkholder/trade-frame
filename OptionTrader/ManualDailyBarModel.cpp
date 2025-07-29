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
 * File:    ManualDailyBarModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 28, 2025 09:00:23
 */

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include "OUCommon/Colour.h"
#include <OUCommon/TimeSource.h>

#include <TFIndicators/Pivots.h>

#include "ManualDailyBarModel.hpp"

namespace {
  static const boost::gregorian::date_duration  c_oneday( 1 );

  static const boost::posix_time::time_duration c_time_bgn_future( 18, 0, 0 ); // eastern time
  static const boost::posix_time::time_duration c_time_end_future( 17, 0, 0 );

  static const boost::posix_time::time_duration c_time_bgn_equity(  9, 30, 0 ); // eastern time
  static const boost::posix_time::time_duration c_time_end_equity( 16,  0, 0 );
}

ManualDailyBarModel::ManualDailyBarModel()
: m_type( ou::tf::InstrumentType::Unknown )
, m_state( EState::wait_for_start )
{
  m_cePriceBars.SetName( "Daily Bars" );

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolume );
  //m_dvChart.Add( EChartSlot::Price, &m_ceStatistics );

}

ManualDailyBarModel::~ManualDailyBarModel() {
}

void ManualDailyBarModel::Set( ou::tf::InstrumentType::EInstrumentType type ) {
  assert( ou::tf::InstrumentType::Unknown != type );
  m_type = type;
}

void ManualDailyBarModel::SetTimeFrame_future( boost::posix_time::ptime dt ) {

  const auto time = dt.time_of_day();
  const auto date = dt.date();

  if ( c_time_bgn_future <= time ) { // evening
    m_dtStart = boost::posix_time::ptime( date, c_time_bgn_future );
    m_dtEnd   = m_dtStart + c_oneday;
    m_dateBarTracking = date;
  }
  else { // morning
    m_dtEnd   = boost::posix_time::ptime( date, c_time_bgn_future );
    m_dtStart = m_dtEnd - c_oneday;
    m_dateBarTracking = date - c_oneday;
  }

  //BOOST_LOG_TRIVIAL(trace) << "future rng  " << m_dtStart << " - " << m_dtEnd;
}

void ManualDailyBarModel::SetTimeFrame_equity( boost::posix_time::ptime dt ) {

  //const auto time = dt.time_of_day();
  const auto date = dt.date();

  m_dtStart = boost::posix_time::ptime( date, c_time_bgn_equity );
  m_dtEnd   = boost::posix_time::ptime( date, c_time_end_equity );
  m_dateBarTracking = date;

  //BOOST_LOG_TRIVIAL(trace) << "equity rng  " << m_dtStart << " - " << m_dtEnd;
}

void ManualDailyBarModel::CloseBar() {
  if ( 0 < m_barsIntraDay.Size() ) {
    OnHistoryDailyBar( m_barAccumulate );
  }
  m_barsIntraDay.Clear();
}

void ManualDailyBarModel::OnHistoryIntraBar( const ou::tf::Bar& bar ) {

  //BOOST_LOG_TRIVIAL(trace) << "ManualDailyBarModel intra bar " << bar.DateTime();

  assert( ou::tf::InstrumentType::Unknown != m_type );

  const auto timeBar = bar.DateTime().time_of_day();
  const auto dateBar = bar.DateTime().date();

  switch ( m_type ) {
    case ou::tf::InstrumentType::Index:
    case ou::tf::InstrumentType::Future:
    case ou::tf::InstrumentType::FuturesOption:
      {
        switch( m_state ) {
          case EState::wait_for_start:
            SetTimeFrame_future( bar.DateTime() );
            m_state = EState::in_session;
            // fall through
          case EState::in_session:
            if ( m_dtEnd <= ( bar.DateTime() ) ) {
              CloseBar();
              SetTimeFrame_future( bar.DateTime() );
            }
            if ( 0 == m_barsIntraDay.Size() ) {
              m_barAccumulate = bar;
            }
            else {
              m_barAccumulate.Accumulate( bar );
            }
            m_barsIntraDay.Append( bar );
            break;
          case EState::out_of_session:
            break;
        }
      }
      break;
    case ou::tf::InstrumentType::Stock:
    case ou::tf::InstrumentType::Option:
      {
        switch( m_state ) {
          case EState::wait_for_start:
            SetTimeFrame_equity( bar.DateTime() );
            m_state = EState::in_session;
            // fall through
          case EState::in_session:
            if ( m_dateBarTracking != bar.DateTime().date() ) {
              CloseBar();
              SetTimeFrame_equity( bar.DateTime() );
            }
            if ( ( m_dtStart <= bar.DateTime() ) && ( m_dtEnd > bar.DateTime() ) ) {
              if ( 0 == m_barsIntraDay.Size() ) {
                m_barAccumulate = bar;
              }
              else {
                m_barAccumulate.Accumulate( bar );
              }
              m_barsIntraDay.Append( bar );
            }
            break;
          case EState::out_of_session:
            break;
        }
      }
      break;
    default:
      assert( false );
  }
}

void ManualDailyBarModel::OnHistoryDailyBar( const ou::tf::Bar& bar_ ) {
  //BOOST_LOG_TRIVIAL(trace) << "ManualDailyBarModel daily bar " << bar_.DateTime();
  ptime dtUtc = ou::TimeSource::ConvertEasternToUtc( bar_.DateTime() );
  ou::tf::Bar bar( dtUtc, bar_.Open(), bar_.High(), bar_.Low(), bar_.Close(), bar_.Volume( ) );
  m_barsInterDay.Append( bar_ );
  //std::cout << "bar close " << bar.Close() << "@" << bar_.DateTime() << "(est)/" << dtUtc << "(utc)" << std::endl;
  HandleBarCompletionPrice( bar );
}

void ManualDailyBarModel::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  m_cePriceBars.AppendBar( bar );
  m_ceVolume.Append( bar );
}

void ManualDailyBarModel::OnHistoryDone() {
  if ( 0 < m_barsIntraDay.Size() ) {
    BOOST_LOG_TRIVIAL(trace) << "ManualDailyBarModel::OnHistoryDone intra bars accumulated: " << m_barsIntraDay.Size();
  }
  OnHistoryDoneStatistics();
}

void ManualDailyBarModel::OnHistoryDoneStatistics() {

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
  m_barsInterDay.ForEachReverse(
    [this,&ix
      , &dblAvg200,&dblAvg100,&dblAvg50,&dblAvg21,&dblAvg7
      , &dblAvgRange200,&dblAvgRange100,&dblAvgRange50,&dblAvgRange21,&dblAvgRange7
    ]( const ou::tf::Bar& bar ){
    if ( 200 >= ix ) {
      std::string sIx = boost::lexical_cast<std::string>( ix );
      m_ceStatistics.AddMark( bar.High(), ou::Colour::LightGreen,  "hi-" + sIx );
      m_ceStatistics.AddMark( bar.Low(),  ou::Colour::LightSalmon, "lo-" + sIx );
    }

    const ou::tf::DatedDatum::price_t close( bar.Close() );
    const ou::tf::DatedDatum::price_t range( bar.High() - bar.Low() );

    if ( 200 >= ix ) {
      dblAvg200      += close / 200.0;
      dblAvgRange200 += range / 200.0;
    }
    if ( 100 >= ix ) {
      dblAvg100      += close / 100.0;
      dblAvgRange100 += range / 100.0;
    }
    if ( 50 >= ix ) {
      dblAvg50       += close / 50;
      dblAvgRange50  += range / 50;
    }
    if ( 21 >= ix ) {
      dblAvg21       += close / 21;
      dblAvgRange21  += range / 21;
    }
    if ( 7 >= ix ) {
      dblAvg7        += close / 7;
      dblAvgRange7   += range / 7;
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

  Pivots();
}

void ManualDailyBarModel::Pivots() {

  if ( 0 < m_barsInterDay.Size() ) {
    const ou::tf::Bar bar( m_barsInterDay.last() );

    using PS = ou::tf::PivotSet;
    ou::tf::PivotSet ps;

    ps.CalcPivots( bar.High(), bar.Low(), bar.Close() );

    m_ceStatistics.AddMark( ps.GetPivotValue( PS::R2 ), ps.GetPivotColour( PS::R2 ), "r2" );
    m_ceStatistics.AddMark( ps.GetPivotValue( PS::R1 ), ps.GetPivotColour( PS::R1 ), "r1" );
    m_ceStatistics.AddMark( ps.GetPivotValue( PS::PV ), ps.GetPivotColour( PS::PV ), "pv" );
    m_ceStatistics.AddMark( ps.GetPivotValue( PS::S1 ), ps.GetPivotColour( PS::S1 ), "s1" );
    m_ceStatistics.AddMark( ps.GetPivotValue( PS::S2 ), ps.GetPivotColour( PS::S2 ), "s2" );

    BOOST_LOG_TRIVIAL(info)
      << "pivots"
      <<  " r2=" << ps.GetPivotValue( PS::R2 )
      << ", r1=" << ps.GetPivotValue( PS::R1 )
      << ", pv=" << ps.GetPivotValue( PS::PV )
      << ", s1=" << ps.GetPivotValue( PS::S1 )
      << ", s2=" << ps.GetPivotValue( PS::S2 )
      ;
  }

}