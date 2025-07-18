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
 * Created: October 12, 2021, 23:04
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

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5WriteTimeSeries.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>
#include <TFHDF5TimeSeries/HDF5Attribute.h>

#include <TFIndicators/RunningStats.h>

#include <TFTrading/BuildInstrument.hpp>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/FrameControls.h>

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>
#include <TFVuTrading/MarketDepth/PanelLevelIIButtons.hpp>
#include <TFVuTrading/MarketDepth/PanelSideBySide.hpp>

#include "AppDoM.h"

namespace {
  static const std::string sDirectory( "." );
  static const std::string sAppName( "Depth of Market" );
  static const std::string sChoicesFilename( sDirectory + "/dom.cfg" );
  static const std::string sDbName( sDirectory + "/dom.db" );
  static const std::string sStateFileName( sDirectory + "/dom.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string sSaveValuesRoot( "/app/appDoM" );
}

IMPLEMENT_APP(AppDoM)

bool AppDoM::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  m_bRecordDepth = false;
  m_bTriggerFeatureSetDump = false;

  int code = true;

  if ( !Load( m_config ) ) {
    code = false;
  }
  else {

    {
      std::stringstream ss;
      auto dt = ou::TimeSource::GlobalInstance().External();
      ss
        << ou::tf::Instrument::BuildDate( dt.date() )
        << "-"
        << dt.time_of_day()
        ;
      m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
    }

    m_pdb = std::make_unique<ou::tf::db>( sDbName );

    m_tws->SetClientId( m_config.ib_client_id );

    m_pFrameMain = new FrameMain( nullptr, wxID_ANY, sAppName + " - " + m_config.sSymbolName );
    wxWindowID idFrameMain = m_pFrameMain->GetId();

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    m_pFrameMain->SetSizer( sizerMain );

    m_pFrameMain->SetSize( 675, 800 );
    SetTopWindow( m_pFrameMain );

    m_pFrameControls = new ou::tf::FrameControls(  m_pFrameMain, wxID_ANY, "Level II Statistics", wxPoint( 10, 10 ) );
    m_pPanelSideBySide = new ou::tf::l2::PanelSideBySide( m_pFrameControls );
    m_pFrameControls->Attach( m_pPanelSideBySide );

    m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppDoM::OnClose, this );  // start close of windows and controls

