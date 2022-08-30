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

 // 2022/03/09 can use stochastic to feed a zigzag indicator, which can be used to
 //   indicate support/resistance as the day progresses
 // 2022/03/21 perform a roll down on put, roll up on call?

 // 2022/08/20 use pivots to determine edge points for option entry

#include <memory>

#include <boost/format.hpp>

//#include <TFOptions/Engine.h>
#include <TFOptions/GatherOptions.h>

#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/PanelOrderButtons_structs.h>

#include "Config.h"
#include "Strategy.hpp"
#include "TradeLifeTime.h"
#include "OptionTracker.hpp"
#include "InteractiveChart.h"

namespace {
  static const size_t nBarSeconds = 3;
  static const size_t nPeriods = 14;
}

InteractiveChart::InteractiveChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: ou::tf::WinChartView( parent, id, pos, size, style )
, m_bConnected( false )
, m_bOptionsReady( false )
, m_bTriggerFeatureSetDump( false )

, m_dblSumVolume {}
, m_dblSumVolumePrice {}

, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )

, m_dblImbalanceMean {}, m_dblImbalanceSlope {}

, m_ceBuySubmit( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceBuyFill( ou::ChartEntryShape::EShape::FillLong, ou::Colour::Blue )
, m_ceSellSubmit( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
, m_ceSellFill( ou::ChartEntryShape::EShape::FillShort, ou::Colour::Red )
, m_ceCancelled( ou::ChartEntryShape::EShape::ShortStop, ou::Colour::Orange )

, m_ceBullCall( ou::ChartEntryShape::EShape::Long, ou::Colour::Blue )
, m_ceBullPut( ou::ChartEntryShape::EShape::Long, ou::Colour::LightBlue )
, m_ceBearCall( ou::ChartEntryShape::EShape::Short, ou::Colour::Pink )
, m_ceBearPut( ou::ChartEntryShape::EShape::Short, ou::Colour::Red )
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
}

void InteractiveChart::Init() {

  m_dvChart.Add( EChartSlot::Price, &m_ceQuoteAsk );
  m_dvChart.Add( EChartSlot::Price, &m_ceTrade );
  m_dvChart.Add( EChartSlot::Price, &m_ceQuoteBid );

  m_dvChart.Add( EChartSlot::Price, &m_cePriceBars );

  m_dvChart.Add( EChartSlot::Price, &m_ceBuySubmit );
  m_dvChart.Add( EChartSlot::Price, &m_ceBuyFill );
  m_dvChart.Add( EChartSlot::Price, &m_ceSellSubmit );
  m_dvChart.Add( EChartSlot::Price, &m_ceSellFill );
  m_dvChart.Add( EChartSlot::Price, &m_ceCancelled );

  //m_dvChart.Add( 1, &m_ceVolume );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolumeUp );
  m_dvChart.Add( EChartSlot::Volume, &m_ceVolumeDn );

  m_cemZero.AddMark( 0, ou::Colour::Black,  "0" );
  m_dvChart.Add( EChartSlot::ImbalanceMean, &m_cemZero );
  //m_dvChart.Add( EChartSlot::ImbalanceB1, &m_cemZero );
  //m_dvChart.Add( EChartSlot::ImbalanceState, &m_cemZero );

  m_dvChart.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceRawMean );
  //m_dvChart.Add( EChartSlot::ImbalanceB1, &m_ceImbalanceRawB1 );

  m_dvChart.Add( EChartSlot::ImbalanceMean, &m_ceImbalanceSmoothMean );
  //m_dvChart.Add( EChartSlot::ImbalanceB1, &m_ceImbalanceSmoothB1 );
  //m_dvChart.Add( EChartSlot::ImbalanceState, &m_ceImbalanceState );

  //m_dvChart.Add( EChartSlot::Sentiment, &m_ceBullCall );
  //m_dvChart.Add( EChartSlot::Sentiment, &m_ceBullPut );
  //m_dvChart.Add( EChartSlot::Sentiment, &m_ceBearCall );
  //m_dvChart.Add( EChartSlot::Sentiment, &m_ceBearPut );

  //m_dvChart.Add( EChartSlot::Sentiment, &m_ceVWAP ); // need to auto scale, then this won't distort the chart

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

  m_ceVWAP.SetColour( ou::Colour::OrangeRed );
  m_ceVWAP.SetName( "vwap" );

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

  m_ceImbalanceRawMean.SetName( "imbalance mean" );
  m_ceImbalanceRawMean.SetColour( ou::Colour::LightGreen );
  m_ceImbalanceSmoothMean.SetColour( ou::Colour::DarkGreen );

  m_ceImbalanceRawB1.SetName( "imbalance slope" );
  m_ceImbalanceRawB1.SetColour( ou::Colour::LightGreen );
  m_ceImbalanceSmoothB1.SetColour( ou::Colour::DarkGreen );

  //m_ceImbalanceState.SetName( "imbalance state" );
  //m_ceImbalanceState.SetColour( ou::Colour::Green );

  m_ceVolume.SetName( "Volume" );

  m_ceProfitLoss.SetName( "P/L" );
  m_dvChart.Add( EChartSlot::PL, &m_ceProfitLoss );

  BindEvents();

  SetChartDataView( &m_dvChart );

}

