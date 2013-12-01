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
// Started 2013/12/01

#pragma once

#include <vector>

#ifdef _M_X64
#include <OUCharting/ChartDirector64/chartdir.h>
#else
#include <OUCharting/ChartDirector/chartdir.h>
#endif

#include <OUCharting/ChartMaster.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFIndicators/ZigZag.h>

class IndicatorPackage {
public: 
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  IndicatorPackage( 
     pInstrument_t pInstTrin, pInstrument_t pInstTick, pInstrument_t pInstIndex,
     pProvider_t pDataProvider
    );
  ~IndicatorPackage(void);

  void SetChartDimensions( unsigned int x, unsigned int y );
  void DrawCharts( void );

  typedef FastDelegate1<const MemBlock&> OnDrawChart_t;
  void SetOnDrawChartIndex( OnDrawChart_t function ) { m_OnDrawChartIndex = function; }
  void SetOnDrawChartArms( OnDrawChart_t function ) { m_OnDrawChartArms = function; }
  void SetOnDrawChartTick( OnDrawChart_t function ) { m_OnDrawChartTick = function; }

  //void Start( void );
protected:
private:

  typedef std::vector<double> vDouble_t;

  bool m_bStarted;
  bool m_bFirstIndexFound;
  bool m_bTrinOfZZPairReady;
  bool m_bIndexOfZZPairReady;

  double m_dblFirstIndex;
  double m_dblTrin;  // track value as it changes
  double m_dblOfsIdx;  // track value as it changes
  double m_dblZZTrin;
  double m_dblZZIndex;

  pInstrument_t m_pTrin;
  pInstrument_t m_pTick;
  pInstrument_t m_pIndex;
  pProvider_t m_pProvider;

  //ou::tf::Trades m_tradesTrin;
  //ou::tf::Trades m_tradesTick;
  //ou::tf::Trades m_tradesIndex;

  ou::tf::ZigZag m_zzTrin;
  ou::tf::ZigZag m_zzIndex;

  vDouble_t m_vTrin;
  vDouble_t m_vOfsIdx;

  vDouble_t m_vTick;
  vDouble_t m_vBarHigh;
  vDouble_t m_vBarOpen;
  vDouble_t m_vBarClose;
  vDouble_t m_vBarLow;

  unsigned int m_nPixelsX;
  unsigned int m_nPixelsY;

  ou::tf::BarFactory m_bfIndex;

  OnDrawChart_t m_OnDrawChartIndex;
  OnDrawChart_t m_OnDrawChartArms;
  OnDrawChart_t m_OnDrawChartTick;

  void DrawChartArms( void );
  void DrawChartIndex( void );

  void HandleOnTrin( const ou::tf::Trade& trade );
  void HandleOnTick( const ou::tf::Trade& trade );
  void HandleOnIndex( const ou::tf::Trade& trade );

  void HandleOnZZTrinPeakFound( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );
  void HandleOnZZIndexPeakFound( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );

  void HandleOnBFIndexUpdated( const ou::tf::Bar& );
  void HandleOnBFIndexComplete( const ou::tf::Bar& );
};

