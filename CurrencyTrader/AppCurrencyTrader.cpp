/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    AppCurrencyTrader.cpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: March 09, 2024 19:58:27
 */

#include <algorithm>

#include <boost/log/trivial.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>

#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>
//#include <TFTrading/BuildInstrument.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/TreeItem.hpp>
#include <TFVuTrading/PanelLogging.h>
#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/PanelProviderControlv2.hpp>

#include "Strategy.hpp"
#include "AppCurrencyTrader.hpp"

namespace {
  static const std::string c_sAppName( "CurrencyTrader" );
  static const std::string c_sVendorName( "One Unified Net Limited" );

  static const std::string c_sDirectory( "." );
  static const std::string c_sDbName(          c_sDirectory + '/' + c_sAppName + ".db" );
  static const std::string c_sStateFileName(   c_sDirectory + '/' + c_sAppName + ".state" );
  static const std::string c_sChoicesFilename( c_sDirectory + '/' + c_sAppName + ".cfg" );

  static const std::string c_sMenuItemPortfolio( "_USD" );

  static const std::string c_sPortfolioCurrencyName( "USD" ); // pre-created, needs to be uppercase
  //static const std::string c_sPortfolioSimulationName( "sim" );
  //static const std::string c_sPortfolioRealTimeName( "live" );
  static const std::string c_sPortfolioName( "forex" );
}

IMPLEMENT_APP(AppCurrencyTrader)

