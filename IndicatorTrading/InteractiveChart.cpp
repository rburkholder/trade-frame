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
#include <boost/lexical_cast.hpp>

#include <TFOptions/Chains.h>
#include <TFOptions/GatherOptions.h>

#include <TFVuTrading/TreeItem.hpp>

#include "Config.hpp"
#include "Strategy.hpp"
#include "TradeLifeTime.hpp"
#include "OptionTracker.hpp"
#include "InteractiveChart.h"

namespace {
  static const size_t nBarSeconds = 1;
  static const size_t nPeriods = 14;
}

InteractiveChart::InteractiveChart(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos, const wxSize& size, long style
  )
: ou::tf::WinChartView( parent, id, pos, size, style )
, m_bConnected( false )
, m_bOptionsReady( false )
, m_bRecordDepths( false )

, m_dblSumVolume {}
, m_dblSumVolumePrice {}

, m_bfPrice( nBarSeconds )
, m_bfPriceUp( nBarSeconds )
, m_bfPriceDn( nBarSeconds )

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

  m_dvChart.Add( EChartSlot::IV, &m_ceImpliedVolatilityCall );
  m_dvChart.Add( EChartSlot::IV, &m_ceImpliedVolatilityPut );

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

  m_ceVolume.SetName( "Volume" );

  //m_ceVWAP.SetColour( ou::Colour::OrangeRed );
  //m_ceVWAP.SetName( "vwap" );

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

  m_ceImbalanceRawMean.SetName( "imbalance mean (raw)" );
  m_ceImbalanceRawMean.SetColour( ou::Colour::LightGreen );
  m_ceImbalanceSmoothMean.SetName( "imbalance mean (smooth)" );
  m_ceImbalanceSmoothMean.SetColour( ou::Colour::DarkGreen );

  //m_ceImbalanceRawB1.SetName( "imbalance slope" );
  //m_ceImbalanceRawB1.SetColour( ou::Colour::LightGreen );
  //m_ceImbalanceSmoothB1.SetColour( ou::Colour::DarkGreen );

  //m_ceImbalanceState.SetName( "imbalance state" );
  //m_ceImbalanceState.SetColour( ou::Colour::Green );

  m_ceImpliedVolatilityCall.SetName( "IV Call" );
  m_ceImpliedVolatilityPut.SetName( "IV Put" );

  m_ceImpliedVolatilityCall.SetColour( ou::Colour::Green );
  m_ceImpliedVolatilityPut.SetColour( ou::Colour::Blue );

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
    }
  }
}

void InteractiveChart::Disconnect() { // TODO: may also need to clear indicators
  if ( m_pPositionUnderlying ) {
    if ( m_bConnected ) {
      pWatch_t pWatch = m_pPositionUnderlying->GetWatch();
      m_bConnected = false;
      pWatch->OnQuote.Remove( MakeDelegate( this, &InteractiveChart::HandleQuote ) );
      pWatch->OnTrade.Remove( MakeDelegate( this, &InteractiveChart::HandleTrade ) );
    }
  }
}

void InteractiveChart::SetPosition(
  pPosition_t pPosition
, const config::Choices& config
, pOptionChainQuery_t pOptionChainQuery
, fBuildOption_t&& fBuildOption
, fBuildPosition_t&& fBuildPosition
, fRegisterOption_t&& fRegisterOption
, fStartCalc_t&& fStartCalc
, fStopCalc_t&& fStopCalc
, fClick_t&& fClickLeft
, fClick_t&& fClickRight
, fTriggerOrder_t&& fTriggerOrder
, fUpdateMarketData_t&& fUpdateMarketData
, fUpdatePosition_t&& fUpdatePosition
, TreeItem* pTreeItemParent
, ou::ChartEntryMark& cemReferenceLevels
) {

  bool bWasConnected = m_bConnected;
  Disconnect();

  // --

  m_nDaysFront = config.nDaysFront;
  m_nDaysBack = config.nDaysBack;

  assert( boost::gregorian::days( 1 ) <= m_nDaysFront );
  assert( boost::gregorian::days( 1 ) <= m_nDaysBack );
  assert( m_nDaysFront < m_nDaysBack );

  m_dvChart.Add( EChartSlot::Price, &cemReferenceLevels );

  m_fBuildOption = std::move( fBuildOption );
  m_fBuildPosition = std::move( fBuildPosition );

  m_fRegisterOption = std::move( fRegisterOption );
  m_fStartCalc = std::move( fStartCalc );
  m_fStopCalc = std::move( fStopCalc );

  m_fClickLeft = std::move( fClickLeft );
  m_fClickRight = std::move( fClickRight );

  m_fTriggerOrder = std::move( fTriggerOrder );
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

  // options

  OptionChainQuery(
    m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF )
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
        "cp", "", "", "1",
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

        pInstrument_t pInstrument( pOption->GetInstrument() );
        mapChains_t::iterator iterChain = ou::tf::option::GetChain( m_mapChains, pInstrument );
        BuiltOption* pBuiltOption = ou::tf::option::UpdateOption<chain_t,BuiltOption>( iterChain->second, pInstrument );
        assert( pBuiltOption );

        pBuiltOption->pOption = pOption;

      });
  }
  std::cout << " .. option chains built." << std::endl;
  SelectChains();
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

  m_quote = quote; // should refer to watch instead?

  m_ceQuoteAsk.Append( dt, quote.Ask() );
  m_ceQuoteBid.Append( dt, quote.Bid() );
  m_ceQuoteSpread.Append( dt, quote.Ask() - quote.Bid() );

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
  }
  else {
    m_bfPriceDn.Add( dt, price, -trade.Volume() );
  }

}

