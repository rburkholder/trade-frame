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
 * File:    AppIndicatorTrading.cpp
 * Author:  raymond@burkholder.net
 * Project: IndicatorTrading
 * Created: February 8, 2022 00:12
 */

/*
  2022/11/27 TODO
    build a table of the synthetic long/short entries based upon long/far, short/near
    actions to close/roll the shart/near at profit levels
    actions to close the synthetic together or separately
    enter with a synthetic rather than the future itself
    use ladder for entry, use a different table or ladder for the maintenance
    need summary of delta/gamma to maintain neutral strategies and margin
*/

// TODO: 200 day, 50 day, 20 day, 7 day moving average to understand direction of
//   trend, then run options starting long or short, depending

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include <TFIQFeed/OptionChainQuery.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/BuildInstrument.hpp>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFOptions/Engine.hpp>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/FrameControls.h>
#include <TFVuTrading/PanelOrderButtons.h>

#include <TFVuTrading/MarketDepth/PanelTrade.hpp>

#include "200DayChart.hpp"
#include "SessionChart.hpp"
#include "InteractiveChart.hpp"
#include "AppIndicatorTrading.hpp"

namespace {
  static const std::string sAppName( "Indicator Trading" );
  static const std::string sFileNameBase( "IndicatorTrading" );
  static const std::string sDbName( sFileNameBase + ".db" );
  static const std::string sConfigFilename( sFileNameBase + ".cfg" );
  static const std::string sStateFileName( sFileNameBase + ".state" );
  static const std::string sSaveValuesRoot( "/app/" + sFileNameBase );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppIndicatorTrading)

