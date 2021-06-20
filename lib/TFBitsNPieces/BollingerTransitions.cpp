/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

#include <OUCharting/ChartEntryMark.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

#include "BollingerTransitions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

BollingerTransitions::BollingerTransitions()
: m_pricesDailyCloseBollinger20( m_pricesDailyClose, time_duration( 0, 0, 0 ), 20 ),
  m_stateBollinger( EBollingerState::Unknown )
{}

// TODO: extracted from BasketStrategy.ManageStrategy, requires fleshing out
void BollingerTransitions::Crossing( double price ) {

  // trigger on crossing mean -> StrategyStrangle, convert to StrategyCondor once in place
  // trigger on touching edge -> StrategyBackSpread
  // trigger on higher volatility -> StrategyStrangle once database has history of volatility

  static const EBollXing lu[EBollingerState::_Count][EBollingerState::_Count]
    = {
/* From:      To:     Unknown           BelowLower     MeanToLower      MeanToUpper       AboveUpper*/
/* Unknown */     { EBollXing::None, EBollXing::None,  EBollXing::None, EBollXing::None, EBollXing::None  },
/* AboveUpper */  { EBollXing::None, EBollXing::Lower, EBollXing::Mean, EBollXing::None, EBollXing::None  },
/* MeanToUpper */ { EBollXing::None, EBollXing::Lower, EBollXing::Mean, EBollXing::None, EBollXing::Upper },
/* MeanToLower */ { EBollXing::None, EBollXing::Lower, EBollXing::None, EBollXing::Mean, EBollXing::Upper },
/* BelowLower */  { EBollXing::None, EBollXing::None,  EBollXing::None, EBollXing::Mean, EBollXing::Upper },
      };

  EBollingerState stateBollinger = EBollingerState::Unknown;
  if ( m_dblBollingerMean <= price ) {
    if ( m_dblBollingerUpper <= price ) {
      stateBollinger = EBollingerState::AboveUpper;
    }
    else {
      stateBollinger = EBollingerState::MeanToUpper;
    }
  }
  else {
    if ( m_dblBollingerLower >= price ) {
      stateBollinger = EBollingerState::BelowLower;
    }
    else {
      stateBollinger = EBollingerState::MeanToLower;
    }
  }

  EBollXing xing = lu[m_stateBollinger][stateBollinger];

  switch ( xing ) {
    case EBollXing::Upper:
      // call back spread
      break;
    case EBollXing::Mean:
      // short strangle
      // condor
      break;
    case EBollXing::Lower:
      // put back spread
      break;
    default:
      break;
  }

  m_stateBollinger = stateBollinger;

}

