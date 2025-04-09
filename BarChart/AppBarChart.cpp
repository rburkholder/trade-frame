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
 * File:    AppBarChart.cpp
 * Author:  raymond@burkholder.net
 * Project: BarChart
 * Created: April 8, 2025 21:29:02
 */

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

#include <wx/sizer.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFVuTrading/FrameMain.h>

#include "AppBarChart.hpp"

namespace {
  static const std::string c_sAppTitle(        "Daily Bars Review" );
  static const std::string c_sAppNamePrefix(   "barchart" );
  static const std::string c_sChoicesFilename( c_sAppNamePrefix + ".cfg" );
  static const std::string c_sDbName(          c_sAppNamePrefix + ".db" );
  static const std::string c_sStateFileName(   c_sAppNamePrefix + ".state" );
  //static const std::string c_sTimeZoneSpec( "../date_time_zonespec.csv" );
  static const std::string c_sVendorName( "One Unified Net Limited" );
}

IMPLEMENT_APP(AppBarChart)

bool AppBarChart::OnInit() {

  wxApp::SetAppDisplayName( c_sAppTitle );
  wxApp::SetVendorName( c_sVendorName );
  wxApp::SetVendorDisplayName( "(c)2025 " + c_sVendorName );

  if ( !wxApp::OnInit() ) {
    return false;
  }

  if ( config::Load( c_sChoicesFilename, m_choices ) ) {
    if ( boost::filesystem::exists( m_choices.m_sHdf5File ) ) {}
    else {
      BOOST_LOG_TRIVIAL(error) << m_choices.m_sHdf5File << " does not exist";
      return false;
    }
  }
  else {
    // choices is default to tradeframe.hdf5
  }

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

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBarChart::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  return true;

}

void AppBarChart::OnFrameMainAutoMove( wxMoveEvent& event ) {
  // load state works properly _after_ first move (library initiated)

  CallAfter(
    [this](){
      LoadState();
      m_pFrameMain->Layout();
      LoadPanelFinancialChart();
    }
  );

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppBarChart::InitStructures( ESymbol eSymbol, const std::string& sName, size_t ixChart, boost::posix_time::time_duration td ) {
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

void AppBarChart::LoadPanelFinancialChart() {

  //m_ptiRoot = m_pPanelFinancialChart->SetRoot( "/", nullptr );

  // inspiration from PanelChartHdf5
  const std::string sFileName( "collector-20250402.hdf5" );
  m_pdm = std::make_unique<ou::tf::HDF5DataManager>( ou::tf::HDF5DataManager::RO, sFileName );

  m_cdv.SetNames( "SPY", sFileName );

    m_pkwmSymbol = new ou::KeyWordMatch<ESymbol>( ESymbol::UKNWN, 6 );
  InitStructures( ESymbol::SPY,  "SPY",    1, boost::posix_time::time_duration( 0, 15, 0 ) );
  //InitStructures( ESymbol::SPY,  "ES-20250620", 1 );
  InitStructures( ESymbol::II6A, "II6A.Z", 2 );
  InitStructures( ESymbol::II6D, "II6D.Z", 3 );
  InitStructures( ESymbol::JT6T, "JT6T.Z", 4 );
  InitStructures( ESymbol::LI6N, "LI6N.Z", 5 );
  InitStructures( ESymbol::TR6T, "TR6T.Z", 6 );

  IterateObjects();

  m_pwcv->SetChartDataView( &m_cdv );
}

void AppBarChart::IterateObjects() {
  ou::tf::hdf5::IterateGroups ig(
    *m_pdm, std::string( "/" ),
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Group(  group, name ); }, // path
    [this]( const std::string& group,const std::string& name ){ HandleLoadTreeHdf5Object( group, name ); }  // timeseries
    );
}

void AppBarChart::HandleLoadTreeHdf5Group( const std::string& sGroup, const std::string& sName ) {
  //BOOST_LOG_TRIVIAL(info) << "1 Group  " << sGroup << ' ' << sName;
}

void AppBarChart::HandleLoadTreeHdf5Object( const std::string& sGroup, const std::string& sName ) {
  // select only ones in the list
  ESymbol eSymbol = m_pkwmSymbol->FindMatch( sName );
  if ( ESymbol::UKNWN != eSymbol ) {
    ou::tf::HDF5Attributes attrObject( *m_pdm, sGroup );
    BOOST_LOG_TRIVIAL(info) << "3 Object," << sGroup << ',' << sName << ',' << attrObject.GetSignature();
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
            indicator.Append( trade.DateTime() - iterSymbol->second.tdDelay, trade.Price() );
          }
          else {
            indicator.Append( trade.DateTime(), trade.Price() );
          }
          if ( bFirst ) {
            first = trade.DateTime();
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

void AppBarChart::SaveState() {
  std::cout << "Saving Config ..." << std::endl;
  std::ofstream ofs( c_sStateFileName );
  boost::archive::text_oarchive oa(ofs);
  oa & *this;
  std::cout << "  done." << std::endl;
}

void AppBarChart::LoadState() {
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

int AppBarChart::OnExit() {
  // Exit Steps: #4
//  DelinkFromPanelProviderControl();  generates stack errors

  return wxAppConsole::OnExit();
}

void AppBarChart::OnClose( wxCloseEvent& event ) {
  // Exit Steps: #2 -> FrameMain::OnClose

  m_pwcv->SetChartDataView( nullptr, false );
  m_mapSymbolInfo.clear();

  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

