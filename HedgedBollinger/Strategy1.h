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

#pragma once

// Started 2013/09/23

// started after MultiExpiryBundle has been populated

#include <map>

#include <boost/smart_ptr.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread_only.hpp>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <OUCharting/ChartEntryIndicator.h>

#include <TFTrading/DailyTradeTimeFrames.h>

#include <TFOptions/Bundle.h>

#include <OUCharting/ChartDataBase.h>

class Strategy: 
  public ou::tf::DailyTradeTimeFrame<Strategy>,
  public ou::ChartDataBase
{
  friend ou::tf::DailyTradeTimeFrame<Strategy>; 
public:
  Strategy( ou::tf::option::MultiExpiryBundle* meb );
  ~Strategy(void);
  //ou::ChartDataView& GetChartDataView( void ) { return m_ChartDataUnderlying.GetChartDataView(); };
protected:
private:

  enum EBollingerState { eBollingerUnknown, eBollingerLow, eBollingerHigh, eBollingerMid };
  std::vector<EBollingerState> m_vBollingerState;

  ou::tf::option::MultiExpiryBundle* m_pBundle;  // keep towards top of variable section

//  ou::ChartDataBase m_ChartDataUnderlying;

  struct BundleAtmIv {
    boost::shared_ptr<ou::ChartEntryIndicator> m_pceCallIV;
    boost::shared_ptr<ou::ChartEntryIndicator> m_pcePutIV;
    BundleAtmIv( void ) {
      m_pceCallIV.reset( new ou::ChartEntryIndicator );
      m_pcePutIV.reset( new ou::ChartEntryIndicator );
    }
    BundleAtmIv( const BundleAtmIv& rhs )
      : m_pceCallIV( rhs.m_pceCallIV ), m_pcePutIV( rhs.m_pcePutIV )
    {}
  };

  typedef std::map<boost::posix_time::ptime,BundleAtmIv> mapAtmIv_t;
  mapAtmIv_t m_mapAtmIv;

  bool m_bThreadPopDatumsActive;
  enum EDatumType { EDatumQuote, EDatumTrade };  // keep track of inbound datum ordering
  // a union of Quote/trade would not work as there is a copy constructor, which a union does not like

  boost::condition_variable m_cvCrossThreadDatums;
  boost::mutex m_mutexCrossThreadDatums;
  boost::thread* m_pThreadPopDatums;

  boost::lockfree::spsc_queue<EDatumType, boost::lockfree::capacity<1024> > m_lfDatumType;
  boost::lockfree::spsc_queue<ou::tf::Quote, boost::lockfree::capacity<512> > m_lfQuote;
  boost::lockfree::spsc_queue<ou::tf::Trade, boost::lockfree::capacity<512> > m_lfTrade;

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void ThreadPopDatums( void );

  void HandleInboundQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleInboundTradeUnderlying( const ou::tf::Trade& trade );

  void HandleCommon( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Quote& quote );

  void HandleCommon( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Trade& trade ) {};

  void HandleCalcIv( const ou::tf::PriceIV& );

};