void InteractiveChart::Connect() {
  if ( m_pPositionUnderlying ) {
    if ( !m_bConnected ) {
      m_bConnected = true;
      pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
      pWatch->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Add( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
      assert( m_pDispatch );
      m_pDispatch->Connect();
    }
  }
}

void InteractiveChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPositionUnderlying ) {
    if ( m_bConnected ) {
      pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
      m_bConnected = false;
      assert( m_pDispatch );
      m_pDispatch->Disconnect();
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition(
  pPosition_t pPosition
, const config::Options& config
, pOptionChainQuery_t pOptionChainQuery
, fBuildOption_t&& fBuildOption
, fBuildPosition_t&& fBuildPosition
, fClick_t&& fClickLeft
, fClick_t&& fClickRight
, fUpdateMarketData_t&& fUpdateMarketData
, fUpdatePosition_t&& fUpdatePosition
, TreeItem* pTreeItemParent
, ou::ChartEntryMark& cemReferenceLevels
) {

  bool bWasConnected = m_bConnected;
  Disconnect();

  // --

  m_dvChart.Add( EChartSlot::Price, &cemReferenceLevels );

  m_fBuildOption = std::move( fBuildOption );
  m_fBuildPosition = std::move( fBuildPosition );

  m_fClickLeft = std::move( fClickLeft );
  m_fClickRight = std::move( fClickRight );
  m_fUpdateMarketData = std::move( fUpdateMarketData );
  m_fUpdatePosition = std::move( fUpdatePosition );

  m_pOptionChainQuery = pOptionChainQuery;

  m_pPositionUnderlying = pPosition;
  m_pActiveInstrument = m_pPositionUnderlying->GetInstrument();
  pWatch_t pWatch = m_pPositionUnderlying->GetWatch();

  m_pTreeItemUnderlying = pTreeItemParent->AppendChild(
    pWatch->GetInstrumentName(),
    [ this, pInstrument=m_pActiveInstrument ](TreeItem* pTreeItem){ // fOnClick_t
      SetChartDataView( &m_dvChart ); // primary underlying chart
      m_pActiveInstrument = pInstrument;
    },
    [this]( TreeItem* pTreeItem ){ // fOnBuildPopUp_t
      pTreeItem->NewMenu();
      pTreeItem->AppendMenuItem(
        "Cancel Orders",
        [this]( TreeItem* pTreeItem ){
          CancelOrders();
        }
      );
      pTreeItem->AppendMenuItem(
        "Status",
        [this]( TreeItem* pTreeItem ){
          EmitStatus();
        }
      );
    }
    );

  m_mapLifeCycle_Position.emplace(
    m_pActiveInstrument->GetInstrumentName(),
    LifeCycle_Position(
      m_pTreeItemUnderlying,
      m_pPositionUnderlying,
      Indicators( m_ceBuySubmit, m_ceBuyFill, m_ceSellSubmit, m_ceSellFill, m_ceCancelled ) )
    );

  StartDepthByOrder( config.nL2Levels );

  assert( 0 < config.nPeriodWidth );
  time_duration td = time_duration( 0, 0, config.nPeriodWidth );

  using vMAPeriods_t = std::vector<int>;
  vMAPeriods_t vMAPeriods;

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

  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[0], td, ou::Colour::Brown, "ma1" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[1], td, ou::Colour::Coral, "ma2" ) );
  m_vMA.emplace_back( MA( pWatch->GetQuotes(), vMAPeriods[2], td, ou::Colour::Gold,  "ma3" ) );

  for ( vMA_t::value_type& ma: m_vMA ) {
    ma.AddToView( m_dvChart );
  }
  //m_vMA[ 0 ].AddToView( m_dvChart, EChartSlot::Sentiment );
  // m_vMA[ 0 ].AddToView( m_dvChart, EChartSlot::StochInd ); // need to mormailze this first

  OptionChainQuery(
    pPosition->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF )
    );

  m_pStrategy = std::make_unique<Strategy>( m_pPositionUnderlying );

  // --

  if ( bWasConnected ) {
    Connect();
  }

}

