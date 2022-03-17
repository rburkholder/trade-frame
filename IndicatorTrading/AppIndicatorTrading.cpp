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

#include <boost/lexical_cast.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include <TFIQFeed/OptionChainQuery.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
#include <TFTrading/BuildInstrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>

#include <TFVuTrading/FrameControls.h>
#include <TFVuTrading/PanelOrderButtons.h>

#include "InteractiveChart.h"
#include "AppIndicatorTrading.h"

namespace {
  static const std::string sAppName( "Indicator Trading" );
  static const std::string sDbName( "IndicatorTrading.db" );
  static const std::string sConfigFilename( "IndicatorTrading.cfg" );
  static const std::string sStateFileName( "IndicatorTrading.state" );
  static const std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
}

namespace {

  class CustomItemData: public wxTreeItemData {
  public:
    std::string sSymbol;
    wxMenu* pMenuPopup;
    CustomItemData(  wxMenu* pMenuPopup_ )
    : pMenuPopup( pMenuPopup_ )
    {}
    CustomItemData( const std::string& sSymbol_ )
    : sSymbol( sSymbol_ ), pMenuPopup( nullptr )
    {}
    ~CustomItemData() {
      if ( nullptr != pMenuPopup ) {
        // assumes binds are cleared as well
        delete pMenuPopup;
        pMenuPopup = nullptr;
      }
    }
  };
}

IMPLEMENT_APP(AppIndicatorTrading)

bool AppIndicatorTrading::OnInit() {

  wxApp::SetAppDisplayName( sAppName );
  wxApp::SetVendorName( "One Unified Net Limited" );
  wxApp::SetVendorDisplayName( "(c)2022 One Unified Net Limited" );

  wxApp::OnInit();

  if ( Load( sConfigFilename, m_config ) ) {
  }
  else {
    return 0;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::Instance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << " "
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  m_pdb = std::make_unique<ou::tf::db>( sDbName );

  m_tws->SetClientId( m_config.nIbInstance );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
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
  wxTreeItemId idRoot = m_ptreeTradables->AddRoot( "/", -1, -1, 0 );
  m_ptreeTradables->Bind( wxEVT_TREE_ITEM_MENU, &AppIndicatorTrading::HandleTreeEventItemMenu, this, m_ptreeTradables->GetId() );

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
  sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 1);

  // m_pPanelLogging
  m_pPanelLogging = new ou::tf::PanelLogging( panelSplitterRight, wxID_ANY );
  sizerControls->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 1);

  // startup splitter
  m_splitterRow->SplitVertically( m_ptreeTradables, panelSplitterRight, 10);
  m_sizerFrame->Add( m_splitterRow, 1, wxEXPAND|wxALL, 1);

  LinkToPanelProviderControl();

  std::cout << "symbol: " << m_config.sSymbol << std::endl;

  m_pInteractiveChart = new InteractiveChart( panelSplitterRight, wxID_ANY );

  sizerSplitterRight->Add( m_pInteractiveChart, 1, wxEXPAND | wxALL, 2 );

  m_pFrameMain->SetAutoLayout( true );
  m_pFrameMain->Layout();
  m_pFrameMain->Show( true );

  m_pFrameControls = new ou::tf::FrameControls(  m_pFrameMain, wxID_ANY, "Controls", wxPoint( 10, 10 ) );
  m_pPanelOrderButtons = new ou::tf::PanelOrderButtons( m_pFrameControls );
  m_pFrameControls->Attach( m_pPanelOrderButtons );

  m_pFrameControls->SetAutoLayout( true );
  m_pFrameControls->Layout();
  m_pFrameControls->Show( true );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppIndicatorTrading::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects
  //vItems.push_back( new mi( "c1 Start Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStartWatch ) ) );
  //vItems.push_back( new mi( "c2 Stop Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionStopWatch ) ) );
  //vItems.push_back( new mi( "d1 Start Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStartChart ) ) );
  //vItems.push_back( new mi( "d2 Stop Chart", MakeDelegate( this, &AppRdafL1::HandleMenuActionStopChart ) ) );
  vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionSaveValues ) ) );
  vItems.push_back( new mi( "Emit Chains Summary", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionEmitChainsSummary ) ) );
  vItems.push_back( new mi( "Emit Chains Full", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionEmitChainsFull ) ) );
  vItems.push_back( new mi( "Process Chains", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionProcessChains ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  vItems.clear();
  vItems.push_back( new mi( "Start Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionWatchStart ) ) );
  vItems.push_back( new mi( "Show Quotes", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionQuoteShow ) ) );
  vItems.push_back( new mi( "Stop Watch", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionWatchStop ) ) );
  //vItems.push_back( new mi( "Emit Options", MakeDelegate( this, &AppIndicatorTrading::HandleMenuActionOptionEmit ) ) );
  m_pFrameMain->AddDynamicMenu( "Option Quotes", vItems );

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
      LoadState();
      m_splitterRow->Layout(); // the sash does not appear to update
      m_pFrameMain->Layout();
      m_sizerFrame->Layout();
    }
  );

  return 1;
}