bool AppCurrencyTrader::OnInit() {

  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetAppDisplayName( c_sAppName );
  wxApp::SetVendorDisplayName( "(c)2024 " + c_sVendorName );

  wxApp::OnInit();

  m_bProvidersConfirmed = false;
  m_nTSDataStreamSequence = 0;

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
  }
  else {
    return false;
  }

  {
    std::stringstream ss;
    auto dt = ou::TimeSource::GlobalInstance().External();
    ss
      << ou::tf::Instrument::BuildDate( dt.date() )
      << " "
      << dt.time_of_day()
      ;
    m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  }

  m_tws = ou::tf::ib::TWS::Factory();
  m_tws->SetName( "ib01" );
  m_tws->SetClientId( m_choices.m_nIbInstance );

  m_pFrameMain = new FrameMain( 0, wxID_ANY,c_sAppName );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );
  //m_pFrameMain->SetAutoLayout( true );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(sizerFrame);

  sizerUpper = new wxBoxSizer(wxHORIZONTAL);
  sizerFrame->Add( sizerUpper, 0, wxEXPAND, 2);

  // TODO: make the panel conditional on simulation flag
  m_pPanelProviderControl = new ou::tf::v2::PanelProviderControl( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
  m_pPanelProviderControl->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  m_pPanelProviderControl->Show();

  sizerUpper->Add( m_pPanelProviderControl, 0, wxALIGN_LEFT, 2);

  m_pPanelProviderControl->Add(
    m_tws,
    true, false, true, false,
    [](){}, // fConnecting
    [this]( bool bD1, bool bD2, bool bX1, bool bX2 ){ // fConnected
      if ( bD1 ) m_data = m_tws;
      if ( bX1 ) m_exec = m_tws;
      ConfirmProviders();
    },
    [](){}, // fDisconnecting
    [](){}  // fDisconnected
  );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pPanelLogging->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  sizerUpper->Add(m_pPanelLogging, 2, wxEXPAND, 2);

  sizerLower = new wxBoxSizer(wxVERTICAL);
  sizerFrame->Add(sizerLower, 1, wxEXPAND, 2);

  m_splitterData = new wxSplitterWindow( m_pFrameMain, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
  m_splitterData->SetMinimumPaneSize(20);

  m_treeSymbols = new wxTreeCtrl( m_splitterData, wxID_ANY, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE );

  m_pWinChartView = new ou::tf::WinChartView( m_splitterData, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
  m_pWinChartView->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

  m_splitterData->SplitVertically(m_treeSymbols, m_pWinChartView, 50);
  sizerLower->Add(m_splitterData, 1, wxGROW, 2);

  // for now, overwrite old database on each start
  if ( boost::filesystem::exists( c_sDbName ) ) {
    boost::filesystem::remove( c_sDbName );
  }

  // this needs to be placed after the providers are registered
  m_pdb = std::make_unique<ou::tf::db>( c_sDbName ); // construct database

  m_ceUnRealized.SetName( "unrealized" );
  m_ceRealized.SetName( "realized" );
  m_ceCommissionsPaid.SetName( "commission" );
  m_ceTotal.SetName( "total" );

  m_ceUnRealized.SetColour( ou::Colour::Blue );
  m_ceRealized.SetColour( ou::Colour::Purple );
  m_ceCommissionsPaid.SetColour( ou::Colour::Red );
  m_ceTotal.SetColour( ou::Colour::Green );

  m_dvChart.Add( 0, &m_ceUnRealized );
  m_dvChart.Add( 0, &m_ceRealized );
  m_dvChart.Add( 0, &m_ceTotal );
  m_dvChart.Add( 2, &m_ceCommissionsPaid );

  m_pWinChartView->SetChartDataView( &m_dvChart );

  TreeItem::Bind( m_pFrameMain, m_treeSymbols );
  m_pTreeItemRoot = new TreeItem( m_treeSymbols, "/" ); // initialize tree
  //wxTreeItemId idPortfolio = m_treeSymbols->AppendItem( idRoot, sMenuItemPortfolio, -1, -1, new CustomItemData( sMenuItemPortfolio ) );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_MENU, &AppAutoTrade::HandleTreeEventItemMenu, this, m_treeSymbols->GetId() );
  //m_treeSymbols->Bind( wxEVT_TREE_ITEM_RIGHT_CLICK, &AppAutoTrade::HandleTreeEventItemRightClick, this, m_treeSymbols->GetId() );
  //m_treeSymbols->Bind( wxEVT_TREE_SEL_CHANGED, &AppAutoTrade::HandleTreeEventItemChanged, this, m_treeSymbols->GetId() );
  m_pTreeItemPortfolio = m_pTreeItemRoot->AppendChild(
    c_sMenuItemPortfolio,
    [this]( TreeItem* pTreeItem ){
      m_pWinChartView->SetChartDataView( &m_dvChart );
    }
  );

  m_treeSymbols->ExpandAll();

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppCurrencyTrader::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

  vItems.clear(); // maybe wrap this whole menu in the sim conditional
  vItems.push_back( new mi( "Close, Done", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionCloseAndDone ) ) );
  //if ( !m_choices.bStartSimulator ) {
    vItems.push_back( new mi( "Save Values", MakeDelegate( this, &AppCurrencyTrader::HandleMenuActionSaveValues ) ) );
  //}
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  CallAfter(
    [this](){
      LoadState();
    }
  );

  m_pFrameMain->Show( true );

  return 1;
}

void AppCurrencyTrader::HandleMenuActionCloseAndDone() {
  std::cout << "Closing & Done" << std::endl;
  for ( mapStrategy_t::value_type& vt: m_mapStrategy ) {
    //vt.second->CloseAndDone();
  }
}

void AppCurrencyTrader::HandleMenuActionSaveValues() {
  std::cout << "Saving collected values ... " << std::endl;
  CallAfter(
    [this](){
      m_nTSDataStreamSequence++;
      //m_pStrategy->SaveWatch(
      //  "/app/" + c_sAppNamePrefix + "/" +
      //  m_sTSDataStreamStarted + "-" +
      //  boost::lexical_cast<std::string>( m_nTSDataStreamSequence ) ); // sequence number on each save
      std::cout << "  ... Done " << std::endl;
    }
  );
}

void AppCurrencyTrader::ConfirmProviders() {
  if ( m_bProvidersConfirmed ) {}
  else {
    if ( m_data && m_exec ) {
      if ( m_data->Connected() && m_exec->Connected() ) {

        m_bProvidersConfirmed = true;

        BOOST_LOG_TRIVIAL(info)
          << "ConfirmProviders: data(" << m_data->GetName() << ") "
          << "& execution(" << m_exec->GetName() << ") "
          << "providers available"
          ;

        LoadPortfolioCurrency();

        std::string sSymbolName( m_choices.m_sSymbolName );
        std::transform(sSymbolName.begin(), sSymbolName.end(), sSymbolName.begin(), ::toupper);

        using pInstrument_t = ou::tf::Instrument::pInstrument_t;

        pInstrument_t pInstrument;
        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );

        pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIB, sSymbolName );
        if ( pInstrument ) { // skip the build
          BuildStrategy( pInstrument );
        }
        else { // build the instrument

          ou::tf::Currency::pair_t pairCurrency( ou::tf::Currency::Split( sSymbolName ) );
          pInstrument
            = std::make_shared<ou::tf::Instrument>(
                m_choices.m_sSymbolName,
                ou::tf::InstrumentType::Currency, m_choices.m_sExchange, // virtual paper
                pairCurrency.first, pairCurrency.second
                );

          m_tws->RequestContractDetails(
            pInstrument->GetInstrumentName(),
            pInstrument,
            [this]( const ContractDetails& details, ou::tf::Instrument::pInstrument_t& pInstrument ){
              std::cout << "IB contract found: "
                      << details.contract.localSymbol
                << ',' << details.contract.conId
                << ',' << details.mdSizeMultiplier
                << ',' << details.contract.exchange
                << ',' << details.validExchanges
                << ',' << details.timeZoneId
                //<< ',' << details.liquidHours
                //<< ',' << details.tradingHours
                << ',' << "market rule id " << details.marketRuleIds
                << std::endl;

              pInstrument->SetAlternateName( ou::tf::keytypes::EProviderIB, pInstrument->GetInstrumentName() );
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
              BuildStrategy( pInstrument );
            },
            [this]( bool bDone ){
              if ( 0 == m_mapStrategy.size() ) {
                std::cout  << "IB contract request incomplete" << std::endl;
              }
            }
          );
        }
      }
    }
  }
}