void InteractiveChart::OptionChainQuery( const std::string& sIQFeedUnderlying ) {

  namespace ph = std::placeholders;

  switch ( m_pPositionUnderlying->GetInstrument()->GetInstrumentType() ) {
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
        "cp", "", "2", "0", "", "",
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

// started from menu after option chains have been loaded
void InteractiveChart::ProcessChains() {

  if ( 0 == m_mapExpiries.size() ) {

    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      size_t nTotalStrikes {};
      size_t nCompleteStrikes {};
      vt.second.Strikes(
        [&nTotalStrikes,&nCompleteStrikes]( double strike, const chain_t::strike_t& options ){
          ++nTotalStrikes;
          if ( ( 0 != options.call.sIQFeedSymbolName.size() ) && ( 0 != options.put.sIQFeedSymbolName.size() ) ) {
            ++nCompleteStrikes;
          }
      } );

      std::cout
        << "chain " << vt.first
        << " with " << nCompleteStrikes << " matching call/puts"
        << " out of " << nTotalStrikes << " total"
        //<< std::endl
        ;
      if ( 20 < nCompleteStrikes ) {
        std::cout << ", marked";
        auto [iter, result ] = m_mapExpiries.emplace( std::make_pair( vt.first, Expiry() ) );
        assert( result );

        // place to put the expiries on the tree
        iter->second.pTreeItem = m_pTreeItemUnderlying->AppendChild(
          ou::tf::Instrument::BuildDate( vt.first ),
          [this]( TreeItem* pTreeItem ){
            // expiry label does nothing
          }
        );
      }
      std::cout << std::endl;
    }

    m_bOptionsReady = true;
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

  const double volume = (double)trade.Volume();
  m_dblSumVolume += volume;
  m_dblSumVolumePrice += volume * trade.Price();

  m_ceTrade.Append( dt, price );

  const double mid = m_quote.Midpoint();

  m_bfPrice.Add( dt, price, trade.Volume() );
  if ( price >= mid ) {
    m_bfPriceUp.Add( dt, price, trade.Volume() );
    m_nMarketOrdersAsk++;
  }
  else {
    m_bfPriceDn.Add( dt, price, -trade.Volume() );
    m_nMarketOrdersBid++;
  }

}

void InteractiveChart::CheckOptions() {

  if ( m_bOptionsReady ) {

    const double mid( m_quote.Midpoint() );
    for ( const mapExpiries_t::value_type& vt : m_mapExpiries ) {

      mapChains_t::iterator iterChains = m_mapChains.find( vt.first );
      assert( m_mapChains.end() != iterChains );
      chain_t& chain( iterChains->second );

      double strike;
      pOption_t pOption;
      pOptionTracker_t pOptionTracker;

      // TODO: set colors for otm and itm

      // call
      strike = chain.Call_Itm( mid );
      pOption = chain.GetStrike( strike ).call.pOption;
      if ( pOption ) { // iqfeed isn't filling strikes properly
        pOptionTracker = AddOptionTracker( strike, pOption );
        if ( pOptionTracker ) {
          std::string sIQFeedName = pOption->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF );
          TreeItem* pTreeItemOption = vt.second.pTreeItem->AppendChild(
            pOption->GetInstrumentName(),
            [this,pOptionTracker](TreeItem* pTreeItem){
              SetChartDataView( pOptionTracker->GetDataViewChart() );
              m_pActiveInstrument = pOptionTracker->GetOption()->GetInstrument();
            },
            [this,sIQFeedName_=std::move(sIQFeedName)]( TreeItem* pTreeItem){
              pTreeItem->NewMenu();
              pTreeItem->AppendMenuItem(
                sIQFeedName_,
                [this,&sIQFeedName_]( TreeItem* pTreeItem ){
                  std::cout << "IQFeed Name: " << sIQFeedName_ << std::endl;
                }
              );
            }
            );
          m_mapLifeCycle_Position.emplace(
            pOption->GetInstrumentName(),
            LifeCycle_Position( pTreeItemOption, Indicators( pOptionTracker->GetIndicators() ))
            );
        }
      }

      strike = chain.Call_Otm( mid );
      pOption = chain.GetStrike( strike ).call.pOption;
      if ( pOption ) {
        pOptionTracker = AddOptionTracker( strike, pOption );
        if ( pOptionTracker ) {
          std::string sIQFeedName = pOption->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF );
          TreeItem* pTreeItemOption = vt.second.pTreeItem->AppendChild(
            pOption->GetInstrumentName(),
            [this,pOptionTracker](TreeItem* pTreeItem){
              SetChartDataView( pOptionTracker->GetDataViewChart() );
              m_pActiveInstrument = pOptionTracker->GetOption()->GetInstrument();
            },
            [this,sIQFeedName_=std::move(sIQFeedName)]( TreeItem* pTreeItem){
              pTreeItem->NewMenu();
              pTreeItem->AppendMenuItem(
                sIQFeedName_,
                [this,&sIQFeedName_]( TreeItem* pTreeItem ){
                  std::cout << "IQFeed Name: " << sIQFeedName_ << std::endl;
                }
              );
            }
            );
          m_mapLifeCycle_Position.emplace(
            pOption->GetInstrumentName(),
            LifeCycle_Position( pTreeItemOption, Indicators( pOptionTracker->GetIndicators() ))
            );
        }
      }

      // put
      strike = chain.Put_Itm( mid );
      pOption = chain.GetStrike( strike ).put.pOption;
      if ( pOption ) {
        pOptionTracker = AddOptionTracker( strike, pOption );
        if ( pOptionTracker ) {
          std::string sIQFeedName = pOption->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF );
          TreeItem* pTreeItemOption = vt.second.pTreeItem->AppendChild(
            pOption->GetInstrumentName(),
            [this,pOptionTracker](TreeItem* pTreeItem){
              SetChartDataView( pOptionTracker->GetDataViewChart() );
              m_pActiveInstrument = pOptionTracker->GetOption()->GetInstrument();
            },
            [this,sIQFeedName_=std::move(sIQFeedName)]( TreeItem* pTreeItem){
              pTreeItem->NewMenu();
              pTreeItem->AppendMenuItem(
                sIQFeedName_,
                [this,&sIQFeedName_]( TreeItem* pTreeItem ){
                  std::cout << "IQFeed Name: " << sIQFeedName_ << std::endl;
                }
              );
            }
            );
          m_mapLifeCycle_Position.emplace(
            pOption->GetInstrumentName(),
            LifeCycle_Position( pTreeItemOption, Indicators( pOptionTracker->GetIndicators() ))
            );
        }
      }

      strike = chain.Put_Otm( mid );
      pOption = chain.GetStrike( strike ).put.pOption;
      if ( pOption ) {
        pOptionTracker = AddOptionTracker( strike, pOption );
        if ( pOptionTracker ) {
          std::string sIQFeedName = pOption->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF );
          TreeItem* pTreeItemOption = vt.second.pTreeItem->AppendChild(
            pOption->GetInstrumentName(),
            [this,pOptionTracker](TreeItem* pTreeItem){
              SetChartDataView( pOptionTracker->GetDataViewChart() );
              m_pActiveInstrument = pOptionTracker->GetOption()->GetInstrument();
            },
            [this,sIQFeedName_=std::move(sIQFeedName)]( TreeItem* pTreeItem){
              pTreeItem->NewMenu();
              pTreeItem->AppendMenuItem(
                sIQFeedName_,
                [this,&sIQFeedName_]( TreeItem* pTreeItem ){
                  std::cout << "IQFeed Name: " << sIQFeedName_ << std::endl;
                }
              );
            }
            );
          m_mapLifeCycle_Position.emplace(
            pOption->GetInstrumentName(),
            LifeCycle_Position( pTreeItemOption, Indicators( pOptionTracker->GetIndicators() ))
            );
        }
      }

    }
  }
}