void AppIndicatorTrading::StartChainQuery() {

  m_pOptionChainQuery = std::make_unique<ou::tf::iqfeed::OptionChainQuery>(
    [this](){
      ConstructInstrument();
    }
  );
  m_pOptionChainQuery->Connect(); // TODO: auto-connect instead?

}

void AppIndicatorTrading::ConstructInstrument() {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  m_pBuildInstrument = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed, m_tws );

  if ( '#' == m_config.sSymbol.back() ) {
    m_pBuildInstrumentIQFeed = std::make_unique<ou::tf::BuildInstrument>( m_iqfeed );
    m_pBuildInstrumentIQFeed->Queue(
      m_config.sSymbol,
      [this]( pInstrument_t pInstrument ){

        boost::gregorian::date expiry( pInstrument->GetExpiry() );
        using OptionChainQuery = ou::tf::iqfeed::OptionChainQuery;
        std::string sBase( m_config.sSymbol.substr( 0, m_config.sSymbol.size() - 1 ) );
        m_pOptionChainQuery->QueryFuturesChain(  // obtain a list of futures
          sBase, "", "234" /* 2022, 2023, 2024 */ , "4" /* 4 months */,
          [this,expiry]( const OptionChainQuery::FuturesList& list ){

            for ( const OptionChainQuery::vSymbol_t::value_type sSymbol: list.vSymbol ) {
              m_pBuildInstrument->Queue(
                sSymbol,
                [this,expiry]( pInstrument_t pInstrument ){

                  std::cout << "future: " << pInstrument->GetInstrumentName() << std::endl;
                  if ( expiry == pInstrument->GetExpiry() ) {

                    pPosition_t pPosition;

                    const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );
                    ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

                    if ( pm.PositionExists( "USD", idInstrument ) ) {
                      pPosition = pm.GetPosition( "USD", idInstrument );
                      std::cout << "position loaded " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
                    }
                    else {
                      using pWatch_t = ou::tf::Watch::pWatch_t;
                      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
                      pPosition = pm.ConstructPosition(
                        "USD", idInstrument, "manual",
                        "ib01", "iq01", m_pExecutionProvider,
                        pWatch
                      );
                      std::cout << "position constructed " << pPosition->GetInstrument()->GetInstrumentName() << std::endl;
                    }

                    SetInteractiveChart( pPosition );
                  }
                } );
            }
          }
          );
      }
    );
  }
  else {
    m_pBuildInstrument->Queue(
      m_config.sSymbol,
      [this]( pInstrument_t pInstrument ){

        using pWatch_t = ou::tf::Watch::pWatch_t;
        pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );

        SetInteractiveChart( std::make_shared<ou::tf::Position>( pWatch, m_pExecutionProvider ) );
      } );
  }

}

