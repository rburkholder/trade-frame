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
 * File:    DailyHistory.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: May 17, 2022 16:24
 */

#include <TFTrading/Watch.h>

#include <TFIndicators/Pivots.h>

#include "DailyHistory.hpp"

DailyHistory::~DailyHistory() {
  Close();
}

void DailyHistory::Load( const std::string& sIQFeedSymbolName, ou::ChartEntryMark& cem, fDone_t&& fDone ) {

  assert( fDone );

  using pWatch_t = ou::tf::Watch::pWatch_t;

  m_pBarHistory = ou::tf::iqfeed::BarHistory::Construct(
    [this,&cem,sIQFeedSymbolName,fDone_=std::move(fDone)](){ // fConnected_t
      m_pBarHistory->Set(
        [this]( const ou::tf::Bar& bar){
          m_barsHistory.Append( bar );
          //m_pHistoryRequest.reset(); // TODO: surface the disconnect and make synchronous
        },
        [this,&cem,fDone__=std::move(fDone_)](){
          ou::Colour::EColour colour( ou::Colour::BlueViolet );
          const ou::tf::Bar& bar( m_barsHistory.last() );

          //pWatch_t pWatchUnderlying = m_pPositionUnderlying->GetWatch();
          std::cout
          //  << pWatchUnderlying->GetInstrumentName()
            << "last bar at " << bar.DateTime()
            << std::endl;

          ou::tf::PivotSet setPivots;

          setPivots.CalcPivots( bar );
          const ou::tf::PivotSet& ps( setPivots );
          using PS = ou::tf::PivotSet;
          cem.AddMark( ps.GetPivotValue( PS::R2 ), colour, "r2" );
          cem.AddMark( ps.GetPivotValue( PS::R1 ), colour, "r1" );
          cem.AddMark( ps.GetPivotValue( PS::PV ), colour, "pv" );
          cem.AddMark( ps.GetPivotValue( PS::S1 ), colour, "s1" );
          cem.AddMark( ps.GetPivotValue( PS::S2 ), colour, "s2" );

          std::cout
            << "pivots"
            <<  " r2=" << ps.GetPivotValue( PS::R2 )
            << ", r1=" << ps.GetPivotValue( PS::R1 )
            << ", pv=" << ps.GetPivotValue( PS::PV )
            << ", s1=" << ps.GetPivotValue( PS::S1 )
            << ", s2=" << ps.GetPivotValue( PS::S2 )
            << std::endl;

          double dblAvg200 {};
          double dblAvg100 {};
          double dblAvg50 {};
          double dblAvg21 {};
          double dblAvg7 {};
          int ix( 1 );

          m_barsHistory.ForEachReverse( [this,&ix,&cem,&dblAvg200,&dblAvg100,&dblAvg50,&dblAvg21,&dblAvg7,colour]( const ou::tf::Bar& bar ){
            //std::cout
            //  << "bar " << ix << " is " << bar.Close()
            //  << std::endl;
            if ( 200 >= ix ) {
              std::string sIx = boost::lexical_cast<std::string>( ix );
              cem.AddMark( bar.High(), colour, "hi-" + sIx );
              cem.AddMark( bar.Low(),  colour, "lo-" + sIx );
            }
            if ( 200 >= ix ) {
              dblAvg200 += bar.Close() / 200.0;
            }
            if ( 100 >= ix ) {
              dblAvg100 += bar.Close() / 100.0;
            }
            if ( 50 >= ix ) {
              dblAvg50 += bar.Close() / 50;
            }
            if ( 21 >= ix ) {
              dblAvg21 += bar.Close() / 21;
            }
            if ( 7 >= ix ) {
              dblAvg7 += bar.Close() / 7;
            }
            ix++;
          });

          std::cout
            << "sma"
            << " 7 day=" << dblAvg7
            << ", 21 day=" << dblAvg21
            << ", 50 day=" << dblAvg50
            << ", 100 day=" << dblAvg100
            << ", 200 day=" << dblAvg200
            << std::endl;

          cem.AddMark(   dblAvg7, colour, "7 day" );
          cem.AddMark(  dblAvg21, colour, "21 day" );
          cem.AddMark(  dblAvg50, colour, "50 day" );
          cem.AddMark( dblAvg100, colour, "100 day" );
          cem.AddMark( dblAvg200, colour, "200 day" );

          fDone__();
        }
      );
      m_pBarHistory->RequestNEndOfDay( sIQFeedSymbolName, 200 );
    }
  );
  m_pBarHistory->Connect();
}

void DailyHistory::Close() {
  if ( m_pBarHistory ) {
    m_pBarHistory.reset();
  }
  
}