void InteractiveChart::TrackCombo() {

  if ( m_bOptionsReady ) {

    double mid( m_quote.Midpoint() );
    for ( const mapExpiries_t::value_type& vt : m_mapExpiries ) {

      mapChains_t::iterator iterChains = m_mapChains.find( vt.first );
      assert( m_mapChains.end() != iterChains );
      chain_t& chain( iterChains->second );

      double strike;
      pOption_t pOption;
      pOptionTracker_t pOptionTracker;
    }
  }
}

// adds to mapOptionTracker if it doesn't already exist, and starts watch
InteractiveChart::pOptionTracker_t InteractiveChart::AddOptionTracker( double strike, pOption_t pOption ) {

  pOptionTracker_t pOptionTracker;

  const std::string& sOptionName( pOption->GetInstrumentName() );

  mapStrikes_t::iterator iterStrike;
  iterStrike = m_mapStrikes.find( strike );
  if ( m_mapStrikes.end() == iterStrike ) {
    std::pair<mapStrikes_t::iterator,bool> pair = m_mapStrikes.emplace( mapStrikes_t::value_type( strike, mapOptionTracker_t() ) );
    assert( pair.second );
    iterStrike = pair.first;
  };

  mapOptionTracker_t& mapOptionTracker( iterStrike->second );

  mapOptionTracker_t::iterator iterOptionTracker = mapOptionTracker.find( sOptionName );
  if ( mapOptionTracker.end() == iterOptionTracker ) {
    pOptionTracker = std::make_shared<OptionTracker>(
      pOption,
      m_ceBullCall, m_ceBullPut,
      m_ceBearCall, m_ceBearPut
    );
    mapOptionTracker.emplace(
      mapOptionTracker_t::value_type( sOptionName, pOptionTracker )
    );
  }
  return pOptionTracker;
}

