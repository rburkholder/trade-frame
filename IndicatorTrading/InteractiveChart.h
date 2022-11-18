/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    InteractiveChart.h
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 13:38
 */

#pragma once

#include <atomic>
#include <functional>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryMark.h>
#include <OUCharting/ChartEntryShape.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFIndicators/TSEMA.h>
#include <TFIndicators/TSSWStochastic.h>

#include <TFTimeSeries/BarFactory.h>

#include <TFIQFeed/OptionChainQuery.h>

#include <TFIQFeed/Level2/Symbols.hpp>
#include <TFIQFeed/Level2/FeatureSet.hpp>

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>

#include <TFVuTrading/WinChartView.h>

#include "Indicators.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
  class TreeItem;
namespace iqfeed { // IQFeed
  class OptionChainQuery;
} // namespace iqfeed
namespace option {
  class Engine;
} // namespace option
  struct PanelOrderButtons_Order;
  struct PanelOrderButtons_MarketData;
  struct PanelOrderButtons_PositionData;
} // namespace tf
} // namespace ou

namespace config {
  class Options;
}

class Strategy;

class OptionTracker;
class TradeLifeTime;

class InteractiveChart:
  public ou::tf::WinChartView
{
public:

  InteractiveChart(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME,
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION,
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE,
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  virtual ~InteractiveChart();

  using TreeItem = ou::tf::TreeItem;

  using idOrder_t = ou::tf::Order::idOrder_t;

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using fOption_t = std::function<void(pOption_t)>;
  using fBuildOption_t = std::function<void(const std::string&,fOption_t&&)>;

  using fBuildPosition_t = std::function<pPosition_t(pInstrument_t)>;

  using fClick_t = std::function<void(double)>; // price level

  using fTriggerOrder_t = std::function<void(ou::tf::PanelOrderButtons_Order::EOrderMethod)>;
  using fUpdateMarketData_t = std::function<void(const ou::tf::PanelOrderButtons_MarketData&)>;
  using fUpdatePosition_t = std::function<void(const ou::tf::PanelOrderButtons_PositionData&)>;

  using pOptionChainQuery_t = std::shared_ptr<ou::tf::iqfeed::OptionChainQuery>;

  void SetPosition(
     pPosition_t
   , const config::Options&
   , pOptionChainQuery_t
   , fBuildOption_t&&
   , fBuildPosition_t&&
   , fClick_t&& left
   , fClick_t&& right
   , fTriggerOrder_t&&
   , fUpdateMarketData_t&&
   , fUpdatePosition_t&&
   , TreeItem*
   , ou::ChartEntryMark& cemReferenceLevels
    );

  void EmitChainFull() const {
    size_t cnt {};
    std::cout << "underlying: " << m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) << std::endl;
    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      std::cout << "chain: " << vt.first << " has " << vt.second.Size() << " entries" << std::endl;
      cnt += vt.second.EmitValues();
      //vt.second.EmitSummary();
    }
    std::cout << "EmitChainFull total chain strikes=" << cnt << std::endl;
  }

  void EmitChainSummary() const {
    size_t cnt {};
    std::cout << "underlying: " << m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) << std::endl;
    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      std::cout << "chain: " << vt.first << " has " << vt.second.Size() << " entries" << std::endl;
      //vt.second.EmitValues();
      cnt += vt.second.EmitSummary();
    }
    std::cout << "EmitChainSummary total sum(call + put)=" << cnt << std::endl;
  }

  void ProcessChains();

  void SaveWatch( const std::string& );
  void EmitOptions();

  void OptionWatchStart();
  void OptionQuoteShow();
  void OptionWatchStop();
  void OptionEmit();

  void OrderBuy( const ou::tf::PanelOrderButtons_Order& );
  void OrderSell( const ou::tf::PanelOrderButtons_Order& );
  void OrderClose( const ou::tf::PanelOrderButtons_Order& );
  void OrderCancel( const ou::tf::PanelOrderButtons_Order& );

  void CancelOrders();

  // are these still required, as menu handlgin performed within TradeLifeTime?
  void OrderCancel( idOrder_t );
  void EmitOrderStatus( idOrder_t );
  void DeleteLifeCycle( idOrder_t );

  void Imbalance( const ou::tf::Depth& );

  void EmitStatus();
  void FeatureSetDump();

  void ReleaseResources(); // mostly release related to ou::tf::PanelOrderButtons_Order

  void Connect();
  void Disconnect();

protected:

  virtual void LeftClick( int nChart, double value );
  virtual void RightClick( int nChart, double value );

