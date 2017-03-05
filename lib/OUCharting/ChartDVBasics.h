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
// Taken from Strategy1/Strategy.h

// used as a base class for super-classes, which would add additional indicators to the existing ones.

#pragma once


#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>
//#include <TFTimeSeries/Adapters.h>

#include <TFIndicators/Crossing.h>
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
#include <OUCharting/ChartEntryMark.h>

namespace ou { // One Unified

class ChartDVBasics {
public:

  ChartDVBasics(void);
  virtual ~ChartDVBasics(void);

  ou::ChartDataView& GetChartDataView( void ) { return m_dvChart; };

//  void HandleFirstQuote( const ou::tf::Quote& quote );
//  void HandleFirstTrade( const ou::tf::Trade& trade );
  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

protected:

  enum enumTradeDirection { ETradeDirUnkn=0, ETradeDirUp, ETradeDirDn } m_TradeDirection;

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
    void Reserve( ou::ChartEntryVolume::size_type n ) {
      ceVolumeUp.Reserve( n );
      ceVolumeNeutral.Reserve( n );
      ceVolumeDn.Reserve( n );
    }
  };
  enum EVolumes_t { VDn=0, VUp, VCnt_ };
  ceVolumes_t m_rVolumes[ VCnt_ ];

  struct infoBollinger {
    ou::tf::Crossing<double> m_stateAccel;
    time_duration m_td;
    ou::tf::Quotes& m_quotes;
    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;
    ou::tf::TSSWStatsMidQuote m_stats;
    ou::ChartEntryIndicator m_ceEma;
    ou::ChartEntryIndicator m_ceUpperBollinger;
    ou::ChartEntryIndicator m_ceLowerBollinger;
    //ou::ChartEntryIndicator m_ceRatio;
    ou::tf::TSSWStatsPrice m_statsSlope;
    ou::ChartEntryIndicator m_ceSlope;
    ou::tf::Prices m_tsStatsSlope;

    ou::tf::TSSWStatsPrice m_statsSlopeBy2;
    ou::ChartEntryIndicator m_ceSlopeBy2;
    ou::tf::Prices m_tsStatsSlopeBy2;

    ou::tf::TSSWStatsPrice m_statsSlopeBy3;
    ou::ChartEntryIndicator m_ceSlopeBy3;

    double m_dblBollingerWidth;

    void SetProperties( ou::Colour::enumColour colour, const std::string& sName ) {
      m_ceEma.SetName( sName );
      m_ceEma.SetColour( colour );
      m_ceUpperBollinger.SetColour( colour );
      m_ceLowerBollinger.SetColour( colour );
      //m_ceRatio.SetColour( colour );
      m_ceSlope.SetColour( colour );
      m_ceSlopeBy2.SetColour( colour );
      m_ceSlopeBy3.SetColour( colour );
    }
    infoBollinger( ou::tf::Quotes& quotes, time_duration td )
      : m_td( td ), m_quotes( quotes ), m_dblBollingerWidth( 0.0 ),
        m_ema( quotes, td ), m_stats( quotes, td ),
        m_statsSlope( m_ema, boost::posix_time::time_duration( 0, 0, 30 ) ),
        m_statsSlopeBy2( m_tsStatsSlope, boost::posix_time::time_duration( 0, 0, 30 ) ),
        m_statsSlopeBy3( m_tsStatsSlopeBy2, boost::posix_time::time_duration( 0, 0, 15 ) )
    {
    }
    infoBollinger( const infoBollinger& rhs ) 
      : m_td( rhs.m_td ), m_quotes( rhs.m_quotes ), m_dblBollingerWidth( rhs.m_dblBollingerWidth ),
      m_ema( m_quotes, m_td ), m_stats( m_quotes, m_td ),
      m_statsSlope( m_ema, boost::posix_time::time_duration( 0, 0, 30 ) ),
      m_statsSlopeBy2( m_tsStatsSlope, boost::posix_time::time_duration( 0, 0, 30 ) ),
      m_statsSlopeBy3( m_tsStatsSlopeBy2, boost::posix_time::time_duration( 0, 0, 15 ) )
    {
    }
    ~infoBollinger( void ) {};
  };

  typedef std::vector<infoBollinger> vInfoBollinger_t;
  vInfoBollinger_t m_vInfoBollinger;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryIndicator m_ceQuoteUpper;
  ou::ChartEntryIndicator m_ceQuoteLower;
  ou::ChartEntryIndicator m_ceQuoteSpread;

  ou::ChartEntryIndicator m_ceTickDiffs;
  ou::ChartEntryIndicator m_ceTickDiffsRoc;

  ou::ChartEntryIndicator m_ceZigZag;

//  ou::ChartEntryMark m_cemRatio;
  ou::ChartEntryMark m_cemSlope;
  ou::ChartEntryMark m_cemSlopeBy2;
  ou::ChartEntryMark m_cemSlopeBy3;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

//  ou::ChartEntryIndicator m_ceSMA2;
//  ou::ChartEntryIndicator m_ceSlopeOfSMA2;
//  ou::ChartEntryIndicator m_ceSlopeOfSlopeOfSMA2;
//  ou::ChartEntryIndicator m_ceBollinger2Offset;
//  ou::ChartEntryIndicator m_ceSlopeOfBollinger2Offset;
  //ou::ChartEntryIndicator m_ceBollinger2Ratio;
//  ou::ChartEntryIndicator m_ceSMA2RR;

private:

  void HandleBarCompletionTrades( const ou::tf::Bar& );
  void HandleBarCompletionBuys( const ou::tf::Bar& );
  void HandleBarCompletionSells( const ou::tf::Bar& );

  void HandleZigZagPeak( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );
  void HandleZigZagUpDp( const ou::tf::ZigZag& );
  void HandleZigZagDnDp( const ou::tf::ZigZag& );

};

} // namespace ou