void InteractiveChart::HandleBarCompletionPrice( const ou::tf::Bar& bar ) {

  //m_ceVolume.Append( bar );
  //m_ceVWAP.Append( bar.DateTime(), m_dblSumVolumePrice / m_dblSumVolume );

  double dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal;
  m_pPositionUnderlying->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal );
  m_ceProfitLoss.Append( bar.DateTime(), dblTotal );

  boost::format format( "%0.2f" ); // TODO: refactor this

  if ( m_fUpdatePosition ) {
    ou::tf::PanelOrderButtons_PositionData data;
    data.m_sSymbol = m_pPositionUnderlying->GetInstrument()->GetInstrumentName();

    format % dblTotal;
    data.m_sProfitLoss = format.str();

    auto n = m_pPositionUnderlying->GetActiveSize();
    if ( 0 < n ) {
      switch ( m_pPositionUnderlying->GetRow().eOrderSideActive ) {
        case ou::tf::OrderSide::Buy:
          data.m_sQuantity = "+" + boost::lexical_cast<std::string>( n );;
          break;
        case ou::tf::OrderSide::Sell:
          data.m_sQuantity = "-" + boost::lexical_cast<std::string>( n );;
          break;
        default:
          break;
      }
    }
    //m_fUpdatePosition( data ); // need to cross over into display thread
  }

  if ( m_fUpdateMarketData ) {
    ou::tf::PanelOrderButtons_MarketData data;
    const ou::tf::Quote& quote( m_pPositionUnderlying->GetWatch()->LastQuote() );

    format % quote.Ask();
    data.m_sBaseAsk = format.str();
    format % quote.Bid();
    data.m_sBaseBid = format.str();

    //m_fUpdateMarketData( data ); // need to cross over into display thread
  }

  //CheckOptions(); // do this differently
}

void InteractiveChart::HandleBarCompletionPriceUp( const ou::tf::Bar& bar ) {
  m_ceVolumeUp.Append( bar );
  m_ceVolumeDn.Append( ou::tf::Bar( bar.DateTime(), 0, 0, 0, 0, 0 ) );
}

void InteractiveChart::HandleBarCompletionPriceDn( const ou::tf::Bar& bar ) {
  m_ceVolumeUp.Append( ou::tf::Bar( bar.DateTime(), 0, 0, 0, 0, 0 ) );
  m_ceVolumeDn.Append( bar );
}

void InteractiveChart::SaveWatch( const std::string& sPrefix ) {
  m_pPositionUnderlying->GetWatch()->SaveSeries( sPrefix );
  for ( mapStrikes_t::value_type& strike: m_mapStrikes ) {
    for ( mapOptionTracker_t::value_type& tracker: strike.second ) {
      tracker.second->SaveWatch( sPrefix );
    }
  }
}

void InteractiveChart::EmitOptions() {

  ou::tf::Trade::volume_t volTotalCallBuy {};
  ou::tf::Trade::volume_t volTotalCallSell {};
  ou::tf::Trade::volume_t volTotalPutBuy {};
  ou::tf::Trade::volume_t volTotalPutSell {};

  ou::tf::Trade::volume_t volTotalBuy {};
  ou::tf::Trade::volume_t volTotalSell {};

  for ( mapStrikes_t::value_type& strike: m_mapStrikes ) {

    ou::tf::Trade::volume_t volCallBuy {};
    ou::tf::Trade::volume_t volCallSell {};
    ou::tf::Trade::volume_t volPutBuy {};
    ou::tf::Trade::volume_t volPutSell {};

    for ( mapOptionTracker_t::value_type& tracker: strike.second ) {
      tracker.second->Emit( volCallBuy, volCallSell, volPutBuy, volPutSell );
    }
    std::cout
      << "strike " << strike.first
      << ": cbv=" << volCallBuy << ", csv=" << volCallSell
      << ", pbv=" << volPutBuy  << ", psv=" << volPutSell
      << std::endl;

    volTotalCallBuy  += volCallBuy;
    volTotalCallSell += volCallSell;
    volTotalPutBuy   += volPutBuy;
    volTotalPutSell  += volPutSell;

  }

  volTotalBuy  += ( volTotalCallBuy  + volTotalPutBuy );
  volTotalSell += ( volTotalCallSell + volTotalPutSell );

  std::cout << " calls: bv=" << volTotalCallBuy << " , sv=" << volTotalCallSell << std::endl;
  std::cout << "  puts: bv=" << volTotalPutBuy  << " , sv=" << volTotalPutSell << std::endl;
  std::cout << "totals: bv=" << volTotalBuy     << " , sv=" << volTotalSell << std::endl;
}

