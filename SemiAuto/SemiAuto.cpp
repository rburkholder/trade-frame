/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <cassert>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>

#include "SemiAuto.h"

using namespace fastdelegate;

IMPLEMENT_APP(AppSemiAuto)

bool AppSemiAuto::OnInit() {

  m_bWatchingOptions = false;
  m_bTrading = false;

  m_bExecConnected = false;
  m_bData1Connected = false;
  m_bData2Connected = false;

  m_tws.reset( new CIBTWS( "U215226" ) );
  m_bIBConnected = false;

  m_iqfeed.reset( new CIQFeedProvider() );
  m_bIQFeedConnected = false;

  m_sim.reset( new CSimulationProvider() ); 
  m_bSimConnected = false;

  m_eMode = EModeLive;

  // this is where we select which provider we will be working with on this run
  // providers need to be registered in order for portfolio/position loading to function properly
  // key needs to match to account
  CProviderManager::Instance().Register( "ib01", static_cast<pProvider_t>( m_tws ) );
  CProviderManager::Instance().Register( "iq01", static_cast<pProvider_t>( m_iqfeed ) );
  CProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

  std::string sDbName;

  switch ( m_eMode ) {
    case EModeSimulation:
      sDbName = ":memory:";
      m_pExecutionProvider = m_sim;
      m_pData1Provider = m_sim;
      break;
    case EModeLive:
      sDbName = "SemiAuto.db";
      m_pExecutionProvider = m_tws;
      m_pData1Provider = m_tws;
      break;
  }

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppSemiAuto::HandlePopulateDatabase ) );

  assert( 0 != sDbName.length() );
  m_db.Open( sDbName );

  m_tws->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIBConnected ) );
  m_tws->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIBDisConnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIQFeedConnected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIQFeedDisConnected ) );

  m_sim->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleSimulatorDisConnected ) );

  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnExecDisconnected ) );

  m_pData1Provider->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData1Connected ) );
  m_pData1Provider->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData1Disconnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData2Disconnected ) );

//  m_pThreadMain = new ThreadMain( EModeLive );

  m_FrameMain = new FrameMain("Semi-Auto Trading", wxPoint(50,50), wxSize(600,900));
  m_FrameMain->Show(TRUE);
  SetTopWindow(m_FrameMain);

  m_FrameMain->SetCreateNewFrameManualOrder( MakeDelegate( this, &AppSemiAuto::HandleCreateNewFrameManualOrder ) );
  m_FrameMain->OnCleanUpForExit.Add( MakeDelegate( this, &AppSemiAuto::HandleOnCleanUpForExitForFrameMain ) );

  m_FrameProviderControl = new FrameProviderControl( m_FrameMain );
  m_FrameProviderControl->Show( true );

  m_FrameProviderControl->SetOnIBStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIBStateChangeRequest ) );
  m_FrameProviderControl->SetOnIQFeedStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIQFeedStateChangeRequest ) ) ;
  m_FrameProviderControl->SetOnSimulatorStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleSimulatorStateChangeRequest ) );

  CInstrumentManager& mgr( CInstrumentManager::Instance() );

  if ( mgr.Exists( "+GCV11" ) ) {
    m_vInstruments.push_back( InstrumentData( mgr.Get( "+GCV11" ) ) );
  }
  else {
    m_vInstruments.push_back( InstrumentData( mgr.ConstructFuture( "+GCV11", "SMART", 2011, 10 ) ) );  // October
  }

  if ( mgr.Exists( "+GCZ11" ) ) {
    m_vInstruments.push_back( InstrumentData( mgr.Get( "+GCZ11" ) ) );
  }
  else {
    m_vInstruments.push_back( InstrumentData( mgr.ConstructFuture( "+GCZ11", "SMART", 2011, 12 ) ) );  // December
  }

  return TRUE;
}

int AppSemiAuto::OnExit() {

  m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnExecDisconnected ) );

  m_pData1Provider->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData1Connected ) );
  m_pData1Provider->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData1Disconnected ) );

  m_iqfeed->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData2Disconnected ) );

  m_tws->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleIBConnected ) );
  m_tws->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleIBDisConnected ) );

  m_iqfeed->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleIQFeedConnected ) );
  m_iqfeed->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleIQFeedDisConnected ) );

  m_sim->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleSimulatorDisConnected ) );

  m_db.Close();

//  delete m_pThreadMain;
  return 0;
}
 

void AppSemiAuto::HandleStateChangeRequest( eProviderState_t state, bool& flag, pProvider_t p ) {
  switch ( state ) {
  case eProviderState_t::ProviderOff:
    assert( !flag );
    break;
  case eProviderState_t::ProviderGoingOn:
    if ( !flag ) {
      p->Connect();
    }
    break;
  case eProviderState_t::ProviderOn:
    assert( flag );
    break;
  case eProviderState_t::ProviderGoingOff:
    if ( flag ) {
      p->Disconnect();
    }
    break;
  }
}

