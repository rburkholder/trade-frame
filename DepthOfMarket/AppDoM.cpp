/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    AppDoM.cpp
 * Author:  raymond@burkholder.net
 * Project: App Depth of Market
 * Created on October 12, 2021, 23:04
 */

/*
TODO:
  * select symbol
  * start watch:
      obtain trades, match against orders
      obtain quotes, match against top of each book
  * start Market Depth
      validate against trade stream for actual orders (limits vs market)
*/

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/lexical_cast.hpp>

#include <wx/defs.h>
#include <wx/sizer.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/FrameControls.h>

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>
#include <TFVuTrading/MarketDepth/PanelSideBySide.hpp>

#include "AppDoM.h"

namespace {
  static const std::string sDirectory( "." );
  static const std::string sAppName( "Depth of Market" );
  static const std::string sChoicesFilename( sDirectory + "/dom.cfg" );
  //static const std::string sDbName( sDirectory + "/example.db" );
  static const std::string sStateFileName( sDirectory + "/dom.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

IMPLEMENT_APP(AppDoM)

bool AppDoM::OnInit() {

  int code = 1;

  if ( !Load( m_config ) ) {
    code = 0;
  }
  else {

    m_pFrameMain = new FrameMain( nullptr, wxID_ANY, sAppName + " - " + m_config.sSymbolName );
    wxWindowID idFrameMain = m_pFrameMain->GetId();

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    m_pFrameMain->SetSizer( sizerMain );

    m_pFrameMain->SetSize( 675, 800 );
    SetTopWindow( m_pFrameMain );

    m_pFrameMain->SetAutoLayout( true );
    m_pFrameMain->Layout();
    m_pFrameMain->Show( true );

    m_pFrameControls = new ou::tf::FrameControls(  m_pFrameMain, wxID_ANY, "Level II Statistics", wxPoint( 10, 10 ) );
    m_pPanelSideBySide = new ou::tf::l2::PanelSideBySide( m_pFrameControls );
    m_pFrameControls->Attach( m_pPanelSideBySide );

    m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppDoM::OnClose, this );  // start close of windows and controls

    wxBoxSizer* sizerControls = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 4 );

    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
    sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 4);
    m_pPanelProviderControl->Show( true );

    wxSize size = sizerMain->GetSize();

    LinkToPanelProviderControl();

    m_pPanelStatistics = new PanelStatistics( m_pFrameMain, wxID_ANY );
    sizerControls->Add( m_pPanelStatistics, 0, wxALIGN_LEFT, 4);
    m_pPanelStatistics->Show( true );

    wxBoxSizer* sizerTrade = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerTrade, 1, wxEXPAND|wxALL, 4 );

    m_pPanelTrade = new ou::tf::l2::PanelTrade( m_pFrameMain );
    sizerTrade->Add( m_pPanelTrade, 1, wxALL | wxEXPAND, 4 );
    m_pPanelTrade->Show( true );

    m_cntLoops = 0;
    m_pPanelTrade->SetOnTimer(
      [this](){
        if ( 0 == m_cntLoops ) {
          m_pPanelStatistics->Update( m_valuesStatistics );
          m_valuesStatistics.Zero();
          m_cntLoops = 5;
        }
        else m_cntLoops--;
        for ( const vMA_t::value_type& vt: m_vMA ) {
          m_pPanelTrade->UpdateDynamicIndicator( vt.sName, vt.Latest() );
        }
      });