    wxBoxSizer* sizerControls = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 4 );

    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
    sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxRIGHT, 4);
    m_pPanelProviderControl->Show( true );

    wxSize size = sizerMain->GetSize();

    LinkToPanelProviderControl();

    m_pPanelStatistics = new PanelStatistics( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
    sizerControls->Add( m_pPanelStatistics, 0, wxALIGN_LEFT|wxRIGHT, 4);
    m_pPanelStatistics->Show( true );

    m_pPanelLevelIIButtons = new ou::tf::l2::PanelLevelIIButtons( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER );
    sizerControls->Add( m_pPanelLevelIIButtons, 0, wxEXPAND, 4);
    m_pPanelLevelIIButtons->Show( true );

    wxBoxSizer* sizerTrade = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerTrade, 10, wxEXPAND|wxALL, 4 );

    m_pPanelTrade = new ou::tf::l2::PanelTrade( m_pFrameMain );
    sizerTrade->Add( m_pPanelTrade, 1, wxEXPAND, 0 );
    m_pPanelTrade->Show( true );

    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize( 100, 100 ) );
    sizerMain->Add( m_pPanelLogging, 1, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 4);
    m_pPanelLogging->Show( true );

    m_pFrameMain->SetAutoLayout( true );
    m_pFrameMain->Layout();
    m_pFrameMain->Show( true );

    m_pPanelLevelIIButtons->Set(
      [this](bool bArm) { // m_fButtonArmed
        HandleArmedFlag( bArm );
      },
      [this]() { // m_fButtonReCenter
      },
      [this]() { // m_fButtonCancel
      },
      [this]() { // m_fButtonClose
      }
    );

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

    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    //FrameMain::vpItems_t vItemsLoadSymbols;
    //vItemsLoadSymbols.push_back( new mi( "Market Maker Maps", MakeDelegate( this, &AppDoM::EmitMarketMakerMaps ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
    //wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Utility", vItemsLoadSymbols );

    FrameMain::vpItems_t vItems;
    vItems.push_back( new mi( "Start", MakeDelegate( this, &AppDoM::MenuItem_RecordWatch_Start ) ) );
    vItems.push_back( new mi( "Status", MakeDelegate( this, &AppDoM::MenuItem_RecordWatch_Status ) ) );
    vItems.push_back( new mi( "Stop", MakeDelegate( this, &AppDoM::MenuItem_RecordWatch_Stop ) ) );
    vItems.push_back( new mi( "Save", MakeDelegate( this, &AppDoM::MenuItem_RecordWatch_Save ) ) );
    wxMenu* pMenu = m_pFrameMain->AddDynamicMenu( "Market Depth", vItems );

    if ( "order_fvs" == m_config.sDepthType ) {
      vItems.clear();
      vItems.push_back( new mi( "FeatureSet Dump", MakeDelegate( this, &AppDoM::MenuItem_FeatureSet_Dump ) ) );
      m_pFrameMain->AddDynamicMenu( "Debug", vItems );
    }

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

    std::cout << "** turn on IB prior to iqfeed" << std::endl;
    std::cout << "** recording l2 is default to off, enable in menu" << std::endl;

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

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1BidAdd++;
              }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1BidDel++;
                uint32_t nTicks = m_nMarketOrdersBid.load();
                // TODO: does arrival rate of deletions affect overall Market rate?
                if ( 0 == nTicks ) {
                }
                else {
                  m_nMarketOrdersBid--;
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
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteBid( price, volume );
      m_pPanelSideBySide->OnL2Bid( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );
      m_valuesStatistics.nL2MsgAsk++;
      m_valuesStatistics.nL2MsgTtl++;

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1AskAdd++;
              }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1AskDel++;

                uint32_t nTicks = m_nMarketOrdersAsk.load();
                // TODO: does arrival rate of deletions affect overall Market rate?
                if ( 0 == nTicks ) {
                }
                else {
                  m_nMarketOrdersAsk--;
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
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteAsk( price, volume );
      m_pPanelSideBySide->OnL2Ask( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

    }
  );

  m_pPanelSideBySide->Set(
    [this](double mean, double slope ){
      m_valuesStatistics.dblRawMean = mean;
      m_valuesStatistics.dblRawSlope = slope;
    }
  );

  // StartDepthByOrder
  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this ](){
      m_FeatureSet.Set( m_config.nLevels );  // use this many levels in the order book for feature vector set
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_config.sSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){

          if ( m_bRecordDepth ) {
            m_depths_byorder.Append( depth );
          }

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
            case 'C':
              // to be implemented - reset statistics?
              break;
            default:
              assert( false );
          }
          m_OrderBased.MarketDepth( depth );
        }
        );
    } );

}

