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
 * File:    AppSP500.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: March 30, 2025 13:49:15
 */

// inspiration from AutoTrade

/*
  * start by viewing SPY or SPX as primary chart from HDF5 file - done
  * add in the SP Tick/Trin/Advance/Decline/ratio indicators - done
  * add in some indicators, maybe try the garch studies?
  * run simulator for validation
  * add in the ml ability?
  * run live simulation - iqfeed
  * run live - ib
*/

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/regex.hpp>

#include <wx/sizer.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFVuTrading/FrameMain.h>

#include "Torch.hpp"
#include "AppSP500.hpp"

namespace {
  static const std::string c_sAppTitle(        "SP500 Trading" );
  static const std::string c_sAppNamePrefix(   "sp500" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppSP500)

bool AppSP500::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetVendorDisplayName( "(C)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  //TorchTest_v2();
  //return false; // preliminary exit for testing

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    if ( boost::filesystem::exists( m_choices.m_sHdf5File ) ) {}
    else {
      BOOST_LOG_TRIVIAL(error) << m_choices.m_sHdf5File << " does not exist";
      return false;
    }
  }
  else {
    return false;
  }

  if ( m_choices.m_bRunSim ) {
    if ( boost::filesystem::exists( c_sDbName ) ) {
      boost::filesystem::remove( c_sDbName );
    }
    m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, m_choices.m_sHdf5File );
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "simulation mode only available";
  }

  m_pdb = std::make_unique<ou::tf::db>( c_sDbName );

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerFrame );

  //m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain );
  //sizerFrame->Add( m_pPanelFinancialChart, 1, wxALL | wxEXPAND, 0 );

  m_pwcv = new ou::tf::WinChartView( m_pFrameMain );
  sizerFrame->Add( m_pwcv, 1,wxALL | wxEXPAND, 0 );
  m_pwcv->SetSim();
  m_pwcv->SetChartDataView( &m_cdv );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppSP500::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppSP500::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  m_cdv.SetNames( "SPY", m_choices.m_sHdf5File );

  return true;

}

void AppSP500::OnFrameMainAutoMove( wxMoveEvent& event ) {

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppSP500::OnFrameMainAutoMove, this );

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
      if ( m_choices.m_bRunSim ) {
        RunSimulation();
      }
      else {
        LoadPanelFinancialChart();
      }
    }
  );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppSP500::RunSimulation() {
 // scan hdf5 file for object groups
 // set the group directory in simulator
 // construct strategy with symbols
 //   provide watch & position constructors

  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object_Sim( group, name ); }  // timeseries
  );

  BuildProviders_Sim();
}

void AppSP500::HandleLoadTreeHdf5Object_Sim( const std::string& sGroup, const std::string& sName ) {
  // for now, confirm all objects in one directory, to be used by simulator

  std::string sPrefix;
  size_t instance {};
  std::string::const_iterator ixLead( sGroup.begin() );
  std::string::const_iterator ixTrail( sGroup.begin() );
  while ( sGroup.end() != ixLead ) {
    if ( '/' == *ixLead ) {
      switch ( instance ) {
        case 0: // leading
          break;
        case 1: // 'app'
          break;
        case 2: // 'collector'
          break;
        case 3: // datetime
          sPrefix = std::string( ixTrail, ixLead );
          break;
        case 4: // collection
          break;
        case 5: // symbol
          break;
      }
      ++instance;
    }
    ++ixLead;
  }

  if ( 0 == m_sSimulatorGroupDirectory.size() ) {
    m_sSimulatorGroupDirectory = sPrefix;
  }
  else {
    assert( sPrefix == m_sSimulatorGroupDirectory );
  }

  ou::tf::HDF5Attributes attrObject( *m_pdm, sGroup );
  BOOST_LOG_TRIVIAL(info)
    << "Sim Object,"
    << sGroup << ',' << sName << ','
    << attrObject.GetSignature() << ','
    << attrObject.GetInstrumentType() << ','
    << attrObject.GetMultiplier() << ','
    << attrObject.GetSignificantDigits()
    ;
  mapHdf5Instrument_t::iterator iterHdf5Instrument = m_mapHdf5Instrument.find( sName );
  if ( m_mapHdf5Instrument.end() == iterHdf5Instrument ) {
    pInstrument_t pInstrument;
    auto type = attrObject.GetInstrumentType();
    switch ( type ) {
      case ou::tf::InstrumentType::EInstrumentType::Stock:
        pInstrument = std::make_shared<ou::tf::Instrument>(
          sName, type, "simulator"
        );
        break;
      case ou::tf::InstrumentType::EInstrumentType::Index:
        pInstrument = std::make_shared<ou::tf::Instrument>(
          sName, type, "simulator"
        );
        break;
      default:
        assert( true ); // ignore other types for now
    }
    if ( pInstrument ) {
      pInstrument->SetMultiplier( attrObject.GetMultiplier() );
      pInstrument->SetSignificantDigits( attrObject.GetSignificantDigits() );
      m_mapHdf5Instrument.emplace( sName, pInstrument );
    }
  }
}

