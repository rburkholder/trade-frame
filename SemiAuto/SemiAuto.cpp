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
#include <TFTrading/OrderManager.h>

#include "SemiAuto.h"

using namespace fastdelegate;

IMPLEMENT_APP(AppSemiAuto)

bool AppSemiAuto::OnInit() {

  //m_stateAcquisition = EStartUp;
  m_stateAcquisition = EQuiescent;

  m_bWatchingOptions = false;
  m_bTrading = false;

  m_bExecConnected = false;
  m_bData1Connected = false;
  m_bData2Connected = false;

  m_tws.reset( new IBTWS( "U215226" ) );
  m_bIBConnected = false;

  m_iqfeed.reset( new IQFeedProvider() );
  m_bIQFeedConnected = false;

  m_sim.reset( new SimulationProvider() ); 
  m_bSimConnected = false;

  m_eMode = EModeLive;

  // this is where we select which provider we will be working with on this run
  // providers need to be registered in order for portfolio/position loading to function properly
  // key needs to match to account
  ProviderManager::Instance().Register( "ib01", static_cast<pProvider_t>( m_tws ) );
  ProviderManager::Instance().Register( "iq01", static_cast<pProvider_t>( m_iqfeed ) );
  ProviderManager::Instance().Register( "sim01", static_cast<pProvider_t>( m_sim ) );

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

  m_idPortfolio = "test";

  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppSemiAuto::HandlePopulateDatabase ) );

  assert( 0 != sDbName.length() );
  m_db.Open( sDbName );

  m_tws->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIBConnected ) );
  m_tws->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIBDisConnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIQFeedConnected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleIQFeedDisConnected ) );

  m_sim->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleSimulatorConnected ) );
  m_sim->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleSimulatorDisConnected ) );

  m_pData1Provider->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData1Connected ) );
  m_pData1Provider->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData1Disconnected ) );

  m_iqfeed->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnData2Disconnected ) );

  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &AppSemiAuto::HandleOnExecDisconnected ) );

//  m_pThreadMain = new ThreadMain( EModeLive );

  m_FrameMain = new FrameMain("Semi-Auto Trading", wxPoint(50,50), wxSize(600,900));
  m_FrameMain->Show(TRUE);
  SetTopWindow(m_FrameMain);

  m_FrameMain->SetCreateNewDialogManualOrder( MakeDelegate( this, &AppSemiAuto::HandleCreateNewFrameManualOrder ) );
  m_FrameMain->SetSaveSeriesEvent( MakeDelegate( this, &AppSemiAuto::HandleSaveSeriesEvent ) );

  m_FrameMain->OnCleanUpForExit.Add( MakeDelegate( this, &AppSemiAuto::HandleOnCleanUpForExitForFrameMain ) );

  m_FrameProviderControl = new FrameProviderControl( m_FrameMain );
  m_FrameProviderControl->Show( true );

  m_FrameProviderControl->SetOnIBStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIBStateChangeRequest ) );
  m_FrameProviderControl->SetOnIQFeedStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleIQFeedStateChangeRequest ) ) ;
  m_FrameProviderControl->SetOnSimulatorStateChangeHandler( MakeDelegate( this, &AppSemiAuto::HandleSimulatorStateChangeRequest ) );

  InstrumentManager& mgr( InstrumentManager::Instance() );

  // XAUUSDO.ABBA hvy, XAUUSDO.COMP hvy, XAUUSDO.MIGF med, XAUUSDO.SAXO med, XAUUSDO.UBSW lgt, XAUUSDO.UWCL med, XAUUSDO.WBLT hvy
  // EURUSD.FXCM, BEURUSD
  // HUI.X, XAU.X, GDX, GDXJ
  // DX.X dollar index  https://www.theice.com/productguide/ProductDetails.shtml?specId=194
  // http://www.dtniq.com/template.cfm?navgroup=supportlist&view=1&urlcode=323406&type=TENFORE  // tenfore symbols and descriptions

  //@YMZ11	E-MINI DOW JONES $5 DECEMBER 2011	CBOT	CBOTMINI	FUTURE			
  //@YMH12	E-MINI DOW JONES $5 MARCH 2012	CBOT	CBOTMINI	FUTURE		Y	
  //@YM#	E-MINI DOW JONES $5 MARCH 2012	CBOT	CBOTMINI	FUTURE	
  //@YMM12	E-MINI DOW JONES $5 JUNE 2012	CBOT	CBOTMINI	FUTURE		
  //@YMU12	E-MINI DOW JONES $5 SEPTEMBER 2012	CBOT	CBOTMINI	FUTURE	
  //@YMZ12	E-MINI DOW JONES $5 DECEMBER 2012	CBOT	CBOTMINI	FUTURE			

  // this form of InstrumentData is constructed three times each step, maybe reduce in the future?
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "XAUUSDO.COMP" ) ? mgr.Get( "XAUUSDO.COMP" ) : mgr.ConstructInstrument( "XAUUSDO.COMP", "SMART", ou::tf::InstrumentType::Commodity ) ) );