//    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
//    sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
//    m_pPanelLogging->Show( true );

    ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_config.sSymbolName );
    m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
    m_pWatch->OnFundamentals.Add( MakeDelegate( this, &AppDoM::OnFundamentals ) );
    m_pWatch->OnQuote.Add( MakeDelegate( this, &AppDoM::OnQuote ) );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &AppDoM::OnTrade ) );

    assert( 0 < m_config.nPeriodWidth );
    time_duration td = time_duration( 0, 0, m_config.nPeriodWidth );

    using vMAPeriods_t = std::vector<int>;
    vMAPeriods_t vMAPeriods;

    vMAPeriods.push_back( m_config.nMA1Periods );
    vMAPeriods.push_back( m_config.nMA2Periods );
    vMAPeriods.push_back( m_config.nMA3Periods );

    assert( 3 == vMAPeriods.size() );
    for ( vMAPeriods_t::value_type value: vMAPeriods ) {
      assert( 0 < value );
    }

    m_vMA.emplace_back( MA( m_pWatch->GetQuotes(), vMAPeriods[0], td, "ma1" ) );
    m_vMA.emplace_back( MA( m_pWatch->GetQuotes(), vMAPeriods[1], td, "ma2" ) );
    m_vMA.emplace_back( MA( m_pWatch->GetQuotes(), vMAPeriods[2], td, "ma3" ) );

    m_vStochastic.emplace_back(
      std::make_unique<Stochastic>( m_pWatch->GetQuotes(), m_config.nStochastic1Periods, td,
      [this]( ptime dt, double k, double min, double max ){
        m_pPanelTrade->UpdateDynamicIndicator( "st1u", max ); // upper
        m_pPanelTrade->UpdateDynamicIndicator( "st1l", min ); // lower
      }
      ) );
    m_vStochastic.emplace_back(
      std::make_unique<Stochastic>( m_pWatch->GetQuotes(), m_config.nStochastic2Periods, td,
      [this]( ptime dt, double k, double min, double max ){
        m_pPanelTrade->UpdateDynamicIndicator( "st2u", max );
        m_pPanelTrade->UpdateDynamicIndicator( "st2l", min );
      }
      ) );
    m_vStochastic.emplace_back(
      std::make_unique<Stochastic>(  m_pWatch->GetQuotes(), m_config.nStochastic3Periods, td,
      [this]( ptime dt, double k, double min, double max ){
        m_pPanelTrade->UpdateDynamicIndicator( "st3u", max );
        m_pPanelTrade->UpdateDynamicIndicator( "st3l", min );
      }
      ) );

    if ( "mm" == m_config.sDepthType ) StartDepthByMM();
    else if ( "order" == m_config.sDepthType ) StartDepthByOrder();
    else {
      std::cout << "l2 needs to be identified with 'mm' or 'order', supplied: " << m_config.sDepthType << std::endl;
      assert( false );
    }

    std::cout << "watching L1/L2: " << m_config.sSymbolName << std::endl;

    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    //FrameMain::vpItems_t vItemsLoadSymbols;
    //vItemsLoadSymbols.push_back( new mi( "Market Maker Maps", MakeDelegate( this, &AppDoM::EmitMarketMakerMaps ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
    //wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Utility", vItemsLoadSymbols );

    m_pFrameControls->SetAutoLayout( true );
    m_pFrameControls->Layout();
    m_pFrameControls->Show( true );

    CallAfter(
      [this](){
        // doesn't cooperate
        LoadState();
        m_pFrameMain->Layout();
      }
    );

  }

  return code;
}

void AppDoM::StartDepthByMM() {

  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this ](){
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_config.sSymbolName,
        [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fBid_
          m_valuesStatistics.nL2MsgBid++;
          m_valuesStatistics.nL2MsgTtl++;
          m_pPanelTrade->OnQuoteBid( price, volume );
          m_pPanelSideBySide->OnL2Bid( price, volume, bAdd );
        },
        [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fAsk_
          m_valuesStatistics.nL2MsgAsk++;
          m_valuesStatistics.nL2MsgTtl++;
          m_pPanelTrade->OnQuoteAsk( price, volume );
          m_pPanelSideBySide->OnL2Ask( price, volume, bAdd );
        });
    } );

}

