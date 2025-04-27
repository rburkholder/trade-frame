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
#include <boost/lexical_cast.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/checklst.h>

#include <TFTrading/AcquireFundamentals.h>

#include <TFHDF5TimeSeries/HDF5Attribute.h>
#include <TFHDF5TimeSeries/HDF5IterateGroups.h>

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelFinancialChart.hpp>

#include "AppBarChart.hpp"
#include "PanelSymbolInfo.hpp"

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

  sizerFrame = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameMain->SetSizer( sizerFrame );

  wxArrayString m_lbTagsStrings;
  m_clbTags = new wxCheckListBox(
    m_pFrameMain, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    m_lbTagsStrings, wxLB_MULTIPLE|wxLB_EXTENDED|wxLB_NEEDED_SB //|wxLB_SORT
  );
  sizerFrame->Add( m_clbTags, 0, wxGROW|wxALL, 1 );

  m_clbTags->Bind( wxEVT_CHECKLISTBOX, &AppBarChart::HandleCheckListBoxEvent, this );

  m_pPanelFinancialChart = new ou::tf::PanelFinancialChart( m_pFrameMain );
  sizerFrame->Add( m_pPanelFinancialChart, 1, wxALL | wxEXPAND, 1 );
  m_pPanelFinancialChart->GetWinChartView()->SetReview();

  //m_pwcv = new ou::tf::WinChartView( m_pFrameMain );
  //sizerFrame->Add( m_pwcv, 1,wxALL | wxEXPAND, 0 );

  m_pFrameSymbolInfo
    = new wxFrame( m_pFrameMain, wxID_ANY, "Symbol Info",
      wxDefaultPosition, wxDefaultSize,
      wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT
    );
  wxBoxSizer* sizerFrameSymbolInfo = new wxBoxSizer( wxHORIZONTAL );
  m_pFrameSymbolInfo->SetSizer( sizerFrameSymbolInfo );
  m_pPanelSymbolInfo = new PanelSymbolInfo( m_pFrameSymbolInfo, wxID_ANY );
  sizerFrameSymbolInfo->Add( m_pPanelSymbolInfo, 1, wxGROW|wxALL, 0 );
  m_pFrameSymbolInfo->Layout();
  m_pFrameSymbolInfo->Show();

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppBarChart::OnClose, this );  // start close of windows and controls
  m_pFrameMain->Bind( wxEVT_MOVE, &AppBarChart::OnFrameMainAutoMove, this ); // intercept first move
  m_pFrameMain->Show( true ); // triggers the auto move

  m_piqfeed = ou::tf::iqfeed::Provider::Factory();
  m_piqfeed->OnConnected.Add( MakeDelegate( this, &AppBarChart::HandleIQFeedConnected ) );
  m_piqfeed->Connect();

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

void AppBarChart::AddTag( const std::string& sTag, const std::string& sSymbol ) {
  mapTagSymbol_t::iterator iterTagSymbol = m_mapTagSymbol.find( sTag );
  if ( m_mapTagSymbol.end() == iterTagSymbol ) {
    setSymbol_t setSymbol = { sSymbol };
    auto result = m_mapTagSymbol.emplace( sTag, std::move( setSymbol ) );
    assert( result.second );
    CallAfter(
      [this,sTag](){
        wxArrayString rTag;
        rTag.Add( sTag );
        m_clbTags->InsertItems( rTag, 0 );
      } );
  }
  else {
    setSymbol_t& setSymbol( iterTagSymbol->second );
    setSymbol_t::iterator iterSymbol = setSymbol.find( sSymbol );
    if ( setSymbol.end() == iterSymbol ) {
      setSymbol.insert( sSymbol );
    }
  }
}

void AppBarChart::DelTag( const std::string& sTag, const std::string& sSymbol ) {
  mapTagSymbol_t::iterator iterTagSymbol = m_mapTagSymbol.find( sTag );
  if ( m_mapTagSymbol.end() == iterTagSymbol ) {}
  else {
    setSymbol_t& setSymbol( iterTagSymbol->second );
    setSymbol_t::iterator iterSymbol = setSymbol.find( sSymbol );
    if ( setSymbol.end() != iterSymbol ) {
      setSymbol.erase( iterSymbol );
      if ( 0 == setSymbol.size() ) {
        m_mapTagSymbol.erase( iterTagSymbol );
        CallAfter(
          [this,sTag](){
            int n = m_clbTags->FindString( sTag );
            m_clbTags->Delete( n );
          } );
      }
    }
  }
}