void AppCurrencyTrader::LoadPortfolioCurrency() {

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );

  if ( pm.PortfolioExists( c_sPortfolioCurrencyName ) ) {
    m_pPortfolioUSD = pm.GetPortfolio( c_sPortfolioCurrencyName );
  }
  else {
    assert( false );  // should already exist
    //m_pPortfolioUSD
    //  = pm.ConstructPortfolio(
    //      sName, "tf01", c_sPortfolioCurrencyName,
    //      ou::tf::Portfolio::EPortfolioType::Standard,
    //      ou::tf::Currency::Name[ ou::tf::Currency::USD ] );
  }
}

void AppCurrencyTrader::BuildStrategy( pInstrument_t pInstrument ) {

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  const ou::tf::Instrument::idInstrument_t& idInstrument( pInstrument->GetInstrumentName() );

  pPosition_t pPosition;
  if ( pm.PositionExists( c_sPortfolioCurrencyName, idInstrument ) ) {
    pPosition = pm.GetPosition( c_sPortfolioCurrencyName, idInstrument );
    BOOST_LOG_TRIVIAL(info) << "position loaded " << pPosition->GetInstrument()->GetInstrumentName();
  }
  else {
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_data );
    switch ( m_exec->ID() ) {
      case ou::tf::keytypes::EProviderIB:
        pPosition = pm.ConstructPosition(
          c_sPortfolioCurrencyName, idInstrument, "algo_" + c_sPortfolioName,
          m_exec->GetName(), m_data->GetName(), m_tws,
          pWatch
        );
        break;
      case ou::tf::keytypes::EProviderIQF:
        //pPosition = pm.ConstructPosition(
        //  c_sPortfolioCurrencyName, idInstrument, c_sPortfolioName,
        //  m_exec->GetName(), m_data->GetName(), m_iqf,
        //  pWatch
        //);
        assert( false );
        break;
      default:
        assert( false );

    }
    BOOST_LOG_TRIVIAL(info) << "real time position constructed: " << pPosition->GetInstrument()->GetInstrumentName();
  }

  mapStrategy_t::iterator iterStrategy = m_mapStrategy.find( idInstrument );
  if ( m_mapStrategy.end() == iterStrategy ) {
    pStrategy_t pStrategy = std::make_unique<Strategy>( pPosition );
    auto result = m_mapStrategy.emplace( idInstrument, std::move( pStrategy ) );
    assert( result.second );
    iterStrategy = result.first;
  }
  else {
    assert( false );
  }

}

void AppCurrencyTrader::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppCurrencyTrader::LoadState() {
  try {
    std::cout << "Loading Config ..." << std::endl;
    std::ifstream ifs( c_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    std::cout << "  done." << std::endl;
  }
  catch(...) {
    std::cout << "load exception" << std::endl;
  }
}

void AppCurrencyTrader::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  //m_pWinChartView->SetChartDataView( nullptr, false );
  //delete m_pChartData;
  //m_pChartData = nullptr;

  //m_pFrameControls->Close();

  //if ( !m_choices.bStartSimulator ) {
  //  m_timerOneSecond.Stop();
  //  Unbind( wxEVT_TIMER, &AppAutoTrade::HandleOneSecondTimer, this, m_timerOneSecond.GetId() );
  //}

  //m_mapStrategy.clear();

  //m_pBuildInstrument.reset();

  if ( m_pdb ) m_pdb.reset();

  // NOTE: when running the simuliation, perform a deletion instead
  //   use the boost file system utilities?
  //   or the object Delete() operator may work
//  if ( m_choices.bStartSimulator ) {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
      //m_pFile->Delete(); // double free or corruption here
//    }
//  }
//  else {
//    if ( m_pFile ) { // performed at exit to ensure no duplication in file
//      m_pFile->Write();
//    }
//  }

//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();

  event.Skip();  // auto followed by Destroy();
}

int AppCurrencyTrader::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}