void InteractiveChart::BindEvents() {
  Bind( wxEVT_DESTROY, &InteractiveChart::OnDestroy, this );
  Bind( wxEVT_KEY_UP, &InteractiveChart::OnKey, this );
  Bind( wxEVT_CHAR, &InteractiveChart::OnChar, this );
}

void InteractiveChart::UnBindEvents() {
  Unbind( wxEVT_DESTROY, &InteractiveChart::OnDestroy, this );
  Unbind( wxEVT_KEY_UP, &InteractiveChart::OnKey, this );
  Unbind( wxEVT_CHAR, &InteractiveChart::OnChar, this );
}

void InteractiveChart::OnDestroy( wxWindowDestroyEvent& event ) {
  SetChartDataView( nullptr );
  m_pStrategy.reset();
  Disconnect();
  m_bfPrice.SetOnBarComplete( nullptr );
  m_mapLifeCycle_Trade.clear();
  UnBindEvents();
  event.Skip();  // auto followed by Destroy();
}

void InteractiveChart::OnKey( wxKeyEvent& event ) {
  //std::cout << "OnKey=" << event.GetKeyCode() << std::endl;
  event.Skip();
}

void InteractiveChart::OnChar( wxKeyEvent& event ) {
  //std::cout << "OnChar=" << event.GetKeyCode() << std::endl;
  switch ( event.GetKeyCode() ) {
    case 'l': // long
    case 'b': // buy
      OrderBuy( ou::tf::PanelOrderButtons_Order() );
      break;
    case 's': // sell/short
      OrderSell( ou::tf::PanelOrderButtons_Order() );
      break;
    case 'x':
      std::cout << "close out" << std::endl;
      m_pPositionUnderlying->ClosePosition();
      break;
  }
  event.Skip();
}

InteractiveChart::LifeCycle_Position& InteractiveChart::Lookup_LifeCycle_Position() {
  assert( m_pActiveInstrument );
  const std::string& sInstrumentName( m_pActiveInstrument->GetInstrumentName() );
  mapLifeCycle_Position_t::iterator iter = m_mapLifeCycle_Position.find( sInstrumentName );
  assert( m_mapLifeCycle_Position.end() != iter );
  mapLifeCycle_Position_t::mapped_type& lcc( iter->second );
  if ( lcc.pPosition ) {}
  else {
    lcc.pPosition = m_fBuildPosition( m_pActiveInstrument );
    assert( lcc.pPosition );
  }
  return lcc;
}

void InteractiveChart::OrderBuy( const ou::tf::PanelOrderButtons_Order& buttons ) {
  if ( m_pActiveInstrument ) { // need to fix the indicators so show on appropriate option - need access to option tracker
    LifeCycle_Position& lcp( Lookup_LifeCycle_Position() );
    pTradeLifeTime_t pTradeLifeTime
      = std::make_shared<TradeWithABuy>( lcp.pPosition, lcp.pTreeItem, buttons, lcp.indicators
      , [this]( TradeLifeTime& tlt ){ // fDone_t
          CallAfter( [this,&tlt](){
            mapLifeCycle_Trade_t::iterator iter = m_mapLifeCycle_Trade.find( tlt.Id() );
            assert( m_mapLifeCycle_Trade.end() != iter );
            m_mapLifeCycle_Trade.erase( iter );
          } );
      }
    );
    ou::tf::Order::idOrder_t id = pTradeLifeTime->Id();
    auto pair = m_mapLifeCycle_Trade.emplace( std::make_pair( id, std::move( LifeCycle_Trade( pTradeLifeTime ) ) ) );
  }
}

void InteractiveChart::OrderSell( const ou::tf::PanelOrderButtons_Order& buttons ) {
  if ( m_pActiveInstrument ) { // need to fix the indicators so show on appropriate option - need access to option tracker
    LifeCycle_Position& lcp( Lookup_LifeCycle_Position() );
    pTradeLifeTime_t pTradeLifeTime
      = std::make_shared<TradeWithASell>( lcp.pPosition, lcp.pTreeItem, buttons, lcp.indicators
      , [this]( TradeLifeTime& tlt ){ // fDone_t
          CallAfter( [this,&tlt](){
            mapLifeCycle_Trade_t::iterator iter = m_mapLifeCycle_Trade.find( tlt.Id() );
            assert( m_mapLifeCycle_Trade.end() != iter );
            m_mapLifeCycle_Trade.erase( iter );
          } );
      }
      );
    ou::tf::Order::idOrder_t id = pTradeLifeTime->Id();
    auto pair = m_mapLifeCycle_Trade.emplace( std::make_pair( id, std::move( LifeCycle_Trade( pTradeLifeTime ) ) ) );
  }
}

void InteractiveChart::OrderClose( const ou::tf::PanelOrderButtons_Order& buttons ) {
  // button disabled till semantics defined by order, some, all, ...
}

