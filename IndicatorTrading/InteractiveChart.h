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

#include <TFTrading/Order.h>
#include <TFTrading/Position.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>

#include <TFVuTrading/WinChartView.h>

#include "Indicators.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed
  class OptionChainQuery;
} // namespace iqfeed
namespace option {
  class Engine;
} // namespace option
  struct PanelOrderButtons_Order;
  struct PanelOrderButtons_MarketData;
} // namespace tf
} // namespace ou

namespace config {
  class Options;
}

class OptionTracker;
class TradeLifeTime;

class InteractiveChart:
  public ou::tf::WinChartView
{
public:

  using idOrder_t = ou::tf::Order::idOrder_t;

  using pOrder_t = ou::tf::Order::pOrder_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pOption_t = ou::tf::option::Option::pOption_t;
  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  using fOption_t = std::function<void(pOption_t)>;
  using fBuildOption_t = std::function<void(const std::string&,fOption_t&&)>;

  using fBuildPosition_t = std::function<pPosition_t(pInstrument_t)>;

  using fUpdateLifeCycle_t = std::function<void(const std::string&)>;
  using fDeleteLifeCycle_t = std::function<void()>;

  struct LifeCycleFunctions {
    fUpdateLifeCycle_t fUpdateLifeCycle;
    fDeleteLifeCycle_t fDeleteLifeCycle;
    LifeCycleFunctions( fUpdateLifeCycle_t&& fUpdateLifeCycle_,fDeleteLifeCycle_t&& fDeleteLifeCycle_ )
    : fUpdateLifeCycle( std::move( fUpdateLifeCycle_ ) ), fDeleteLifeCycle( std::move( fDeleteLifeCycle_ ) )
    {}
  };

  using fOnClick_t = std::function<void()>;
  using fAddLifeCycleToTree_t = std::function<LifeCycleFunctions(idOrder_t)>;
  using fAddOptionToTree_t = std::function<void( const std::string&, fOnClick_t&& )>;
  using fAddExpiryToTree_t = std::function<fAddOptionToTree_t( const std::string& )>;

  struct SubTreesForUnderlying {
    fAddLifeCycleToTree_t fAddLifeCycleToTree;
    fAddExpiryToTree_t fAddExpiryToTree;
    SubTreesForUnderlying( fAddLifeCycleToTree_t&& fAddLifeCycleToTree_, fAddExpiryToTree_t&& fAddExpiryToTree_)
    : fAddLifeCycleToTree( std::move( fAddLifeCycleToTree_ ) ), fAddExpiryToTree( std::move( fAddExpiryToTree_ ) )
    {}
  };

  using fAddUnderlying_t = std::function<SubTreesForUnderlying(const std::string&, fOnClick_t&&)>; // primary start for tree hierarchy

  using pOptionChainQuery_t = std::shared_ptr<ou::tf::iqfeed::OptionChainQuery>;

  InteractiveChart();
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

  void SetPosition(
     pPosition_t
   , const config::Options&
   , pOptionChainQuery_t
   , fBuildOption_t&&
   , fBuildPosition_t&&
   , fAddUnderlying_t&&
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

  void OrderCancel( idOrder_t );
  void EmitOrderStatus( idOrder_t );
  void DeleteLifeCycle( idOrder_t );

  void EmitStatus();

  void Connect();
  void Disconnect();

protected:
private:

  enum EChartSlot { Price, Volume, StochInd, Sentiment, PL, Spread }; // IndMA = moving averate indicator

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  bool m_bConnected;
  bool m_bOptionsReady;

  ou::ChartDataView m_dvChart; // the data

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

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::ChartEntryMark m_cemStochastic;

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
          m_ceStochastic.Append( dt, k );
          m_ceStochasticMax.Append( dt, max );
          m_ceStochasticMin.Append( dt, min );
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
  fAddLifeCycleToTree_t m_fAddLifeCycleToTree;
  fAddExpiryToTree_t m_fAddExpiryToTree;

  struct BuiltOption: public ou::tf::option::chain::OptionName {
    pOption_t pOption;
  };

  using chain_t = ou::tf::option::Chain<BuiltOption>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  pOptionChainQuery_t m_pOptionChainQuery; // need to disconnect

  struct Expiry {
    fAddOptionToTree_t fAddOptionToTree;
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

  struct LifeCycleComponents {
    pPosition_t pPosition;
    Indicators indicators;
    LifeCycleComponents( Indicators indicators_ )
    : indicators( indicators_ ) {}
    LifeCycleComponents( pPosition_t pPosition_, Indicators indicators_ )
    : pPosition( pPosition_ ), indicators( indicators_ ) {}
  };

  using mapLifeCycleComponents_t = std::map<std::string,LifeCycleComponents>;
  mapLifeCycleComponents_t m_mapLifeCycleComponents;

  using pTradeLifeTime_t = std::shared_ptr<TradeLifeTime>;

  struct LifeCycle {
    pTradeLifeTime_t pTradeLifeTime;
    fDeleteLifeCycle_t fDeleteLifeCycle;
    LifeCycle( pTradeLifeTime_t pTradeLifeTime_ )
    : pTradeLifeTime( pTradeLifeTime_ )
    {}
  };

  using mapLifeCycle_t = std::map<idOrder_t,LifeCycle>;
  mapLifeCycle_t m_mapLifeCycle;

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

  LifeCycleComponents& LookupLifeCycleComponents();

  void CheckOptions();
  pOptionTracker_t AddOptionTracker( double strike, pOption_t );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(InteractiveChart, 1)