bool AppSP500::BuildProviders_Sim() {

  bool bOk( true );

  // construct m_sim
  //   does not need to be in PanelProviderControl
  m_sim = ou::tf::SimulationProvider::Factory();
  m_data = m_exec = m_sim;
  //m_sim->SetThreadCount( m_choices.nThreads );  // don't do this, will post across unsynchronized threads

  try {
    if ( 0 < m_choices.m_sHdf5File.size() ) {
      m_sim->SetHdf5FileName( m_choices.m_sHdf5File );
    }
    m_sim->SetGroupDirectory( m_sSimulatorGroupDirectory );
  }
  catch( const H5::Exception& e ) {
    // need to look at lib/TFHDF5TimeSeries/HDF5DataManager.cpp line 100 for refinement
    BOOST_LOG_TRIVIAL(error) << "group open failed (1) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( const std::exception& e ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (2) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }
  catch( ... ) {
    BOOST_LOG_TRIVIAL(error) << "group open failed (3) " << m_choices.m_sHdf5File << ',' << m_sSimulatorGroupDirectory;
    bOk = false;
  }

  // extract date, time from group directory name
  // TODO: maybe set datetime as an attribute in the hdf5 group
  if ( bOk ) {
    // 20221220-09:20:13.187534
    boost::smatch what;

    static const boost::regex exprDate { "(20[2-3][0-9][0-1][0-9][0-3][0-9])" };
    if ( boost::regex_search( m_sSimulatorGroupDirectory, what, exprDate ) ) {
      m_startDateUTC = boost::gregorian::from_undelimited_string( what[ 0 ] );
    }
    else bOk = false;

    static const boost::regex exprTime { "([0-9][0-9]:[0-9][0-9]:[0-9][0-9])" };
    if ( boost::regex_search( m_sSimulatorGroupDirectory, what, exprTime ) ) {
      m_startTimeUTC = boost::posix_time::duration_from_string( what[ 0 ] );
    }
    else {
      bOk = false;
    }
  }

  // construct the simulation date/time
  if ( bOk ) {
    const boost::posix_time::ptime dtSimulation( ptime( m_startDateUTC, m_startTimeUTC ) );
    boost::local_time::local_date_time lt( dtSimulation, ou::TimeSource::TimeZoneNewYork() );
    boost::posix_time::ptime dtStart = lt.local_time();
    BOOST_LOG_TRIVIAL(info) << "times: " << dtSimulation << "(UTC) is " << dtStart << "(eastern)";
    //dateSim = Strategy::Futures::MarketOpenDate( dtUTC ); //
    //std::cout << "simulation date: " << dateSim << std::endl;

    //m_sSimulationDateTime = boost::posix_time::to_iso_string( dtUTC );
  }

  // construct the simulation menu, attach the events, and start simulation
  if ( bOk ) {
    FrameMain::vpItems_t vItems;
    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    vItems.push_back( new mi( "Start", MakeDelegate( this, &AppSP500::HandleMenuActionSimStart ) ) );
    vItems.push_back( new mi( "Stop",  MakeDelegate( this, &AppSP500::HandleMenuActionSimStop ) ) );
    vItems.push_back( new mi( "Stats",  MakeDelegate( this, &AppSP500::HandleMenuActionSimEmitStats ) ) );
    m_pFrameMain->AddDynamicMenu( "Simulation", vItems );

    m_sim->OnConnected.Add( MakeDelegate( this, &AppSP500::HandleSimConnected ) );
    m_sim->SetOnSimulationComplete( MakeDelegate( this, &AppSP500::HandleSimComplete ) );

    CallAfter(
      [this](){
        m_sim->Connect();
      }
    );
  }

  return bOk;
}

void AppSP500::HandleSimConnected( int ) {
  using pWatch_t = Strategy::pWatch_t;
  using pPosition_t = Strategy::pPosition_t;
  BOOST_LOG_TRIVIAL(info) << "building strategy";
  m_pStrategy = std::make_unique<Strategy>(
    m_cdv,
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedWatch_t&& f ){ // fConstructWatch_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_data );
      f( pWatch );
    },
    [this]( const std::string& sIQFeedSymbolName, Strategy::fConstructedPosition_t&& f ){ // fConstructPosition_t
      mapHdf5Instrument_t::iterator iter = m_mapHdf5Instrument.find( sIQFeedSymbolName );
      assert( m_mapHdf5Instrument.end() != iter );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( iter->second, m_data );
      pPosition_t pPosition = std::make_shared<ou::tf::Position>( pWatch, m_exec );
      f( pPosition );
    },
    [this](){
      CallAfter( [this](){
        BOOST_LOG_TRIVIAL(info) << "simulation run";
        m_sim->Run();
      } );
    },
    [this](){
    }
  );
  assert( m_pStrategy );
  m_pStrategy->InitForUSEquityExchanges( m_startDateUTC );
  m_pStrategy->InitForNextDay();
}