void InteractiveChart::UpdateImbalance( boost::posix_time::ptime dt, double dblMean, double dblSmoothed ) {

  m_ceImbalanceRawMean.Append( dt, dblMean );
  //m_ceImbalanceRawB1.Append( depth.DateTime(), stats.b1 );

  m_ceImbalanceSmoothMean.Append( dt, dblSmoothed );
  //m_ceImbalanceSmoothB1.Append( depth.DateTime(), m_dblImbalanceSlope );

  m_pStrategy->SetImbalance( dblMean, 0.0 ); // TODO: remove second paramenter (slope), not computed or used
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
    m_fUpdatePosition( data );
  }

  if ( m_fUpdateMarketData ) {
    ou::tf::PanelOrderButtons_MarketData data;
    const ou::tf::Quote& quote( m_pPositionUnderlying->GetWatch()->LastQuote() );

    format % quote.Ask();
    data.m_sBaseAsk = format.str();
    format % quote.Bid();
    data.m_sBaseBid = format.str();

    CheckOptions_v2( format, data );

    m_fUpdateMarketData( data );
  }

}

void InteractiveChart::SelectChains() {

  auto dt = ou::TimeSource::GlobalInstance().External();

  m_iterChainFront = ou::tf::option::SelectChain( m_mapChains, dt.date(), m_nDaysFront );
  m_iterChainBack  = ou::tf::option::SelectChain( m_mapChains, dt.date(), m_nDaysBack );

  assert( m_mapChains.end() != m_iterChainFront );
  assert( m_mapChains.end() != m_iterChainBack  );

  std::cout
    << "front chain "
    << ou::tf::Instrument::BuildDate( m_iterChainFront->first )
    << ", back chain "
    << ou::tf::Instrument::BuildDate( m_iterChainBack->first )
    << std::endl;

}

void InteractiveChart::UpdateImpliedVolatilityCall( const ou::tf::Greek& greek ) {
  m_ceImpliedVolatilityCall.Append( greek.DateTime(), greek.ImpliedVolatility() );
}

void InteractiveChart::UpdateImpliedVolatilityPut( const ou::tf::Greek& greek ) {
  m_ceImpliedVolatilityPut.Append( greek.DateTime(), greek.ImpliedVolatility() );
}

using pOption_t = InteractiveChart::pOption_t;

// https://stackoverflow.com/questions/3209225/how-to-pass-a-method-pointer-as-a-template-parameter
//template<typename Object>
//void UpdateImpliedVolatility( Object& object, void (Object::*f)( const ou::tf::Greek& ), pOption_t& pCurrent, pOption_t& pSelected, ou::ChartEntryIndicator& indicator ) {

void InteractiveChart::UpdateImpliedVolatility( void (InteractiveChart::*f)( const ou::tf::Greek& ), pOption_t& pCurrent, pOption_t& pSelected, ou::ChartEntryIndicator& indicator ) {
  assert( pSelected );
  bool bChanged( false );
  if ( !pCurrent ) {
    pCurrent = pSelected;
    bChanged = true;
  }
  else {
    if ( pCurrent->GetStrike() != pSelected->GetStrike() ) {
      m_fStopCalc( pCurrent, m_pPositionUnderlying->GetWatch() );
      pCurrent->StopWatch();
      pCurrent->OnGreek.Remove( MakeDelegate( this, f ) );
      pCurrent = pSelected;
      bChanged = true;
    }
  }
  if ( bChanged ) {
    const ou::tf::Instrument::idInstrument_t& idInstrument( pCurrent->GetInstrumentName() );
    umapOptions_t::iterator iter = m_umapOptionsRegistered.find( idInstrument );
    if ( m_umapOptionsRegistered.end() == iter ) {
      auto pair = m_umapOptionsRegistered.emplace( idInstrument, pCurrent );
      assert( pair.second );
      //iter = pair.first;
      m_fRegisterOption( pCurrent );
    }
    pCurrent->OnGreek.Add( MakeDelegate( this, f ) );
    pCurrent->StartWatch();
    m_fStartCalc( pCurrent, m_pPositionUnderlying->GetWatch() );
  }
}

