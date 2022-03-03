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
, bOptionsReady( false )
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
, bOptionsReady( false )
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
  m_dvChart.Add( EChartSlot::StochInd, &m_cemStochastic );

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

  SetChartDataView( &m_dvChart );

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
      pWatch_t pWatch = m_pPosition->GetWatch();
      m_bConnected = false;
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition(
  pPosition_t pPosition,
  const config::Options& config,
  pOptionChainQuery_t pOptionChainQuery,
  fBuildOption_t&& fBuildOption
) {

  bool bConnected = m_bConnected;
  Disconnect();

  // --

  m_fBuildOption = std::move( fBuildOption );
  m_pOptionChainQuery = pOptionChainQuery;

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

  m_vStochastic.clear();

  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "1", pWatch->GetQuotes(), config.nStochastic1Periods, td, ou::Colour::DeepSkyBlue ) );
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "2", pWatch->GetQuotes(), config.nStochastic2Periods, td, ou::Colour::DodgerBlue ) );  // is dark: MediumSlateBlue; MediumAquamarine is greenish; MediumPurple is dark; Purple is dark
  m_vStochastic.emplace_back( std::make_unique<Stochastic>( "3", pWatch->GetQuotes(), config.nStochastic3Periods, td, ou::Colour::MediumSlateBlue ) ); // no MediumTurquoise, maybe Indigo

  for ( vStochastic_t::value_type& vt: m_vStochastic ) {
    vt->AddToChart( m_dvChart );
  }

  m_vMA.clear();

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[0], td, ou::Colour::Gold,  "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[2], td, ou::Colour::Brown, "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_dvChart );
  }

  OptionChainQuery(
    pPosition->GetInstrument()->GetInstrumentName(
      ou::tf::Instrument::eidProvider_t::EProviderIQF)
    );

  // --

  if ( bConnected ) {
    Connect();
  }

}

void InteractiveChart::OptionChainQuery( const std::string& sIQFeedUnderlying ) {

  namespace ph = std::placeholders;

  using fOption_t = ou::tf::option::fOption_t;

  switch ( m_pPosition->GetInstrument()->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      m_pOptionChainQuery->QueryFuturesOptionChain(
        sIQFeedUnderlying,
        "cp", "", "234", "1",
        std::bind( &InteractiveChart::PopulateChains, this, ph::_1 )
        );
      break;
    case ou::tf::InstrumentType::Stock:
      m_pOptionChainQuery->QueryEquityOptionChain(
        sIQFeedUnderlying,
        "cp", "", "1", "2", "3", "3",
        std::bind( &InteractiveChart::PopulateChains, this, ph::_1 )
        );
      break;
    default:
      assert( false );
      break;
  }
}

void InteractiveChart::PopulateChains( const query_t::OptionList& list ) {
  std::cout
    << "chain request " << list.sUnderlying << " has "
    << list.vSymbol.size() << " options"
    << std::endl;

  for ( const query_t::vSymbol_t::value_type& sSymbol: list.vSymbol ) {
    m_fBuildOption(
      sSymbol,
      [this]( pOption_t pOption ){

        mapChains_t::iterator iterChain = ou::tf::option::GetChain( m_mapChains, pOption );
        BuiltOption* pBuiltOption = ou::tf::option::UpdateOption<chain_t,BuiltOption>( iterChain->second, pOption );
        assert( pBuiltOption );

        pBuiltOption->pOption = pOption;

      });
  }
  std::cout << " .. option chains built." << std::endl;
}

void InteractiveChart::ProcessChains() {

  if ( 0 == m_vExpiries.size() ) {

    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      size_t nStrikes {};
      vt.second.Strikes(
        [&nStrikes]( double strike, const chain_t::strike_t& options ){
          if ( ( 0 != options.call.sIQFeedSymbolName.size() ) && ( 0 != options.put.sIQFeedSymbolName.size() ) ) {
            nStrikes++;
          }
      } );

      std::cout << "chain " << vt.first << " with " << nStrikes << " matching call/puts" << std::endl;
      if ( 10 < nStrikes ) {
        std::cout << "chain " << vt.first << " marked" << std::endl;
        m_vExpiries.emplace_back( vt.first );
      }
    }

    bOptionsReady = true;
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

void InteractiveChart::CheckOptions() {

  if ( bOptionsReady ) {

    double mid( m_quote.Midpoint() );
    for ( const vExpiries_t::value_type& expiry : m_vExpiries ) {

      mapChains_t::iterator iterChains = m_mapChains.find( expiry );
      assert( m_mapChains.end() != iterChains );
      chain_t& chain( iterChains->second );

      double strike;
      pOption_t pOption;

      // call
      strike = chain.Call_Itm( mid );
      pOption = chain.GetStrike( strike ).call.pOption;
      AddOptionTracker( strike, pOption );

      strike = chain.Call_Atm( mid );
      pOption = chain.GetStrike( strike ).call.pOption;
      AddOptionTracker( strike, pOption );

      strike = chain.Call_Otm( mid );
      pOption = chain.GetStrike( strike ).call.pOption;
      AddOptionTracker( strike, pOption );

      // put
      strike = chain.Put_Itm( mid );
      pOption = chain.GetStrike( strike ).put.pOption;
      AddOptionTracker( strike, pOption );

      strike = chain.Put_Atm( mid );
      pOption = chain.GetStrike( strike ).put.pOption;
      AddOptionTracker( strike, pOption );

      strike = chain.Put_Otm( mid );
      pOption = chain.GetStrike( strike ).put.pOption;
      AddOptionTracker( strike, pOption );

    }
  }
}

void InteractiveChart::AddOptionTracker( double strike, pOption_t pOption ) {

  const std::string& sOptionName( pOption->GetInstrumentName() );

  mapStrikes_t::iterator iterStrike;
  iterStrike = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iterStrike ) {
    std::pair<mapStrikes_t::iterator,bool> pair = m_mapStrikes.emplace( mapStrikes_t::value_type( strike, mapOptionTracker_t() ) );
    assert( pair.second );
    iterStrike = std::move( pair.first );
  };

  mapOptionTracker_t& mapOptionTracker( iterStrike->second );

  mapOptionTracker_t::iterator iterOptionTracker = mapOptionTracker.find( sOptionName );
  if ( mapOptionTracker.end() == iterOptionTracker ) {
    mapOptionTracker.emplace( mapOptionTracker_t::value_type( sOptionName, OptionTracker( pOption, m_ceLongEntries, m_ceShortEntries ) ) );
  }
}

void InteractiveChart::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {
  //m_ceVolume.Append( bar );
  //m_ceVWAP.Append( bar.DateTime(), m_dblSumVolumePrice / m_dblSumVolume );
  CheckOptions();
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