void AppDoM::StartDepthByOrder() {

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_OrderBased.Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_
      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );
      m_valuesStatistics.nL2MsgBid++;
      m_valuesStatistics.nL2MsgTtl++;

      m_FeatureSet.HandleBookChangesBid( op, ix, depth );
      auto state = m_OrderBased.State();
      assert( EState::Ready != state );
      if ( ( EState::Add == state ) || ( EState::Delete == state ) ) {
        switch ( op ) {
          case ou::tf::iqfeed::l2::EOp::Increase:
          case ou::tf::iqfeed::l2::EOp::Insert:
            if ( 1 == ix ) {
              m_valuesStatistics.nLvl1BidAdd++;
            }
            m_FeatureSet.Bid_IncLimit( ix, depth );
            break;
          case ou::tf::iqfeed::l2::EOp::Decrease:
          case ou::tf::iqfeed::l2::EOp::Delete:
            if ( 1 == ix ) {
              m_valuesStatistics.nLvl1BidDel++;

              uint32_t nTicks = m_nMarketOrdersBid.load();
              // TODO: does arrival rate of deletions affect overall Market rate?
              if ( 0 == nTicks ) {
                m_FeatureSet.Bid_IncCancel( 1, depth );
              }
              else {
                m_nMarketOrdersBid--;
                m_FeatureSet.Bid_IncMarket( 1, depth );
              }
            }
            else { // 1 < ix
              m_FeatureSet.Bid_IncCancel( ix, depth );
            }
            break;
          default:
            break;
        }
      }

      m_pPanelTrade->OnQuoteBid( price, volume );
      m_pPanelSideBySide->OnL2Bid( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );
    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_
      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );
      m_valuesStatistics.nL2MsgAsk++;
      m_valuesStatistics.nL2MsgTtl++;

      m_FeatureSet.HandleBookChangesAsk( op, ix, depth );
      auto state = m_OrderBased.State();
      assert( EState::Ready != state );
      if ( ( EState::Add == state ) || ( EState::Delete == state ) ) {
        switch ( op ) {
          case ou::tf::iqfeed::l2::EOp::Increase:
          case ou::tf::iqfeed::l2::EOp::Insert:
            if ( 1 == ix ) {
              m_valuesStatistics.nLvl1AskAdd++;
            }
            m_FeatureSet.Ask_IncLimit( ix, depth );
            break;
          case ou::tf::iqfeed::l2::EOp::Decrease:
          case ou::tf::iqfeed::l2::EOp::Delete:
            if ( 1 == ix ) {
              m_valuesStatistics.nLvl1AskDel++;

              uint32_t nTicks = m_nMarketOrdersAsk.load();
              if ( 0 == nTicks ) {
                m_FeatureSet.Ask_IncCancel( 1, depth );
              }
              else {
                m_nMarketOrdersAsk--;
                m_FeatureSet.Ask_IncMarket( 1, depth );
              }
            }
            else { // 1 < ix
              m_FeatureSet.Ask_IncCancel( ix, depth );
            }
            break;
          default:
            break;
        }
      }

      m_pPanelTrade->OnQuoteAsk( price, volume );
      m_pPanelSideBySide->OnL2Ask( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );
    }
  );

  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this ](){
      m_FeatureSet.Set( 10 );
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_config.sSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){
          switch ( depth.MsgType() ) {
            case '4': // Update
              switch ( depth.Side() ) {
                case 'A':
                  m_valuesStatistics.nL2UpdAsk++;
                  break;
                case 'B':
                  m_valuesStatistics.nL2UpdBid++;
                  break;
                default:
                  assert( false );
                  break;
              }
              m_valuesStatistics.nL2UpdTtl++;
              break;
            case '3': // add
              switch ( depth.Side() ) {
                case 'A':
                  m_valuesStatistics.nL2AddAsk++;
                  break;
                case 'B':
                  m_valuesStatistics.nL2AddBid++;
                  break;
                default:
                  assert( false );
                  break;
              }
              m_valuesStatistics.nL2AddTtl++;
              break;
            case '5':
              switch ( depth.Side() ) {
                case 'A':
                  m_valuesStatistics.nL2DelAsk++;
                  break;
                case 'B':
                  m_valuesStatistics.nL2DelBid++;
                  break;
                default:
                  assert( false );
                  break;
              }
              m_valuesStatistics.nL2DelTtl++;
              break;
            case '6': // Summary - will need to categorize this properly
              m_valuesStatistics.nL2AddTtl++;
              break;
            default:
              assert( false );
          }
          m_OrderBased.MarketDepth( depth );
        }
        );
    } );

}

void AppDoM::EmitMarketMakerMaps() {
  // m_pDispatch->EmitMarketMakerMaps(); TODO: need to make this work
  std::cout << "not implemented" << std::endl;
}

void AppDoM::OnClose( wxCloseEvent& event ) {

  if ( m_bData1Connected ) { // TODO: fix this logic to work with OnData1Disconnecting
    m_pDispatch->Disconnect();
  }

  // m_pDispatch.reset(); // TODO: need to do this in a callback?

  //if ( m_worker.joinable() ) m_worker.join();
  //m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a

  SaveState();

  //if ( m_db.IsOpen() ) m_db.Close();
  event.Skip();  // auto followed by Destroy();
}

int AppDoM::OnExit() {

//  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppDoM::OnData1Connected( int ) {
  std::cout << "Depth of Market connected" << std::endl;
  m_pWatch->StartWatch();
  m_pDispatch->Connect();
  LoadDailyHistory();
}

void AppDoM::OnData1Disconnecting( int ) {
  m_pDispatch->Disconnect();
  m_pWatch->StopWatch();
}

void AppDoM::OnData1Disconnected( int ) {
  std::cout << "Depth of Market disconnected" << std::endl;
}

// TODO: there is an order interval, and there is a quote interval
void AppDoM::OnFundamentals( const ou::tf::Watch::Fundamentals& fundamentals ) {
  if ( m_pPanelTrade ) {
    if ( 0 < fundamentals.dblTickSize ) { // eg QQQ shows 0 (equities are 0?)
      m_pPanelTrade->SetInterval( fundamentals.dblTickSize );
    }
  }
}