void AppDoM::StartDepthByOrderWithFVS() {

  using EState = ou::tf::iqfeed::l2::OrderBased::EState;

  m_OrderBased.Set(
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fBid_
      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );
      m_valuesStatistics.nL2MsgBid++;
      m_valuesStatistics.nL2MsgTtl++;

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (bid) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          std::cout << m_FeatureSet << std::endl;
        }

        m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesBid( op, ix, depth );
        m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          std::cout << m_FeatureSet << std::endl;
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1BidAdd++;
              }
              if ( 0 != ix ) {
                m_FeatureSet.Bid_IncLimit( ix, depth );
              }
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
                if ( 0 != ix ) {
                  m_FeatureSet.Bid_IncCancel( ix, depth ); // TODO: use order id to determine cancel/change
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
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteBid( price, volume );
      m_pPanelSideBySide->OnL2Bid( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

      if ( 1 == ix ) { // may need to recalculate at any level change instead
        ou::tf::linear::Stats stats;
        m_FeatureSet.ImbalanceSummary( stats );
        m_valuesStatistics.dblFvsMean = stats.meanY;
        m_valuesStatistics.dblFvsSlope = stats.b1;
      }
    },
    [this]( ou::tf::iqfeed::l2::EOp op, unsigned int ix, const ou::tf::Depth& depth ){ // fBookChanges_t&& fAsk_

      ou::tf::Trade::price_t price( depth.Price() );
      ou::tf::Trade::volume_t volume( depth.Volume() );
      m_valuesStatistics.nL2MsgAsk++;
      m_valuesStatistics.nL2MsgTtl++;

      if ( 0 != ix ) {
        if ( m_bTriggerFeatureSetDump ) {
          std::cout << "fs dump (ask) "
            << (int)op
            << "," << ix
            << "," << depth.MsgType()
            << "," << depth.Price() << "," << depth.Volume()
            << "," << depth.Side()
            << std::endl;
          std::cout << m_FeatureSet << std::endl;
        }

        m_FeatureSet.IntegrityCheck();
        m_FeatureSet.HandleBookChangesAsk( op, ix, depth );
        m_FeatureSet.IntegrityCheck();

        if ( m_bTriggerFeatureSetDump ) {
          std::cout << m_FeatureSet << std::endl;
          m_bTriggerFeatureSetDump = false;
        }
      }

      switch ( m_OrderBased.State() ) {
        case EState::Add:
        case EState::Delete:
          switch ( op ) {
            case ou::tf::iqfeed::l2::EOp::Increase:
            case ou::tf::iqfeed::l2::EOp::Insert:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1AskAdd++;
              }
              if ( 0 != ix ) {
                m_FeatureSet.Ask_IncLimit( ix, depth );
              }
              break;
            case ou::tf::iqfeed::l2::EOp::Decrease:
            case ou::tf::iqfeed::l2::EOp::Delete:
              if ( 1 == ix ) {
                m_valuesStatistics.nLvl1AskDel++;

                uint32_t nTicks = m_nMarketOrdersAsk.load();
                // TODO: does arrival rate of deletions affect overall Market rate?
                if ( 0 == nTicks ) {
                  m_FeatureSet.Ask_IncCancel( 1, depth );
                }
                else {
                  m_nMarketOrdersAsk--;
                  m_FeatureSet.Ask_IncMarket( 1, depth );
                }
              }
              else { // 1 < ix
                if ( 0 != ix ) {
                  m_FeatureSet.Ask_IncCancel( ix, depth ); // TODO: use order id to determine cancel/change
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
        case EState::Clear:
          break;
        case EState::Ready:
          assert( false ); // not allowed
          break;
      }

      m_pPanelTrade->OnQuoteAsk( price, volume );
      m_pPanelSideBySide->OnL2Ask( price, volume, ou::tf::iqfeed::l2::EOp::Delete != op );

      if ( 1 == ix ) { // may need to recalculate at any level change instead
        ou::tf::linear::Stats stats;
        m_FeatureSet.ImbalanceSummary( stats );
        m_valuesStatistics.dblFvsMean = stats.meanY;
        m_valuesStatistics.dblFvsSlope = stats.b1;
        //m_valuesStatistics.dblFvsSlope = m_FeatureSet.;
      }
    }
  );

  m_pPanelSideBySide->Set(
    [this](double mean, double slope ){
      m_valuesStatistics.dblRawMean = mean;
      m_valuesStatistics.dblRawSlope = slope;
    }
  );

  // StartDepthByOrderWithFVS
  m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
    [ this ](){
      m_FeatureSet.Set( m_config.nLevels );  // use this many levels in the order book for feature vector set
      m_pDispatch->Single( true );
      m_pDispatch->WatchAdd(
        m_config.sSymbolName,
        [this]( const ou::tf::DepthByOrder& depth ){

          if ( m_bRecordDepth ) {
            m_depths_byorder.Append( depth ); // is this in watch already, else send it there
          }

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
            case 'C':
              // to be implemented - reset statistics & vectors?
              break;
            default:
              assert( false );
          }
          m_OrderBased.MarketDepth( depth );
        }
        );
    } );

}

