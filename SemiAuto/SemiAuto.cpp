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

#pragma warning( disable: 4996 4482 )

#include <cassert>

#include <OUCommon/TimeSource.h>

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
  m_FrameMain->SetSaveSeriesEvent( MakeDelegate( this, &AppSemiAuto::HandleSaveSeriesEvent ) );

  m_FrameMain->OnCleanUpForExit.Add( MakeDelegate( this, &AppSemiAuto::HandleOnCleanUpForExitForFrameMain ) );

  m_FrameProviderControl = new FrameProviderControl( m_FrameMain );
  m_FrameProviderControl->Show( true );

  m_FrameProviderControl->SetOnIBStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIBStateChangeRequest ) );
  m_FrameProviderControl->SetOnIQFeedStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIQFeedStateChangeRequest ) ) ;
  m_FrameProviderControl->SetOnSimulatorStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleSimulatorStateChangeRequest ) );

  CInstrumentManager& mgr( CInstrumentManager::Instance() );

  // XAUUSDO.ABBA hvy, XAUUSDO.COMP hvy, XAUUSDO.MIGF med, XAUUSDO.SAXO med, XAUUSDO.UBSW lgt, XAUUSDO.UWCL med, XAUUSDO.WBLT hvy
  // EURUSD.FXCM, BEURUSD
  // HUI.X, XAU.X, GDX, GDXJ
  // DX.X dollar index  https://www.theice.com/productguide/ProductDetails.shtml?specId=194
  // http://www.dtniq.com/template.cfm?navgroup=supportlist&view=1&urlcode=323406&type=TENFORE  // tenfore symbols and descriptions

  // this form of InstrumentData is constructed three times each step, maybe reduce in the future?
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "XAUUSDO.COMP" ) ? mgr.Get( "XAUUSDO.COMP" ) : mgr.ConstructInstrument( "XAUUSDO.COMP", "SMART", ou::tf::InstrumentType::Commodity ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCV11" ) ? mgr.Get( "+GCV11" ) : mgr.ConstructFuture( "+GCV11", "SMART", 2011, 10 ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCZ11" ) ? mgr.Get( "+GCZ11" ) : mgr.ConstructFuture( "+GCZ11", "SMART", 2011, 12 ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "GLD" ) ? mgr.Get( "GLD" ) : mgr.ConstructInstrument( "GLD", "SMART", ou::tf::InstrumentType::Stock ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "EURUSD.COMP" ) ? mgr.Get( "EURUSD.COMP" ) : mgr.ConstructInstrument( "EURUSD.COMP", "SMART", ou::tf::InstrumentType::Currency ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "USDCAD.COMP" ) ? mgr.Get( "USDCAD.COMP" ) : mgr.ConstructInstrument( "USDCAD.COMP", "SMART", ou::tf::InstrumentType::Currency ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "EURCAD.COMP" ) ? mgr.Get( "EURCAD.COMP" ) : mgr.ConstructInstrument( "EURCAD.COMP", "SMART", ou::tf::InstrumentType::Currency ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "DX.X" ) ? mgr.Get( "DX.X" ) : mgr.ConstructInstrument( "DX.X", "SMART", ou::tf::InstrumentType::Index ) ) );

//  wxGridTableBase::SetColLabelValue( 0, "Low" );
//  wxGridTableBase::SetColLabelValue( 1, "Price" );
//  wxGridTableBase::SetColLabelValue( 2, "High" );
//  wxGridTableBase::SetColLabelValue( 3, "ROC" );
//  wxGridTableBase::SetColLabelValue( 4, "%D" );

//  int ix = 0;
//  for ( vInstrumentData_iter_t iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++ iter ) {
    //wxGridTableBase::SetRowLabelValue( ix++, iter->GetInstrument()->GetInstrumentName() );
//  }

  m_pattrCell = new wxGridCellAttr();
  m_pattrCell->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTRE );

  m_FrameGridInstrumentData = new FrameInstrumentStatus( m_FrameMain, "Instrument Status", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX );
  m_FrameGridInstrumentData->Show( true );

  wxGridCellFloatRenderer* ren = new wxGridCellFloatRenderer(-1, 2 );
  m_FrameGridInstrumentData->Grid()->EnableEditing( false );
  m_FrameGridInstrumentData->Grid()->SetDefaultRenderer( ren );
  m_FrameGridInstrumentData->Grid()->SetTable( this );

  Start( 300 ); // 200ms grid interface update

  return TRUE;
}

wxString AppSemiAuto::GetRowLabelValue(int row) {
  wxString s( m_vInstruments[ row ].GetInstrument()->GetInstrumentName() );
  return s;
}

wxString AppSemiAuto::GetColLabelValue(int col) {
  switch ( col ) {
  case 0:
    return "Low";
    break;
  case 1:
    return "Price";
    break;
  case 2:
    return "High";
    break;
  case 3:
    return "ROC";
    break;
  case 4:
    return "%D";
    break;
  }
  return "";
}

bool AppSemiAuto::CanGetValueAs( int row, int col, const wxString& name ) {
  return "double" == name;
}