void InteractiveChart::OrderCancel( const ou::tf::PanelOrderButtons_Order& buttons ) {
  // button disabled till semantics defined by order, some, all, ...
}

void InteractiveChart::CancelOrders() {
  for ( mapLifeCycle_Trade_t::value_type& vt: m_mapLifeCycle_Trade ) {
    OrderCancel( vt.second.pTradeLifeTime->Id() );
  }
}

void InteractiveChart::OrderCancel( idOrder_t id ) {
  mapLifeCycle_Trade_t::iterator iter = m_mapLifeCycle_Trade.find( id );
  if ( m_mapLifeCycle_Trade.end() == iter ) {
    std::cout << "OrderCancel: can not find idOrder=" << id << std::endl;
  }
  else {
    iter->second.pTradeLifeTime->Cancel();
  }
}

void InteractiveChart::EmitStatus() {
  for ( mapLifeCycle_Trade_t::value_type& vt: m_mapLifeCycle_Trade ) {
    // TODO: require a way to skip finished TradeLifeTimes
    vt.second.pTradeLifeTime->EmitStatus();
  }
}

void InteractiveChart::EmitOrderStatus( idOrder_t id ) {
  mapLifeCycle_Trade_t::iterator iter = m_mapLifeCycle_Trade.find( id );
  if ( m_mapLifeCycle_Trade.end() == iter ) {
    std::cout << "OrderStatus: can not find idOrder=" << id << std::endl;
  }
  else {
    iter->second.pTradeLifeTime->EmitStatus();
  }
}

void InteractiveChart::DeleteLifeCycle( idOrder_t id ) {
  mapLifeCycle_Trade_t::iterator iter = m_mapLifeCycle_Trade.find( id );
  if ( m_mapLifeCycle_Trade.end() == iter ) {
    std::cout << "DeleteLifeCycle: can not find idOrder=" << id << std::endl;
  }
  else {
    // TODO: need to perform some validation that nothing is in progress.
    //iter->second.fDeleteLifeCycle();
    m_mapLifeCycle_Trade.erase( iter );
  }
}

void InteractiveChart::OptionWatchStart() {
  if ( m_bOptionsReady ) {
    if ( 0 != m_vOptionForQuote.size() ) {
      std::cout << "OptionForQuote: stop watch first" << std::endl;
    }
    else {
      double mid = m_quote.Midpoint();
      for ( mapChains_t::value_type& vt: m_mapChains ) {
        chain_t& chain( vt.second );

        double strike;
        pOption_t pOption;

        strike = chain.Call_Itm( mid );
        pOption = chain.GetStrike( strike ).call.pOption;
        m_vOptionForQuote.push_back( pOption );
        pOption->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleOptionWatchQuote ) );
        pOption->EnableStatsAdd();
        pOption->StartWatch();

        strike = chain.Put_Itm( mid );
        pOption = chain.GetStrike( strike ).put.pOption;
        m_vOptionForQuote.push_back( pOption );
        pOption->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleOptionWatchQuote ) );
        pOption->EnableStatsAdd();
        pOption->StartWatch();

      }
      std::cout << "OptionForQuote: started" << std::endl;
    }
  }
}

void InteractiveChart::OptionQuoteShow() {
  // used to determine low premium/low spread option to trade
  if ( m_bOptionsReady ) {
    if ( 0 == m_vOptionForQuote.size() ) {
      std::cout << "optionForQuote: no options running" << std::endl;
    }
    else {
      bool bOk;
      size_t best_count;
      double best_spread;
      std::cout
        << m_pPositionUnderlying->GetInstrument()->GetInstrumentName()
        << " @ " << m_pPositionUnderlying->GetWatch()->LastTrade().Price()
        << std::endl;
      for ( pOption_t pOption: m_vOptionForQuote ) {
        ou::tf::Quote quote( pOption->LastQuote() );
        std::tie( bOk, best_count, best_spread ) = pOption->SpreadStats();
        std::cout
          << pOption->GetInstrumentName()
          << ": "
          << "b=" << quote.Bid()
          << ",a=" << quote.Ask()
          << ",cnt=" << best_count
          << ",spread=" << best_spread
          << std::endl;
      }
    }
  }
}

void InteractiveChart::OptionWatchStop() {
  if ( m_bOptionsReady ) {
    if ( 0 == m_vOptionForQuote.size() ) {
      std::cout << "OptionForQuote: start watch first" << std::endl;
    }
    else {
      for ( pOption_t pOption: m_vOptionForQuote ) {
        pOption->StopWatch();
        pOption->EnableStatsRemove();
        pOption->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleOptionWatchQuote ) );
      }
      m_vOptionForQuote.clear();
      std::cout << "OptionForQuote: stopped" << std::endl;
    }
  }
}

