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

  void Test( void );

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

  double m_dblOpen;

  ou::tf::Trade m_TradeLatest;
  ou::tf::Quote m_QuoteLatest;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  double m_cntUpReturn;
  double m_cntDnReturn;

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

  // TODO: convert to generic watch, and put into library
  class SpreadCandidate {
  public:
    SpreadCandidate(): m_nConsecutiveSpreadOk {} {}
    SpreadCandidate( const SpreadCandidate& rhs ) = delete;
    SpreadCandidate( const SpreadCandidate&& rhs )
    : m_quote( rhs.m_quote ), m_nConsecutiveSpreadOk( rhs.m_nConsecutiveSpreadOk ),
      m_pWatch( std::move( rhs.m_pWatch ) )
    {}
//    SpreadCandidate( pWatch_t pWatch )
//    : m_nConsecutiveSpreadOk {}
//    {
//      SetWatch( pWatch );
//    }
    ~SpreadCandidate() {
      Clear();
    }
    void Clear() {
      if ( m_pWatch ) {
        m_pWatch->StopWatch();
        m_pWatch->OnQuote.Remove( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
        m_pWatch.reset();
      }
    }
    void SetWatch( pWatch_t pWatch ) {
      Clear();
      m_pWatch = pWatch;
      if ( m_pWatch ) {
        m_nConsecutiveSpreadOk = 0;
        m_pWatch->OnQuote.Add( MakeDelegate( this, &SpreadCandidate::UpdateQuote ) );
        m_pWatch->StartWatch();
      }
    }
    pWatch_t GetWatch() { return m_pWatch; }
    bool ValidateSpread( size_t nDuration ) {
      bool bOk( false );
      if ( m_pWatch ) {
        if ( m_quote.IsValid() ) {
          double spread( m_quote.Spread() );
          if ( ( 0.005 <= spread ) && ( spread < 0.10 ) ) {
            m_nConsecutiveSpreadOk++;
          }
          else {
            m_nConsecutiveSpreadOk = 0;
          }
        }
        bOk = ( nDuration <= m_nConsecutiveSpreadOk );
      }
      return bOk;
    }
  private:
    ou::tf::Quote m_quote;
    size_t m_nConsecutiveSpreadOk;
    pWatch_t m_pWatch;
    void UpdateQuote( const ou::tf::Quote& quote ) {
      m_quote = quote;
    }
  };

  // ==========================

// convert to generic class and put into library
// devoted to one Order at a time
// NOTE: will require a tick between close/cancel and new order
  class MonitorOrder {
  public:
    MonitorOrder(): m_CountDownToAdjustment {}, m_dblOffset {} {}
    MonitorOrder( pPosition_t& pPosition )
    : m_CountDownToAdjustment {}, m_dblOffset {}, m_state( State::NoPosition ),
      m_pPosition( pPosition )
    {}
    MonitorOrder( const MonitorOrder& rhs ) = delete;
    MonitorOrder( const MonitorOrder&& rhs )
    : m_CountDownToAdjustment( rhs.m_CountDownToAdjustment ),
      m_state( rhs.m_state ),
      m_dblOffset( rhs.m_dblOffset ),
      m_pPosition( std::move( rhs.m_pPosition ) ),
      m_pOrder( std::move( rhs.m_pOrder ) )
    {}
    void SetPosition( pPosition_t pPosition ) {
      m_pPosition = pPosition;
      m_state = State::NoOrder;
    }
    // can only work on one order at a time
    bool PlaceOrder( boost::uint32_t nOrderQuantity, ou::tf::OrderSide::enumOrderSide side ) {
      bool bOk( false );
      switch ( m_state ) {
        case State::NoOrder:
        case State::Cancelled:  // can overwrite?
        case State::Filled:     // can overwrite?
          {
            double mid = m_pPosition->GetWatch()->LastQuote().Midpoint();
            double dblNormalizedPrice = m_pPosition->GetInstrument()->NormalizeOrderPrice( mid );
            m_pOrder = m_pPosition->ConstructOrder( ou::tf::OrderType::Limit, side, nOrderQuantity, dblNormalizedPrice );
            if ( m_pOrder ) {
              m_pOrder->OnOrderFilled.Add( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
              m_pOrder->OnOrderCancelled.Add( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
              m_CountDownToAdjustment = 7;
              m_dblOffset = 0.0;
              m_state = State::Active;
              m_pPosition->PlaceOrder( m_pOrder );
              std::cout << m_pPosition->GetInstrument()->GetInstrumentName() << ": placed at " << dblNormalizedPrice << std::endl;
              bOk = true;
            }
          }
          break;
        case State::Active:
          std::cout << m_pPosition->GetInstrument()->GetInstrumentName() << ": active, cannot place order" << std::endl;
          break;
        case State::NoPosition:
          break;
      }
      return bOk;
    }
    void CancelOrder() {  // TODO: need to fix this, and take the Order out of UpdateOrder
      switch ( m_state ) {
        case State::Active:
          m_pPosition->CancelOrder( m_pOrder->GetOrderId() );
          break;
        case State::NoOrder:
        case State::Cancelled:
        case State::Filled:
        case State::NoPosition:
          break;
      }
    }

    void Tick() {
      switch ( m_state ) {
        case State::Active:
          UpdateOrder();
          break;
        case State::Cancelled:
        case State::Filled:
          m_pOrder.reset();
          m_state = State::NoOrder;
          break;
        case State::NoOrder:
        case State::NoPosition:
          break;
      }
    }

    bool IsOrderActive() const { return ( State::Active == m_state ); }

  private:

    enum class State { NoPosition, NoOrder, Active, Filled, Cancelled };
    State m_state;
    size_t m_CountDownToAdjustment;
    double m_dblOffset;
    pPosition_t m_pPosition;
    pOrder_t m_pOrder;

    void UpdateOrder() { // true when order has been filled
      if ( 0 == m_pOrder->GetQuanRemaining() ) { // not sure if a cancel adjusts remaining
        // TODO: generate message? error on filled, but may be present on cancel
      }
      else {
        assert( 0 < m_CountDownToAdjustment );
        m_CountDownToAdjustment--;
        if ( 0 == m_CountDownToAdjustment ) {
          m_dblOffset += 0.01; // TODO: may need to put a cap on size of offset
          const ou::tf::Quote& quote( m_pPosition->GetWatch()->LastQuote() );
          double mid = quote.Midpoint();
          double spread = quote.Spread();
          double dblNormalizedPrice = m_pPosition->GetInstrument()->NormalizeOrderPrice( mid );
          switch ( m_pOrder->GetOrderSide() ) {
            case ou::tf::OrderSide::Buy:
              m_pOrder->SetPrice1( dblNormalizedPrice + m_dblOffset );
              break;
            case ou::tf::OrderSide::Sell:
              m_pOrder->SetPrice1( dblNormalizedPrice - m_dblOffset );
              break;
          }
          std::cout
            << m_pPosition->GetInstrument()->GetInstrumentName()
            << ": update order by " << m_dblOffset
            << " on " << dblNormalizedPrice
            << " spread " << spread
            << std::endl;
          m_pPosition->UpdateOrder( m_pOrder );
          m_CountDownToAdjustment = 7;
        }
      }
    }

    void OrderCancelled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
      switch ( m_state ) {
        case State::Active:
          m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
          m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
          m_state = State::Cancelled;
          break;
      }
    }
    void OrderFilled( const ou::tf::Order& order ) { // TODO: delegate should have const removed?
      switch ( m_state ) {
        case State::Active:
          m_pOrder->OnOrderCancelled.Remove( MakeDelegate( this, &MonitorOrder::OrderCancelled ) );
          m_pOrder->OnOrderFilled.Remove( MakeDelegate( this, &MonitorOrder::OrderFilled ) );
          m_state = State::Filled;
          break;
      }
    }
  };

  // ==========================

  class Leg {
  public:
    Leg() {
      Init();
    }
    Leg( pPosition_t pPosition ) // implies candidate will not be used
    {
      Init();
      SetPosition( pPosition );
    }
    Leg( const Leg& rhs ) = delete;
    Leg( const Leg&& rhs )
    : m_pPosition( std::move( rhs.m_pPosition ) ),
      m_candidate( std::move( rhs.m_candidate ) ),
      m_monitor( std::move( rhs.m_monitor ) )
    {
      Init();
    }
    void SetOption( pOption_t pOption ) { m_candidate.SetWatch( pOption ); }
    pOption_t GetOption() { return boost::dynamic_pointer_cast<ou::tf::option::Option>( m_candidate.GetWatch() ); }
    bool ValidateSpread( size_t nDuration ) {
      return m_candidate.ValidateSpread( nDuration );
    }
    void SetPosition( pPosition_t pPosition ) {
      m_candidate.Clear(); // implies candidate is finished and no longer required
      m_pPosition = pPosition;
      m_monitor.SetPosition( pPosition );
    }
    pPosition_t GetPosition() { return m_pPosition; }
    void Tick() {
      m_monitor.Tick();
    }
    void OrderLong( boost::uint32_t nOrderQuantity ) {
      if ( m_pPosition ) {
        m_monitor.PlaceOrder( nOrderQuantity, ou::tf::OrderSide::Buy );
      }

    }
    void OrderShort( boost::uint32_t nOrderQuantity ) {
      if ( m_pPosition ) {
        m_monitor.PlaceOrder( nOrderQuantity, ou::tf::OrderSide::Sell );
      }

    }
    void CancelOrder() {
      if ( m_pPosition ) {
        m_monitor.CancelOrder();
      }
    }
    void ClosePosition() {
      if ( m_pPosition ) {
        const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
        if ( m_monitor.IsOrderActive() ) {
          std::cout << row.sName << ": error, monitor has active order, no close possible" << std::endl;
        }
        else {
          if ( 0 != row.nPositionPending ) {
            std::cout << row.sName << ": warning, has pending size of " << row.nPositionPending << " during close" << std::endl;
          }
          if ( 0 != row.nPositionActive ) {
            std::cout << row.sName << ": closing position" << std::endl;
            switch ( row.eOrderSideActive ) {
              case ou::tf::OrderSide::Buy:
                m_monitor.PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Sell );
                break;
              case ou::tf::OrderSide::Sell:
                m_monitor.PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Buy );
                break;
            }
          }
        }
      }
    }
    bool IsOrderActive() const { return m_monitor.IsOrderActive(); }

    void SaveSeries( const std::string& sPrefix ) {
      m_pPosition->GetWatch()->SaveSeries( sPrefix );
    }

    void SetColour( ou::Colour::enumColour colour ) { m_ceProfitLoss.SetColour( colour ); }

    void AddChartData( pChartDataView_t pChartData ) {
      pChartData->Add( 2, &m_ceProfitLoss );
    }

  private:
    pPosition_t m_pPosition;
    SpreadCandidate m_candidate;
    MonitorOrder m_monitor;
    ou::ChartEntryIndicator m_ceProfitLoss; // TODO: add to chart

    void Init() {
      m_ceProfitLoss.SetName( m_pPosition->GetInstrument()->GetInstrumentName() + " P/L" );
    }
  };

  // ==========================

  // TODO: reload from database
  // TODO: add logic for management of other spreads (bull put), (bear call), (ratio back spread) ...
  class Strike {
  public:
    enum class State { Initializing, Validating, Positions, Executing, Watching, Canceled };
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
      m_legCall.SetOption( pCall );
      m_legCall.SetColour( colour );
      m_state = State::Validating;
    }
    pOption_t GetOptionCall() { return m_legCall.GetOption(); }
    void SetOptionPut( pOption_t pPut, ou::Colour::enumColour colour ) {
      m_legPut.SetOption( pPut );
      m_legPut.SetColour( colour );
      m_state = State::Validating;
    }
    pOption_t GetOptionPut() { return m_legPut.GetOption(); }

    bool ValidateSpread( size_t nDuration ) {
      bool bResult( false );
      switch ( m_state ) {
        case State::Validating:
          bResult = ( m_legCall.ValidateSpread( nDuration ) && m_legPut.ValidateSpread( nDuration ) );
          break;
      }
      return bResult;
    }

    void SetPositionCall( pPosition_t pCall ) {
      m_legCall.SetPosition( pCall );
      m_state = State::Positions;
    }
    pPosition_t GetPositionCall() { return m_legCall.GetPosition(); }
    void SetPositionPut( pPosition_t pPut ) {
      m_legPut.SetPosition( pPut );
      m_state = State::Positions;
    }
    pPosition_t GetPositionPut() { return m_legPut.GetPosition(); }

    void Tick( bool bInTrend, double dblPriceUnderlying ) { // TODO: make use of bInTrend to trigger exit latch
      m_legCall.Tick();
      m_legPut.Tick();
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

  private:
    double m_dblStrikeUpper;
    double m_dblStrikeAtm;
    double m_dblStrikeLower;
    Leg m_legCall;
    Leg m_legPut;

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

  ou::tf::BarFactory m_bfTrades01Sec; // ema calcs
  ou::tf::BarFactory m_bfTrades06Sec; // charting
  //ou::tf::BarFactory m_bfTrades60Sec; // sentiment analysis

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;

  ou::ChartEntryIndicator m_ceProfitLossPortfolio;

  ou::ChartEntryIndicator m_ceUpReturn;
  ou::ChartEntryIndicator m_ceDnReturn;

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

  void HandleBarTrades01Sec( const ou::tf::Bar& bar );
  void HandleBarTrades06Sec( const ou::tf::Bar& bar );
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

  void HandleGoingNeutral( const ou::tf::Bar& bar );
  void HandleGoingNeutral( const ou::tf::Quote& quote ) {};
  void HandleGoingNeutral( const ou::tf::Trade& trade ) {};

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Bar& bar );

  void RHEquity( const ou::tf::Bar& bar );
  void RHOption( const ou::tf::Bar& bar );
};

#endif /* MANAGESTRATEGY_H */