void AppDoM::MenuItem_RecordWatch_Start() { // this is more of a series save enable
  if ( ( "order" == m_config.sDepthType )
    || ( "order_fvs" == m_config.sDepthType ) )
  {
    m_bRecordDepth = true;
    if ( m_pWatch ) {
      m_pWatch->RecordSeries( true );
    }

    std::cout << "Depth recording enabled" << std::endl;
  }
  else {
    std::cerr << "depth recording available for 'depth by order' only" << std::endl;
  }
}

void AppDoM::MenuItem_RecordWatch_Status() {
  CallAfter(
    [this](){
      std::cout << "L2 Time Series: "
        << ( m_bRecordDepth ? "is" : "not" ) << " being recorded, "
        << m_depths_byorder.Size() << " elements, "
        << sizeof( ou::tf::DepthsByOrder ) << " bytes each"
        << std::endl;
      std::cout
        << "Feature Vector Set is " << sizeof( ou::tf::iqfeed::l2::FeatureSet_Level) << " bytes "
        << "per each of " << m_config.nLevels << " levels"
        << std::endl;
    }
  );
}

void AppDoM::MenuItem_RecordWatch_Stop() {
  if ( m_bRecordDepth ) {
    m_bRecordDepth = false;
    if ( m_pWatch ) {
      m_pWatch->RecordSeries( false );
    }
    std::cout << "Depth recording disabled" << std::endl;
  }
  else {
    std::cerr << "depth recording wasn't started" << std::endl;
  }
}