void AppSP500::HandleMenuActionSimStart() {
  CallAfter(
    [this](){
      m_sim->Run();
    }
  );
}

void AppSP500::HandleMenuActionSimStop() {
  CallAfter(
    [this](){
      m_sim->Stop();
    }
  );
}

void AppSP500::HandleMenuActionSimEmitStats() {
  std::stringstream ss;
  m_sim->EmitStats( ss );
  std::cout << "Sim Stats: " << ss.str() << std::endl;
}

void AppSP500::HandleSimComplete() {
  BOOST_LOG_TRIVIAL(info) << "simulation complete";
}

void AppSP500::InitStructures( ESymbol eSymbol, const std::string& sName, size_t ixChart, boost::posix_time::time_duration td ) {
  m_pkwmSymbol->AddPattern( sName, eSymbol );
  auto result = m_mapSymbolInfo.emplace( eSymbol, SymbolInfo( sName, ixChart, td ) );
  assert( result.second );
  SymbolInfo& si( result.first->second );
  si.indicatorTrade.SetColour( ou::Colour::Green );
  m_cdv.Add( ixChart, &si.indicatorTrade );
  if ( 1 == ixChart ) {
    si.indicatorAsk.SetColour( ou::Colour::Red );
    m_cdv.Add( ixChart, &si.indicatorAsk );
    si.indicatorBid.SetColour( ou::Colour::Blue );
    m_cdv.Add( ixChart, &si.indicatorBid );
  }
}

void AppSP500::LoadPanelFinancialChart() {


  m_pkwmSymbol = new ou::KeyWordMatch<ESymbol>( ESymbol::UKNWN, 6 );
  //InitStructures( ESymbol::SPY,  "SPY",    1, boost::posix_time::time_duration( 0, 15, 0 ) );
  InitStructures( ESymbol::SPY,  "SPY",    1 );
  //InitStructures( ESymbol::SPY,  "ES-20250620", 1 );
  InitStructures( ESymbol::II6A, "II6A.Z", 2 );
  InitStructures( ESymbol::II6D, "II6D.Z", 3 );
  InitStructures( ESymbol::JT6T, "JT6T.Z", 4 );
  InitStructures( ESymbol::LI6N, "LI6N.Z", 5 );
  InitStructures( ESymbol::TR6T, "TR6T.Z", 6 );

  IterateObjects();

}

