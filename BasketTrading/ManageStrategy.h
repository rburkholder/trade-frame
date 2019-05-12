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
    TSInitializing, TSWaitForFirstTrade, TSWaitForCalc, TSWaitForEntry,
    TSWaitForContract, TSMonitorLong, TSMonitorShort, TSMonitorStraddle,
    TSNoMore
  };
  enum class EOptionState {
    Initial, ValidatingSpread, MonitorPositionEntry, MonitorPositionExit
  };

  enum class ETradeDirection { None, Up, Down };

  enum class EmaState {
    EmaUnstable, EmaUp, EmaDown
  };

  enum class EBarDirection { None, Up, Down };

  using volume_t = ou::tf::DatedDatum::volume_t;

  std::string m_sUnderlying;

  ETradeDirection m_eTradeDirection;
  EOptionState m_eOptionState;

  EmaState m_stateEma;
  size_t m_nConfirmationIntervals;

  //EBarDirection m_rBarDirection[ 3 ];

  //bool m_bToBeTraded; // may not be used, other than as a flag for remote state manipulation
  double m_dblFundsToTrade;
  volume_t m_nSharesToTrade;

  double m_dblOpen;

  ou::tf::Trade m_TradeLatest;
  ou::tf::Quote m_QuoteLatest;

  ou::tf::Quotes m_quotes;
  ou::tf::Trades m_trades;

  double m_cntUpReturn;
  double m_cntDnReturn;

  ETradingState m_stateTrading;

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
  pPosition_t m_pPositionCall;
  pPosition_t m_pPositionPut;

  pPortfolio_t m_pPortfolioStrategy;

  pcdvStrategyData_t m_pcdvStrategyData;

  // TODO: convert to generic watch, and put into library
  class OptionCandidate {
  private:
    void CheckSpread( const ou::tf::Quote& quote ) {
      m_quote = quote;
    }
  public:
    OptionCandidate(): m_nConsecutiveSpreadOk {} {}
    void Clear() {
      if ( m_pOption ) {
        m_pOption->StopWatch();
        m_pOption->OnQuote.Remove( MakeDelegate( this, &OptionCandidate::CheckSpread ) );
        m_pOption.reset();
        m_nConsecutiveSpreadOk = 0;
      }
    }
    void SetOption( pOption_t& pOption ) {
      Clear();
      m_pOption = pOption;
      if ( m_pOption ) {
        m_pOption->OnQuote.Add( MakeDelegate( this, &OptionCandidate::CheckSpread ) );
        m_pOption->StartWatch();
      }
    }
    pOption_t& GetOption() { return m_pOption; }
    ~OptionCandidate() {
      Clear();
    }
    bool SpreadValidated( size_t nDuration ) {
      bool bOk( false );
      if ( m_pOption ) {
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
    double CurrentStrike() const {
      assert( m_pOption );
      return m_pOption->GetStrike();
    }
  private:
    ou::tf::Quote m_quote;
    size_t m_nConsecutiveSpreadOk;
    pOption_t m_pOption;
  };

  OptionCandidate m_candidateCall;
  OptionCandidate m_candidatePut;

  // convert to generic class and put into library
  class MonitorOrder {
  public:
    MonitorOrder(): m_CountDownToAdjustment {}, m_dblOffset {} {}
    bool PlaceOrder( pPosition_t& pPosition ) {
      bool bOk( false );
      m_pPosition = pPosition;
      double mid = m_pPosition->GetWatch()->LastQuote().Midpoint();
      double dblNormalizedPrice = m_pPosition->GetInstrument()->NormalizeOrderPrice( mid );
      pOrder_t pOrder = pPosition->ConstructOrder( ou::tf::OrderType::Limit, ou::tf::OrderSide::Buy, 1, dblNormalizedPrice );
      if ( !m_pOrder ) {
        m_CountDownToAdjustment = 7;
        m_dblOffset = 0.0;
        m_pOrder = pOrder;
        pPosition->PlaceOrder( pOrder );
        bOk = true;
      }
      return bOk;
    }
    void Clear() {
      m_pOrder.reset();
      m_pPosition.reset();
    }
    bool UpdateOrder() { // true when order has been filled
      bool bFilled( false );
      if ( m_pOrder ) {
        if ( 0 != m_pOrder->GetQuanRemaining() ) {
          assert( 0 < m_CountDownToAdjustment );
          m_CountDownToAdjustment--;
          if ( 0 == m_CountDownToAdjustment ) {
            m_dblOffset += 0.01;
            double mid = m_pPosition->GetWatch()->LastQuote().Midpoint();
            double dblNormalizedPrice = m_pPosition->GetInstrument()->NormalizeOrderPrice( mid );
            switch ( m_pOrder->GetOrderSide() ) {
              case ou::tf::OrderSide::Buy:
                m_pOrder->SetPrice1( dblNormalizedPrice + m_dblOffset );
                break;
              case ou::tf::OrderSide::Sell:
                m_pOrder->SetPrice1( dblNormalizedPrice - m_dblOffset );
                break;
            }
            m_pPosition->UpdateOrder( m_pOrder );
            m_CountDownToAdjustment = 7;
          }
        }
        else {
          Clear();
          bFilled = true;
        }
      }
      else {
        bFilled = true; // fake a clear, as no order exists, maybe generate an exception
      }
      return bFilled;
    }
  private:
    size_t m_CountDownToAdjustment;
    double m_dblOffset;
    pPosition_t m_pPosition;
    pOrder_t m_pOrder;
  };

  MonitorOrder m_monitorCallOrder;
  MonitorOrder m_monitorPutOrder;

  // strikes above and below the entry strike
  double m_strikeUpper;
  double m_strikeLower;

  class Strike {
  public:

  private:
  };

  struct OrderManager { // update limit order
    pOption_t m_pOption; // active option
    pPosition_t m_pPosition; // active option position
    pOrder_t m_pOrder; // active order
    double m_dblMidDiff;  // increment in 0.01 from mid to get order executed
    OrderManager(): m_dblMidDiff {} {}
    /*
     * For orders, opening, as well as closing
     * find atm strike (done)
     * create option at strike (done)
     * verify that quotes are within designated spread (done)
     * create position (done)
     * create order from position, submit as limit at midpoint (done)
     * periodically, if order still executing,
     *   update middiff, +/- based upon buy or sell (done)
     * => need call,put spreads to be < 0.10 && >= 0.01 (for a 6s interval)  (done)
     * => adjacent strikes need to be within 0.51 (done)
     * => a roll up or down needs to retain some profit after commission and spread
     * => roll once directional momentum on underlying has changed
     * => check open interest
     * => needs to be multi-day affair to reduce entry/exit spreads/commissions
     * => need end of week calendar roll, preferably when already about to roll on a strike
     *       start wed/thurs on the calendar rolls
     * => autonomously monitor entries, seek confirmation from money manager prior to entry
     * => allow daily and long term portfolios (allows another attempt at the ema strategy)
     * => to reduce symbol count, load up call first to examine spread, then load up put for verification?
     */
    void Start( pPosition_t pPositionUnderlying ) {}
    void Update( double price ) {} // periodic (~6s) re-evaluate order, find new atm if necessary
  };

  ou::tf::BarFactory m_bfTrades01Sec; // ema calcs
  ou::tf::BarFactory m_bfTrades06Sec; // charting
  //ou::tf::BarFactory m_bfTrades60Sec; // sentiment analysis

  ou::ChartEntryBars m_cePrice;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryMark m_cePivots;

  ou::ChartEntryIndicator m_ceProfitLossPortfolio;
  ou::ChartEntryIndicator m_ceProfitLossCall;
  ou::ChartEntryIndicator m_ceProfitLossPut;

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
    pcdvStrategyData_t pChartDataView;
    EMA( unsigned int nIntervals, pcdvStrategyData_t pChartDataView_, ou::Colour::enumColour colour )
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

  // Datum based inherited states:
  void HandleRHTrading( const ou::tf::Quote& quote );
  void HandleRHTrading( const ou::tf::Trade& trade );
  void HandleRHTrading( const ou::tf::Bar& bar );

  void RHEquity( const ou::tf::Bar& bar );
  void RHOption( const ou::tf::Bar& bar );
};

#endif /* MANAGESTRATEGY_H */