//  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCU12" ) ? mgr.Get( "+GCU12" ) : mgr.ConstructFuture( "+GCU12", "SMART", 2012,  9 ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCV12" ) ? mgr.Get( "+GCV12" ) : mgr.ConstructFuture( "+GCV12", "SMART", 2012, 10 ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCX12" ) ? mgr.Get( "+GCX12" ) : mgr.ConstructFuture( "+GCX12", "SMART", 2012, 11 ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCZ12" ) ? mgr.Get( "+GCZ12" ) : mgr.ConstructFuture( "+GCZ12", "SMART", 2012, 12 ) ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+GCG13" ) ? mgr.Get( "+GCG13" ) : mgr.ConstructFuture( "+GCG13", "SMART", 2013,  2 ) ) );

//  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+CLG12" ) ? mgr.Get( "+CLG12" ) : mgr.ConstructFuture( "+CLG12", "SMART", 2012, 02 ) ) );
//  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+CLH12" ) ? mgr.Get( "+CLH12" ) : mgr.ConstructFuture( "+CLH12", "SMART", 2012, 03 ) ) );
//  m_vInstruments.push_back( InstrumentData( mgr.Exists( "+CLJ12" ) ? mgr.Get( "+CLJ12" ) : mgr.ConstructFuture( "+CLJ12", "SMART", 2012, 04 ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "GLD" ) ? mgr.Get( "GLD" ) : mgr.ConstructInstrument( "GLD", "SMART", ou::tf::InstrumentType::Stock ) ) );

  m_vInstruments.push_back( InstrumentData( mgr.Exists( "EURUSD.COMP" ) ? mgr.Get( "EURUSD.COMP" ) : mgr.ConstructInstrument( "EURUSD.COMP", "SMART", ou::tf::InstrumentType::Currency ), 4 ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "USDCAD.COMP" ) ? mgr.Get( "USDCAD.COMP" ) : mgr.ConstructInstrument( "USDCAD.COMP", "SMART", ou::tf::InstrumentType::Currency ), 4 ) );
  m_vInstruments.push_back( InstrumentData( mgr.Exists( "EURCAD.COMP" ) ? mgr.Get( "EURCAD.COMP" ) : mgr.ConstructInstrument( "EURCAD.COMP", "SMART", ou::tf::InstrumentType::Currency ), 4 ) );

//  m_vInstruments.push_back( InstrumentData( mgr.Exists( "DX.X" ) ? mgr.Get( "DX.X" ) : mgr.ConstructInstrument( "DX.X", "SMART", ou::tf::InstrumentType::Index ) ) );

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

  // need to add custom renderer for currency with 6 decimal places.

  Start( 300 ); // 300ms grid interface update via Notify

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
  return m_vInstruments[ row ].Var( static_cast<InstrumentData::enumIndex>( col ) );
}

int AppSemiAuto::GetNumberRows() {
  return m_vInstruments.size();
}

int AppSemiAuto::GetNumberCols() {
  return 5;
}

