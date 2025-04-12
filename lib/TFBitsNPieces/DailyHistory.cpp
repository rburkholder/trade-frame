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
 * Project: lib/TFBitsNPieces
 * Created: May 17, 2022 16:24
 */

#include <TFIndicators/Pivots.h>

#include "DailyHistory.hpp"

DailyHistory::~DailyHistory() {
  Close();
}

void DailyHistory::Load( const std::string& sIQFeedSymbolName, fAddMark_t&& fAddMark, fDone_t&& fDone ) {

  assert( fAddMark );
  assert( fDone );

  m_pBarHistory = ou::tf::iqfeed::BarHistory::Construct(
    [this,sIQFeedSymbolName,fAddMark_=std::move(fAddMark),fDone_=std::move(fDone)](){ // fConnected_t
      m_pBarHistory->Set(
        [this]( const ou::tf::Bar& bar){ // BarHistory::fBar_t
          m_barsHistory.Append( bar );
          //m_pHistoryRequest.reset(); // TODO: surface the disconnect and make synchronous
        },
        [this,fAddMark__=std::move(fAddMark_),fDone__=std::move(fDone_)](){ // BarHistory::fDone_t

          const ou::tf::Bar& barLast( m_barsHistory.last() );

          std::cout
            << "last bar at " << barLast.DateTime()
            << std::endl;

          ou::tf::PivotSet setPivots;

          setPivots.CalcPivots( barLast );
          const ou::tf::PivotSet& ps( setPivots );
          using PS = ou::tf::PivotSet;
          fAddMark__( ps.GetPivotValue( PS::R2 ), "r2" );
          fAddMark__( ps.GetPivotValue( PS::R1 ), "r1" );
          fAddMark__( ps.GetPivotValue( PS::PV ), "pv" );
          fAddMark__( ps.GetPivotValue( PS::S1 ), "s1" );
          fAddMark__( ps.GetPivotValue( PS::S2 ), "s2" );

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

          double dblAvgRange200 {};
          double dblAvgRange100 {};
          double dblAvgRange50 {};
          double dblAvgRange21 {};
          double dblAvgRange7 {};

          int ix( 1 );

          m_barsHistory.ForEachReverse(
            [this,&ix,&fAddMark__
             , &dblAvg200,&dblAvg100,&dblAvg50,&dblAvg21,&dblAvg7
             , &dblAvgRange200,&dblAvgRange100,&dblAvgRange50,&dblAvgRange21,&dblAvgRange7
            ]( const ou::tf::Bar& bar ){
            //std::cout
            //  << "bar " << ix << " is " << bar.Close()
            //  << std::endl;
            if ( 200 >= ix ) {
              std::string sIx = boost::lexical_cast<std::string>( ix );
              fAddMark__( bar.High(), "hi-" + sIx );
              fAddMark__( bar.Low(),  "lo-" + sIx );
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

          std::cout
            << "sma"
            << " 7 day=" << dblAvg7
            << ", 21 day=" << dblAvg21
            << ", 50 day=" << dblAvg50
            << ", 100 day=" << dblAvg100
            << ", 200 day=" << dblAvg200
            << std::endl;

          fAddMark__(   dblAvg7,   "7 day" );
          fAddMark__(  dblAvg21,  "21 day" );
          fAddMark__(  dblAvg50,  "50 day" );
          fAddMark__( dblAvg100, "100 day" );
          fAddMark__( dblAvg200, "200 day" );

          std::cout
            << "range"
            <<    " 7 day=" << dblAvgRange7
            <<  ", 21 day=" << dblAvgRange21
            <<  ", 50 day=" << dblAvgRange50
            << ", 100 day=" << dblAvgRange100
            << ", 200 day=" << dblAvgRange200
            << std::endl;

          fDone__(m_barsHistory);
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
