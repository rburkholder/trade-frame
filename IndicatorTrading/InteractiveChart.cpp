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
 * File:    InteractiveChart.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 13:38
 */

#include <memory>

#include <TFIndicators/TSSWStochastic.h>

#include <TFIQFeed/OptionChainQuery.h>

#include <TFOptions/Engine.h>
#include <TFOptions/GatherOptions.h>

#include "Config.h"
#include "InteractiveChart.h"

namespace {
  static const size_t nBarSeconds = 3;
  static const size_t nPeriods = 14;
}

InteractiveChart::InteractiveChart()
: WinChartView::WinChartView()
, m_bConnected( false )
, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
, m_dblSumVolume {}, m_dblSumVolumePrice {}
{
  Init();
}

InteractiveChart::InteractiveChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: WinChartView::WinChartView( parent, id, pos, size, style )
, m_bConnected( false )
, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )
, m_ceShortEntries( ou::ChartEntryShape::EShort, ou::Colour::Red )
, m_ceLongEntries( ou::ChartEntryShape::ELong, ou::Colour::Blue )
, m_ceShortFills( ou::ChartEntryShape::EFillShort, ou::Colour::Red )
, m_ceLongFills( ou::ChartEntryShape::EFillLong, ou::Colour::Blue )
, m_ceShortExits( ou::ChartEntryShape::EShortStop, ou::Colour::Red )
, m_ceLongExits( ou::ChartEntryShape::ELongStop, ou::Colour::Blue )
, m_dblSumVolume {}, m_dblSumVolumePrice {}
{
  Init();
}

bool InteractiveChart::Create(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style )
{
  bool bOk = WinChartView::Create( parent, id, pos, size, style );
  Init();
  return bOk;
}

InteractiveChart::~InteractiveChart() {
  m_bfPrice.SetOnBarComplete( nullptr );
  m_pOptionChainQuery->Disconnect();
  m_pOptionChainQuery.reset();
}

void InteractiveChart::Init() {

  m_dvChart.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_dvChart.Add( EChartSlot::Price, &m_ceTrade );
  m_dvChart.Add( EChartSlot::Price, &m_ceQuoteBid );

  //m_dvChart.Add( EChartSlot::Price, &m_ceVWAP ); // need to auto scale, then this won't distort the chart

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );

  m_dvChart.Add( EChartSlot::Price, &m_ceShortEntries );
  m_dvChart.Add( EChartSlot::Price, &m_ceLongEntries );
  m_dvChart.Add( EChartSlot::Price, &m_ceShortFills );
  m_dvChart.Add( EChartSlot::Price, &m_ceLongFills );
  m_dvChart.Add( EChartSlot::Price, &m_ceShortExits );
  m_dvChart.Add( EChartSlot::Price, &m_ceLongExits );

  //m_dvChart.Add( 1, &m_ceVolume );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolumeUp );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolumeDn );

  m_dvChart.Add( EChartSlot::Spread, &m_ceQuoteSpread );

  // need to present the marks prior to presenting the data
  m_cemStochastic.AddMark( 100, ou::Colour::Black,    "" ); // hidden by legend
  m_cemStochastic.AddMark(  80, ou::Colour::Red,   "80%" );
  m_cemStochastic.AddMark(  50, ou::Colour::Green, "50%" );
  m_cemStochastic.AddMark(  20, ou::Colour::Blue,  "20%" );
  m_cemStochastic.AddMark(   0, ou::Colour::Black,  "0%" );
  m_dvChart.Add( EChartSlot::Stochastic, &m_cemStochastic );

  m_dvChart.Add( EChartSlot::Stochastic, &m_ceStochastic1 );
  m_dvChart.Add( EChartSlot::Stochastic, &m_ceStochastic2 );
  m_dvChart.Add( EChartSlot::Stochastic, &m_ceStochastic3 );

  m_dvChart.Add( EChartSlot::Price, &m_ceStochasticMax );
  m_dvChart.Add( EChartSlot::Price, &m_ceStochasticMin );

  m_bfPrice.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPrice ) );
  m_bfPriceUp.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPriceUp ) );
  m_bfPriceDn.SetOnBarComplete( MakeDelegate( this, &InteractiveChart::HandleBarCompletionPriceDn ) );

  //m_ceVWAP.SetColour( ou::Colour::OrangeRed );
  //m_ceVWAP.SetName( "VWAP" );

  m_ceQuoteAsk.SetColour( ou::Colour::Red );
  m_ceQuoteBid.SetColour( ou::Colour::Blue );
  m_ceTrade.SetColour( ou::Colour::DarkGreen );
  m_ceQuoteSpread.SetColour( ou::Colour::Black );

  m_ceQuoteAsk.SetName( "Ask" );
  m_ceTrade.SetName( "Tick" );
  m_ceQuoteBid.SetName( "Bid" );

  m_ceQuoteSpread.SetName( "Spread" );

  m_ceVolumeUp.SetColour( ou::Colour::Green );
  m_ceVolumeUp.SetName( "Volume Up" );
  m_ceVolumeDn.SetColour( ou::Colour::Red );
  m_ceVolumeDn.SetName( "Volume Down" );

  m_ceVolume.SetName( "Volume" );

  m_ceStochastic1.SetName( "Stochastic 14x20s" );
  m_ceStochastic1.SetColour( ou::Colour::Red );
  m_ceStochastic2.SetName( "Stochastic 14x60s" );
  m_ceStochastic2.SetColour( ou::Colour::Green );
  m_ceStochastic3.SetName( "Stochastic 14x180s" );
  m_ceStochastic3.SetColour( ou::Colour::Blue );

  m_ceStochasticMax.SetName( "Stoch Max" );
  m_ceStochasticMax.SetColour( ou::Colour::ForestGreen );
  m_ceStochasticMin.SetName( "Stoch Min" );
  m_ceStochasticMin.SetColour( ou::Colour::ForestGreen );

  SetChartDataView( &m_dvChart );

  m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
    [this](){
    }
  );
  m_pOptionChainQuery->Connect(); // TODO: auto-connect instead?

}

