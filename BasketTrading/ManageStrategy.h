/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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
 * File:   ManageStrategy.h
 * Author: raymond@burkholder.net
 *
 * Created on August 26, 2018, 6:46 PM
 */

#ifndef MANAGESTRATEGY_H
#define MANAGESTRATEGY_H

#include <string>
#include <memory>
#include <functional>

#include <TFIQFeed/MarketSymbol.h>

#include <TFTimeSeries/TimeSeries.h>
#include <TFTimeSeries/BarFactory.h>

#include <TFOptions/IvAtm.h>
#include <TFOptions/Option.h>

#include <TFTrading/Position.h>
#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryMark.h>

#include <TFTrading/Portfolio.h>
#include <TFTrading/DailyTradeTimeFrames.h>

class ManageStrategy: public ou::tf::DailyTradeTimeFrame<ManageStrategy> {
  friend ou::tf::DailyTradeTimeFrame<ManageStrategy>;
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t     = ou::tf::option::Option::pOption_t;

  using pPosition_t  = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using fOptionDefinition_t        = std::function<void(const ou::tf::iqfeed::MarketSymbol::TableRowDef&)>;
  using fGatherOptionDefinitions_t = std::function<void(const std::string&, fOptionDefinition_t)>;

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  using fConstructedOption_t = std::function<void(pOption_t)>;

  using fConstructWatch_t  = std::function<void(const std::string&, fConstructedWatch_t)>;
  using fConstructOption_t = std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t)>;  // source from IQFeed Symbol Name

  using fConstructPosition_t = std::function<pPosition_t( const ou::tf::Portfolio::idPortfolio_t, pWatch_t )>;

  using fStartCalc_t = ou::tf::option::IvAtm::fStartCalc_t;
  using fStopCalc_t  = ou::tf::option::IvAtm::fStopCalc_t;

  using fFirstTrade_t = std::function<void(ManageStrategy&,const ou::tf::Trade&)>;
  using fBar_t        = std::function<void(ManageStrategy&,const ou::tf::Bar&)>;

  using pcdvStrategyData_t = ou::ChartDataView::pChartDataView_t;

  ManageStrategy(
    const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily,
    pPortfolio_t,
    fGatherOptionDefinitions_t,
    fConstructWatch_t,
    fConstructOption_t,
    fConstructPosition_t,
    fStartCalc_t,
    fStopCalc_t,
    fFirstTrade_t,
    fBar_t,
    pcdvStrategyData_t
    );
  virtual ~ManageStrategy( );

  enum class ETradeDirection { None, Up, Down };

  const std::string& GetUnderlying() const { return m_sUnderlying; }

  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount ) const;
  void SetFundsToTrade( double dblFundsToTrade ) { m_dblFundsToTrade = dblFundsToTrade; };
  //bool& ToBeTraded( void ) { return m_bToBeTraded; };  // remote set/get - TODO: fix this?
  void SetPivots( double dblS1, double dblPV, double dblR1 );
  void Start( ETradeDirection );
  void Stop( void );
  void SaveSeries( const std::string& sPrefix );

  void Test( void );

protected:
private:

  typedef ou::tf::DatedDatum::volume_t volume_t;

  std::string m_sUnderlying;

  ETradeDirection m_eTradeDirection;

  //bool m_bToBeTraded; // may not be used, other than as a flag for remote state manipulation
  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  double m_dblOpen;

  //ou::tf::Bar m_barInfo;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  enum enumTradingState {
    TSInitializing, TSWaitForFirstTrade, TSWaitForCalc, TSWaitForEntry, TSWaitForContract, TSMonitorLong, TSMonitorShort, TSNoMore
  };

  enumTradingState m_stateTrading;

  using mapChains_t = std::map<boost::gregorian::date, ou::tf::option::IvAtm>;
  mapChains_t m_mapChains;

  fConstructWatch_t m_fConstructWatch;
  fConstructOption_t m_fConstructOption;
  fConstructPosition_t m_fConstructPosition;

  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;

  fFirstTrade_t m_fFirstTrade;
  fBar_t m_fBar;

  const ou::tf::Bar& m_barPriorDaily;

  pPosition_t m_pPositionUnderlying;
  pPosition_t m_PositionOption_Current; // current active put, depending upon roll-downs
  //pPosition_t m_pPositionOption_Previous;  // previous put if there was a roll-down

  pPortfolio_t m_pPortfolioStrategy;

  pcdvStrategyData_t m_pcdvStrategyData;

  //ou::tf::Bars m_bars60s;

  ou::tf::BarFactory m_bfTrades1Sec; // ema calcs
  ou::tf::BarFactory m_bfTrades6Sec; // charting
  ou::tf::BarFactory m_bfTrades60Sec; // sentiment analysis

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;
  ou::ChartEntryIndicator m_ceProfitLoss;

  struct EMA {
    double dblCoef1;
    double dblCoef2;
    double dblEmaLatest;

    ou::ChartEntryIndicator m_ceEma;
    pcdvStrategyData_t pChartDataView;
    EMA( unsigned int nIntervals, pcdvStrategyData_t pChartDataView_ )
    : dblEmaLatest {}, pChartDataView( pChartDataView_ )
    {
      dblCoef1 = 2.0 / ( nIntervals + 1 );
      dblCoef2 = 1.0 - dblCoef1;
      pChartDataView->Add( 0, &m_ceEma );
    }
    EMA( const EMA& rhs )
    : dblCoef1( rhs.dblCoef1 ), dblCoef2( rhs.dblCoef2 ), dblEmaLatest( rhs.dblEmaLatest ),
      pChartDataView( rhs.pChartDataView )//, m_ceEma( std::move( rhs.m_ceEma ) )
    {
      pChartDataView->Add( 0, &m_ceEma ); // TODO: fix classes to handle a std::move
    }
    ~EMA() {
      pChartDataView->Remove( 0, &m_ceEma ); // required when moving EMA into vector
    }
    double First( boost::posix_time::ptime dt, double value ) {
      dblEmaLatest = value;
      m_ceEma.Append( dt, dblEmaLatest );
      return dblEmaLatest;
    }
    double Update( boost::posix_time::ptime dt, double value ) {
      dblEmaLatest = ( dblCoef1 * dblEmaLatest ) + ( dblCoef2 * value );
      m_ceEma.Append( dt, dblEmaLatest );
      return dblEmaLatest;
    }
  };

  using pEMA_t = std::shared_ptr<EMA>;
  using vEMA_t = std::vector<pEMA_t>;
  vEMA_t m_vEMA;

  void HandleBarTrades1Sec( const ou::tf::Bar& bar );
  void HandleBarTrades6Sec( const ou::tf::Bar& bar );
  void HandleBarTrades60Sec( const ou::tf::Bar& bar );

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  void HandleAfterRH( const ou::tf::Quote& quote );
  void HandleAfterRH( const ou::tf::Trade& trade );
  void HandleAfterRH( const ou::tf::Bar& bar );

  // one shot inherited states:
  void HandleBellHeard( void );
  void HandleCancel( void );
  void HandleGoNeutral( void );

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Bar& bar );
};

#endif /* MANAGESTRATEGY_H */