bool AppIndicatorTrading::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( sVendorName );
  wxApp::SetVendorDisplayName( "(c)2022 " + sVendorName );

  wxApp::OnInit();

  if ( Load( sConfigFilename, m_config ) ) {
  }
  else {
    return 0;
  }

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
/*
  m_threadPortAudioInit = std::move( std::thread(
    [this](){
      if ( m_pPortAudio ) {}
      else {
        m_pPortAudio = std::make_unique<ou::PortAudio>();
        m_pPortAudio->Enumerate();
        if ( m_pChords ) {}
        else {
          m_pChords = std::make_unique<ou::music::Chords>( *m_pPortAudio );
        }
      }
    } )
  );
  m_threadPortAudioInit.detach();
*/
  m_pdb = std::make_unique<ou::tf::db>( sDbName );

  m_tws->SetClientId( m_config.ib_client_id );
  m_tws->SetClientPort( m_config.ib_client_port );
  m_iqfeed->SetThreadCount( m_config.nThreads );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  m_sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer( m_sizerFrame );

  // splitter
  m_splitterRow = new wxSplitterWindow( m_pFrameMain );
  m_splitterRow->SetMinimumPaneSize(10);
  m_splitterRow->SetSashGravity(0.2);

  // tree for viewed symbols
  m_ptreeTradables = new wxTreeCtrl( m_splitterRow, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxTR_HAS_BUTTONS |wxTR_TWIST_BUTTONS|wxTR_SINGLE );
  TreeItem::Bind( m_pFrameMain, m_ptreeTradables );
  m_pTreeItemRoot = new TreeItem( m_ptreeTradables, "/" ); // initialize tree

  // panel for right side of splitter
  wxPanel* panelSplitterRight;
  panelSplitterRight = new wxPanel( m_splitterRow );

  // sizer for right side of splitter
  wxBoxSizer* sizerSplitterRight;
  sizerSplitterRight = new wxBoxSizer( wxVERTICAL );
  panelSplitterRight->SetSizer( sizerSplitterRight );

  // Sizer for Controls
  wxBoxSizer* sizerControls;
  sizerControls = new wxBoxSizer( wxHORIZONTAL );
  sizerSplitterRight->Add( sizerControls, 0, wxStretch::wxEXPAND|wxALL, 5 );

  // m_pPanelProviderControl
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( panelSplitterRight, wxID_ANY );
  sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxRIGHT, 1);

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( panelSplitterRight, wxID_ANY );
  sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND, 1);

  // startup splitter
  m_splitterRow->SplitVertically( m_ptreeTradables, panelSplitterRight, 10);
  m_sizerFrame->Add( m_splitterRow, 1, wxEXPAND|wxALL, 1);

  LinkToPanelProviderControl();

  std::cout << "symbol: " << m_config.sSymbol << std::endl;

  m_pInteractiveChart = new InteractiveChart( panelSplitterRight, wxID_ANY );

  sizerSplitterRight->Add( m_pInteractiveChart, 1, wxEXPAND | wxALL, 2 );

  m_pFrameOrderButtons = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Trading Tools", wxPoint( 10, 10 ) );
  m_pPanelOrderButtons = new ou::tf::PanelOrderButtons( m_pFrameOrderButtons );
  m_pFrameOrderButtons->Attach( m_pPanelOrderButtons );

  m_pFrameOrderButtons->SetAutoLayout( true );
  m_pFrameOrderButtons->Layout();
  m_pFrameOrderButtons->Show( true );

  m_pFrameSessionChart = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Session Bars (1min)" );
  m_pSessionChart = new SessionChart( m_pFrameSessionChart );
  m_pFrameSessionChart->Attach( m_pSessionChart );

  m_pFrameSessionChart->SetAutoLayout( true );
  m_pFrameSessionChart->Layout();
  m_pFrameSessionChart->Show( true );

  m_pFrameChart200Day = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "200 Days" );
  m_pChart200Day = new Chart200Day( m_pFrameChart200Day );
  m_pFrameChart200Day->Attach( m_pChart200Day );

  m_pFrameChart200Day->SetAutoLayout( true );
  m_pFrameChart200Day->Layout();
  m_pFrameChart200Day->Show( true );

  m_pFrameLadderTrade = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Ladder Trading" );
  m_pPanelTrade = new ou::tf::l2::PanelTrade( m_pFrameLadderTrade );
  m_pFrameLadderTrade->Attach( m_pPanelTrade );

  m_pFrameLadderTrade->SetAutoLayout( true );
  m_pFrameLadderTrade->Layout();
  m_pFrameLadderTrade->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIndicatorTrading::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Process Chains", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionProcessChains ) ) );
  vItems.push_back( new mi( "Emit Option Volume", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionEmitOptionVolume ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionSaveValues ) ) );
  vItems.push_back( new mi( "Emit Chains Summary", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionEmitChainsSummary ) ) );
  vItems.push_back( new mi( "Emit Chains Full", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionEmitChainsFull ) ) );
  //vItems.push_back( new mi( "Test Chords Up", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionTestChordsUp ) ) );
  //vItems.push_back( new mi( "Test Chords Down", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionTestChordsDn ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  vItems.clear();
  vItems.push_back( new mi( "Start Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionWatchStart ) ) );
  vItems.push_back( new mi( "Show Quotes", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionQuoteShow ) ) );
  vItems.push_back( new mi( "Stop Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionWatchStop ) ) );
  m_pFrameMain->AddDynamicMenu( "Option Quotes", vItems );

  vItems.clear();
  vItems.push_back( new mi( "FeatureSet Dump", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionFeatureSetDump ) ) );
  m_pFrameMain->AddDynamicMenu( "Debug", vItems );

  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  using pob = ou::tf::PanelOrderButtons;
  m_pPanelOrderButtons->Set(
    [this]( const ou::tf::PanelOrderButtons_Order& order, pob::fBtnDone_t&& fDone ){ // m_fBtnOrderBuy
      m_pInteractiveChart->OrderBuy( order );
      fDone();
    },
    [this]( const ou::tf::PanelOrderButtons_Order& order, pob::fBtnDone_t&& fDone ){ // m_fBtnOrderSell
      m_pInteractiveChart->OrderSell( order );
      fDone();
    },
    [this]( const ou::tf::PanelOrderButtons_Order& order, pob::fBtnDone_t&& fDone ){ // m_fBtnOrderClose
      m_pInteractiveChart->OrderClose( order );
      fDone();
    },
    [this]( const ou::tf::PanelOrderButtons_Order& order, pob::fBtnDone_t&& fDone ){ // m_fBtnOrderCancel
      m_pInteractiveChart->OrderCancel( order );
      fDone();
    }
  );

  CallAfter(
    [this](){
      m_pFrameMain->Show();
      LoadState();
      //m_splitterRow->Layout(); // the sash does not appear to update
      //m_sizerFrame->Layout(); // seems to make it consistent
      m_pFrameMain->Layout();
    }
  );

  return true;
}

void AppIndicatorTrading::ConstructUnderlying() {
  switch ( m_pExecutionProvider->ID() ) {
    case ou::tf::keytypes::EProviderIQF:
      m_pComposeInstrument = std::make_shared<ou::tf::ComposeInstrument>(
        m_iqfeed,
        [this](){
          m_pComposeInstrument->Compose(
            m_config.sSymbol,
            [this]( pInstrument_t pInstrument, bool bConstructed ){
              assert( pInstrument );
              if ( bConstructed ) {
                ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
                im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
              }
              InitializeUnderlying( pInstrument );
            }
          );
        }
      );
      break;
    case ou::tf::keytypes::EProviderIB:
      m_pComposeInstrument = std::make_shared<ou::tf::ComposeInstrument>(
        m_iqfeed, m_tws,
        [this](){
          m_pComposeInstrument->Compose(
            m_config.sSymbol,
            [this]( pInstrument_t pInstrument, bool bConstructed ){
              assert( pInstrument );
              if ( bConstructed ) {
                ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
                im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
              }
              InitializeUnderlying( pInstrument );
            }
          );
        }
      );
      break;
    default:
      assert( false );
      break;
  }
}

void AppIndicatorTrading::InitializeUnderlying( pInstrument_t pInstrument ) {

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

  pPosition_t pPosition = ConstructPosition( pInstrument );
  SetInteractiveChart( pPosition );

  m_pOptionEngine->RegisterUnderlying( pPosition->GetWatch() );

}

AppIndicatorTrading::pPosition_t AppIndicatorTrading::ConstructPosition( pInstrument_t pInstrument ) {

  pPosition_t pPosition;

  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PositionExists( m_pPortfolio->Id(), idInstrument ) ) {
    pPosition = pm.GetPosition( m_pPortfolio->Id(), idInstrument );
    std::cout << "position loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }
  else {
    using pWatch_t = ou::tf::Watch::pWatch_t;
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );

    switch ( m_pExecutionProvider->ID() ) {
      case ou::tf::keytypes::EProviderIQF:
        pPosition = pm.ConstructPosition(
          m_pPortfolio->Id(), idInstrument, "manual-iq",
          "iq01", "iq01", m_pExecutionProvider,
          pWatch
        );
        break;
      case ou::tf::keytypes::EProviderIB:
        pPosition = pm.ConstructPosition(
          m_pPortfolio->Id(), idInstrument, "manual-ib",
          "ib01", "iq01", m_pExecutionProvider,
          pWatch
        );
        break;
      default:
        assert( false );
    }
    std::cout << "position constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }

  return pPosition;

}

void AppIndicatorTrading::SetInteractiveChart( pPosition_t pPosition ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  assert( m_pPanelTrade );
  m_pPanelTrade->SetInterval( pPosition->GetInstrument()->GetMinTick() );

  m_pFeedModel = std::make_shared<FeedModel>( pPosition->GetWatch(), m_config );
  m_pExecModel = std::make_shared<ExecModel>();
  m_pExecutionControl = std::make_shared<ou::tf::l2::ExecutionControl>( pPosition, m_config.nBlockSize );

  namespace ph = std::placeholders;

  using pOption_t = ou::tf::option::Option::pOption_t;

  m_pInteractiveChart->SetPosition(
    pPosition,
    m_config,
    m_pComposeInstrument->OptionChainQuery(),
  // fBuildOption_t:
    [this]( const std::string& sIQFeedOptionSymbol, InteractiveChart::fOption_t&& fOption ){
      m_pComposeInstrument->Compose(
        sIQFeedOptionSymbol,
        [this,fOption_=std::move( fOption )]( pInstrument_t pInstrument, bool bConstructed ){
          if ( pInstrument ) {
            if ( bConstructed ) {
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
            }
            fOption_( std::make_shared<ou::tf::option::Option>( pInstrument, m_iqfeed ) );
            // instrument is registered during transit through BuildInstrument
          }
          else {
            //std::cout << "SetInteractiveChart couldn't BuildInstrument" << std::endl;
          }
        }
      );
    },
  // fBuildPosition_t:
    std::bind( &AppIndicatorTrading::ConstructPosition, this, std::placeholders::_1 ),
  // ManageStrategy::fRegisterOption_t:
    std::bind( &ou::tf::option::Engine::RegisterOption, m_pOptionEngine.get(), ph::_1 ),
  // ManageStrategy::fStartCalc_t:
    [this]( pOption_t pOption, pWatch_t pUnderlying ){
      m_pOptionEngine->Add( pOption, pUnderlying );
    },
  // ManageStrategy::m_fStopCalc:
    [this]( pOption_t pOption, pWatch_t pUnderlying ){
      m_pOptionEngine->Remove( pOption, pUnderlying );
    },
  // m_fClickLeft:
    [this]( double value ) {
      std::string s;
      s = boost::lexical_cast<std::string>( value );
      m_pPanelOrderButtons->SetPriceAtFocus( s );
    },
  // m_fClickRight:
    [this]( double value ) {
    },
  //
    [this]( const ou::tf::PanelOrderButtons_Order::EOrderMethod method ) {
      m_pPanelOrderButtons->Trigger( method );
    },
  // m_fUpdateMarketData:
    [this]( const ou::tf::PanelOrderButtons_MarketData& market_data ) {
      m_pPanelOrderButtons->Update( market_data );
    },
  // m_fUpdatePosition:
    [this]( const ou::tf::PanelOrderButtons_PositionData& position_data ){
      m_pPanelOrderButtons->Update( position_data );
    },
  //
    m_pTreeItemRoot,
    m_cemReferenceLevels
  );
  m_pSessionChart->SetPosition( pPosition, m_cemReferenceLevels );

  LoadDailyHistory( pPosition );

  m_ptreeTradables->ExpandAll();

  m_pExecutionControl->Set( m_pPanelTrade );

  m_pFeedModel->Set( m_pInteractiveChart);
  m_pFeedModel->Set( m_pPanelTrade);
  m_pFeedModel->AddToView(
    m_pInteractiveChart->ChartDataView(),
    InteractiveChart::EChartSlot::Price,
    InteractiveChart::EChartSlot::StochInd
    );

  m_pInteractiveChart->Connect();

  m_pFeedModel->Connect();
}

void AppIndicatorTrading::HandleMenuActionStartChart() {
}

void AppIndicatorTrading::HandleMenuActionStopChart() {
  //m_pWinChartView->SetChartDataView( nullptr );
}

void AppIndicatorTrading::HandleMenuActionStartWatch() {
  //m_pChartData->StartWatch();
}

void AppIndicatorTrading::HandleMenuActionStopWatch() {
  //m_pChartData->StopWatch();
}

void AppIndicatorTrading::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      std::string sPath( sSaveValuesRoot + "/" + m_sTSDataStreamStarted );
      m_fedrate.SaveSeries( sPath );
      m_pInteractiveChart->SaveWatch( sPath );
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppIndicatorTrading::HandleMenuActionEmitChainsSummary() {
  CallAfter(
    [this](){
      m_pInteractiveChart->EmitChainSummary();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionEmitChainsFull() {
  CallAfter(
    [this](){
      m_pInteractiveChart->EmitChainFull();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionTestChordsUp() {
  CallAfter(
    [this](){
       //m_pChords->Play( ou::music::Chords::EProgression::Up );
    }
  );
}

void AppIndicatorTrading::HandleMenuActionTestChordsDn() {
  CallAfter(
    [this](){
      //m_pChords->Play( ou::music::Chords::EProgression::Down );
    }
  );
}

void AppIndicatorTrading::HandleMenuActionProcessChains() {
  CallAfter(
    [this](){
      m_pInteractiveChart->ProcessChains();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionEmitOptionVolume() {
  CallAfter(
    [this](){
      m_pInteractiveChart->EmitOptions();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionWatchStart() {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionWatchStart();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionQuoteShow() {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionQuoteShow();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionWatchStop() {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionWatchStop();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionEmit() {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionEmit();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionFeatureSetDump() {
  CallAfter(
    [this](){
      m_pFeedModel->FeatureSetDump();
    }
  );
}

void AppIndicatorTrading::LoadDailyHistory( pPosition_t pPosition ) {

  using pWatch_t = ou::tf::Watch::pWatch_t;

  pWatch_t pWatchUnderlying = pPosition->GetWatch();
  const std::string& sSymbol( pWatchUnderlying->GetInstrument()->GetInstrumentName( ou::tf::keytypes::eidProvider_t::EProviderIQF ) );

  std::cout << "daily history for " << sSymbol << std::endl;

  ou::Colour::EColour colour( ou::Colour::BlueViolet );

  m_DailyHistory.Load( // change to per bar and done, then Daily history is computed here?
    sSymbol,
    [this,colour](double price, const std::string& sName){
      m_cemReferenceLevels.AddMark( price, colour, sName );
      m_pPanelTrade->AppendStaticIndicator( price, sName );
    },
    [this](const ou::tf::Bars& bars){
      m_pChart200Day->Add( bars );
    } );

}

int AppIndicatorTrading::OnExit() {

  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppIndicatorTrading::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pWinChartView->SetChartDataView( nullptr, false );
  //delete m_pChartData;
  //m_pChartData = nullptr;

  //m_pFrameControls->Close();

  //if ( m_threadPortAudioInit.joinable() ) {
  //  m_threadPortAudioInit.join();
  //}

  m_pPanelTrade->SetOnTimer( nullptr );

  m_pExecutionControl.reset();

  SaveState();

  m_DailyHistory.Close();

  // may need this earlier?
  m_pInteractiveChart->ReleaseResources();
  m_pInteractiveChart->Disconnect();
  if ( m_pFeedModel ) {
    m_pFeedModel->Disconnect();
  }

  m_pComposeInstrument.reset();

  m_pOptionEngine.reset();

  DelinkFromPanelProviderControl();

//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

void AppIndicatorTrading::OnData1Connected( int n ) {
  m_bData1Connected = true;
  OnConnected( n );
}

void AppIndicatorTrading::OnData2Connected( int ) {
  m_bData2Connected = true;
  if ( m_bData2Connected & m_bExecConnected ) {
  }
}

void AppIndicatorTrading::OnExecConnected( int n ) {
  m_bExecConnected = true;
  OnConnected( n );
}

void AppIndicatorTrading::OnData1Disconnected( int ) {
  m_bData1Connected = false;
  if ( nullptr != m_pInteractiveChart ) {
    m_pInteractiveChart->Disconnect();
  }
}

void AppIndicatorTrading::OnData2Disconnected( int ) {
  m_bData2Connected = false;
}

void AppIndicatorTrading::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}

void AppIndicatorTrading::OnConnected( int ) {
  using pOption_t = ou::tf::option::Option::pOption_t;
  if ( m_bData1Connected & m_bExecConnected ) {
    if ( !m_pOptionEngine) {
      m_fedrate.SetWatchOn( m_iqfeed );
      m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );
    }
    ConstructUnderlying();
  }
}

void AppIndicatorTrading::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppIndicatorTrading::LoadState() {
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