wxString AppSemiAuto::GetValue(int row, int col) {
//  wxString s( m_vInstruments[ row ].Var( static_cast<InstrumentData::enumIndex>( col ) ).str() );
//  if ( 0 != s.Length() ) {
//    int i = 1;
//  }
//  return s;
  return "";
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

  switch ( m_stateAcquisition ) {
  case EStartUp:
    HandleStateChangeRequest( eProviderState_t::ProviderGoingOn, m_bIQFeedConnected, m_iqfeed );
    m_stateAcquisition = EWaitForMarketOpen;
    break;
  case EWaitForMarketOpen: { // while logging, wait for market open
      ptime dt = ou::TimeSource::Instance().Internal();
      time_duration tdMarketOpen( 19, 0, 0 );
      if ( tdMarketOpen <= dt.time_of_day() ) {
        m_stateAcquisition = EWaitForMarketClose;
      }
    }
    break;
  case EWaitForMarketClose: {
      ptime dt = ou::TimeSource::Instance().Internal();
      time_duration tdMarketClose( 18, 20, 0 );
      if ( tdMarketClose <= dt.time_of_day() ) {
        HandleStateChangeRequest( eProviderState_t::ProviderGoingOff, m_bIQFeedConnected, m_iqfeed );
        // disconnecting will set the state of EWriteData
      }
    }
    break;
  case EWriteData:
    HandleSaveSeriesEvent();
    m_stateAcquisition = EResetStructures;
    break;
  case EResetStructures:
    for ( vInstrumentData_iter_t iter = m_vInstruments.begin(); iter != m_vInstruments.end(); ++iter ) {
      iter->Reset();
    }
    m_stateAcquisition = EWaitToStartLogging;
    break;
  case EWaitToStartLogging: {
      ptime dt = ou::TimeSource::Instance().Internal();
      time_duration tdLoggingStart( 18, 40, 0 );
      if ( tdLoggingStart <= dt.time_of_day() ) {
        m_stateAcquisition = EStartUp;
      }
    }
    break;
  case EQuiescent: // nothing happening
    break;
  }
}

int AppSemiAuto::OnExit() {

  // Note: disconnecting from frame events should be done in HandleOnCleanUpForExitForFrameMain

  m_pData1Provider->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData1Connected ) );
  m_pData1Provider->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData1Disconnected ) );

  m_iqfeed->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData2Connected ) );
  m_iqfeed->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnData2Disconnected ) );

  m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &AppSemiAuto::HandleOnExecDisconnected ) );

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
        ss << ou::TimeSource::Instance().Internal();
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
  if ( EQuiescent != m_stateAcquisition ) {
    m_stateAcquisition = EWriteData;
  }
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
  mo.pDialogManualOrder = new ou::tf::PanelManualOrder( m_FrameMain );
  mo.pDialogManualOrder->SetIxStruct( m_vManualOrders.size() - 1 );
  mo.pDialogManualOrder->SetOnNewOrderHandler( MakeDelegate( this, &AppSemiAuto::HandleManualOrder ) );
  mo.pDialogManualOrder->SetOnSymbolTextUpdated( MakeDelegate( this, &AppSemiAuto::HandleCheckSymbolNameAgainstIB ) );
  mo.pDialogManualOrder->SetOnFocusPropogate( MakeDelegate( this, &AppSemiAuto::HandleFrameManualOrderFocus ) );
  mo.pDialogManualOrder->Show( true );
  // update events in HandleOnCleanUpForExitForFrameMain
}

void AppSemiAuto::HandleCheckSymbolNameAgainstIB( const std::string& sSymbol ) {
  ou::tf::IBTWS::Contract contract;
  contract.currency = "USD";
  contract.exchange = "SMART";
  contract.secType = "STK";
  contract.symbol = sSymbol;
  // IB responds only when symbol is found, bad symbols will not illicit a response
  m_vManualOrders[ m_curDialogManualOrder ].pDialogManualOrder->SetInstrumentDescription( "" );
  m_tws->RequestContractDetails( contract, MakeDelegate( this, &AppSemiAuto::HandleIBContractDetails ), MakeDelegate( this, &AppSemiAuto::HandleIBContractDetailsDone ) );
}