void InteractiveChart::Connect() {

  if ( m_pPosition ) {
    if ( !m_bConnected ) {
      m_bConnected = true;
      pWatch_t pWatch = m_pPosition->GetWatch();
      pWatch->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Add( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }

}

void InteractiveChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPosition ) {
    if ( m_bConnected ) {
      m_pIndicatorStochastic1.reset();
      m_pIndicatorStochastic2.reset();
      m_pIndicatorStochastic3.reset();
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_bConnected = false;
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition( pPosition_t pPosition, const config::Options& config, fBuildOption_t&& fBuildOption ) {

  bool bConnected = m_bConnected;
  Disconnect();

  // --

  using vMAPeriods_t = std::vector<int>;
  vMAPeriods_t vMAPeriods;

  m_pPosition = pPosition;
  pWatch_t pWatch = m_pPosition->GetWatch();

  time_duration td = time_duration( 0, 0, config.nPeriodWidth );

  assert( 0 < config.nPeriodWidth );

  vMAPeriods.push_back( config.nMA1Periods );
  vMAPeriods.push_back( config.nMA2Periods );
  vMAPeriods.push_back( config.nMA3Periods );

  assert( 3 == vMAPeriods.size() );
  for ( vMAPeriods_t::value_type value: vMAPeriods ) {
    assert( 0 < value );
  }

  m_pIndicatorStochastic1 = std::make_shared<ou::tf::TSSWStochastic>(
    pWatch->GetQuotes(), config.nStochastic1Periods, td,
    [this]( ptime dt, double k, double min, double max ){
      m_ceStochastic1.Append( dt, k );
      m_ceStochasticMax.Append( dt, max );
      m_ceStochasticMin.Append( dt, min );
    }
  );
  m_pIndicatorStochastic2 = std::make_shared<ou::tf::TSSWStochastic>(
    pWatch->GetQuotes(), config.nStochastic2Periods, td,
    [this]( ptime dt, double k, double min, double max ){
      m_ceStochastic2.Append( dt, k );
    }
  );
  m_pIndicatorStochastic3 = std::make_shared<ou::tf::TSSWStochastic>(
    pWatch->GetQuotes(), config.nStochastic3Periods, td,
    [this]( ptime dt, double k, double min, double max ){
      m_ceStochastic3.Append( dt, k );
    }
  );

  m_vMA.clear();

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[0], td, ou::Colour::Gold,  "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[2], td, ou::Colour::Brown, "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_dvChart );
  }

  OptionChainQuery(
    pPosition->GetInstrument()->GetInstrumentName(
      ou::tf::Instrument::eidProvider_t::EProviderIQF),
    std::move( fBuildOption )
    );

  // --

  if ( bConnected ) {
    Connect();
  }

}

void InteractiveChart::OptionChainQuery( const std::string& sIQFeedUnderlying, fBuildOption_t&& fBuildOption ) {

  using query_t = ou::tf::iqfeed::OptionChainQuery;
  using fOption_t = ou::tf::option::fOption_t;

  switch ( m_pPosition->GetInstrument()->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      ou::tf::option::PopulateMap<mapChains_t>(
        m_mapChains,
        sIQFeedUnderlying,
        [ this, fBuildOption_=std::move( fBuildOption ) ](const std::string& sIQFeedUnderlying, fOption_t&& fOption ) {
          m_pOptionChainQuery->QueryFuturesOptionChain( // TODO: need selection of equity vs futures
            sIQFeedUnderlying,
            "pc", "", "", "4", sIQFeedUnderlying,
            [ this, &sIQFeedUnderlying, fBuildOption__=std::move( fBuildOption_ ), fOption_ = std::move( fOption ) ] ( const query_t::OptionChain& chains ){
              std::cout
                << "chain request " << chains.sKey << " has "
                << chains.vOption.size() << " options"
                << std::endl;

              for ( const query_t::vSymbol_t::value_type& sSymbol: chains.vOption ) {
                fBuildOption__(
                  sSymbol,
                  [this, fOption_ ]( pOption_t pOption ){
                    fOption_( pOption ); // places into chain
                  });
              }
            });
        }
      );
      break;
    case ou::tf::InstrumentType::Stock:
      ou::tf::option::PopulateMap<mapChains_t>(
        m_mapChains,
        sIQFeedUnderlying,
        [ this, fBuildOption_=std::move( fBuildOption ) ](const std::string& sIQFeedUnderlying, fOption_t&& fOption ) {
          m_pOptionChainQuery->QueryEquityOptionChain(
            sIQFeedUnderlying,
            "pc", "", "4", "0", "", "", sIQFeedUnderlying, // four months, all strikes
            [ this, &sIQFeedUnderlying, fBuildOption__=std::move( fBuildOption_ ), fOption_ = std::move( fOption ) ] ( const query_t::OptionChain& chains ){
              std::cout
                << "chain request " << chains.sKey << " has "
                << chains.vOption.size() << " options"
                << std::endl;

              for ( const query_t::vSymbol_t::value_type& sSymbol: chains.vOption ) {
                fBuildOption__(
                  sSymbol,
                  [this, fOption_]( pOption_t pOption ){
                    fOption_( pOption ); // places into chain
                  });
              }
            });
        }
      );
      break;
    default:
      assert( false );
      break;
  }
}

void InteractiveChart::ProcessChains() {
  for ( const mapChains_t::value_type& vt: m_mapChains ) {
    size_t nStrikes {};
    vt.second.Strikes(
      [&nStrikes]( double strike, const chain_t::strike_t& options ){
        if ( ( 0 != options.call.sIQFeedSymbolName.size() ) && ( 0 != options.put.sIQFeedSymbolName.size() ) ) {
          nStrikes++;
        }
    } );
    if ( 50 < nStrikes ) {
      m_vChains.emplace_back( vt.first );
    }
  }
}

void InteractiveChart::HandleQuote( const ou::tf::Quote& quote ) {

  if ( !quote.IsValid() ) {
    return;
  }

  ptime dt( quote.DateTime() );

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

  m_quote = quote;

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.Update( dt );
  }

}

