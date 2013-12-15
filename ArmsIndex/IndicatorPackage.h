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

#include <boost/lockfree/spsc_queue.hpp>

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
     pProvider_t pDataProvider,
     pInstrument_t pInstIndex, pInstrument_t pInstTick, pInstrument_t pInstTrin
    );
  ~IndicatorPackage(void);

  void ToggleView( void );

  void SetChartDimensions( unsigned int x, unsigned int y );
  void PopData( void );
  void DrawCharts( void );

  typedef FastDelegate1<const MemBlock&> OnDrawChart_t;
  void SetOnDrawChartIndex( OnDrawChart_t function ) { m_bdIndex.m_cb = function; }
  void SetOnDrawChartTick( OnDrawChart_t function ) { m_bdTicks.m_cb = function; }
  void SetOnDrawChartArms( OnDrawChart_t function ) { m_OnDrawChartArms = function; }

  //void Start( void );
protected:
private:

  typedef std::vector<double> vDouble_t;

  bool m_bStarted;
  bool m_bFirstIndexFound;
  bool m_bTrinOfZZPairReady;
  bool m_bIndexOfZZPairReady;
  bool m_bDayView;

  double m_dblFirstIndex;
  double m_dblTrin;  // track value as it changes
  double m_dblOfsIdx;  // track value as it changes
  double m_dblZZTrin;
  double m_dblZZIndex;

  typedef std::vector<int> vColours_t;
  vColours_t m_vColours;

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

  double m_ctDayBegin;
  double m_ctDayEnd;

  double m_ctViewBegin;
  double m_ctViewEnd;

  struct ZZPair {
    double dblOfsIndx;
    double dblTrin;
    ZZPair( void ): dblOfsIndx( 0.0 ), dblTrin( 0.0 ) {};
    ZZPair( double dblOfsIndx_, double dblTrin_ ): dblOfsIndx( dblOfsIndx_ ), dblTrin( dblTrin_ ) {};
    ZZPair( const ZZPair& rhs ): dblOfsIndx( rhs.dblOfsIndx ), dblTrin( rhs.dblTrin ) {};
  };

  struct BarDoubles {
    vDouble_t m_vBarHigh;
    vDouble_t m_vBarOpen;
    vDouble_t m_vBarClose;
    vDouble_t m_vBarLow;
    vDouble_t m_vTime;
    ou::tf::Bar m_barWorking;
    OnDrawChart_t m_cb;

    void PushBack( const ou::tf::Bar& bar ) {
      m_vBarHigh.push_back( bar.High() );
      m_vBarOpen.push_back( bar.Open() );
      m_vBarClose.push_back( bar.Close() );
      m_vBarLow.push_back( bar.Low() );
      boost::posix_time::ptime dt( bar.DateTime() );
      m_vTime.push_back( 
        Chart::chartTime( 
          dt.date().year(), dt.date().month(), dt.date().day(),
          dt.time_of_day().hours(), dt.time_of_day().minutes(), dt.time_of_day().seconds() ) );
    }
    void PopBack( void ) {
      m_vBarHigh.pop_back();
      m_vBarOpen.pop_back();
      m_vBarClose.pop_back();
      m_vBarLow.pop_back();
      m_vTime.pop_back();
    }
    void WorkingBar( const ou::tf::Bar& bar ) {
      m_barWorking = bar;
    }
    void PushWorkingBar( void ) {
      PushBack( m_barWorking );
    }
    DoubleArray High(void) { return DoubleArray( &m_vBarHigh.front(), m_vBarHigh.size() ); };
    DoubleArray Open(void) { return DoubleArray( &m_vBarOpen.front(), m_vBarOpen.size() ); };
    DoubleArray Close(void) { return DoubleArray( &m_vBarClose.front(), m_vBarClose.size() ); };
    DoubleArray Low(void) { return DoubleArray( &m_vBarLow.front(), m_vBarLow.size() ); };
    DoubleArray Time(void) { return DoubleArray( &m_vTime.front(), m_vTime.size() ); };
  };

  BarDoubles m_bdIndex;
  BarDoubles m_bdTicks;

  unsigned int m_nPixelsX;
  unsigned int m_nPixelsY;

  ou::tf::BarFactory m_bfTick;
  ou::tf::BarFactory m_bfIndex;

  boost::lockfree::spsc_queue<ou::tf::Trade, boost::lockfree::capacity<512> > m_lfIndex;
  boost::lockfree::spsc_queue<ou::tf::Trade, boost::lockfree::capacity<512> > m_lfTick;
//  boost::lockfree::spsc_queue<ou::tf::Trade, boost::lockfree::capacity<512> > m_lfTrin;
  boost::lockfree::spsc_queue<ZZPair, boost::lockfree::capacity<512> > m_lfIndexTrinPair;

  OnDrawChart_t m_OnDrawChartArms;

  void PushZZPair( void );

  void DrawChart( BarDoubles& bd, const std::string& sName );
  void DrawChartIndex( void );
  void DrawChartTick( void );
  void DrawChartArms( void );

  void HandleOnTrin( const ou::tf::Trade& trade );
  void HandleOnTick( const ou::tf::Trade& trade );
  void HandleOnIndex( const ou::tf::Trade& trade );

  void HandleOnZZTrinPeakFound( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );
  void HandleOnZZIndexPeakFound( const ou::tf::ZigZag&, ptime, double, ou::tf::ZigZag::EDirection );

};

