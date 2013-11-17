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

  // maybe set scenario with database and with in memory data structure
//  m_idPortfolio = boost::gregorian::to_iso_string( boost::gregorian::day_clock::local_day() ) + "StickShift";
  m_idPortfolio = "StickShift";  // keeps name constant over multiple days

  std::string sDbName( "StickShift2.db" );
  if ( boost::filesystem::exists( sDbName ) ) {
    boost::filesystem::remove( sDbName );
  }

  m_db.Open( sDbName );

  m_bData1Connected = false;
  m_bExecConnected = false;

  m_dblMinPL = m_dblMaxPL = 0.0;

  m_pIQFeedSymbolListOps = new ou::tf::IQFeedSymbolListOps( m_listIQFeedSymbols ); 

  FrameMain::vpItems_t vItems;
  typedef FrameMain::structMenuItem mi;  // vxWidgets takes ownership of the objects
  vItems.push_back( new mi( "a1 New Symbol List Remote", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListRemote ) ) );
  vItems.push_back( new mi( "a2 New Symbol List Local", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::ObtainNewIQFeedSymbolListLocal ) ) );
  vItems.push_back( new mi( "a3 Load Symbol List", MakeDelegate( m_pIQFeedSymbolListOps, &ou::tf::IQFeedSymbolListOps::LoadIQFeedSymbolList ) ) );
  vItems.push_back( new mi( "a4 Save Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionSaveSymbolSubset ) ) );
  vItems.push_back( new mi( "a5 Load Symbol Subset", MakeDelegate( this, &AppStickShift::HandleMenuActionLoadSymbolSubset ) ) );
  m_pFrameMain->AddDynamicMenu( "Actions", vItems );


  m_timerGuiRefresh.SetOwner( this );

  Bind( wxEVT_TIMER, &AppStickShift::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_pFrameMain->Bind( wxEVT_CLOSE_WINDOW, &AppStickShift::OnClose, this );  // start close of windows and controls

  m_pPanelManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppStickShift::HandlePanelNewOrder ) );
  m_pPanelManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppStickShift::HandlePanelSymbolText ) );
  m_pPanelManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppStickShift::HandlePanelFocusPropogate ) );

  m_pFPPOE = new FrameMain( m_pFrameMain, wxID_ANY, "Portfolio Management", wxDefaultPosition, wxSize( 800, 400 ),  
    wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
    );

  m_sizerPM = new wxBoxSizer(wxVERTICAL);
  m_pFPPOE->SetSizer(m_sizerPM);

  //m_scrollPM = new wxScrolledWindow( m_pFPPOE, -1, wxDefaultPosition, wxSize(200, 400), wxVSCROLL );
  m_scrollPM = new wxScrolledWindow( m_pFPPOE, -1, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerPM->Add(m_scrollPM, 1, wxGROW|wxALL, 5);
  m_scrollPM->SetScrollbars(1, 1, 0, 0);

  m_sizerScrollPM = new wxBoxSizer(wxVERTICAL);
  m_scrollPM->SetSizer( m_sizerScrollPM );

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Add( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );
  pm.LoadActivePortfolios();

  m_pFPPOE->Show();

  Bind( EVENT_IB_INSTRUMENT, &AppStickShift::HandleIBInstrument, this );

  return 1;

}

void AppStickShift::HandleMenuActionSaveSymbolSubset( void ) {

  m_vExchanges.clear();
  m_vExchanges.insert( "NYSE" );
  //m_vExchanges.push_back( "NYSE_AMEX" );
  //m_vExchanges.push_back( "NYSE,ARCA" );
  m_vExchanges.insert( "NGSM" );
  //m_vExchanges.push_back( "NASDAQ,NMS" );
  //m_vExchanges.push_back( "NASDAQ,SMCAP" );
  //m_vExchanges.push_back( "NASDAQ,OTCBB" );
  //m_vExchanges.push_back( "NASDAQ,OTC" );
  //m_vExchanges.insert( "CANADIAN,TSE" );  // don't do yet, simplifies contract creation for IB

  m_vClassifiers.insert( ou::tf::IQFeedSymbolListOps::classifier_t::Equity );

  std::cout << "Subsetting symbols ... " << std::endl;
  ou::tf::iqfeed::InMemoryMktSymbolList listIQFeedSymbols;
  m_listIQFeedSymbols.SelectSymbolsByExchange( m_vExchanges.begin(), m_vExchanges.end(), ou::tf::IQFeedSymbolListOps::SelectSymbols( m_vClassifiers, listIQFeedSymbols ) );
  std::cout << "  " << listIQFeedSymbols.Size() << " symbols in subset." << std::endl;

  std::string sFileName( "stickshift.ser" );
  std::cout << "Saving subset to " << sFileName << " ..." << std::endl;
//  listIQFeedSymbols.HandleParsedStructure( m_listIQFeedSymbols.GetTrd( m_sNameUnderlying ) );
//  m_listIQFeedSymbols.SelectOptionsByUnderlying( m_sNameOptionUnderlying, listIQFeedSymbols );
  listIQFeedSymbols.SaveToFile( sFileName );  // __.ser
  std::cout << " ... done." << std::endl;

  // next step will be to add in the options for the underlyings selected.
}