private:

  enum EChartSlot { Price, Volume, StochInd, ImbalanceMean, ImbalanceB1, ImbalanceState, Sentiment, PL, Spread }; // IndMA = moving averate indicator

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  bool m_bConnected;
  bool m_bOptionsReady;
  bool m_bTriggerFeatureSetDump;

  ou::ChartDataView m_dvChart; // the data

  fClick_t m_fClickLeft;
  fClick_t m_fClickRight;

  fTriggerOrder_t m_fTriggerOrder;
  fUpdateMarketData_t m_fUpdateMarketData;
  fUpdatePosition_t m_fUpdatePosition;

  TreeItem* m_pTreeItemUnderlying;

  pOrder_t m_pOrder;
  pPosition_t m_pPositionUnderlying;
  pInstrument_t m_pActiveInstrument;  // selected from the tree for use in selecting position

  double m_dblSumVolume; // part of vwap
  double m_dblSumVolumePrice; // part of vwap

  ou::tf::BarFactory m_bfPrice;
  ou::tf::BarFactory m_bfPriceUp;
  ou::tf::BarFactory m_bfPriceDn;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryBars m_cePriceBars;

  ou::ChartEntryIndicator m_ceVWAP;

  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryVolume m_ceVolumeUp;
  ou::ChartEntryVolume m_ceVolumeDn;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;
  ou::ChartEntryIndicator m_ceQuoteSpread;

  ou::ChartEntryShape m_ceBuySubmit;
  ou::ChartEntryShape m_ceBuyFill;
  ou::ChartEntryShape m_ceSellSubmit;
  ou::ChartEntryShape m_ceSellFill;
  ou::ChartEntryShape m_ceCancelled;

  ou::ChartEntryShape m_ceBullCall;
  ou::ChartEntryShape m_ceBullPut;
  ou::ChartEntryShape m_ceBearCall;
  ou::ChartEntryShape m_ceBearPut;

  ou::ChartEntryIndicator m_ceImbalanceRawMean;
  ou::ChartEntryIndicator m_ceImbalanceRawB1;

  double m_dblImbalanceMean, m_dblImbalanceSlope;

  ou::ChartEntryIndicator m_ceImbalanceSmoothMean;
  ou::ChartEntryIndicator m_ceImbalanceSmoothB1;
  //ou::ChartEntryIndicator m_ceImbalanceState;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::ChartEntryMark m_cemStochastic;
  ou::ChartEntryMark m_cemZero;

  ou::tf::Quote m_quote;

  struct Stochastic {

    using pTSSWStochastic_t = std::unique_ptr<ou::tf::TSSWStochastic>;

    pTSSWStochastic_t m_pIndicatorStochastic;
    ou::ChartEntryIndicator m_ceStochastic;
    ou::ChartEntryIndicator m_ceStochasticMax;
    ou::ChartEntryIndicator m_ceStochasticMin;

    Stochastic( const std::string sIx, ou::tf::Quotes& quotes, int nPeriods, time_duration td, ou::Colour::EColour colour ) {

      m_ceStochastic.SetColour( colour );
      m_ceStochasticMax.SetColour( colour );
      m_ceStochasticMin.SetColour( colour );

      m_ceStochastic.SetName( "Stoch" + sIx );
      m_ceStochasticMax.SetName( "Stoch" + sIx + " Max" );
      m_ceStochasticMin.SetName( "Stoch" + sIx + " Min" );

      m_pIndicatorStochastic = std::make_unique<ou::tf::TSSWStochastic>(
        quotes, nPeriods, td,
        [this,sIx]( ptime dt, double k, double min, double max ){
          //std::cout << sIx << " is " << k << "," << max << "," << min << std::endl;
          m_ceStochasticMax.Append( dt, max );
          m_ceStochasticMin.Append( dt, min );
          m_ceStochastic.Append( dt, k ); // resides on top of min/max
        }
      );
    }

    Stochastic( const Stochastic& ) = delete;
    Stochastic( Stochastic&& rhs ) = delete;

    void AddToChart( ou::ChartDataView& cdv ) {
      cdv.Add( EChartSlot::Price, &m_ceStochasticMax );
      cdv.Add( EChartSlot::Price, &m_ceStochasticMin );
      cdv.Add( EChartSlot::StochInd, &m_ceStochastic );
    }

    ~Stochastic() {
      m_pIndicatorStochastic.reset();
      m_ceStochastic.Clear();
      m_ceStochasticMax.Clear();
      m_ceStochasticMin.Clear();
    }

  };

  using pStochastic_t = std::unique_ptr<Stochastic>;
  using vStochastic_t = std::vector<pStochastic_t>;
  vStochastic_t m_vStochastic;

  struct MA {

    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;
    ou::ChartEntryIndicator m_ceMA;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::EColour colour, const std::string& sName )
    : m_ema( quotes, nPeriods, tdPeriod )
    {
      m_ceMA.SetName( sName );
      m_ceMA.SetColour( colour );
    }

    MA( MA&& rhs )
    : m_ema(  std::move( rhs.m_ema ) )
    , m_ceMA( std::move( rhs.m_ceMA ) )
    {}

    void AddToView( ou::ChartDataView& cdv ) {
      cdv.Add( EChartSlot::Price, &m_ceMA );
    }

    void AddToView( ou::ChartDataView& cdv, EChartSlot slot ) {
      cdv.Add( slot, &m_ceMA );
    }

    void Update( ptime dt ) {
      m_ceMA.Append( dt, m_ema.GetEMA() );
    }

    double Latest() const { return m_ema.GetEMA(); }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  fBuildOption_t m_fBuildOption;
  fBuildPosition_t m_fBuildPosition;

  struct BuiltOption: public ou::tf::option::chain::OptionName {
    pOption_t pOption;
  };

  using chain_t = ou::tf::option::Chain<BuiltOption>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  boost::gregorian::days m_nDaysFront;
  boost::gregorian::days m_nDaysBack;

  mapChains_t::const_iterator m_iterChainFront;
  mapChains_t::const_iterator m_iterChainBack;

  struct Synthetic {
    pOption_t pBackBuy;   // for the hedge
    pOption_t pFrontSell; // for the premium
  };

  Synthetic m_synthLong;  // long call, short put - buy on bottom side
  Synthetic m_synthShort; // long put, short call - buy on top side

  pOptionChainQuery_t m_pOptionChainQuery; // need to disconnect

  struct Expiry {
    TreeItem* pTreeItem;
  };
  using mapExpiries_t = std::map<boost::gregorian::date,Expiry>; // usable chains
  mapExpiries_t m_mapExpiries; // possibly change this to a map of iterators

  using pOptionTracker_t = std::shared_ptr<OptionTracker>;
  using mapOptionTracker_t = std::map<std::string, pOptionTracker_t>; // map<option name,tracker>
  using mapStrikes_t = std::map<double,mapOptionTracker_t>; // map of options across strikes
  mapStrikes_t m_mapStrikes;

  using vOptionForQuote_t = std::vector<pOption_t>;
  vOptionForQuote_t m_vOptionForQuote;

  using query_t = ou::tf::iqfeed::OptionChainQuery;

  struct LifeCycle_Position {
    TreeItem* pTreeItem;
    pPosition_t pPosition;
    Indicators indicators;
    LifeCycle_Position( TreeItem* pTreeItem_, Indicators indicators_ )
    : pTreeItem( pTreeItem_ ), indicators( indicators_ ) {}
    LifeCycle_Position( TreeItem* pTreeItem_, pPosition_t pPosition_, Indicators indicators_ )
    : pTreeItem( pTreeItem_ ), pPosition( pPosition_ ), indicators( indicators_ ) {}
  };

  using mapLifeCycle_Position_t = std::map<std::string,LifeCycle_Position>;
  mapLifeCycle_Position_t m_mapLifeCycle_Position;

  using pTradeLifeTime_t = std::shared_ptr<TradeLifeTime>;

  struct LifeCycle_Trade {
    pTradeLifeTime_t pTradeLifeTime;
    LifeCycle_Trade( pTradeLifeTime_t pTradeLifeTime_ )
    : pTradeLifeTime( pTradeLifeTime_ )
    {}
  };

  using mapLifeCycle_Trade_t = std::map<idOrder_t,LifeCycle_Trade>;
  mapLifeCycle_Trade_t m_mapLifeCycle_Trade;

  //ou::tf::iqfeed::HistoryRequest::pHistoryRequest_t m_pHistoryRequest;
  //ou::tf::Bars m_barsHistory;
  //ou::tf::PivotSet m_setPivots;

  bool m_bRecordDepths;
  ou::tf::DepthsByOrder m_depths_byorder; // time series for persistence

  std::atomic_uint32_t m_nMarketOrdersAsk; // pull from InteractiveChart
  std::atomic_uint32_t m_nMarketOrdersBid; // pull from InteractiveChart

  ou::tf::iqfeed::l2::OrderBased m_OrderBased; // direct access
  ou::tf::iqfeed::l2::FeatureSet m_FeatureSet;
  std::unique_ptr<ou::tf::iqfeed::l2::Symbols> m_pDispatch;

  std::unique_ptr<Strategy> m_pStrategy;

  void Init();

  void BindEvents();
  void UnBindEvents();

  void OnKey( wxKeyEvent& );
  void OnChar( wxKeyEvent& );
  void OnDestroy( wxWindowDestroyEvent& );

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleOptionWatchQuote( const ou::tf::Quote& ) {}

  void HandleBarCompletionPrice( const ou::tf::Bar& );
  void HandleBarCompletionPriceUp( const ou::tf::Bar& );
  void HandleBarCompletionPriceDn( const ou::tf::Bar& );

  void OptionChainQuery( const std::string& );
  void PopulateChains( const query_t::OptionList& );
  void SelectChains();

  void UpdateSynthetic( pOption_t& pCurrent, pOption_t pSelected );

  LifeCycle_Position& Lookup_LifeCycle_Position();

  void CheckOptions();
  pOptionTracker_t AddOptionTracker( double strike, pOption_t );

  void CheckOptions_v2();

  void TrackCombo();

  void StartDepthByOrder( size_t nLevels );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(InteractiveChart, 1)