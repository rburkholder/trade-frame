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
  show daily bars for a select number instruments
  show live data for a select number of instruments

  * load up tree from config file
  * use indicator trading for example of loading daily bars
*/

#include <boost/log/trivial.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>
#include <wx/textdlg.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelFinancialChart.hpp>

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

  //if ( config::Load( c_sChoicesFilename, m_choices ) ) {
  //}
  //else {
  //  return false;
  //}

  m_pFrameMain = new FrameMain( 0, wxID_ANY, c_sAppTitle );
  wxWindowID idFrameMain = m_pFrameMain->GetId();

  m_pFrameMain->SetSize( 800, 500 );

  SetTopWindow( m_pFrameMain );

  wxBoxSizer* sizerFrame;
  wxBoxSizer* sizerUpper;
  wxBoxSizer* sizerLower;

  sizerFrame = new wxBoxSizer( wxVERTICAL );
  m_pFrameMain->SetSizer( sizerFrame );

  m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain );
  sizerFrame->Add( m_pPanelFinancialChart, 1, wxALL | wxEXPAND, 0 );
  m_pPanelFinancialChart->GetWinChartView()->SetReview();

  //m_pwcv = new ou::tf::WinChartView( m_pFrameMain );
  //sizerFrame->Add( m_pwcv, 1,wxALL | wxEXPAND, 0 );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBarChart::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  return true;

}

void AppBarChart::OnFrameMainAutoMove( wxMoveEvent& event ) {
  // load state works properly _after_ first move (library initiated)

  CallAfter(
    [this](){

      m_ptiRoot = m_pPanelFinancialChart->SetRoot( "Charts", nullptr );

      LoadState();
      m_pFrameMain->Layout();

      m_pBarHistory = ou::tf::iqfeed::BarHistory::Construct(
        [this](){
          CallAfter(
            [this](){ // run in gui thread
              LoadPanelFinancialChart();
            } );

        } );
      assert( m_pBarHistory );
      m_pBarHistory->Connect();
    }
  );

  m_pFrameMain->Unbind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this );

  event.Skip(); // set to false if we want to ignore auto move

}

void AppBarChart::LoadPanelFinancialChart() {

  m_ptiRoot->SetOnBuildPopUp(
    [this]( ou::tf::TreeItem* pti ){
      pti->NewMenu();
      pti->AppendMenuItem(
        "Add Group",
        [this]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog* dialog = new wxTextEntryDialog( m_pFrameMain, "Group Name:", "Add Group" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog->ShowModal() ) {
            std::string sGroupName = dialog->GetValue().Upper();
            if ( 0 < sGroupName.size() ) {
              ou::tf::TreeItem* ptiGroup = LoadGroupInfo( sGroupName, m_ptiRoot );
              }
          }
        });
      pti->AppendMenuItem(
        "Sort",
        []( ou::tf::TreeItem* pti ){
          pti->SortChildren();
        } );
    }
  );

}

ou::tf::TreeItem* AppBarChart::LoadGroupInfo( const std::string& sGroupName, ou::tf::TreeItem* ptiRoot ) {

  ou::tf::TreeItem* ptiGroup = m_ptiRoot->AppendChild( sGroupName );

  ptiGroup->SetOnBuildPopUp(
    [this]( ou::tf::TreeItem* ptiGroup ){
      ptiGroup->NewMenu();
      ptiGroup->AppendMenuItem(
        "Add Symbol",
        [this]( ou::tf::TreeItem* ptiGroup ){
          wxTextEntryDialog* dialog = new wxTextEntryDialog( m_pFrameMain, "Symbol Name:", "Add Symbol" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog->ShowModal() ) {
            std::string sSymbolName = dialog->GetValue().Upper();
            if ( 0 < sSymbolName.size() ) {
              LoadSymbolInfo( sSymbolName, ptiGroup );
              }
          }
        } );
      ptiGroup->AppendMenuItem(
        "Rename",
        [this]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog* dialog = new wxTextEntryDialog( m_pFrameMain, "New Group Name:", "Change Group Name" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog->ShowModal() ) {
            std::string sGroupName = dialog->GetValue().Upper();
            if ( 0 < sGroupName.size() ) {
              pti->UpdateText( sGroupName );
              }
          }
        } );
      ptiGroup->AppendMenuItem(
        "Sort",
        []( ou::tf::TreeItem* pti ){
          pti->SortChildren();
        } );
    } );

  return ptiGroup;
}

bool AppBarChart::LoadSymbolInfo( const std::string& sSecurityName, ou::tf::TreeItem* pti ) {
  bool bAdded( true );
  mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSecurityName );
  if ( m_mapSymbolInfo.end() != iterSymbolInfo ) {
    BOOST_LOG_TRIVIAL(error) << "Ignoring duplicate security: " << sSecurityName;
    bAdded = false;
  }
  else {
    auto result = m_mapSymbolInfo.emplace( sSecurityName, SymbolInfo() );
    assert( result.second );
    iterSymbolInfo = result.first;
    SymbolInfo& si( iterSymbolInfo->second );

    si.m_pti = pti->AppendChild( sSecurityName );
    si.m_cePriceBars.SetName( "Daily" );

    si.m_dvChart.Add( EChartSlot::Price, &si.m_cePriceBars );
    si.m_dvChart.Add( EChartSlot::Volume, &si.m_ceVolume );

    si.m_pti->SetOnClick(
      [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
        //BOOST_LOG_TRIVIAL(info) << "clicked: " << iterSymbolInfo->first;
        SymbolInfo& si( iterSymbolInfo->second );
        if ( si.m_bBarsLoaded ) {
          /*
          ou::ChartEntryTime::range_t range( si.m_dvChart.GetViewPort() );
          if ( range.dtBegin == range.dtEnd ) {
            range = si.m_dvChart.GetExtents();
            si.m_dvChart.SetViewPort( range );
          }
          else {
            boost::posix_time::time_duration td = range.dtEnd - range.dtBegin;
            auto ticks( td.ticks() );
          }
          */
          m_pPanelFinancialChart->SetChartDataView( &si.m_dvChart );
        }
        else {
          m_pBarHistory->Set(
            [&si]( const ou::tf::Bar& bar ){ // fBar_t&&
              si.m_cePriceBars.AppendBar( bar );
              si.m_ceVolume.Append( bar );
            },
            [this,&si](){ // fDone_t&&
              m_pPanelFinancialChart->SetChartDataView( &si.m_dvChart );
            } );
          m_pBarHistory->RequestNEndOfDay( iterSymbolInfo->first, 200 );
          si.m_bBarsLoaded = true;
        }
      } );
    si.m_pti->SetOnBuildPopUp(
      [this]( ou::tf::TreeItem* pti ){
        pti->NewMenu();
        pti->AppendMenuItem(
          "Delete",
          [this]( ou::tf::TreeItem* pti ){
            m_pPanelFinancialChart->SetChartDataView( nullptr );
            mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( pti->GetText() );
            assert( m_mapSymbolInfo.end() != iterSymbolInfo );
            m_mapSymbolInfo.erase( iterSymbolInfo );
            pti->Delete();
          } );
      } );
  }
  return bAdded;
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

  //m_pwcv->SetChartDataView( nullptr, false );

  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  if ( m_pBarHistory ) {
    m_pBarHistory->Disconnect();
  }
  SaveState();
  event.Skip();  // auto followed by Destroy();
}

