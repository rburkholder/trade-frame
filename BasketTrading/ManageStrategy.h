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
#include <TFOptions/SpreadValidation.h>
#include <TFOptions/Strangle.h>

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryMark.h>

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>


// 2019/05/23 Trading Day
//   ES dropped from 2056 at futures open to about 2016 in the morning (-1.15->-1.2% drop)
//   strangles were profitable on the basket elements
//   profitable legs could be exited
//   TODO: watch ATM IV to see if profitable legs should be rolled-up/down or just exited
//      and new strikes entered when IV returns closer to noral
//      or sell premium(short the same leg?)

class ManageStrategy:
  public ou::tf::DailyTradeTimeFrame<ManageStrategy>
{
  friend ou::tf::DailyTradeTimeFrame<ManageStrategy>;
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t     = ou::tf::option::Option::pOption_t;

  using pPosition_t  = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;

  using idPortfolio_t = ou::tf::Portfolio::idPortfolio_t;

  using pOrder_t = ou::tf::Position::pOrder_t;

  using fOptionDefinition_t        = std::function<void(const ou::tf::iqfeed::MarketSymbol::TableRowDef&)>;
  using fGatherOptionDefinitions_t = std::function<void(const std::string&, fOptionDefinition_t)>;

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  using fConstructedOption_t = std::function<void(pOption_t)>;

  using fConstructWatch_t  = std::function<void(const std::string&, fConstructedWatch_t)>;
  using fConstructOption_t = std::function<void(const std::string&, const pInstrument_t, fConstructedOption_t)>;  // source from IQFeed Symbol Name

  using fConstructPosition_t = std::function<pPosition_t( const ou::tf::Portfolio::idPortfolio_t, pWatch_t )>;

  using fConstructPortfolio_t = std::function<pPortfolio_t( const idPortfolio_t&, const idPortfolio_t)>; // id of new, id of ManageStrategy portfolio

  using fStartCalc_t = ou::tf::option::IvAtm::fStartCalc_t;
  using fStopCalc_t  = ou::tf::option::IvAtm::fStopCalc_t;

  using fFirstTrade_t = std::function<void(ManageStrategy&,const ou::tf::Trade&)>;
  using fBar_t        = std::function<void(ManageStrategy&,const ou::tf::Bar&)>;

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  ManageStrategy(
    const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily,
    pPortfolio_t,
    fGatherOptionDefinitions_t,
    fConstructWatch_t,
    fConstructOption_t,
    fConstructPosition_t,
    fConstructPortfolio_t,
    fStartCalc_t,
    fStopCalc_t,
    fFirstTrade_t,
    fBar_t,
    pChartDataView_t
    );
  virtual ~ManageStrategy( );

  const std::string& GetUnderlying() const { return m_sUnderlying; }

  ou::tf::DatedDatum::volume_t CalcShareCount( double dblAmount ) const;
  void SetFundsToTrade( double dblFundsToTrade ) { m_dblFundsToTrade = dblFundsToTrade; };
  void SetPivots( double dblS1, double dblPV, double dblR1 );
  void Start( void );
  void Stop( void );
  void SaveSeries( const std::string& sPrefix );

  void Add( pPosition_t ); // add pre-existing position

  void Test( void );

  void TakeProfits();  // close any profitable leg
  void CloseExpiryItm( boost::gregorian::date );
  void CloseFarItm(); // if one leg is far otm, close the far itm leg (far otm < 0.10, profit > 0.20?
  void CloseForProfits(); // sum(positions) > 0.10 profit (slippage, commissions), not CloseFarItm.
  void CloseItmLeg(); // leg needs to be positive, but overall combo may not be in the profit

  void AddStrangle( bool bForced, ou::tf::OrderSide::enumOrderSide );

  double EmitInfo();

protected:
private:

  enum ETradingState {
    TSInitializing, // set for duration of class initialization
    TSWaitForFirstTrade,  // wait for first trade during Regular Trading Hours
    TSWaitForFundsAllocation,  // flagged, reached only after first trade has arrived
    TSWaitForEntry, // start of equity trading
    TSOptionEvaluation, // start of option trading
    TSMonitorStrangle,
    TSMonitorLong, TSMonitorShort,
    TSNoMore
  };

  ETradingState m_stateTrading;

  enum class ETradeDirection { None, Up, Down };

  enum class EmaState {
    EmaUnstable, EmaUp, EmaDown
  };

  enum class EBarDirection { None, Up, Down };

  ETradeDirection m_eTradeDirection;

  EmaState m_stateEma;
  size_t m_nConfirmationIntervals;

  ou::tf::OrderSide::enumOrderSide m_OrderSide;

  //EBarDirection m_rBarDirection[ 3 ];

  using volume_t = ou::tf::DatedDatum::volume_t;

  std::string m_sUnderlying;

  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  bool m_bClosedItmLeg; // when leg closed, allow new combo upon command
  bool m_bAllowComboAdd; // allows state machine to open new combo

  double m_dblOpen;

  ou::tf::Trade m_TradeUnderlyingLatest;
  ou::tf::Quote m_QuoteUnderlyingLatest;

//  ou::tf::Quotes m_quotes; // not in use at the moment
//  ou::tf::Trades m_trades; // not in use at the moment

//  double m_cntUpReturn;
//  double m_cntDnReturn;

  using mapChains_t = std::map<boost::gregorian::date, ou::tf::option::IvAtm>;
  mapChains_t m_mapChains;
  mapChains_t::iterator m_iterChainExpiryInUse;

  fConstructWatch_t m_fConstructWatch;
  fConstructOption_t m_fConstructOption;
  fConstructPosition_t m_fConstructPosition;
  fConstructPortfolio_t m_fConstructPortfolio;

  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;

  fFirstTrade_t m_fFirstTrade;
  fBar_t m_fBar;

  const ou::tf::Bar& m_barPriorDaily;

  pPosition_t m_pPositionUnderlying;

  pPortfolio_t m_pPortfolioStrategy;

  pChartDataView_t m_pChartDataView;

  size_t m_ixColour;  // index into rColour for assigning colours to leg p/l

  ou::tf::SpreadValidation m_SpreadValidation;

  using Strangle = ou::tf::option::Strangle;

  using mapCombo_t = std::map<std::string,Strangle>;
  mapCombo_t m_mapCombo;

  ou::tf::BarFactory m_bfQuotes01Sec; // need Order Monitoring ticks more frequently

  ou::tf::BarFactory m_bfTrades01Sec; // ema calcs
  ou::tf::BarFactory m_bfTrades06Sec; // charting
  //ou::tf::BarFactory m_bfTrades60Sec; // sentiment analysis

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;

  ou::ChartEntryIndicator m_ceProfitLossPortfolio;

  //ou::ChartEntryIndicator m_ceUpReturn;
  //ou::ChartEntryIndicator m_ceDnReturn;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

  // https://stats.stackexchange.com/questions/111851/standard-deviation-of-an-exponentially-weighted-mean
  // http://people.ds.cam.ac.uk/fanf2/hermes/doc/antiforgery/stats.pdf
  struct EMA {
    double dblCoef1; // smaller - used on arriving value
    double dblCoef2; // 1 - dblCoef1 (larger), used on prior ema
    double dblEmaLatest;
    //double dblSD;
    //double dblSn;
    enum class State { falling, same, rising };
    State state;

    //ou::ChartEntryIndicator m_ceUpperBollinger;
    ou::ChartEntryIndicator m_ceEma;
    //ou::ChartEntryIndicator m_ceLowerBollinger;
    //ou::ChartEntryIndicator m_ceSD;
    pChartDataView_t pChartDataView;
    EMA( unsigned int nIntervals, pChartDataView_t pChartDataView_, ou::Colour::enumColour colour )
    : dblEmaLatest {}, pChartDataView( pChartDataView_ ), state( State::same )//, dblSD {}, dblSn {}
    {
      dblCoef1 = 2.0 / ( nIntervals + 1 );
      dblCoef2 = 1.0 - dblCoef1;
      //pChartDataView->Add( 0, &m_ceUpperBollinger );
      pChartDataView->Add( 0, &m_ceEma );
      //pChartDataView->Add( 0, &m_ceLowerBollinger );
      //pChartDataView->Add( 0, &m_ceUpperBollinger );
      //pChartDataView->Add( 3, &m_ceSD );
      //m_ceUpperBollinger.SetColour( colour );
      m_ceEma.SetColour( colour );
      //m_ceLowerBollinger.SetColour( colour );
      //m_ceSD.SetColour( colour );
    }
    EMA( const EMA& rhs )
    : dblCoef1( rhs.dblCoef1 ), dblCoef2( rhs.dblCoef2 ), dblEmaLatest( rhs.dblEmaLatest ),
      state( rhs.state ),
      pChartDataView( rhs.pChartDataView )//, m_ceEma( std::move( rhs.m_ceEma ) )
    {
      pChartDataView->Add( 0, &m_ceEma ); // TODO: fix classes to handle a std::move
      m_ceEma.SetColour( rhs.m_ceEma.GetColour() );
    }
    ~EMA() {
      pChartDataView->Remove( 0, &m_ceEma ); // required when moving EMA into vector
    }
    void SetName( const std::string& sName ) {
      m_ceEma.SetName( sName );
    }
    double First( boost::posix_time::ptime dt, double value ) {
      dblEmaLatest = value;
      //m_ceUpperBollinger.Append( dt, 0.0 );
      m_ceEma.Append( dt, dblEmaLatest );
      //m_ceLowerBollinger.Append( dt, 0.0 );
      //m_ceSD.Append( dt, 0.0 );
      return dblEmaLatest;
    }
    double Update( boost::posix_time::ptime dt, double value ) {
      double dblPrevious( dblEmaLatest );
      dblEmaLatest = ( dblCoef1 * value ) + ( dblCoef2 * dblEmaLatest );
      //double diff = ( value - dblPrevious );
      //dblSn = dblCoef2 * ( dblSn + dblCoef1 * diff * diff );
      // std dev is sqrt( dblSn ) ie, Sn is variance
      if ( dblEmaLatest == dblPrevious ) {
        state = State::same;
      }
      else {
        state = ( dblEmaLatest > dblPrevious ) ? State::rising : State::falling;
      }
      //dblSD = std::sqrt( dblSn ); // calc std dev
      //double range = 2.0 * dblSD; // calc constant * std dev
      //m_ceUpperBollinger.Append( dt, dblEmaLatest + range );
      m_ceEma.Append( dt, dblEmaLatest );
      //m_ceLowerBollinger.Append( dt, dblEmaLatest - range );
      //m_ceSD.Append( dt, range );

      return dblEmaLatest;
    }
  };

  using pEMA_t = std::shared_ptr<EMA>;
  using vEMA_t = std::vector<pEMA_t>;
  vEMA_t m_vEMA;

  double CurrentAtmStrike( double mid );

  void HandleBarQuotes01Sec( const ou::tf::Bar& bar );

  void HandleBarTrades01Sec( const ou::tf::Bar& bar );
  void HandleBarTrades06Sec( const ou::tf::Bar& bar );
  void HandleBarTrades60Sec( const ou::tf::Bar& bar );

  void HandleQuoteUnderlying( const ou::tf::Quote& quote );
  void HandleTradeUnderlying( const ou::tf::Trade& trade );

  // one shot inherited states:
  void HandleBellHeard( void );
  void HandleCancel( void );
  void HandleGoNeutral( void );

  void RHEquity( const ou::tf::Bar& bar );
  void RHOption( const ou::tf::Bar& bar );

  void HandleGoingNeutral( const ou::tf::Bar& bar );
  void HandleGoingNeutral( const ou::tf::Quote& quote ) {};
  void HandleGoingNeutral( const ou::tf::Trade& trade ) {};

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Bar& bar );

  void HandleAfterRH( const ou::tf::Quote& quote );
  void HandleAfterRH( const ou::tf::Trade& trade );
  void HandleAfterRH( const ou::tf::Bar& bar );

};

#endif /* MANAGESTRATEGY_H */