void AppBarChart::LoadPanelFinancialChart() {

  m_ptiRoot->SetOnBuildPopUp(
    [this]( ou::tf::TreeItem* pti ){
      pti->NewMenu();
      pti->AppendMenuItem(
        "Add Symbol",
        [this]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog dialog( m_pFrameMain, "Symbol Name:", "Add Symbol" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog.ShowModal() ) {
            std::string sSymbolName = dialog.GetValue().Upper();
            if ( 0 < sSymbolName.size() ) {
              mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSymbolName );
              //assert( m_mapSymbolInfo.end() == iterSymbolInfo ); // symbols are unique across groups
              if ( m_mapSymbolInfo.end() != iterSymbolInfo ) {
                std::cout << "symbol " << sSymbolName << " already exists" << std::endl;
              }
              else {
                auto result = m_mapSymbolInfo.emplace( sSymbolName, SymbolInfo() );
                assert( result.second );
                LoadSymbolInfo( sSymbolName, pti );
              }
            }
          }
        } );
      pti->AppendMenuItem(
        "Sort",
        []( ou::tf::TreeItem* pti ){
          pti->SortChildren();
        } );
    }
  );

}

void AppBarChart::LoadSymbolInfo( const std::string& sSecurityName, ou::tf::TreeItem* pti ) {

  mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSecurityName );
  assert( m_mapSymbolInfo.end() != iterSymbolInfo );

  //auto result = m_mapSymbolInfo.emplace( sSecurityName, SymbolInfo() );
  //assert( result.second );
  //iterSymbolInfo = result.first;
  SymbolInfo& si( iterSymbolInfo->second );

  si.m_pti = pti->AppendChild( sSecurityName );

  si.m_pti->SetOnClick(
    [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
      //BOOST_LOG_TRIVIAL(info) << "clicked: " << iterSymbolInfo->first;
      SymbolFundamentals( iterSymbolInfo );
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
    [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
      pti->NewMenu();
      pti->AppendMenuItem(
        "Note",
        [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog dialog( m_pFrameMain, "Notes:", "Update Notes" );
          dialog.SetValue( iterSymbolInfo->second.m_sNotes );
          if ( wxID_OK == dialog.ShowModal() ) {
            std::string sNotes = dialog.GetValue();
            iterSymbolInfo->second.m_sNotes = sNotes;
          }
        } );
      pti->AppendMenuItem(
        "Add Tag",
        [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog dialog( m_pFrameMain, "Tag Name:", "Add Tag" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog.ShowModal() ) {
            std::string sTag = dialog.GetValue().Upper();
            if ( 0 < sTag.size() ) {
              AddTag( sTag, iterSymbolInfo->first );
              CallAfter(
                [this](){
                  FilterByTag();
              } );
            }
          }
        } );
        pti->AppendMenuItem(
          "Delete Tag",
          [this,iterSymbolInfo]( ou::tf::TreeItem* pti ){
            wxTextEntryDialog dialog( m_pFrameMain, "Tag Name:", "Delete Tag" );
            //dialog->ForceUpper(); // prints charters in reverse
            if ( wxID_OK == dialog.ShowModal() ) {
              std::string sTag = dialog.GetValue().Upper();
              if ( 0 < sTag.size() ) {
                DelTag( sTag, iterSymbolInfo->first );
                CallAfter(
                  [this](){
                    FilterByTag();
                } );
              }
            }
          } );
        pti->AppendMenuItem(
        "Delete",
        [this]( ou::tf::TreeItem* pti ){
          m_pPanelFinancialChart->SetChartDataView( nullptr );
          const std::string sSymbol( pti->GetText() );
          mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSymbol );
          assert( m_mapSymbolInfo.end() != iterSymbolInfo );

          for ( mapTagSymbol_t::value_type& vt: m_mapTagSymbol ) {
            CallAfter(
              [this,sTag_=vt.first,sSymbol_=std::move(sSymbol)](){
                DelTag( sTag_, sSymbol_ );
              } );
          }

          m_mapSymbolInfo.erase( iterSymbolInfo );
          pti->Delete();
        } );
    } );
}

void AppBarChart::HandleCheckListBoxEvent( wxCommandEvent& event ) {
  auto id = event.GetSelection();
  bool b( m_clbTags->IsChecked( id ) );
  //std::cout << "selection " << id << ',' << b << std::endl;
  FilterByTag();
  event.Skip();
}

