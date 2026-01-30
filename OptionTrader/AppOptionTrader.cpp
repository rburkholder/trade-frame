/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    AppOptionTrader.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 17, 2025 08:41:21
 */

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>

#include <TFTrading/InstrumentManager.h>

#include <TFIQFeed/BarHistory.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/FrameControls.h>
#include <TFVuTrading/PanelDividendNotes.hpp>

#include "AppOptionTrader.hpp"
#include "PanelDebug.hpp"
#include "PanelInstrumentViews.hpp"
#include "TFInteractiveBrokers/IBTWS.h"

namespace {
  static const std::string c_sAppTitle(        "Option Trader" );
  static const std::string c_sAppNamePrefix(   "optiontrader" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  static const std::string c_sVendorName(      "One Unified Net Limited" );
}

IMPLEMENT_APP(AppOptionTrader)

bool AppOptionTrader::OnInit() {

  wxApp::SetAppDisplayName(    c_sAppTitle );
  wxApp::SetVendorName(        c_sVendorName );
  wxApp::SetVendorDisplayName( "(c)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  m_pFrameMain = nullptr;
  m_pPanelDebug = nullptr;
  m_pFrameDebug = nullptr;
  m_pFrameWatchList = nullptr;
  m_pFrameWinChartView_session = nullptr;
  m_pFrameWinChartView_daily = nullptr;
  m_pInstrumentViews = nullptr;

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  // Sizer for FrameMain
  wxBoxSizer* sizerFrame;
  sizerFrame = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameMain->SetSizer( sizerFrame );

  m_pInstrumentViews = new ou::tf::PanelInstrumentViews( m_pFrameMain );
  sizerFrame->Add( m_pInstrumentViews, 1, wxALL | wxEXPAND, 0 );

  m_pFrameDividendNotes
    = new wxFrame( m_pFrameMain, wxID_ANY, "Dividend & Trade Notes",
      wxDefaultPosition, wxDefaultSize,
      wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT
    );
  wxBoxSizer* sizerFrameSymbolInfo = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameDividendNotes->SetSizer( sizerFrameSymbolInfo );
  m_pPanelDividendNotes = new ou::tf::PanelDividendNotes( m_pFrameDividendNotes, wxID_ANY );
  sizerFrameSymbolInfo->Add( m_pPanelDividendNotes, 1, wxGROW|wxALL, 0 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppOptionTrader::OnClose, this );  // start close of windows and controls

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "Save State", MakeDelegate( this, &AppOptionTrader::HandleMenuActionSaveState ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );

  {
    m_pFrameWinChartView_session = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Session Bars (1min)" );
    m_pWinChartView_session = new ou::tf::WinChartView( m_pFrameWinChartView_session );
    m_pFrameWinChartView_session->Attach( m_pWinChartView_session );

    m_pFrameWinChartView_daily = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Daily Bars" );
    m_pWinChartView_daily = new ou::tf::WinChartView( m_pFrameWinChartView_daily );
    m_pFrameWinChartView_daily->Attach( m_pWinChartView_daily );

    m_pFrameWatchList = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Watch List" );

    m_pFrameDebug = new ou::tf::FrameControls( m_pFrameMain, wxID_ANY, "Debug" );
    m_pPanelDebug = new ou::tf::PanelDebug( m_pFrameDebug );
    m_pFrameDebug->Attach( m_pPanelDebug );
  }

  LoadState();

  m_pFrameMain->Layout();
  m_pFrameMain->Show(); // triggers the auto move

  m_pFrameWinChartView_session->Layout();
  m_pFrameWinChartView_session->Show();

  m_pFrameDividendNotes->Layout();
  m_pFrameDividendNotes->Show();

  m_pFrameWinChartView_daily->Layout();
  m_pFrameWinChartView_daily->Show();

  m_pFrameWatchList->Layout();
  m_pFrameWatchList->Show();

  m_pFrameDebug->Layout();
  m_pFrameDebug->Show();

  ConnectionsStart();

  return true;

}

void AppOptionTrader::HandleMenuActionSaveState() {
  CallAfter(
    [this](){
      SaveState();  // TODO: make a backup file
    } );
}

void AppOptionTrader::ConnectionsStart() {
  // ensure both are constructed prior to connecting
  m_pIQFeed = ou::tf::iqfeed::Provider::Factory();
  m_pIB = ou::tf::ib::TWS::Factory();

  BOOST_LOG_TRIVIAL(info) << "connecting: iqfeed";
  m_pIQFeed->OnConnected.Add( MakeDelegate( this, &AppOptionTrader::HandleIQFeedConnected ) );
  m_pIQFeed->Connect();

  BOOST_LOG_TRIVIAL(info) << "connecting: ib";
  m_pIB->OnConnected.Add( MakeDelegate( this, & AppOptionTrader::HandleIBConnected ));
  m_pIB->Connect();
}

void AppOptionTrader::HandleIQFeedConnected( int ) {
  BOOST_LOG_TRIVIAL(info) << "connected: iqfeed";

  m_fedrate.SetWatchOn( m_pIQFeed );
  m_pOptionEngine = std::make_unique<ou::tf::option::Engine>( m_fedrate );

  m_pBarHistory = std::make_unique<ou::tf::iqfeed::BarHistory>(
    [this](){ // m_fConnected
      BOOST_LOG_TRIVIAL(info) << "connected: bar history";
      ConnecttionsStarted();
    }
  );
  m_pBarHistory->Connect();
}

void AppOptionTrader::HandleIBConnected( int ) {
  BOOST_LOG_TRIVIAL(info) << "connected: ib";
  ConnecttionsStarted();
}

void AppOptionTrader::ConnecttionsStarted() {
  if ( m_pIQFeed->Connected() && m_pIB->Connected() ) {
    // this needs to be placed after the providers are registered
    m_db.Open( c_sDbName );
    BOOST_LOG_TRIVIAL(info) << "connected: database";
    SetComposeInstrument();
  }
}

void AppOptionTrader::SetComposeInstrument() {
  m_pComposeInstrumentIQFeed = std::make_shared<ou::tf::ComposeInstrument>(
    m_pIQFeed,
    [this](){
      CallAfter( // ensures m_pComposeInstrument is set properly prior to use
        [this](){
          m_pInstrumentViews->Set(
            m_pIB,
            m_pComposeInstrumentIQFeed,
            [this]( pInstrument_t pInstrument )->pWatch_t { // fBuildWatch_t
              pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_pIQFeed );
              return pWatch;
            },
            [this]( pInstrument_t pInstrument )->pOption_t { // fBuildOption_t
              pOption_t pOption = std::make_shared<ou::tf::option::Option>( pInstrument, m_pIQFeed );
              //m_pOptionEngine->RegisterOption( pOption );
              return pOption;
            },
            m_pOptionEngine,
            std::move( m_pBarHistory ),
            m_pWinChartView_session,
            m_pWinChartView_daily,
            [this]( const ou::tf::PanelDividendNotes::Fields& fields, const wxArrayString& rTag ){
              m_pFrameDividendNotes->SetTitle( "Symbol Info - " + fields.sSymbol );
              m_pPanelDividendNotes->SetFields( fields );
              m_pPanelDividendNotes->SetTags( rTag );
            },
            [this]( const std::string& key, const std::string& value ){
              CallAfter(
                [this,key,value](){
                  m_pPanelDebug->Update( key, value );
                } );
            }
          );
        } );
    } );
}

void AppOptionTrader::QueryChains( pInstrument_t pUnderlying, fInstrumentOption_t&& fIO ) {

  auto f =
    [this,fIO_ = std::move( fIO )]( const ou::tf::iqfeed::OptionChainQuery::OptionList& list ){
      BOOST_LOG_TRIVIAL(info)
        << "chain request " << list.sUnderlying << " has "
        << list.vSymbol.size() << " options"
        ;

      ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );

      size_t zero = list.vSymbol.size(); // signal when done
      for ( const ou::tf::iqfeed::OptionChainQuery::vSymbol_t::value_type& sSymbol: list.vSymbol ) {
        zero--;
        pInstrument_t pInstrument;
        pInstrument = im.LoadInstrument( ou::tf::keytypes::EProviderIQF, sSymbol );
        if ( pInstrument ) { // skip the build
          fIO_( zero, pInstrument );
        }
        else {
          m_pComposeInstrumentIQFeed->Compose(
            sSymbol,
            [ zero, fIO_ /* make a copy */]( pInstrument_t pInstrument, bool bConstructed ){ // bConstructed - false for error or loaded from db, true when newly constructed
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
              fIO_( zero, pInstrument );
            } );
        }
      }
    };