void AppSemiAuto::HandleFrameManualOrderFocus( unsigned int ix ) {
  m_curDialogManualOrder = ix;
}

void AppSemiAuto::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {  // need to handle cross thread
  assert( m_curDialogManualOrder < m_vManualOrders.size() );
  m_vManualOrders[ m_curDialogManualOrder ].details = details;
  m_vManualOrders[ m_curDialogManualOrder ].pInstrument = pInstrument;
  m_vManualOrders[ m_curDialogManualOrder ].pDialogManualOrder->SetInstrumentDescription( details.longName );
}

void AppSemiAuto::HandleIBContractDetailsDone( void ) {  // called only on successful contract found, need to handle cross thread
}

void AppSemiAuto::HandleManualOrder( const ManualOrder_t& order ) {
  try {
    InstrumentManager& mgr( InstrumentManager::Instance() );
    pInstrument_t pInstrument = m_vManualOrders[ m_curDialogManualOrder ].pInstrument;
    if ( !mgr.Exists( pInstrument ) ) {
      mgr.Register( pInstrument );
    }
    if ( 0 == m_pPosition.get() ) {
      m_pPosition = ou::tf::PortfolioManager::Instance().ConstructPosition( m_idPortfolio, "Dell", "manual", "ib01", "ib01", m_pExecutionProvider, m_pData1Provider, pInstrument );
    }
    ou::tf::OrderManager& om( ou::tf::OrderManager::Instance() );
    ou::tf::OrderManager::pOrder_t pOrder;
    switch ( order.eOrderType ) {
    case OrderType::Market: 
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity );
      m_pPosition->PlaceOrder( OrderType::Market, order.eOrderSide, order.nQuantity );
      break;
    case OrderType::Limit:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      m_pPosition->PlaceOrder( OrderType::Limit, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    case OrderType::Stop:
      //pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      m_pPosition->PlaceOrder( OrderType::Stop, order.eOrderSide, order.nQuantity, order.dblPrice1 );
      break;
    }
    //ou::tf::OrderManager::pOrder_t pOrder = om.ConstructOrder( pInstrument, order.eOrderType, order.eOrderSide, order.nQuantity, order.dblPrice1, order.dblPrice2 );
    //om.PlaceOrder( m_tws.get(), pOrder );
  }
  catch (...) {
    int i = 1;
  }
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

  m_FrameMain->SetCreateNewDialogManualOrder( 0 );
  m_FrameMain->SetSaveSeriesEvent( 0 );
  m_FrameMain->SetCreateNewDialogManualOrder( 0 );

  // this doesn't work properly for user closed windows
  for ( vManualOrder_t::iterator iter = m_vManualOrders.begin(); iter != m_vManualOrders.end(); ++iter ) {
    iter->pDialogManualOrder->SetOnNewOrderHandler( 0 );
    iter->pDialogManualOrder->SetOnSymbolTextUpdated( 0 );
    iter->pDialogManualOrder->SetOnFocusPropogate( 0 );
    iter->pDialogManualOrder->Close();
    delete iter->pDialogManualOrder;
  }
  m_vManualOrders.clear();

  m_FrameProviderControl->SetOnIBStateChangeHandler( 0 );
  m_FrameProviderControl->SetOnIQFeedStateChangeHandler( 0 ) ;
  m_FrameProviderControl->SetOnSimulatorStateChangeHandler( 0 );

//  m_FrameProviderControl->Close();

  m_FrameMain->OnCleanUpForExit.Add( MakeDelegate( this, &AppSemiAuto::HandleOnCleanUpForExitForFrameMain ) );

}

void AppSemiAuto::HandlePopulateDatabase( void ) {

  ou::tf::AccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::AccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::AccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::AccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::AccountManager::pAccount_t pAccountIB
    = ou::tf::AccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::AccountManager::pAccount_t pAccountIQFeed
    = ou::tf::AccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::PortfolioManager::pPortfolio_t pPortfolio
    = ou::tf::PortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );

}