void AppDoM::MenuItem_RecordWatch_Save() {
  MenuItem_RecordWatch_Status();
  std::cout << "  Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      const std::string sPathName = sSaveValuesRoot + "/" + m_sTSDataStreamStarted;
      m_pWatch->SaveSeries( sPathName );

      // TODO: need to get the watch in pWatch_t operational
      if ( 0 != m_depths_byorder.Size() ) {
        ou::tf::HDF5DataManager dm( ou::tf::HDF5DataManager::RDWR );
        const std::string sPathNameDepth = sPathName + ou::tf::DepthsByOrder::Directory() + m_pWatch->GetInstrumentName();
        ou::tf::HDF5WriteTimeSeries<ou::tf::DepthsByOrder> wtsDepths( dm, true, true, 5, 256 );
        wtsDepths.Write( sPathNameDepth, &m_depths_byorder );
        ou::tf::HDF5Attributes attrDepths( dm, sPathNameDepth );
        attrDepths.SetSignature( ou::tf::DepthByOrder::Signature() );
        //attrDepths.SetMultiplier( m_pInstrument->GetMultiplier() );
        //attrDepths.SetSignificantDigits( m_pInstrument->GetSignificantDigits() );
        attrDepths.SetProviderType( m_pWatch->GetProvider()->ID() );
      }
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppDoM::MenuItem_FeatureSet_Dump() {
  m_bTriggerFeatureSetDump = true;
}

void AppDoM::HandleArmedFlag( bool bArm ) {
  if ( bArm ) {
    m_pExecutionControl = std::make_shared<ou::tf::l2::ExecutionControl>( m_pPosition, m_config.nBlockSize );
    m_pExecutionControl->Set( m_pPanelTrade );
  }
  else {
    m_pExecutionControl.reset();
  }
}

void AppDoM::EmitMarketMakerMaps() {
  // m_pDispatch->EmitMarketMakerMaps(); TODO: need to make this work
  std::cout << "not implemented" << std::endl;
}

void AppDoM::OnClose( wxCloseEvent& event ) {

  m_pPanelTrade->SetOnTimer( nullptr );

  m_pExecutionControl.reset();

  if ( m_bData1Connected ) { // TODO: fix this logic to work with OnData1Disconnecting
    if ( m_pDispatch ) {
      m_pDispatch->Disconnect();
    }
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
  std::cout << "L1 Data connected" << std::endl;
  BuildPosition();
}

void AppDoM::OnData1Disconnecting( int ) {
  std::cout << "L1 Data disconnecting" << std::endl;
  m_pDispatch->Disconnect();
  m_pWatch->StopWatch();
}

void AppDoM::OnData1Disconnected( int ) {
  std::cout << "L1 Data disconnected" << std::endl;
}

void AppDoM::OnExecConnected( int ) {
  std::cout << "Exec connected" << std::endl;
  BuildPosition();
}

void AppDoM::OnExecDisconnected( int ) {
  std::cout << "Exec disconnected" << std::endl;
}

void AppDoM::BuildPosition() {
  if ( m_tws->Connected() && m_iqfeed->Connected() ) {
    if ( m_pPosition ) {}
    else { // build position
      m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed, m_tws );
      m_pBuildInstrument->Queue(
        m_config.sSymbolName,
        [this]( pInstrument_t pInstrument, bool bConstructed ){
          if ( pInstrument ) {
            if ( bConstructed ) {
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
            }
            InitializePosition( pInstrument );
          }
          else {
            std::cout << "Instrument Not Found" << std::endl;
          }
        } );
    }
  }
}

void AppDoM::InitializePosition( pInstrument_t pInstrument ) {

  m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
  m_pWatch->OnFundamentals.Add( MakeDelegate( this, &AppDoM::OnFundamentals ) );
  m_pWatch->OnQuote.Add( MakeDelegate( this, &AppDoM::OnQuote ) );
  m_pWatch->OnTrade.Add( MakeDelegate( this, &AppDoM::OnTrade ) );

  m_pWatch->RecordSeries( m_bRecordDepth );

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

  if (      "mm" == m_config.sDepthType ) StartDepthByMM();
  else if ( "order" == m_config.sDepthType ) StartDepthByOrder();
  else if ( "order_fvs" == m_config.sDepthType ) StartDepthByOrderWithFVS();
  else {
    std::cout << "l2 needs to be identified with 'mm', 'order' or 'order_fvs', is: " << m_config.sDepthType << std::endl;
    assert( false );
  }

  std::cout << "watching L1/L2: " << m_config.sSymbolName << std::endl;

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( idInstrument ) ) {
    m_pPortfolio = pm.GetPortfolio( idInstrument );
  }
  else {
    m_pPortfolio
      = pm.ConstructPortfolio(
          idInstrument, "tf01", "USD",
          ou::tf::Portfolio::EPortfolioType::Standard,
          ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }

  if ( pm.PositionExists( idInstrument, idInstrument ) ) {
    m_pPosition = pm.GetPosition( idInstrument, idInstrument );
  }
  else {
    m_pPosition = pm.ConstructPosition(
      idInstrument, idInstrument, "dom", "ib01", "iq01", m_tws, m_pWatch
    );
  }

  assert( m_pPosition );

  LoadDailyHistory();

  m_pWatch->StartWatch();
  //std::cout << "Depth of Market connecting" << std::endl;
  m_pDispatch->Connect();

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
          if ( 0 == m_barsHistory.Size() ) {
            std::cout
              << m_pWatch->GetInstrumentName()
              << " not history"
              << std::endl;
          }
          else {
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
            m_pPanelTrade->AppendStaticIndicator( dblSum50,   "50day" );
          }
        }
      );
      CallAfter(
        [this](){
          m_pHistoryRequest.reset();
        });
    }
  );
  m_pHistoryRequest->Connect();
}