void AppSemiAuto::HandleIBStateChangeRequest( eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bIBConnected, m_tws );
}

void AppSemiAuto::HandleIQFeedStateChangeRequest( eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bIQFeedConnected, m_iqfeed );
}

void AppSemiAuto::HandleSimulatorStateChangeRequest( eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bSimConnected, m_sim );
}

void AppSemiAuto::HandleIBConnected( int ) { // cross thread event
  m_bIBConnected = true;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderOn ) );
}

void AppSemiAuto::HandleIQFeedConnected( int ) {  // cross thread event
  m_bIQFeedConnected = true;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderOn ) );
  for ( vInstrumentData_t::iterator iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++iter ) {
    iter->AddQuoteHandler( m_iqfeed );
    iter->AddTradeHandler( m_iqfeed );
  }
}

void AppSemiAuto::HandleSimulatorConnected( int ) { // cross thread event
  m_bSimConnected = true;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderOn ) );
}

void AppSemiAuto::HandleIBDisConnected( int ) {  // cross thread event
  m_bIBConnected = false;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderOff ) );
}

void AppSemiAuto::HandleIQFeedDisConnected( int ) { // cross thread event
  for ( vInstrumentData_t::iterator iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++iter ) {
    iter->RemoveQuoteHandler( m_iqfeed );
    iter->RemoveTradeHandler( m_iqfeed );
  }
  m_bIQFeedConnected = false;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderOff ) );
}

void AppSemiAuto::HandleSimulatorDisConnected( int ) {  // cross thread event
  m_bSimConnected = false;
  m_FrameProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderOff ) );
}

void AppSemiAuto::HandleCreateNewFrameManualOrder( void ) {
  // need to keep a vector of these so can trade multiple symbols simulataneously
  // maybe something like genesis with market depth book built in
  FrameManualOrder* frame = new FrameManualOrder( m_FrameMain );
  m_vFrameManualOrders.push_back( frame );
  frame->SetOnNewOrderHandler( MakeDelegate( this, &AppSemiAuto::HandleManualOrder ) );
  frame->SetOnSymbolTextUpdated( MakeDelegate( this, &AppSemiAuto::HandleCheckSymbolNameAgainstIB ) );
  frame->Show( true );
}

void AppSemiAuto::HandleCheckSymbolNameAgainstIB( const std::string& sSymbol ) {
  ou::tf::CIBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "STK";
  contract.symbol = sSymbol;
  // as IB only responds when symbol is found, 
  m_tws->RequestContractDetails( contract, MakeDelegate( this, &AppSemiAuto::HandleIBContractDetails ), MakeDelegate( this, &AppSemiAuto::HandleIBContractDetailsDone ) );
}

void AppSemiAuto::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& ) {  // need to handle cross thread
}

void AppSemiAuto::HandleIBContractDetailsDone( void ) {  // called only on successful contract found, need to handle cross thread
}

void AppSemiAuto::HandleManualOrder( const ManualOrder_t& order ) {
}

void AppSemiAuto::HandleOnData1Connected(int e) {
  m_bData1Connected = true;
//  HandleOnConnected(e);
//  CIQFeedHistoryQuery<CProcess>::Connect();  
}

void AppSemiAuto::HandleOnData1Disconnected(int e) {
  m_bData1Connected = false;
//  HandleOnConnected(e);
}

void AppSemiAuto::HandleOnData2Connected(int e) {
//  CIQFeedHistoryQuery<CProcess>::Connect();  
  m_bData2Connected = true;
//  HandleOnConnected( e );
}

void AppSemiAuto::HandleOnData2Disconnected(int e) {
  m_bData2Connected = false;
//  HandleOnConnected( e );
}

void AppSemiAuto::HandleOnExecConnected(int e) {
  m_bExecConnected = true;
}

void AppSemiAuto::HandleOnExecDisconnected(int e) {
  m_bExecConnected = false;
}

void AppSemiAuto::HandleOnCleanUpForExitForFrameMain( int ) {

  m_FrameMain->SetCreateNewFrameManualOrder( 0 );

  // this doesn't work properly for user closed windows
  for ( vFrameManualOrder_t::iterator iter = m_vFrameManualOrders.begin(); iter != m_vFrameManualOrders.end(); ++iter ) {
    (*iter)->SetOnNewOrderHandler( 0 );
    (*iter)->Close();
    delete *iter;
  }
  m_vFrameManualOrders.clear();

  m_FrameProviderControl->SetOnIBStateChangeHandler( 0 );
  m_FrameProviderControl->SetOnIQFeedStateChangeHandler( 0 ) ;
  m_FrameProviderControl->SetOnSimulatorStateChangeHandler( 0 );

//  m_FrameProviderControl->Close();

  m_FrameMain->OnCleanUpForExit.Add( MakeDelegate( this, &AppSemiAuto::HandleOnCleanUpForExitForFrameMain ) );

}

void AppSemiAuto::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

//  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
//    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "TradeGldOptions" );

}

