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

  if ( !Load( m_options ) ) {
    code = 0;
  }
  else {

    m_pFrameMain = new FrameMain( nullptr, wxID_ANY, sAppName + " - " + m_options.sSymbolName );
    wxWindowID idFrameMain = m_pFrameMain->GetId();
    //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
    //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
    //m_pFrameMain->Center();
  //  m_pFrameMain->Move( -2500, 50 );

    wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
    m_pFrameMain->SetSizer( sizerMain );

    m_pFrameMain->SetSize( 675, 800 );
    SetTopWindow( m_pFrameMain );

    m_pFrameMain->SetAutoLayout( true );
    m_pFrameMain->Layout();
    m_pFrameMain->Show( true );

    m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppDoM::OnClose, this );  // start close of windows and controls

    //Bind(
    //  wxEVT_SIZE,
    //  [this](wxSizeEvent& event){
    //    std::cout << "w=" << event.GetSize().GetWidth() << ",h=" << event.GetSize().GetHeight() << std::endl;
    //    event.Skip();
    //    }//,
    //  //idFrameMain
    //  );

    wxSize size;

    wxBoxSizer* sizerControls = new wxBoxSizer( wxHORIZONTAL );
    sizerMain->Add( sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 4 );

    m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
    sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 4);
    m_pPanelProviderControl->Show( true );

    size = sizerMain->GetSize();

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
      });

    //wxBoxSizer* sizerStatus = new wxBoxSizer( wxHORIZONTAL );
    //sizerMain->Add( sizerStatus, 1, wxEXPAND|wxALL, 5 );

//    m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
//    sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
//    m_pPanelLogging->Show( true );

    ou::tf::Instrument::pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( m_options.sSymbolName );
    m_pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_iqfeed );
    m_pWatch->OnFundamentals.Add( MakeDelegate( this, &AppDoM::OnFundamentals ) );
    m_pWatch->OnQuote.Add( MakeDelegate( this, &AppDoM::OnQuote ) );
    m_pWatch->OnTrade.Add( MakeDelegate( this, &AppDoM::OnTrade ) );

    m_pDispatch = std::make_unique<ou::tf::iqfeed::l2::Symbols>(
      [ this ](){
        m_pDispatch->Single( true );
        m_pDispatch->WatchAdd(
          m_options.sSymbolName,
          [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fBid_
            m_valuesStatistics.nL2MsgBid++;
            m_valuesStatistics.nL2MsgTtl++;
            m_pPanelTrade->OnQuoteBid( price, volume );
          },
          [this]( double price, int volume, bool bAdd ){ // fVolumeAtPrice_t&& fAsk_
            m_valuesStatistics.nL2MsgAsk++;
            m_valuesStatistics.nL2MsgTtl++;
            m_pPanelTrade->OnQuoteAsk( price, volume );
          });
      } );

    std::cout << "watching L1/L2: " << m_options.sSymbolName << std::endl;

    using mi = FrameMain::structMenuItem;  // vxWidgets takes ownership of the objects

    //FrameMain::vpItems_t vItemsLoadSymbols;
    //vItemsLoadSymbols.push_back( new mi( "Market Maker Maps", MakeDelegate( this, &AppDoM::EmitMarketMakerMaps ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "New Symbol List Local", MakeDelegate( this, &AppIQFeedGetHistory::HandleNewSymbolListLocal ) ) );
  //  vItemsLoadSymbols.push_back( new mi( "Local Binary Symbol List", MakeDelegate( this, &AppIQFeedGetHistory::HandleLocalBinarySymbolList ) ) );
    //wxMenu* pMenuSymbols = m_pFrameMain->AddDynamicMenu( "Utility", vItemsLoadSymbols );

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
  m_valuesStatistics.nL1MsgBid++;
  m_valuesStatistics.nL1MsgAsk++;
  m_valuesStatistics.nL1MsgTtl++;
  if ( m_pPanelTrade ) {
    m_pPanelTrade->OnQuote( quote );
  }
}

void AppDoM::OnTrade( const ou::tf::Trade& trade ) {
  m_valuesStatistics.nTicks++;
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