void AppSP500::IterateObjects() {
  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object_Static( group, name ); }  // timeseries
    );
}

void AppSP500::HandleLoadTreeHdf5Group( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "1 Group  " << sGroup << ' ' << sName;
}

void AppSP500::HandleLoadTreeHdf5Object_Static( const std::string& sGroup, const std::string& sName ) {
  // select only ones in the list
  ESymbol eSymbol = m_pkwmSymbol->FindMatch( sName );
  if ( ESymbol::UKNWN != eSymbol ) {
    ou::tf::HDF5Attributes attrObject( *m_pdm, sGroup );
    BOOST_LOG_TRIVIAL(info)
      << "3 Object,"
      << sGroup << ',' << sName << ','
      << attrObject.GetSignature() << ','
      << attrObject.GetInstrumentType() << ','
      << attrObject.GetMultiplier() << ','
      << attrObject.GetSignificantDigits()
      ;
    mapSymbolInfo_t::iterator iterSymbol = m_mapSymbolInfo.find( eSymbol );
    assert ( m_mapSymbolInfo.end() != iterSymbol );

    if ( ou::tf::Trade::Signature() == attrObject.GetSignature() ) {
      ou::tf::HDF5TimeSeriesContainer<ou::tf::Trade> tsRepository( *m_pdm, sGroup );
      ou::tf::HDF5TimeSeriesContainer<ou::tf::Trade>::iterator begin, end;
      begin = tsRepository.begin();
      end = tsRepository.end();
      hsize_t cnt = end - begin;
      ou::tf::Trades& trades( iterSymbol->second.trades );
      trades.Resize( cnt );
      tsRepository.Read( begin, end, &trades );

      ou::ChartEntryIndicator& indicator( iterSymbol->second.indicatorTrade );
      indicator.SetName( sName );

      boost::posix_time::ptime first;
      bool bFirst( true );

      trades.ForEach(
        [&indicator,&first,&bFirst,iterSymbol]( const ou::tf::Trade& trade ){
          boost::posix_time::ptime dt( trade.DateTime() );
          if ( 1 == iterSymbol->second.ixChart ) {
            indicator.Append( dt - iterSymbol->second.tdDelay, trade.Price() );
          }
          else {
            indicator.Append( dt, trade.Price() );
          }
          if ( bFirst ) {
            first = dt;
            bFirst = false;
          }
        } );

      if ( false && ( 1 == iterSymbol->second.ixChart ) ) {
        ou::tf::Quotes& quotes( iterSymbol->second.quotes );
        ou::ChartEntryIndicator& asks( iterSymbol->second.indicatorAsk );
        ou::ChartEntryIndicator& bids( iterSymbol->second.indicatorBid );
        quotes.ForEach(
          [&bids,&asks,&first]( const ou::tf::Quote& quote ){
            boost::posix_time::ptime dt( quote.DateTime() );
            if ( first <= dt ) {
              asks.Append( dt, quote.Ask() );
              bids.Append( dt, quote.Bid() );
              }
          } );
      }

    }

    if ( 1 == iterSymbol->second.ixChart ) {
      if ( ou::tf::Quote::Signature() == attrObject.GetSignature() ) {
        ou::tf::HDF5TimeSeriesContainer<ou::tf::Quote> tsRepository( *m_pdm, sGroup );
        ou::tf::HDF5TimeSeriesContainer<ou::tf::Quote>::iterator begin, end;
        begin = tsRepository.begin();
        end = tsRepository.end();
        hsize_t cnt = end - begin;
        ou::tf::Quotes& quotes( iterSymbol->second.quotes );
        quotes.Resize( cnt );
        tsRepository.Read( begin, end, &quotes );

        ou::ChartEntryIndicator& asks( iterSymbol->second.indicatorAsk );
        asks.SetName( "ask" );
        ou::ChartEntryIndicator& bids( iterSymbol->second.indicatorBid );
        bids.SetName( "bid" );
      }
    }
  }
}

void AppSP500::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppSP500::LoadState() {
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

int AppSP500::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppSP500::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  m_pwcv->SetChartDataView( nullptr );
  m_mapSymbolInfo.clear();

  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

