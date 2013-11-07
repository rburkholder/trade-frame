/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace boost::gregorian;

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/lexical_cast.hpp>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>

#include "StickShift2.h"

IMPLEMENT_APP(AppStickShift)

bool AppStickShift::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Manual Trading" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 800, 500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  // populate variable in FrameWork01
  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();

  m_pPanelManualOrder = new ou::tf::PanelManualOrder( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelManualOrder, 0, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelManualOrder->Show( true );

/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND|wxALIGN_LEFT, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/

  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  m_db.OnRegisterTables.Add( MakeDelegate( this, &AppStickShift::HandleRegisterTables ) );
  m_db.OnRegisterRows.Add( MakeDelegate( this, &AppStickShift::HandleRegisterRows ) );
  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppStickShift::HandlePopulateDatabase ) );

  m_bData1Connected = false;
  m_bExecConnected = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_timerGuiRefresh.SetOwner( this );

  Bind( wxEVT_TIMER, &AppStickShift::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppStickShift::OnClose, this );  // start close of windows and controls

  m_pPanelManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppStickShift::HandlePanelNewOrder ) );
  m_pPanelManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppStickShift::HandlePanelSymbolText ) );
  m_pPanelManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppStickShift::HandlePanelFocusPropogate ) );

  // maybe set scenario with database and with in memory data structure
  m_idPortfolio = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "StickShift";

  m_db.Open( "StickShift2.db" );

  m_pFPPOE = new FrameMain( m_pFrameMain, wxID_ANY, "Portfolio Management" );

//  m_pMPPOE = new MPPOE_t;

//  m_pPPPOE = new PPPOE_t( m_pMPPOE, m_pFPPOE );
//  m_pPPPOE->Show();

//  m_pCPPOE = new CPPOE_t( m_pMPPOE, m_pPPPOE );
//  m_pCPPOE->LoadInitialData();
  
  m_pFPPOE->Show();

  return 1;

}

void AppStickShift::HandleGuiRefresh( wxTimerEvent& event ) {
  // update portfolio results and tracker timeseries for portfolio value
  double dblUnRealized;
  double dblRealized;
  double dblCommissionsPaid;
/*  m_pPortfolio->QueryStats( dblUnRealized, dblRealized, dblCommissionsPaid );
  double dblCurrent = dblUnRealized + dblRealized - dblCommissionsPaid;
  m_dblMaxPL = std::max<double>( m_dblMaxPL, dblCurrent );
  m_dblMinPL = std::min<double>( m_dblMinPL, dblCurrent );
  m_pPanelPortfolioStats->SetStats( 
    boost::lexical_cast<std::string>( m_dblMinPL ),
    boost::lexical_cast<std::string>( dblCurrent ),
    boost::lexical_cast<std::string>( m_dblMaxPL )
    );
    */
}

int AppStickShift::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  if ( m_db.IsOpen() ) m_db.Close();

//  delete m_pCPPOE;
//  m_pCPPOE = 0;

  return 0;
}

void AppStickShift::HandleRegisterTables(  ou::db::Session& session ) {
}

void AppStickShift::HandleRegisterRows(  ou::db::Session& session ) {
}

void AppStickShift::HandlePopulateDatabase( void ) {

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountSimulator
    = ou::tf::AccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

  std::string sNull;

  m_pPortfolio
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( 
      m_idPortfolio, "aoRay", sNull, ou::tf::Portfolio::Master, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "StickShift" );

  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::USD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::USD ], "Currency Monitor" );
  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "Currency Monitor" );
  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "Currency Monitor" );
  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "Currency Monitor" );
  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
    ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "Currency Monitor" );
    
}

void AppStickShift::HandlePanelNewOrder( const ou::tf::PanelManualOrder::Order_t& order ) {
  try {
    ou::tf::InstrumentManager& mgr( ou::tf::InstrumentManager::Instance() );
    //pInstrument_t pInstrument = m_vManualOrders[ m_curDialogManualOrder ].pInstrument;
    pInstrument_t pInstrument = m_IBInstrumentInfo.pInstrument;
    if ( !mgr.Exists( pInstrument ) ) {
      mgr.Register( pInstrument );
    }
    if ( 0 == m_pPosition.get() ) {
      m_pPosition = ou::tf::PortfolioManager::Instance().ConstructPosition( 
        m_idPortfolio, pInstrument->GetInstrumentName(), "manual", "ib01", "ib01", m_pExecutionProvider, m_pData1Provider, pInstrument );
    }
    ou::tf::OrderManager& om( ou::tf::OrderManager::Instance() );
    ou::tf::OrderManager::pOrder_t pOrder;
    switch ( order.eOrderType ) {
    case ou::tf::OrderType::Market: 
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity );
      m_pPosition->PlaceOrder( ou::tf::OrderType::Market, order.eOrderSide, order.nQuantity );
      break;
    case ou::tf::OrderType::Limit:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      m_pPosition->PlaceOrder( ou::tf::OrderType::Limit, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    case ou::tf::OrderType::Stop:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      m_pPosition->PlaceOrder( ou::tf::OrderType::Stop, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    }
    //ou::tf::OrderManager::pOrder_t pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1, order.dblPrice2 );
    //om.PlaceOrder( m_tws.get(), pOrder );
  }
  catch (...) {
    int i = 1;
  }
}

void AppStickShift::HandlePanelSymbolText( const std::string& sName ) {
  ou::tf::IBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "STK";
  contract.symbol = sName;
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_pPanelManualOrder->SetInstrumentDescription( "" );
  m_tws->RequestContractDetails( 
    contract, 
    MakeDelegate( this, &AppStickShift::HandleIBContractDetails ), MakeDelegate( this, &AppStickShift::HandleIBContractDetailsDone ) );
}

void AppStickShift::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {
  //assert( m_curDialogManualOrder < m_vManualOrders.size() );
  //m_vManualOrders[ m_curDialogManualOrder ].details = details;
  //m_vManualOrders[ m_curDialogManualOrder ].pInstrument = pInstrument;
  //m_vManualOrders[ m_curDialogManualOrder ].pDialogManualOrder->SetInstrumentDescription( details.longName );
  m_IBInstrumentInfo.details = details;
  m_IBInstrumentInfo.pInstrument = pInstrument;
  m_pPanelManualOrder->SetInstrumentDescription( details.longName );
}

void AppStickShift::HandleIBContractDetailsDone( void ) {
}

void AppStickShift::HandlePanelFocusPropogate( unsigned int ix ) {
}

void AppStickShift::OnClose( wxCloseEvent& event ) {
  m_timerGuiRefresh.Stop();
  DelinkFromPanelProviderControl();
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

void AppStickShift::OnData1Connected( int ) {
  m_bData1Connected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppStickShift::OnExecConnected( int ) {
  m_bExecConnected = true;
  if ( m_bData1Connected & m_bExecConnected ) {
    // set start to enabled
  }
}

void AppStickShift::OnData1Disconnected( int ) {
  m_bData1Connected = false;
}

void AppStickShift::OnExecDisconnected( int ) {
  m_bExecConnected = false;
}