double AppSemiAuto::GetValueAsDouble( int row, int col ) {
  return m_vInstruments[ row ].Var( static_cast<InstrumentData::enumIndex>( col ) ).Value();
}

int AppSemiAuto::GetNumberRows() {
  return m_vInstruments.size();
}

int AppSemiAuto::GetNumberCols() {
  return 5;
}

wxString AppSemiAuto::GetValue(int row, int col) {
  wxString s( m_vInstruments[ row ].Var( static_cast<InstrumentData::enumIndex>( col ) ).str() );
  if ( 0 != s.Length() ) {
    int i = 1;
  }
  return s;
}

void AppSemiAuto::SetValue(int row, int col, const wxString &value) {
  // external doesn't set value
}

wxGridCellAttr*	AppSemiAuto::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind) {
  //return m_pattrCell;
  wxGridCellAttr* pattrCell = new wxGridCellAttr();
  pattrCell->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTRE );
  return pattrCell;
}

void AppSemiAuto::Notify( void ) {
  /*
  int iy = 0;
  try {
    m_FrameGridInstrumentData->Grid()->BeginBatch();
    for ( vInstrumentData_iter_t iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++ iter ) {
      for ( unsigned int ix = InstrumentData::Low; InstrumentData::_Count != ix; ++ix ) {
        m_FrameGridInstrumentData->Grid()->SetCellValue( iy, ix, iter->Var( static_cast<InstrumentData::enumIndex>( ix ) ).str() );
      }
    }
    m_FrameGridInstrumentData->Grid()->EndBatch();
  }
  catch (...) {
    int i = 1;
  }
  */
  m_FrameGridInstrumentData->Grid()->ForceRefresh();
}

int AppSemiAuto::OnExit() {

  // Note: disconnecting from frame events should be done in HandleOnCleanUpForExitForFrameMain

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
      {
        std::stringstream ss;
        ss.str( "" );
        ss << ou::CTimeSource::Instance().Internal();
        m_sTSDataStreamOpened = "/app/semiauto/" + ss.str();  // will need to make this generic if need some for multiple providers.
      }
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
  // need to keep a vector of these so can trade multiple symbols simultaneously
  // maybe something like genesis with market depth book built in
  m_vManualOrders.resize( m_vManualOrders.size() + 1 );
  structManualOrder& mo( m_vManualOrders.back() );
  mo.pFrameManualOrder = new FrameManualOrder( m_FrameMain );
  mo.pFrameManualOrder->SetIxStruct( m_vManualOrders.size() - 1 );
  mo.pFrameManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppSemiAuto::HandleManualOrder ) );
  mo.pFrameManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppSemiAuto::HandleCheckSymbolNameAgainstIB ) );
  mo.pFrameManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppSemiAuto::HandleFrameManualOrderFocus ) );
  mo.pFrameManualOrder->Show( true );
  // update events in HandleOnCleanUpForExitForFrameMain
}

void AppSemiAuto::HandleCheckSymbolNameAgainstIB( const std::string& sSymbol ) {
  ou::tf::CIBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "STK";
  contract.symbol = sSymbol;
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_tws->RequestContractDetails( contract, MakeDelegate( this, &AppSemiAuto::HandleIBContractDetails ), MakeDelegate( this, &AppSemiAuto::HandleIBContractDetailsDone ) );
}

void AppSemiAuto::HandleFrameManualOrderFocus( unsigned int ix ) {
  m_curFrameManualOrder = ix;
}

void AppSemiAuto::HandleIBContractDetails( const ou::tf::CIBTWS::ContractDetails& details ) {  // need to handle cross thread
  assert( m_curFrameManualOrder < m_vManualOrders.size() );
  m_vManualOrders[ m_curFrameManualOrder ].details = details;
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

void AppSemiAuto::HandleSaveSeriesEvent( void ) {
  // data collection must be stopped before doing this
  // is there a state machine state somewhere to view?
  assert( 0 < m_sTSDataStreamOpened.length() );
  for ( vInstrumentData_iter_t iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++iter ) {
    iter->SaveSeries( m_sTSDataStreamOpened );
  }
}

void AppSemiAuto::HandleOnCleanUpForExitForFrameMain( int ) {

  Stop();

  m_FrameMain->SetCreateNewFrameManualOrder( 0 );
  m_FrameMain->SetSaveSeriesEvent( 0 );
  m_FrameMain->SetCreateNewFrameManualOrder( 0 );

  // this doesn't work properly for user closed windows
  for ( vManualOrder_t::iterator iter = m_vManualOrders.begin(); iter != m_vManualOrders.end(); ++iter ) {
    iter->pFrameManualOrder->SetOnNewOrderHandler( 0 );
    iter->pFrameManualOrder->SetOnSymbolTextUpdated( 0 );
    iter->pFrameManualOrder->SetOnFocusPropogate( 0 );
    iter->pFrameManualOrder->Close();
    delete iter->pFrameManualOrder;
  }
  m_vManualOrders.clear();

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

