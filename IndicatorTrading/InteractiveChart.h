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

#include <TFTimeSeries/BarFactory.h>

#include <TFTrading/Position.h>

#include <TFOptions/Chain.h>
#include <TFOptions/Chains.h>
#include <TFOptions/Option.h>

#include <TFVuTrading/WinChartView.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
  class TSSWStochastic;
namespace iqfeed { // IQFeed
  class OptionChainQuery;
} // namespace iqfeed
namespace option {
  class Engine;
} // namespace option
} // namespace tf
} // namespace ou

namespace config {
  class Options;
}

class InteractiveChart:
  public ou::tf::WinChartView
{
public:

  using pOption_t = ou::tf::option::Option::pOption_t;
  //using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  using fOption_t = std::function<void(pOption_t)>;
  using fBuildOption_t = std::function<void(const std::string&,fOption_t&&)>;

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

  void SetPosition( pPosition_t, const config::Options&, fBuildOption_t&& );

  void EmitChainInfo() const {
    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      std::cout << "chain: " << vt.first << " has " << vt.second.Size() << " entries" << std::endl;
      vt.second.EmitValues();
    }
  }

  void SaveWatch( const std::string& );

  void Connect();
  void Disconnect();

protected:
private:

  enum EChartSlot { Price, Volume, Spread, Stochastic, PL }; // IndMA = moving averate indicator

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  bool m_bConnected;

  ou::ChartDataView m_dvChart; // the data

  pPosition_t m_pPosition;

  double m_dblSumVolumePrice; // part of vwap
  double m_dblSumVolume; // part of vwap

  ou::tf::BarFactory m_bfPrice;
  ou::tf::BarFactory m_bfPriceUp;
  ou::tf::BarFactory m_bfPriceDn;

  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryBars m_cePriceBars;

  //ou::ChartEntryIndicator m_ceVWAP;

  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryVolume m_ceVolumeUp;
  ou::ChartEntryVolume m_ceVolumeDn;

  ou::ChartEntryIndicator m_ceQuoteAsk;
  ou::ChartEntryIndicator m_ceQuoteBid;
  ou::ChartEntryIndicator m_ceQuoteSpread;

  ou::ChartEntryShape m_ceShortEntries;
  ou::ChartEntryShape m_ceLongEntries;
  ou::ChartEntryShape m_ceShortFills;
  ou::ChartEntryShape m_ceLongFills;
  ou::ChartEntryShape m_ceShortExits;
  ou::ChartEntryShape m_ceLongExits;

  ou::ChartEntryMark m_cemStochastic;

  ou::ChartEntryIndicator m_ceStochastic1;
  ou::ChartEntryIndicator m_ceStochastic2;
  ou::ChartEntryIndicator m_ceStochastic3;

  ou::tf::Quote m_quote;

  using pTSSWStochastic_t = std::shared_ptr<ou::tf::TSSWStochastic>;
  pTSSWStochastic_t m_pIndicatorStochastic1;
  pTSSWStochastic_t m_pIndicatorStochastic2;
  pTSSWStochastic_t m_pIndicatorStochastic3;

  ou::ChartEntryIndicator m_ceStochasticMax;
  ou::ChartEntryIndicator m_ceStochasticMin;

  struct MA {

    ou::tf::hf::TSEMA<ou::tf::Quote> m_ema;
    ou::ChartEntryIndicator m_ceMA;

    MA( ou::tf::Quotes& quotes, size_t nPeriods, time_duration tdPeriod, ou::Colour::enumColour colour, const std::string& sName )
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

    void Update( ptime dt ) {
      m_ceMA.Append( dt, m_ema.GetEMA() );
    }

    double Latest() const { return m_ema.GetEMA(); }
  };

  using vMA_t = std::vector<MA>;
  vMA_t m_vMA;

  using chain_t = ou::tf::option::Chain<ou::tf::option::chain::OptionName>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  mapChains_t m_mapChains;

  std::unique_ptr<ou::tf::iqfeed::OptionChainQuery> m_pOptionChainQuery; // need to disconnect

  void Init();

  void HandleQuote( const ou::tf::Quote& );
  void HandleTrade( const ou::tf::Trade& );

  void HandleBarCompletionPrice( const ou::tf::Bar& );
  void HandleBarCompletionPriceUp( const ou::tf::Bar& );
  void HandleBarCompletionPriceDn( const ou::tf::Bar& );

  void OptionChainQuery( const std::string&, fBuildOption_t&& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

BOOST_CLASS_VERSION(InteractiveChart, 1)