void AppBarChart::FilterByTag() {

  m_ptiRoot->DeleteChildren();

  wxArrayInt rChecked;
  unsigned int nChecked = m_clbTags->GetCheckedItems( rChecked );

  if ( 0 == nChecked ) { // show all
    for ( mapSymbolInfo_t::value_type& vt: m_mapSymbolInfo ) {
      LoadSymbolInfo( vt.first, m_ptiRoot );
    }
  }
  else { // show subset
    setSymbol_t setSymbol_aggregate;
    for ( wxArrayInt::value_type vt: rChecked ) {
      std::string sTag( m_clbTags->GetString( vt ) );
      mapTagSymbol_t::iterator iterTagSymbol = m_mapTagSymbol.find( sTag );
      if ( m_mapTagSymbol.end() != iterTagSymbol ) {
        setSymbol_t& setSymbol_tag( iterTagSymbol->second );
        setSymbol_aggregate.insert( setSymbol_tag.begin(), setSymbol_tag.end() );
      }
    }
    for ( const setSymbol_t::value_type& vt: setSymbol_aggregate ) {
      mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( vt );
      if ( m_mapSymbolInfo.end() == iterSymbolInfo ) {
        std::cout << "FilterByTag symbol " << vt << " not found" << std::endl;
      }
      else {
        LoadSymbolInfo( vt, m_ptiRoot );
      }
    }
  }
}

void AppBarChart::HandleIQFeedConnected( int ) {
}

void AppBarChart::SymbolFundamentals( mapSymbolInfo_t::iterator iterSymbolInfo ) {

  auto f =
    [this]( mapSymbolInfo_t::iterator iterSymbolInfo ){
      KeyInfo& ki( iterSymbolInfo->second.m_key_info );

      //std::cout
      //<< iterSymbolInfo->first
      //<< ",yld=" << ki.dblYield
      //<< ",lst=" << ki.dblLast
      //<< ",rate=" << ki.dblRate
      //<< "," << ki.sCompanyName
      //<< std::endl;

      PanelSymbolInfo::Fields fields;
      fields.sYield = boost::lexical_cast<std::string>( ki.dblYield );
      fields.sLast = boost::lexical_cast<std::string>( ki.dblLast );
      fields.sAmount = boost::lexical_cast<std::string>( ki.dblAmount );
      fields.sRate = boost::lexical_cast<std::string>( ki.dblRate );
      fields.sExDiv = std::string();
      fields.sPayed = std::string();
      fields.sNotes = std::string();
      fields.sName = boost::lexical_cast<std::string>( ki.sCompanyName );
      fields.fBtnUndo =
        []()->std::string{
          return std::string();
        };
      fields.fBtnSave =
        []( const std::string& sNotes ){

        };
      m_pPanelSymbolInfo->SetFields( fields );
    };

  if ( iterSymbolInfo->second.m_key_info.bLoaded ) {
    f( iterSymbolInfo );
  }
  else {
    using pWatch_t = ou::tf::Watch::pWatch_t;
    using Summary = ou::tf::Watch::Summary;
    using Fundamentals = ou::tf::Watch::Fundamentals;
    using pInstrument_t = ou::tf::Instrument::pInstrument_t;

    const std::string& sSymbol( iterSymbolInfo->first );

    assert( m_piqfeed );
    assert( m_piqfeed->Connected() );

    pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sSymbol );
    pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sSymbol );
    pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

    assert( nullptr == m_pAcquireFundamentals );

    m_pAcquireFundamentals
      = ou::tf::AcquireFundamentals::Factory (
        std::move( pWatch ),
        [this,iterSymbolInfo,f_=std::move(f)]( pWatch_t pWatch ){
          KeyInfo& ki( iterSymbolInfo->second.m_key_info );
          const Summary& summary( pWatch->GetSummary() );
          ki.dblLast = summary.dblTrade;
          const Fundamentals& fundamentals( pWatch->GetFundamentals() );
          ki.sCompanyName = fundamentals.sCompanyName;
          ki.dblRate = fundamentals.dblDividendRate;
          ki.dblYield = fundamentals.dblDividendYield;
          ki.dblAmount = fundamentals.dblDividendAmount;
          //dividend.datePayed = fundamentals.datePayed;
          //dividend.dateExDividend = fundamentals.dateExDividend;

          f_( iterSymbolInfo );

          m_pAcquireFundamentals_burial = std::move( m_pAcquireFundamentals );
          m_pAcquireFundamentals.reset();
          ki.bLoaded = true;

        }
      );
      m_pAcquireFundamentals->Start();
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

  //m_pwcv->SetChartDataView( nullptr, false );

  //DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a
  if ( m_pBarHistory ) {
    m_pBarHistory->Disconnect();
  }

  m_pAcquireFundamentals_burial.reset();

  m_piqfeed->Disconnect();
  m_piqfeed.reset();

  m_clbTags->Unbind( wxEVT_CHECKLISTBOX, &AppBarChart::HandleCheckListBoxEvent, this );
  m_pFrameMain->Unbind( wxEVT_CLOSE_WINDOW, &AppBarChart::OnClose, this );

  SaveState();
  event.Skip();  // auto followed by Destroy();
}