bool InteractiveChart::UpdateSynthetic( pOption_t& pCurrent, pOption_t& pSelected ) {
  bool bChanged( false );
  if ( !pCurrent ) {
    pCurrent = pSelected;
    bChanged = true;
  }
  else {
    if ( pCurrent->GetStrike() != pSelected->GetStrike() ) {
      pCurrent->StopWatch();
      pCurrent = pSelected;
      bChanged = true;
    }
  }
  if ( bChanged ) {
    pCurrent->StartWatch();
  }
  return bChanged;
}

void InteractiveChart::CheckOptions_v2( boost::format& format, ou::tf::PanelOrderButtons_MarketData& data ) {

  if ( m_bOptionsReady ) {

    const double mid( m_quote.Midpoint() );

    double strikeItm, strikeAtm, strikeOtm;
    pOption_t pOption;

    chain_t& chainFront( const_cast<chain_t&>( m_iterChainFront->second ) );
    chain_t& chainBack(  const_cast<chain_t&>( m_iterChainBack->second ) );

    // long synth - long call
    strikeItm = chainBack.Call_Itm( mid );
    pOption = chainBack.GetStrike( strikeItm ).call.pOption;
    if ( UpdateSynthetic( m_synthLong.pBackBuy, pOption ) ) {
    }
    else{
      data.m_sCall1 = "C" + boost::lexical_cast<std::string>( strikeItm ) + "L";
      const ou::tf::Quote& quote( pOption->LastQuote() );
      format % quote.Ask();
      data.m_sCall1Ask = std::move( format.str() );
      format % quote.Bid();
      data.m_sCall1Bid = std::move( format.str() );
    }

    // long synth - short put
    strikeOtm = chainFront.Put_Otm( mid );
    pOption = chainFront.GetStrike( strikeOtm ).put.pOption;
    if ( UpdateSynthetic( m_synthLong.pFrontSell, pOption ) ) {
    }
    else {
      data.m_sPut1 = "P" + boost::lexical_cast<std::string>( strikeOtm ) + "S";
      const ou::tf::Quote& quote( pOption->LastQuote() );
      format % quote.Ask();
      data.m_sPut1Ask = std::move( format.str() );
      format % quote.Bid();
      data.m_sPut1Bid = std::move( format.str() );
    }

    // TODO: test strikeItm == strikeOtm - no, far strikes are sometimes missing

    // short synth - long put
    strikeItm = chainBack.Put_Itm( mid );
    pOption = chainBack.GetStrike( strikeItm ).put.pOption;
    if ( UpdateSynthetic( m_synthShort.pBackBuy, pOption ) ) {
    }
    else {
      data.m_sPut2 = "P" + boost::lexical_cast<std::string>( strikeItm ) + "L";
      const ou::tf::Quote& quote( pOption->LastQuote() );
      format % quote.Ask();
      data.m_sPut2Ask = std::move( format.str() );
      format % quote.Bid();
      data.m_sPut2Bid = std::move( format.str() );
    }

    // short synth - short call
    strikeOtm = chainFront.Call_Otm( mid );
    pOption = chainFront.GetStrike( strikeOtm ).call.pOption;
    if ( UpdateSynthetic( m_synthShort.pFrontSell, pOption ) ) {
    }
    else {
      data.m_sCall2 = "C" + boost::lexical_cast<std::string>( strikeOtm ) + "S";
      const ou::tf::Quote& quote( pOption->LastQuote() );
      format % quote.Ask();
      data.m_sCall2Ask = std::move( format.str() );
      format % quote.Bid();
      data.m_sCall2Bid = std::move( format.str() );
    }

    data.m_bOptionPresent = true;

    // TODO: test strikeItm == strikeOtm - no, far strikes are sometimes missing

    // near call for IV
    strikeAtm = chainFront.Call_Atm( mid );
    pOption = chainFront.GetStrike( strikeAtm ).call.pOption;
    UpdateImpliedVolatility( &InteractiveChart::UpdateImpliedVolatilityCall, m_pOptionIVCall, pOption, m_ceImpliedVolatilityCall );

    // near put for IV
    strikeAtm = chainFront.Put_Atm( mid );
    pOption = chainFront.GetStrike( strikeAtm ).put.pOption;
    UpdateImpliedVolatility( &InteractiveChart::UpdateImpliedVolatilityPut, m_pOptionIVPut, pOption, m_ceImpliedVolatilityPut );

  }
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
  //for ( mapStrikes_t::value_type& strike: m_mapStrikes ) {
  //  for ( mapOptionTracker_t::value_type& tracker: strike.second ) {
  //    tracker.second->SaveWatch( sPrefix );
  //  }
  //}
  for ( umapOptions_t::value_type& entry: m_umapOptionsRegistered ) {
    entry.second->SaveSeries( sPrefix );
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

void InteractiveChart::ReleaseResources() {

  if ( m_pOptionIVCall ) {
    m_fStopCalc( m_pOptionIVCall, m_pPositionUnderlying->GetWatch() );
    m_pOptionIVCall->StopWatch();
    m_pOptionIVCall->OnGreek.Remove( MakeDelegate( this, &InteractiveChart::UpdateImpliedVolatilityCall ) );
    m_pOptionIVCall.reset();
  }

  if ( m_pOptionIVPut ) {
    m_fStopCalc( m_pOptionIVPut, m_pPositionUnderlying->GetWatch() );
    m_pOptionIVPut->StopWatch();
    m_pOptionIVPut->OnGreek.Remove( MakeDelegate( this, &InteractiveChart::UpdateImpliedVolatilityPut ) );
    m_pOptionIVPut.reset();
  }

  m_umapOptionsRegistered.clear();

  Disconnect();
  SetChartDataView( nullptr );

  m_mapLifeCycle_Trade.clear();
  m_pStrategy.reset();
  m_pOptionChainQuery.reset();
}

void InteractiveChart::OnDestroy( wxWindowDestroyEvent& event ) {
  // ReleaseResources() comes explicitly earlier in caller

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
      if ( m_fTriggerOrder ) {
        m_fTriggerOrder(ou::tf::PanelOrderButtons_Order::EOrderMethod::Buy );
      }
      break;
    case 's': // sell/short
      if ( m_fTriggerOrder ) {
        m_fTriggerOrder(ou::tf::PanelOrderButtons_Order::EOrderMethod::Sell );
      }
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
      = std::make_shared<TradeWithABuy>(
          lcp.pPosition, lcp.pTreeItem, buttons, lcp.indicators,
          [this]( TradeLifeTime& tlt ){ // fDone_t
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
      = std::make_shared<TradeWithASell>(
          lcp.pPosition, lcp.pTreeItem, buttons, lcp.indicators,
          [this]( TradeLifeTime& tlt ){ // fDone_t
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

  void InteractiveChart::EmitChainFull() const {
    size_t cnt {};
    std::cout << "underlying: " << m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) << std::endl;
    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      std::cout << "chain: " << vt.first << " has " << vt.second.Size() << " entries" << std::endl;
      cnt += vt.second.EmitValues();
      //vt.second.EmitSummary();
    }
    std::cout << "EmitChainFull total chain strikes=" << cnt << std::endl;
  }

  void InteractiveChart::EmitChainSummary() const {
    size_t cnt {};
    std::cout << "underlying: " << m_pPositionUnderlying->GetInstrument()->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) << std::endl;
    for ( const mapChains_t::value_type& vt: m_mapChains ) {
      std::cout << "chain: " << vt.first << " has " << vt.second.Size() << " entries" << std::endl;
      //vt.second.EmitValues();
      cnt += vt.second.EmitSummary();
    }
    std::cout << "EmitChainSummary total sum(call + put)=" << cnt << std::endl;
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
        pOption->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleOptionWatchQuote ) );
        pOption->EnableStatsAdd();
        pOption->StartWatch();
        m_vOptionForQuote.push_back( std::move( pOption ) );

        strike = chain.Put_Itm( mid );
        pOption = chain.GetStrike( strike ).put.pOption;
        pOption->OnQuote.Add( MakeDelegate( this, &InteractiveChart::HandleOptionWatchQuote ) );
        pOption->EnableStatsAdd();
        pOption->StartWatch();
        m_vOptionForQuote.push_back( std::move( pOption ) );

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