void AppDoM::OnQuote( const ou::tf::Quote& quote ) {

  m_dblLastAsk = quote.Ask();
  m_dblLastBid = quote.Bid();

  m_valuesStatistics.nL1MsgBid++;
  m_valuesStatistics.nL1MsgAsk++;
  m_valuesStatistics.nL1MsgTtl++;
  if ( m_pPanelTrade ) {
    m_pPanelTrade->OnQuote( quote );
  }
}

void AppDoM::OnTrade( const ou::tf::Trade& trade ) {

  m_valuesStatistics.nTicks++;

  // TODO: track when bid == ask, examine which closes the gap
  //   the side not moving has the order,
  //   the side moving is the execution side

  const double price = trade.Price();
  const double mid = ( m_dblLastAsk + m_dblLastBid ) / 2.0;
  if ( mid == price ) {
    m_valuesStatistics.nLvl1UnkTick++;
  }
  else {
    if ( mid < price ) {
      m_valuesStatistics.nLvl1AskTick++;
      m_nMarketOrdersAsk++;
    }
    else {
      m_valuesStatistics.nLvl1BidTick++;
      m_nMarketOrdersBid++;
    }
  }

  if ( m_pPanelTrade ) {
    m_pPanelTrade->OnTrade( trade );
  }
}

void AppDoM::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppDoM::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppDoM::LoadDailyHistory() {
  m_pHistoryRequest = ou::tf::iqfeed::HistoryRequest::Construct(
    [this](){ // fConnected_t
      m_pHistoryRequest->Request(
        m_pWatch->GetInstrumentName(),
        200,
        [this]( const ou::tf::Bar& bar ){
          m_barsHistory.Append( bar );
          //m_pHistoryRequest.reset(); // TODO: surface the disconnect and make synchronous
        },
        [this](){
          const ou::tf::Bar& bar( m_barsHistory.last() );

          std::cout
            << m_pWatch->GetInstrumentName()
            << ", bar=" << bar.DateTime()
            << std::endl;

          m_setPivots.CalcPivots( bar );
          const ou::tf::PivotSet& ps( m_setPivots );
          using PS = ou::tf::PivotSet;
          m_pPanelTrade->AppendStaticIndicator( ps.GetPivotValue( PS::R2 ), "r2" );
          m_pPanelTrade->AppendStaticIndicator( ps.GetPivotValue( PS::R1 ), "r1" );
          m_pPanelTrade->AppendStaticIndicator( ps.GetPivotValue( PS::PV ), "pv" );
          m_pPanelTrade->AppendStaticIndicator( ps.GetPivotValue( PS::S1 ), "s1" );
          m_pPanelTrade->AppendStaticIndicator( ps.GetPivotValue( PS::S2 ), "s2" );

          std::cout
            << "pivots"
            <<  " r2=" << ps.GetPivotValue( PS::R2 )
            << ", r1=" << ps.GetPivotValue( PS::R1 )
            << ", pv=" << ps.GetPivotValue( PS::PV )
            << ", s1=" << ps.GetPivotValue( PS::S1 )
            << ", s2=" << ps.GetPivotValue( PS::S2 )
            << std::endl;

          double dblSum200 {};
          double dblSum100 {};
          double dblSum50 {};
          int ix( 1 );

          m_barsHistory.ForEachReverse( [this,&ix,&dblSum200,&dblSum100,&dblSum50]( const ou::tf::Bar& bar ){
            //std::cout
            //  << "bar " << ix << " is " << bar.Close()
            //  << std::endl;
            if ( 200 >= ix ) {
              std::string sIx = boost::lexical_cast<std::string>( ix );
              m_pPanelTrade->AppendStaticIndicator( bar.High(), "hi-" + sIx );
              m_pPanelTrade->AppendStaticIndicator( bar.Low(), "lo-" + sIx  );
            }
            if ( 200 >= ix ) {
              dblSum200 += bar.Close() / 200.0;
            }
            if ( 100 >= ix ) {
              dblSum100 += bar.Close() / 100.0;
            }
            if ( 50 >= ix ) {
              dblSum50 += bar.Close() / 50;
            }
            ix++;
          });

          std::cout
            << "sma"
            << " 50 day=" << dblSum50
            << ", 100 day=" << dblSum100
            << ", 200 day=" << dblSum200
            << std::endl;

          m_pPanelTrade->AppendStaticIndicator( dblSum200, "200day" );
          m_pPanelTrade->AppendStaticIndicator( dblSum100, "100day" );
          m_pPanelTrade->AppendStaticIndicator( dblSum50, "50day" );
        }
      );
    }
  );
  m_pHistoryRequest->Connect();
}
