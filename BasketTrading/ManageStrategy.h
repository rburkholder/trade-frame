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

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryMark.h>

#include <TFTrading/Portfolio.h>
#include <TFTrading/Position.h>
#include <TFTrading/DailyTradeTimeFrames.h>
#include <TFTrading/MonitorOrder.h>

#include <TFBitsNPieces/Leg.h>

#include "SpreadCandidate.h"

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

  using pOrder_t = ou::tf::Position::pOrder_t;

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

  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

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

  double TakeProfits();
  void CloseExpiryItm( boost::gregorian::date );

protected:
private:

  enum ETradingState {
    TSInitializing, // set for duration of class initialization
    TSWaitForFirstTrade,  // wait for first trade during Regular Trading Hours
    TSWaitForFundsAllocation,  // flagged, reached only after first trade has arrived
    TSWaitForEntry, // start of equity trading
    TSOptionEvaluation, // start of option trading
    TSMonitorStraddle,
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
  //EOptionState m_eOptionState; // incorporated into Strike

  EmaState m_stateEma;
  size_t m_nConfirmationIntervals;

  //EBarDirection m_rBarDirection[ 3 ];

  using volume_t = ou::tf::DatedDatum::volume_t;

  std::string m_sUnderlying;

  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  size_t m_nLegs;

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

  fStartCalc_t m_fStartCalc;
  fStopCalc_t m_fStopCalc;

  fFirstTrade_t m_fFirstTrade;
  fBar_t m_fBar;

  const ou::tf::Bar& m_barPriorDaily;

  pPosition_t m_pPositionUnderlying;

  pPortfolio_t m_pPortfolioStrategy;

  pChartDataView_t m_pChartDataView;

  size_t m_ixColour;  // index into rColour for assigning colours to leg p/l


  // ==========================

  // TODO: add logic for management of other spreads (bull put), (bear call), (ratio back spread) ...
  class Strike {
  public:
    enum class State { Initializing, Validating, Positions, Executing, Watching, Canceled, Closing };
    State m_state;

    Strike( double dblStrikeLower, double dblStrikeAtm, double dblStrikeUpper )
    : m_state( State::Initializing ),
      m_bUpperClosed( false ), m_bLowerClosed( false ),
      m_dblStrikeLower( dblStrikeLower ), m_dblStrikeAtm( dblStrikeAtm ), m_dblStrikeUpper( dblStrikeUpper )
    {}
    Strike( const Strike& rhs ) = delete;
    Strike& operator=( const Strike& rhs ) = delete;
    Strike( const Strike&& rhs )
    : m_state( rhs.m_state ),
      m_dblStrikeUpper( rhs.m_dblStrikeUpper ),
      m_dblStrikeAtm( rhs.m_dblStrikeUpper ),
      m_dblStrikeLower( rhs.m_dblStrikeLower ),
      m_legCall( std::move( rhs.m_legCall ) ),
      m_legPut( std::move( rhs.m_legPut ) )
    {}

    void SetOptionCall( pOption_t pCall, ou::Colour::enumColour colour ) {
      m_scCall.SetWatch( pCall );
      m_legCall.SetColour( colour );
      m_state = State::Validating;
    }
    pOption_t GetOptionCall() { return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_scCall.GetWatch() ); }
    void SetOptionPut( pOption_t pPut, ou::Colour::enumColour colour ) {
      m_scPut.SetWatch( pPut );
      m_legPut.SetColour( colour );
      m_state = State::Validating;
    }
    pOption_t GetOptionPut() { return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_scPut.GetWatch() ); }

    bool ValidateSpread( size_t nDuration ) {
      bool bResult( false );
      switch ( m_state ) {
        case State::Validating:
          bResult = ( m_scCall.ValidateSpread( nDuration ) && m_scPut.ValidateSpread( nDuration ) );
          break;
      }
      return bResult;
    }

    void SetPositionCall( pPosition_t pCall ) {
      m_scCall.Clear();
      m_legCall.SetPosition( pCall );
      m_state = State::Positions;
    }
    pPosition_t GetPositionCall() { return m_legCall.GetPosition(); }
    void SetPositionPut( pPosition_t pPut ) {
      m_scPut.Clear();
      m_legPut.SetPosition( pPut );
      m_state = State::Positions;
    }
    pPosition_t GetPositionPut() { return m_legPut.GetPosition(); }

    void Tick( bool bInTrend, double dblPriceUnderlying, ptime dt ) { // TODO: make use of bInTrend to trigger exit latch
      m_legCall.Tick( dt );
      m_legPut.Tick( dt );
      switch ( m_state ) {  // TODO: make this a per-leg test?  even need state management?
        case State::Executing:
          if ( !AreOrdersActive() ) {
            m_state = State::Watching;
          }
          break;
        case State::Watching:
          Update( bInTrend, dblPriceUnderlying );
          break;
      }
    }

    void OrderLongStraddle() { // if volatility drops, then losses occur on premium
      switch ( m_state ) {
        case State::Positions: // doesn't confirm both put/call are available
        case State::Watching:
          m_legCall.OrderLong( 1 );
          m_legPut.OrderLong( 1 );
          m_state = State::Executing;
          m_bUpperClosed = false;
          m_bLowerClosed = false;
          break;
      }
    }
    void CancelOrders() {
      m_legCall.CancelOrder();
      m_legPut.CancelOrder();
      m_state = State::Canceled;
    }
    void ClosePositions() {
      if ( !m_bUpperClosed ) {
        m_legCall.ClosePosition();
        m_bUpperClosed = true;
      }
      if ( !m_bLowerClosed ) {
        m_legPut.ClosePosition();
        m_bLowerClosed = true;
      }
      m_state = State::Closing;
    }

    bool AreOrdersActive() const { return m_legCall.IsOrderActive() || m_legPut.IsOrderActive(); }
    void SaveSeries( const std::string& sPrefix ) {
      m_legCall.SaveSeries( sPrefix );
      m_legPut.SaveSeries( sPrefix );
    }
    void AddChartData( pChartDataView_t pChartData ) {
      m_legCall.AddChartData( pChartData );
      m_legPut.AddChartData( pChartData );
    }

    void SetColours( ou::Colour::enumColour colourCall, ou::Colour::enumColour colourPut ) {
      m_legCall.SetColour( colourCall );
      m_legPut.SetColour( colourPut );
    }
    void SetColourCall( ou::Colour::enumColour colour ) {
      m_legCall.SetColour( colour );
    }
    void SetColourPut( ou::Colour::enumColour colour ) {
      m_legPut.SetColour( colour );
    }

    double GetNet() {
      double dblNet {};
      pPosition_t pPositionCall = m_legCall.GetPosition();
      if ( pPositionCall ) {
        double dblCallValue = pPositionCall->GetUnRealizedPL();
        std::cout
          << "leg call: "
          << pPositionCall->GetInstrument()->GetInstrumentName()
          << "=" << dblCallValue;
        dblNet += dblCallValue;
        if ( 0.0 == dblCallValue ) {
          const ou::tf::Quote& quote( pPositionCall->GetWatch()->LastQuote() );
          std::cout
            << ", quote: a" << quote.Ask()
            << ", b" << quote.Bid()
            ;
        }
        std::cout << std::endl;
      }
      pPosition_t pPositionPut = m_legPut.GetPosition();
      if ( pPositionPut ) {
        double dblPutValue = pPositionPut->GetUnRealizedPL();
        std::cout
          << "leg put: "
          << pPositionPut->GetInstrument()->GetInstrumentName()
          << "=" << dblPutValue;
        dblNet += dblPutValue;
        if ( 0.0 == dblPutValue ) {
          const ou::tf::Quote& quote( pPositionPut->GetWatch()->LastQuote() );
          std::cout
            << ", quote: a" << quote.Ask()
            << ", b" << quote.Bid()
            ;
        }
        std::cout << std::endl;
      }
//      double dblNetValue = dblCallValue + dblPutValue;
//      if ( ( 0.0 != dblCallValue ) && ( 0.0 != dblPutValue ) ) {
//        std::cout
//          << "net value=" << dblNetValue
//          << " "
//          << pPositionCall->GetInstrument()->GetInstrumentName() << ":" << dblCallValue
//          << " - "
//          << pPositionPut->GetInstrument()->GetInstrumentName() << ":" << dblPutValue
//          << std::endl;
//      }
      return dblNet;
    }

    void CloseExpiryItm( const boost::gregorian::date date, double price ) {
      if ( price >= m_dblStrikeAtm ) {
        pPosition_t pPosition = m_legCall.GetPosition();
        if ( pPosition ) {
          if ( date == pPosition->GetInstrument()->GetExpiry() ) {
            m_legCall.ClosePosition();
          }
        }
      }
      if ( price <= m_dblStrikeAtm ) {
        pPosition_t pPosition = m_legPut.GetPosition();
        if ( pPosition ) {
          if ( date == pPosition->GetInstrument()->GetExpiry() ) {
            m_legPut.ClosePosition();
          }
        }
      }
    }

  private:
    double m_dblStrikeUpper;
    double m_dblStrikeAtm;
    double m_dblStrikeLower;

    SpreadCandidate m_scCall;
    SpreadCandidate m_scPut;

    ou::tf::Leg m_legCall;
    ou::tf::Leg m_legPut;

    bool m_bUpperClosed;
    bool m_bLowerClosed;

    void Update( bool bTrending, double dblPrice ) { // TODO: incorporate trending underlying
      if ( !m_bUpperClosed ) {
        if ( dblPrice >= m_dblStrikeUpper ) {
//          m_legCall.ClosePosition(); // closing too early
//          m_bUpperClosed = true;
        }
      }
      if ( !m_bLowerClosed ) {
        if ( dblPrice <= m_dblStrikeLower ) {
//          m_legPut.ClosePosition(); // closing too early
//          m_bLowerClosed = true;
        }
      }
    }
  };

  using mapStrike_t = std::map<double,Strike>;
  mapStrike_t m_mapStrike;

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