void AppStickShift::HandleMenuActionLoadSymbolSubset( void ) {
  std::string sFileName( "stickshift.ser" );
  std::cout << "Loading From " << sFileName << " ..." << std::endl;
  m_listIQFeedSymbols.LoadFromFile( sFileName );  // __.ser
  std::cout << "  " << m_listIQFeedSymbols.Size() << " symbols loaded." << std::endl;
}

void AppStickShift::HandlePortfolioLoad( const idPortfolio_t& idPortfolio ) {
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  ou::tf::PanelPortfolioPosition* pPPP( new ou::tf::PanelPortfolioPosition( m_scrollPM ) );
  m_sizerScrollPM->Add( pPPP, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
  pPPP->SetPortfolio( pm.GetPortfolio( idPortfolio ) );
  pPPP->SetNameLookup( MakeDelegate( this, &AppStickShift::LookupDescription ) );
  pPPP->SetConstructPosition( MakeDelegate( this, &AppStickShift::ConstructEquityPosition0 ) );
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

  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance() );
  pm.OnPortfolioLoaded.Remove( MakeDelegate( this, &AppStickShift::HandlePortfolioLoad ) );

//  DelinkFromPanelProviderControl();  generates stack errors
  //m_timerGuiRefresh.Stop();
  if ( m_db.IsOpen() ) m_db.Close();

//  delete m_pCPPOE;
//  m_pCPPOE = 0;

  return 0;
}

void AppStickShift::LookupDescription( const std::string& sSymbolName, std::string& sDescription ) {
  sDescription = "";
  try {
    const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t& trd( m_listIQFeedSymbols.GetTrd( sSymbolName ) );
    sDescription = trd.sDescription;
  }
  catch ( std::runtime_error& e ) {
  }
}

void AppStickShift::ConstructEquityPosition0( const std::string& sName, pPortfolio_t pPortfolio, DelegateAddPosition_t function ) {

  m_EquityPositionCallbackInfo.pPortfolio = pPortfolio;
  m_EquityPositionCallbackInfo.function = function;

  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
  ou::tf::Instrument::pInstrument_t pInstrument;

  if ( im.Exists( sName, pInstrument ) ) {
    ConstructEquityPosition1( pInstrument );
  }
  else {
    ou::tf::IBTWS::Contract contract;
    contract.currency = "USD";
    contract.exchange = "SMART";
    contract.secType = "STK";
    contract.symbol = sName;
    m_tws->RequestContractDetails( 
      contract, 
      MakeDelegate( this, &AppStickShift::HandleIBContractDetails ), MakeDelegate( this, &AppStickShift::HandleIBContractDetailsDone ) );
  }
}

void AppStickShift::ConstructEquityPosition1( pInstrument_t& pInstrument ) {
  ou::tf::PortfolioManager& pm( ou::tf::PortfolioManager::GlobalInstance().Instance() );
  pPosition_t pPosition( pm.ConstructPosition( 
    m_EquityPositionCallbackInfo.pPortfolio->GetRow().idPortfolio,
    pInstrument->GetInstrumentName(),
    "",
    "ib01", "iq01", this->m_pExecutionProvider, this->m_pData1Provider,
    pInstrument ) 
    );
  if ( 0 != m_EquityPositionCallbackInfo.function ) {
    m_EquityPositionCallbackInfo.function( pPosition );
  }
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
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::CAD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::EUR ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::AUD ], "Currency Monitor" );
//  ou::tf::PortfolioManager::Instance().ConstructPortfolio(
//    ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "aoRay", m_idPortfolio, ou::tf::Portfolio::CurrencySummary, ou::tf::Currency::Name[ ou::tf::Currency::GBP ], "Currency Monitor" );
    
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
  QueueEvent( new EventIBInstrument( EVENT_IB_INSTRUMENT, -1, pInstrument ) );
  // this is in another thread, and should be handled in a thread safe manner.
}

void AppStickShift::HandleIBContractDetailsDone( void ) {
}

void AppStickShift::HandleIBInstrument( EventIBInstrument& event ) {
  ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
  im.Register( event.GetInstrument() ); // by stint of being here, should be new, and therefore registerable
  ConstructEquityPosition1( event.GetInstrument() );
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