void InteractiveChart::OptionEmit() {
}

void InteractiveChart::FeatureSetDump() {
  m_bTriggerFeatureSetDump = true;
}

void InteractiveChart::StartDepthByOrder( size_t nLevels ) { // see AppDoM as reference

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_OrderBased.Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_
      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (bid) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          m_FeatureSet.Emit();
        }

        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesBid( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          m_FeatureSet.Emit();
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 0 != ix ) {
                m_FeatureSet.Bid_IncLimit( ix, depth );
              }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                uint32_t nTicks = m_nMarketOrdersBid.load();
                // TODO: does arrival rate of deletions affect overall Market rate?
                if ( 0 == nTicks ) {
                  m_FeatureSet.Bid_IncCancel( 1, depth );
                }
                else {
                  --m_nMarketOrdersBid;
                  m_FeatureSet.Bid_IncMarket( 1, depth );
                }
              }
              else { // 1 < ix
                if ( 0 != ix ) {
                  m_FeatureSet.Bid_IncCancel( ix, depth );
                }
              }
              break;
            default:
              break;
          }
          break;
        case EState::Update:
          // simply a change, no interesting statistics
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
      }
    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_
      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (ask) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          m_FeatureSet.Emit();
        }

        //m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesAsk( op, ix, depth );
        //m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          m_FeatureSet.Emit();
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 0 != ix ) {
                m_FeatureSet.Ask_IncLimit( ix, depth );            }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                uint32_t nTicks = m_nMarketOrdersAsk.load();
                if ( 0 == nTicks ) {
                  m_FeatureSet.Ask_IncCancel( 1, depth );
                }
                else {
                  --m_nMarketOrdersAsk;
                  m_FeatureSet.Ask_IncMarket( 1, depth );
                }
              }
              else { // 1 < ix
                if ( 0 != ix ) {
                  m_FeatureSet.Ask_IncCancel( ix, depth );
                }
              }
              break;
            default:
              break;
          }
        case EState::Update:
          // simply a change, no interesting statistics
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      if ( ( 1 == ix ) || ( 2 == ix ) ) { // may need to recalculate at any level change instead
        Imbalance( depth );
      }
    }
  );

  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this, nLevels ](){
      m_FeatureSet.Set( nLevels );  // use this many levels in the order book for feature vector set
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ),
        [this]( const ou::tf::DepthByOrder& depth ){

          if ( m_bRecordDepths ) {
            m_depths_byorder.Append( depth );
          }

          m_OrderBased.MarketDepth( depth );
        }
        );
    } );

}

void InteractiveChart::Imbalance( const ou::tf::Depth& depth ) {

  static const double w1( 19.0 / 20.0 );
  assert( 1.0 > w1 );
  static const double w2( 1.0 - w1 );

  ou::tf::RunningStats::Stats stats;
  m_FeatureSet.ImbalanceSummary( stats );

  m_ceImbalanceRawMean.Append( depth.DateTime(), stats.meanY );
  //m_ceImbalanceRawB1.Append( depth.DateTime(), stats.b1 );

  m_dblImbalanceMean  = w1 * m_dblImbalanceMean  + w2 * stats.meanY;
  //m_dblImbalanceSlope = w1 * m_dblImbalanceSlope + w2 * stats.b1;

  m_ceImbalanceSmoothMean.Append( depth.DateTime(), m_dblImbalanceMean );
  //m_ceImbalanceSmoothB1.Append( depth.DateTime(), m_dblImbalanceSlope );

  //double state = 0.0;
  //if ( ( 0.0 == m_dblImbalanceMean ) || ( 0.0 == m_dblImbalanceSlope ) ) {} // nothing
  //else {
  //  if ( 0.0 < m_dblImbalanceMean ) {
  //    if ( 0.0 < m_dblImbalanceSlope ) state = 1.0;
  //    else state = 2.0;
  //  }
  //  else {
  //    if ( 0.0 < m_dblImbalanceSlope ) state = -1.0;
  //    else state = -2.0;
  //  }
  //}
  //m_ceImbalanceState.Append( depth.DateTime(), state );

  m_pStrategy->SetImbalance( m_dblImbalanceMean, m_dblImbalanceSlope );
}

void InteractiveChart::LeftClick( int nChart, double value ) {
  //std::cout << "left click: " << nChart << "," << value << std::endl;
  if ( 0 == nChart ) {
    if ( m_fClickLeft ) m_fClickLeft( value );
  }
}

void InteractiveChart::RightClick( int nChart, double value ) {
  //std::cout << "right click: " << nChart << "," << value << std::endl;
  if ( 0 == nChart ) {
    if ( m_fClickRight ) m_fClickRight( value );
  }
}