void InteractiveChart::HandleTrade( const ou::tf::Trade& trade ) {

  ptime dt( trade.DateTime() );
  ou::tf::Trade::price_t price = trade.Price();

  double volume = trade.Volume();
  m_dblSumVolume += volume;
  m_dblSumVolumePrice += volume * trade.Price();

  m_ceTrade.Append( dt, price );

  double mid = m_quote.Midpoint();

  m_bfPrice.Add( dt, price, trade.Volume() );
  if ( price >= mid ) {
    m_bfPriceUp.Add( dt, price, trade.Volume() );
  }
  else {
    m_bfPriceDn.Add( dt, price, -trade.Volume() );
  }
}

void InteractiveChart::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  //m_ceVolume.Append( bar );
  //m_ceVWAP.Append( bar.DateTime(), m_dblSumVolumePrice / m_dblSumVolume );
}

void InteractiveChart::HandleBarCompletionPriceUp( const ou::tf::Bar& bar ) {
  m_ceVolumeUp.Append( bar );
}

void InteractiveChart::HandleBarCompletionPriceDn( const ou::tf::Bar& bar ) {
  m_ceVolumeDn.Append( bar );
}

void InteractiveChart::SaveWatch( const std::string& sPrefix ) {
  m_pPosition->GetWatch()->SaveSeries( sPrefix );
}