  const std::string& sIQFeedUnderlying( pUnderlying->GetInstrumentName( ou::tf::Instrument::eidProvider_t::EProviderIQF ) );
  switch ( pUnderlying->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      m_pComposeInstrumentIQFeed->OptionChainQuery()->QueryFuturesOptionChain(
        sIQFeedUnderlying,
        "pc", "", "", "1", // 1 near month
        std::move( f )
      );
      break;
    case ou::tf::InstrumentType::Stock:
      m_pComposeInstrumentIQFeed->OptionChainQuery()->QueryEquityOptionChain(
        sIQFeedUnderlying,
        "pc", "", "1", "0","0","0",  // 1 near month
        std::move( f )
      );
      break;
    default:
      assert( false );
      break;
  }
}


//void AppOptionTrader::HandleMenuActionAddSymbol() {
//}

void AppOptionTrader::SaveState() {
  BOOST_LOG_TRIVIAL(info) << "saving Config ...";
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  BOOST_LOG_TRIVIAL(info) << "saving done";
}

void AppOptionTrader::LoadState() {
  try {
    BOOST_LOG_TRIVIAL(info) << "loading config ...";
    std::ifstream ifs( c_sStateFileName );
    boost::archive::text_iarchive ia(ifs);
    ia & *this;
    BOOST_LOG_TRIVIAL(info) << "loading done";
  }
  catch( const std::runtime_error& e ) {
    BOOST_LOG_TRIVIAL(error) << "load exception " << e.what();
  }
  catch(...) {
    BOOST_LOG_TRIVIAL(error) << "load exception (generic)";
  }
}

int AppOptionTrader::OnExit() {
  // Exit Steps: #4

  m_pBarHistory.reset();

  m_pOptionEngine.reset();
  m_fedrate.SetWatchOff();

  m_pComposeInstrumentIQFeed.reset();

  m_db.Close(); // before or after providers?

  m_pIQFeed->Disconnect();
  m_pIQFeed.reset();

  m_pIB->Disconnect();
  m_pIB.reset();

  return wxAppConsole::OnExit();
}

void AppOptionTrader::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose
  m_pFrameMain->Unbind( wxEVT_CLOSE_WINDOW, &AppOptionTrader::OnClose, this );

  SaveState();

  event.Skip();  // auto followed by Destroy();
}