void BollingerTransitions::ReadDailyBars( const std::string& sPath, ou::ChartEntryMark& cePivots ) {

  //void ChartTimeSeries( ou::tf::HDF5DataManager* pdm, ou::ChartDataView* pChartDataView, const std::string& sName, const std::string& sPath )

  size_t nPassedUpper {};
  size_t nPassedLower {};

  size_t ixSdMin {};
  size_t ixSdMax {};

  double dblBollingerSDMax {};
  double dblBollingerSDMin {};

  ou::tf::Bars barsDaily;

  ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RO );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar> tsRepository( dm, sPath );
  ou::tf::HDF5TimeSeriesContainer<ou::tf::Bar>::iterator begin, end;
  begin = tsRepository.begin();
  end = tsRepository.end();
  barsDaily.Clear();
  const hsize_t cnt = end - begin;
  barsDaily.Resize( cnt );
  tsRepository.Read( begin, end, &barsDaily );

  m_pricesDailyClose.Clear();
  m_pricesDailyCloseBollinger20.Reset();

  bool bLoopStarted( false );
  bool bPassedUpper( false );
  bool bPassedLower( false );

  nPassedUpper = nPassedLower = 0;
  ixSdMax = ixSdMin = 0;
  size_t cntMarking = cnt;

  for ( ou::tf::Bars::const_iterator iterBars = barsDaily.begin(); barsDaily.end() != iterBars; ++iterBars ) {

    if ( bLoopStarted ) { // calculations use previous day's bollinger
      bPassedUpper = bPassedLower = false;
      if ( iterBars->High() >= m_pricesDailyCloseBollinger20.BBUpper() ) {
        bPassedUpper = true;
      }
      if ( iterBars->Low() <= m_pricesDailyCloseBollinger20.BBLower() ) {
        bPassedLower = true;
      }

      if ( bPassedUpper || bPassedLower ) {
        if ( bPassedUpper && bPassedLower ) {
          nPassedUpper++;
          nPassedLower++;
        }
        else {
          if ( bPassedUpper ) {
            nPassedUpper++;
            nPassedLower = 0;
          }
          if ( bPassedLower ) {
            nPassedUpper = 0;
            nPassedLower++;
          }
        }
      }
      else {
        nPassedUpper = 0;
        nPassedLower = 0;
      }
    }
    else {
      bLoopStarted = true;
    }

    ou::tf::Price price( iterBars->DateTime(), iterBars->Close() );
    m_pricesDailyClose.Append( price ); // automatically updates indicators (bollinger)
    if ( 55 >= cntMarking ) { // only last bars show attractors
      cePivots.AddMark( iterBars->High(), ou::Colour::LightSalmon, "High" );
      cePivots.AddMark( iterBars->Low(),  ou::Colour::LightPink,   "Low" );

      const double sd = m_pricesDailyCloseBollinger20.SD();
      if ( sd > dblBollingerSDMax ) {
        dblBollingerSDMax = sd;
        ixSdMax = cntMarking;
      }
      if ( sd < dblBollingerSDMin ) {
        dblBollingerSDMin = sd;
        ixSdMin = cntMarking;
      }
    }
    else {
      dblBollingerSDMax = dblBollingerSDMin = m_pricesDailyCloseBollinger20.SD();
      ixSdMax = ixSdMin = cntMarking;
    }
    cntMarking--;
  } // end for

  m_dblBollingerUpper = m_pricesDailyCloseBollinger20.BBUpper();
  m_dblBollingerMean = m_pricesDailyCloseBollinger20.MeanY();
  m_dblBollingerLower = m_pricesDailyCloseBollinger20.BBLower();

  cePivots.AddMark( m_dblBollingerUpper, ou::Colour::Purple,     "BollUp" );
  cePivots.AddMark( m_dblBollingerMean,  ou::Colour::Salmon,     "BollMn" );
  cePivots.AddMark( m_dblBollingerLower, ou::Colour::PowderBlue, "BollLo" );

  std::cout
    << sPath
    << " sd min=" << dblBollingerSDMin << "@" << ixSdMin
    << ",cur= " << m_pricesDailyCloseBollinger20.SD()
    << ",max=" << dblBollingerSDMax << "@" << ixSdMax;
  if ( 1 == nPassedUpper ) std::cout << " - first touch on upper bollinger";
  if ( 1 == nPassedLower ) std::cout << " - first touch on lower bollinger";
  std::cout << std::endl;

  // trigger: if cross a bollinger band today, with m_nPassedxx 0, then a successful trigger for entry
  //          and ixSDmxx is not 1, then probably best time for backspread on near band as volatility has range to increase.
  //          and a normal spread on far band
  //          (within 6 days of expiry, allows some time for movement)
  // trigger: if crosses bollinger mean, then run a bull-put and bear-call
  //          (within min of 1 day of expiry to catch last day, or weekend theta decay)
  // trigger: darvas, initiate synthetic with protective option
  //          (synthetic 13 days, protective 4 days)
  // trigger:  sell strangle on high volatility, and exit on normal volatility
  //           will need database to indicate historical implied volatility
  //           will need to run options all the time in order to monitor implied volatility
  //           (1 day to expiry to reduce duration risk)

//    AddChartEntries( pChartDataView, series );

}


} // namespace tf
} // namespace ou