void AppIndicatorTrading::SetInteractiveChart( pPosition_t pPosition ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;

  wxTreeItemId idRoot = m_ptreeTradables->GetRootItem();
  const std::string& sSymbol( pPosition->GetInstrument()->GetInstrumentName() );
  wxTreeItemId tiidSymbol = m_ptreeTradables->AppendItem( idRoot, sSymbol, -1, -1, new CustomItemData( sSymbol ) ); // can use popup to cancel/close orders

  m_pInteractiveChart->SetPosition(
    pPosition,
    m_config,
    m_pOptionChainQuery,
    [this]( const std::string& sIQFeedOptionSymbol, InteractiveChart::fOption_t&& fOption ){ // fBuildOption_t
      m_pBuildInstrument->Queue(
        sIQFeedOptionSymbol,
        [this,fOption_=std::move( fOption )](pInstrument_t pInstrument){
          fOption_( std::make_shared<ou::tf::option::Option>( pInstrument, m_iqfeed ) );
        }
      );
    },
    [this,tiidSymbol]( ou::tf::Order::idOrder_t id ){ // fAddLifeCycle_t

      std::string sId( boost::lexical_cast<std::string>( id ) );

      wxMenu* pMenuPopup = new wxMenu();

      wxMenuItem* pMenuItem;

      pMenuItem = pMenuPopup->Append( wxID_ANY, "Close" );
      int idPopUpClose = pMenuItem->GetId();
      pMenuPopup->Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [this, id]( wxCommandEvent& event ){
          std::string sId( boost::lexical_cast<std::string>( id ) );
          //std::cout << "Close: " << sId << "," << event.GetId() << std::endl;
          m_pInteractiveChart->OrderClose( id );
        },
        idPopUpClose
        );

      pMenuItem = pMenuPopup->Append( wxID_ANY, "Cancel" );
      int idPopUpCancel = pMenuItem->GetId();
      pMenuPopup->Bind(
        wxEVT_COMMAND_MENU_SELECTED,
        [this,id]( wxCommandEvent& event ){
          std::string sId( boost::lexical_cast<std::string>( id ) );
          //std::cout << "Cancel: " << sId << "," << event.GetId() << std::endl;
          m_pInteractiveChart->OrderCancel( id );
        },
        idPopUpCancel
        );

      wxTreeItemId idLifeCycle = m_ptreeTradables->AppendItem( tiidSymbol, "Entry Order " + sId, -1, -1, new CustomItemData( pMenuPopup ) );

    }
    );

  m_ptreeTradables->ExpandAll();

  m_pInteractiveChart->Connect();
}

void AppIndicatorTrading::HandleMenuActionStartChart( void ) {
}

void AppIndicatorTrading::HandleMenuActionStopChart( void ) {
  //m_pWinChartView->SetChartDataView( nullptr );
}

void AppIndicatorTrading::HandleMenuActionStartWatch( void ) {
  //m_pChartData->StartWatch();
}

void AppIndicatorTrading::HandleMenuActionStopWatch( void ) {
  //m_pChartData->StopWatch();
}

void AppIndicatorTrading::HandleMenuActionSaveValues( void ) {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      m_pInteractiveChart->SaveWatch( "/app/InddicatorTrading/" + m_sTSDataStreamStarted );
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppIndicatorTrading::HandleMenuActionEmitChainsSummary( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->EmitChainSummary();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionEmitChainsFull( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->EmitChainFull();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionProcessChains( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->ProcessChains();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionWatchStart( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionWatchStart();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionQuoteShow( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionQuoteShow();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionWatchStop( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionWatchStop();
    }
  );
}

void AppIndicatorTrading::HandleMenuActionOptionEmit( void ) {
  CallAfter(
    [this](){
      m_pInteractiveChart->OptionEmit();
    }
  );
}

void AppIndicatorTrading::HandleTreeEventItemMenu( wxTreeEvent& event ) {
  wxTreeItemData* pData = m_ptreeTradables->GetItemData( event.GetItem() );
  if ( nullptr != pData ) {
    CustomItemData* pCustom = dynamic_cast<CustomItemData*>( pData );
    if ( pCustom->pMenuPopup ) {
      m_pFrameMain->PopupMenu( pCustom->pMenuPopup, event.GetPoint() );
    }
  }
  else {
    std::cout << "no item data" << std::endl;
  }
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

  SaveState();

  if ( m_pOptionChainQuery ) {
    m_pOptionChainQuery->Disconnect();
    m_pOptionChainQuery.reset();
  }

  DelinkFromPanelProviderControl();

//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  event.Skip();  // auto followed by Destroy();
}

void AppIndicatorTrading::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    StartChainQuery();
  }
}

void AppIndicatorTrading::OnData2Connected( int ) {
  m_bData2Connected = true;
  if ( m_bData2Connected & m_bExecConnected ) {
  }
}

void AppIndicatorTrading::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    StartChainQuery();
  }
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
