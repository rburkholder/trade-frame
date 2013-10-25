/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/09/29
// Taken from Strateg1/Strategy.h

// used as a base class for super-classes, which would add additional indicators to the existing ones.

#pragma once

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>
#include <TFTimeSeries/Adapters.h>

#include <TFIndicators/ZigZag.h>
#include <TFIndicators/TSEMA.h>
//#include <TFIndicators/TSDifferential.h>
//#include <TFIndicators/TSVariance.h>
#include <TFIndicators/TSSWRunningTally.h>
#include <TFIndicators/TSSWRateOfChange.h>
#include <TFIndicators/TSSWStats.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>

namespace ou { // One Unified

class ChartDataBase {
public:

  ChartDataBase(void);
  virtual ~ChartDataBase(void);

  ou::ChartDataView& GetChartDataView( void ) { return m_dvChart; };

//  void HandleFirstQuote( const ou::tf::Quote& quote );
//  void HandleFirstTrade( const ou::tf::Trade& trade );
  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

protected:

  enum enumTradeDirection { ETradeDirUnkn, ETradeDirUp, ETradeDirDn } m_TradeDirection;

  bool m_bFirstTrade;

  ou::ChartDataView m_dvChart;

  double m_dblUpTicks, m_dblMdTicks, m_dblDnTicks;
  double m_dblUpVolume, m_dblMdVolume, m_dblDnVolume;

  ou::tf::Prices m_pricesTickDiffs;
  ou::tf::TSSWRunningTally m_rtTickDiffs;

  ou::tf::Prices m_pricesTickDiffsROC;
  ou::tf::TSSWRateOfChange m_rocTickDiffs;

  ou::tf::Quote m_quoteLast;  // used for classifying the current trade direction

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  ou::tf::BarFactory m_bfTrades;
  ou::tf::BarFactory m_bfBuys;
  ou::tf::BarFactory m_bfSells;

  ou::ChartEntryBars m_ceBars;

  ou::tf::ZigZag m_zigzagPrice;

  struct ceVolumes_t {
    ou::ChartEntryVolume ceVolumeUp;
    ou::ChartEntryVolume ceVolumeNeutral;
    ou::ChartEntryVolume ceVolumeDn;
  };
  enum EVolumes_t { VDn, VUp, VCnt_ };
  ceVolumes_t m_rVolumes[ VCnt_ ];

  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema1;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema2;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema3;
  ou::tf::hf::TSEMA<ou::tf::Quote> m_ema4;

  ou::tf::TSSWStatsMidQuote m_stats1;
  ou::tf::TSSWStatsMidQuote m_stats2;
  ou::tf::TSSWStatsMidQuote m_stats3;
  ou::tf::TSSWStatsMidQuote m_stats4;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryIndicator m_ceQuoteUpper;
  ou::ChartEntryIndicator m_ceQuoteLower;
  ou::ChartEntryIndicator m_ceQuoteSpread;

  ou::ChartEntryIndicator m_ceEma1;
  ou::ChartEntryIndicator m_ceEma2;
  ou::ChartEntryIndicator m_ceEma3;
  ou::ChartEntryIndicator m_ceEma4;

//  ou::ChartEntryIndicator m_ce11;
//  ou::ChartEntryIndicator m_ce12;
//  ou::ChartEntryIndicator m_ce13;
//  ou::ChartEntryIndicator m_ce14;
//  ou::ChartEntryIndicator m_ce21;
//  ou::ChartEntryIndicator m_ce22;
//  ou::ChartEntryIndicator m_ce23;
//  ou::ChartEntryIndicator m_ce24;
//  ou::ChartEntryIndicator m_ce31;

  ou::ChartEntryIndicator m_ceTickDiffs;
  ou::ChartEntryIndicator m_ceTickDiffsRoc;

  ou::ChartEntryIndicator m_ceZigZag;

  ou::ChartEntryIndicator m_ceUpperBollinger1;
  ou::ChartEntryIndicator m_ceLowerBollinger1;
//  ou::ChartEntryIndicator m_ceBollinger1Offset;

//  ou::ChartEntryIndicator m_ceSMA2;
//  ou::ChartEntryIndicator m_ceSlopeOfSMA2;
//  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA2;
  ou::ChartEntryIndicator m_ceUpperBollinger2;
  ou::ChartEntryIndicator m_ceLowerBollinger2;
//  ou::ChartEntryIndicator m_ceBollinger2Offset;
//  ou::ChartEntryIndicator m_ceSlopeOfBollinger2Offset;
  //ou::ChartEntryIndicator m_ceBollinger2Ratio;
//  ou::ChartEntryIndicator m_ceSMA2RR;

//  ou::ChartEntryIndicator m_ceSMA3;
//  ou::ChartEntryIndicator m_ceSlopeOfSMA3;
  ou::ChartEntryIndicator m_ceUpperBollinger3;
  ou::ChartEntryIndicator m_ceLowerBollinger3;
//  ou::ChartEntryIndicator m_ceBollinger3Offset;
  //ou::ChartEntryIndicator m_ceBollinger3Ratio;
//  ou::ChartEntryIndicator m_ceSMA3RR;

  ou::ChartEntryIndicator m_ceUpperBollinger4;
  ou::ChartEntryIndicator m_ceLowerBollinger4;


private:

  void HandleBarCompletionTrades( const ou::tf::Bar& );
  void HandleBarCompletionBuys( const ou::tf::Bar& );
  void HandleBarCompletionSells( const ou::tf::Bar& );

  void HandleZigZagPeak( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );
  void HandleZigZagUpDp( const ou::tf::ZigZag& );
  void HandleZigZagDnDp( const ou::tf::ZigZag& );

};

} // namespace ou