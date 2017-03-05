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
#include <TFTrading/OrdersOutstanding.h>

#include <TFOptions/Bundle.h>

#include <OUCharting/ChartDVBasics.h>

class Strategy: 
  public ou::ChartDVBasics,
  public ou::tf::DailyTradeTimeFrame<Strategy>
{
  friend ou::tf::DailyTradeTimeFrame<Strategy>; 
public:

  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Position::pPosition_t pPosition_t;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

  Strategy( ou::tf::option::MultiExpiryBundle* meb, 
    pPortfolio_t pPortfolioLongs, pPortfolio_t pPortfolioShorts, pProvider_t pExecutionProvider );
  ~Strategy(void);

  void EmitStats( void ) const;

protected:
private:

  enum ETradingState { 
    eTSUnknown, 
    eTSSlopeRisingAboveMean,eTSSlopeRisingBelowMean,
    eTSSlopeFallingAboveMean, eTSSlopeFallingBelowMean, 
    eTSCount
  };
  enum EBollingerState { eBollingerUnknown, eBollingerLow, eBollingerHigh, eBollingerMid };
  enum ESlope { eSlopeUnknown, eSlopeNeg, eSlopePos };
  enum EInd1 { eInd1WaitForEntry, eInd1InRising, eInd1InFalling, eInd1FollowLongStop, eINd1FollowShortStop };

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

  bool m_bTrade;  // if execution provider available, then trade
  unsigned int m_nPositions; // used for creating unique position keys

  ptime m_dtQuote;
  ou::tf::Order::pOrder_t m_pOrderTrending;
  double m_dblStop;

  ou::tf::option::MultiExpiryBundle* m_pBundle;  // keep towards top of variable section
  pProvider_t m_pExecutionProvider;

  pPortfolio_t m_pPortfolioLongs;
  pPortfolio_t m_pPortfolioShorts;

  pPosition_t m_pPositionLongs;
  pPosition_t m_pPositionShorts;

  ou::tf::OrdersOutstandingLongs* m_pOrdersOutstandingLongs;
  ou::tf::OrdersOutstandingShorts* m_pOrdersOutstandingShorts;

  //ou::ChartDataBase m_ChartDataUnderlying;

  int m_nLongs;
  int m_nShorts;

  ou::ChartEntryIndicator m_ceCountLongs;
  ou::ChartEntryIndicator m_ceCountShorts;
  ou::ChartEntryIndicator m_cePL;
  //ou::ChartEntryIndicator m_cePLLongs;
  //ou::ChartEntryIndicator m_cePLShorts;

  EInd1 m_eInd1;
  ETradingState m_eTradingState;
  ESlope m_eBollinger1EmaSlope;
  std::vector<EBollingerState> m_vBollingerState;

  struct TradeStateHistory {
    ETradingState eTradingState;
    ou::tf::Quote quote;
    TradeStateHistory( ETradingState state, const ou::tf::Quote& quote_ ): eTradingState( state ), quote( quote_ ) {};
  };
  typedef std::vector<TradeStateHistory> vTradeStateHistory_t;
  vTradeStateHistory_t m_vTradeStateHistory;

  struct TradeStateHistorySummary {
    size_t nLongs;
    double dblTotalLongs;
    size_t nShorts;
    double dblTotalShorts;
    double dblSpread;
    double dblBollingerWidth;
    TradeStateHistorySummary( void )
      : nLongs( 0 ), dblTotalLongs( 0.0 ), nShorts( 0 ), dblTotalShorts( 0.0 ),
      dblSpread( 0.0 ), dblBollingerWidth( 0.0 )
    {};
  };
  TradeStateHistorySummary m_mxTradeStateHistorySummary[ eTSCount ][ eTSCount ][ eTSCount ];

  typedef std::map<boost::posix_time::ptime,BundleAtmIv> mapAtmIv_t;
  mapAtmIv_t m_mapAtmIv;

  bool m_bThreadPopDatumsActive;
  enum EDatumType { EDatumQuote, EDatumTrade };  // keep track of inbound datum ordering
  // a union of Quote/trade would not work as there is a copy constructor, which a union does not like

  boost::condition_variable m_cvCrossThreadDatums;
  boost::mutex m_mutexCrossThreadDatums;
  boost::thread* m_pThreadPopDatums;

  boost::lockfree::spsc_queue<EDatumType, boost::lockfree::capacity<1024> > m_lfDatumType;  // needs to be sum of quote, trade capacity
  boost::lockfree::spsc_queue<ou::tf::Quote, boost::lockfree::capacity<512> > m_lfQuote;
  boost::lockfree::spsc_queue<ou::tf::Trade, boost::lockfree::capacity<512> > m_lfTrade;

  void GoLong( void );
  void TakeLongProfits( void );
  void GoShort( void );
  void TakeShortProfits( void );

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void ThreadPopDatums( void );

  void HandleInboundQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleInboundTradeUnderlying( const ou::tf::Trade& trade );

  void HandleCancel( void );  // called with trade and quote

  void HandleCommon( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Quote& quote );

  void HandleCommon( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Trade& trade ) {};

  void HandleCalcIv( const ou::tf::PriceIV& );

  void HandleOrderFilled( const ou::tf::Order& order );

  typedef ou::tf::Position::PositionDelta_delegate_t PositionDelta_delegate_t;
  void HandleExecution( const PositionDelta_delegate_t& del );
  void HandleCommission( const PositionDelta_delegate_t& del );

};

