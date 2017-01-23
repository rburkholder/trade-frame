/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <OUCommon/TimeSource.h>

#include <TFTrading/ProviderManager.h>

#include <TFInteractiveBrokers/IBTWS.h>
#include <TFIQFeed/IQFeedProvider.h>
#include <TFSimulation/SimulationProvider.h>

#include <TFVuTrading/PanelProviderControl.h>

namespace ou { // One Unified
namespace tf { // TradeFrame
  
template<typename CRTP>
class FrameWork01 {
public:

  enum Mode_t {
    EModeUnknown,
    EModeSimulation,
    EModeLive
  } m_eMode;

  FrameWork01(void);
  ~FrameWork01(void);

  Mode_t Mode( void ) const { return m_mode; };

protected:

  typedef ou::tf::ProviderManager ProviderManager;
  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;
  typedef ou::tf::eProviderState_t eProviderState_t;

  typedef ou::tf::IBTWS::pProvider_t pProviderIBTWS_t;
  typedef ou::tf::IQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef ou::tf::SimulationProvider::pProvider_t pProviderSim_t;

  std::string m_sTSDataStreamStarted;

  Mode_t m_mode;

  pProviderIQFeed_t m_iqfeed;
  bool m_bIQFeedConnected;

  pProviderIBTWS_t m_tws;
  bool m_bIBConnected;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pData1Provider;
  bool m_bData1Connected;
  pProvider_t m_pData2Provider;
  bool m_bData2Connected;
  pProvider_t m_pExecutionProvider;
  bool m_bExecConnected;

  PanelProviderControl* m_pPanelProviderControl;

  // for CRTP
  void      OnIQFeedConnecting( int ) {};
  void          OnIBConnecting( int ) {};
  void   OnSimulatorConnecting( int ) {};
  
  void       OnIQFeedConnected( int ) {};
  void           OnIBConnected( int ) {}; 
  void    OnSimulatorConnected( int ) {};
  
  void    OnIQFeedDisconnecting( int ) {};
  void        OnIBDisconnecting( int ) {};
  void OnSimulatorDisconnecting( int ) {};

  void    OnIQFeedDisconnected( int ) {};
  void        OnIBDisconnected( int ) {};
  void OnSimulatorDisconnected( int ) {};

  // for CRTP
  void   OnData1Connecting( int ) {};
  void   OnData2Connecting( int ) {};
  void    OnExecConnecting( int ) {};
  void    OnData1Connected( int ) {};
  void    OnData2Connected( int ) {};
  void     OnExecConnected( int ) {};
  void OnData1Disconnecting( int ) {};
  void OnData2Disconnecting( int ) {};
  void  OnExecDisconnecting( int ) {}
  void OnData1Disconnected( int ) {};
  void OnData2Disconnected( int ) {};
  void  OnExecDisconnected( int ) {};

  void LinkToPanelProviderControl( void );
  void DelinkFromPanelProviderControl( void );

private:

  void SetMode( void );

  void HandleStateChangeRequest( ou::tf::eProviderState_t, bool bConnected, pProvider_t );

  void HandleProviderStateChangeIQFeed( ou::tf::eProviderState_t );
  void HandleProviderStateChangeIB( ou::tf::eProviderState_t );
  void HandleProviderStateChangeSimulation( ou::tf::eProviderState_t ); 

  void HandleIBConnecting( int );
  void HandleIBConnected( int );
  void HandleIBDisconnecting( int );
  void HandleIBDisconnected( int );
  void HandleIBError( size_t );
  
  void HandleIQFeedConnecting( int );
  void HandleIQFeedConnected( int );
  void HandleIQFeedDisconnecting( int );
  void HandleIQFeedDisconnected( int );
  void HandleIQFeedError( size_t );
  
  void HandleSimulatorConnecting( int );
  void HandleSimulatorConnected( int );
  void HandleSimulatorDisconnecting( int );
  void HandleSimulatorDisconnected( int );
  void HandleSimulatorError( size_t );

  void HandleOnData1Connecting( int );
  void HandleOnData1Connected( int );
  void HandleOnData1Disconnecting( int );
  void HandleOnData1Disconnected( int );

  void HandleOnData2Connecting( int );
  void HandleOnData2Connected( int );
  void HandleOnData2Disconnecting( int );
  void HandleOnData2Disconnected( int );

  void HandleOnExecConnecting( int );  // need to test for connection failure, when ib is not running
  void HandleOnExecConnected( int ); 
  void HandleOnExecDisconnecting( int );
  void HandleOnExecDisconnected( int );

  void HandleProviderSelectD1( ou::tf::PanelProviderControl::Provider_t );
  void HandleProviderSelectD2( ou::tf::PanelProviderControl::Provider_t );
  void HandleProviderSelectX( ou::tf::PanelProviderControl::Provider_t );

  void HandlePanelClosing( void );

};

template<typename CRTP>
FrameWork01<CRTP>::FrameWork01( void ) :
  m_mode( EModeUnknown ),
  m_pPanelProviderControl( 0 ),
  m_tws( new ou::tf::IBTWS( "U000000" ) ), m_bIBConnected( false ), 
  m_iqfeed( new ou::tf::IQFeedProvider() ), m_bIQFeedConnected( false ),
  m_sim( new ou::tf::SimulationProvider() ), m_bSimConnected( false ),
  m_bData1Connected( false ), m_bData2Connected( false ), m_bExecConnected( false )

{

  std::string sTimeZoneSpec( "../date_time_zonespec.csv" );
  if ( !boost::filesystem::exists( sTimeZoneSpec ) ) {
    std::cout << "Required file does not exist:  " << sTimeZoneSpec << std::endl;
  }

  std::stringstream ss;
  ss.str( "" );
  // http://www.boost.org/doc/libs/1_54_0/doc/html/date_time/date_time_io.html
  boost::posix_time::time_facet* pFacet( new boost::posix_time::time_facet( "%Y-%m-%d %H:%M:%S%F" ) );
  ss.imbue( std::locale( ss.getloc(), pFacet ) );
  ss << ou::TimeSource::Instance().External();
  m_sTSDataStreamStarted = ss.str();

  // this is where we select which provider we will be working with on this run
  // providers need to be registered in order for portfolio/position loading to function properly
  // key needs to match to account
  // ensure providers have been initialized above first
  pProvider_t p;
  
  p = m_iqfeed;
  //p = boost::static_pointer_cast<pProvider_t>( m_iqfeed );
  ProviderManager::LocalCommonInstance().Register(  "iq01", p );
  
  p = m_tws;
  //p = boost::static_pointer_cast<pProvider_t>( m_tws );
  ProviderManager::LocalCommonInstance().Register(  "ib01", p );
  
  p = m_sim;
  //p = boost::static_pointer_cast<pProvider_t>( m_sim );
  ProviderManager::LocalCommonInstance().Register( "sim01", p );

  m_iqfeed->OnConnecting.Add( MakeDelegate( this, &FrameWork01::HandleIQFeedConnecting ) );
  m_iqfeed->OnConnected.Add( MakeDelegate( this, &FrameWork01::HandleIQFeedConnected ) );
  m_iqfeed->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01::HandleIQFeedDisconnecting ) );
  m_iqfeed->OnDisconnected.Add( MakeDelegate( this, &FrameWork01::HandleIQFeedDisconnected ) );
  m_iqfeed->OnError.Add( MakeDelegate( this, &FrameWork01::HandleIQFeedError ) );

  m_tws->OnConnecting.Add( MakeDelegate( this, &FrameWork01::HandleIBConnecting ) );
  m_tws->OnConnected.Add( MakeDelegate( this, &FrameWork01::HandleIBConnected ) );
  m_tws->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01::HandleIBDisconnecting ) );
  m_tws->OnDisconnected.Add( MakeDelegate( this, &FrameWork01::HandleIBDisconnected ) );
  m_tws->OnError.Add( MakeDelegate( this, &FrameWork01::HandleIBError ) );

  m_sim->OnConnecting.Add( MakeDelegate( this, &FrameWork01::HandleSimulatorConnecting ) );
  m_sim->OnConnected.Add( MakeDelegate( this, &FrameWork01::HandleSimulatorConnected ) );
  m_sim->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01::HandleSimulatorDisconnecting ) );
  m_sim->OnDisconnected.Add( MakeDelegate( this, &FrameWork01::HandleSimulatorDisconnected ) );
  m_sim->OnError.Add( MakeDelegate( this, &FrameWork01::HandleSimulatorError ) );
}

template<typename CRTP>
FrameWork01<CRTP>::~FrameWork01( void ) {

  m_iqfeed->OnConnecting.Remove( MakeDelegate( this, &FrameWork01::HandleIQFeedConnecting ) );
  m_iqfeed->OnConnected.Remove( MakeDelegate( this, &FrameWork01::HandleIQFeedConnected ) );
  m_iqfeed->OnDisconnecting.Remove( MakeDelegate( this, &FrameWork01::HandleIQFeedDisconnecting ) );
  m_iqfeed->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01::HandleIQFeedDisconnected ) );
  m_iqfeed->OnError.Remove( MakeDelegate( this, &FrameWork01::HandleIQFeedError ) );

  m_tws->OnConnecting.Remove( MakeDelegate( this, &FrameWork01::HandleIBConnecting ) );
  m_tws->OnConnected.Remove( MakeDelegate( this, &FrameWork01::HandleIBConnected ) );
  m_tws->OnDisconnecting.Remove( MakeDelegate( this, &FrameWork01::HandleIBDisconnecting ) );
  m_tws->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01::HandleIBDisconnected ) );
  m_tws->OnError.Remove( MakeDelegate( this, &FrameWork01::HandleIBError ) );

  m_sim->OnConnecting.Remove( MakeDelegate( this, &FrameWork01::HandleSimulatorConnecting ) );
  m_sim->OnConnected.Remove( MakeDelegate( this, &FrameWork01::HandleSimulatorConnected ) );
  m_sim->OnDisconnecting.Remove( MakeDelegate( this, &FrameWork01::HandleSimulatorDisconnecting ) );
  m_sim->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01::HandleSimulatorDisconnected ) );
  m_sim->OnError.Remove( MakeDelegate( this, &FrameWork01::HandleSimulatorError ) );

  ou::tf::ProviderManager::LocalCommonInstance().Release( "iq01" );
  ou::tf::ProviderManager::LocalCommonInstance().Release( "ib01" );
  ou::tf::ProviderManager::LocalCommonInstance().Release( "sim01" );

}

template<typename CRTP>
void FrameWork01<CRTP>::LinkToPanelProviderControl( void ) {
  m_pPanelProviderControl->SetOnIQFeedStateChangeHandler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderStateChangeIQFeed ) );
  m_pPanelProviderControl->SetOnIBStateChangeHandler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderStateChangeIB ) );
  m_pPanelProviderControl->SetOnSimulatorStateChangeHandler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderStateChangeSimulation ) );

  m_pPanelProviderControl->SetOnProviderSelectD1Handler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderSelectD1 ) );
  m_pPanelProviderControl->SetOnProviderSelectD2Handler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderSelectD2 ) );
  m_pPanelProviderControl->SetOnProviderSelectXHandler( MakeDelegate( this, &FrameWork01<CRTP>::HandleProviderSelectX ) );

  m_pPanelProviderControl->SyncInitialState();
}

template<typename CRTP>
void FrameWork01<CRTP>::DelinkFromPanelProviderControl( void ) {
  if ( 0 != m_pPanelProviderControl ) {
    m_pPanelProviderControl->SetOnProviderSelectD1Handler( 0 );
    m_pPanelProviderControl->SetOnProviderSelectD2Handler( 0 );
    m_pPanelProviderControl->SetOnProviderSelectXHandler( 0 );

    m_pPanelProviderControl->SetOnIQFeedStateChangeHandler( 0 );
    m_pPanelProviderControl->SetOnIBStateChangeHandler( 0 );
    m_pPanelProviderControl->SetOnSimulatorStateChangeHandler( 0 );
  }
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleProviderStateChangeIB( ou::tf::eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bIBConnected, m_tws );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleProviderStateChangeIQFeed( ou::tf::eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bIQFeedConnected, m_iqfeed );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleProviderStateChangeSimulation( ou::tf::eProviderState_t state ) {
  HandleStateChangeRequest( state, m_bSimConnected, m_sim );
}

template<typename CRTP> // 2017.12.22 is this actually useful for anything?
void FrameWork01<CRTP>::HandleStateChangeRequest( eProviderState_t state, bool bConnected, pProvider_t p ) {
  switch ( state ) {
  case eProviderState_t::ProviderGoingOn:
    if ( !bConnected ) {
      {
        std::stringstream ss;
        ss.str( "" );
        ss << ou::TimeSource::LocalCommonInstance().Internal();
//        m_sTSDataStreamOpened = "/app/semiauto/" + ss.str();  // will need to make this generic if need some for multiple providers.
      }
      p->Connect();
    }
    break;
  case eProviderState_t::ProviderOn:
    assert( bConnected );
    break;
  case eProviderState_t::ProviderGoingOff:
    if ( bConnected ) {
      p->Disconnect();
    }
    break;
  case eProviderState_t::ProviderOff:
    assert( !bConnected );
    break;
  }
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIQFeedConnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderGoingOn ) );
  static_cast<CRTP*>(this)->OnIQFeedConnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIBConnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderGoingOn ) );
  static_cast<CRTP*>(this)->OnIBConnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleSimulatorConnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderGoingOn ) );
  static_cast<CRTP*>(this)->OnSimulatorConnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIQFeedConnected( int e ) {  // cross thread event
  m_bIQFeedConnected = true;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderOn ) );
  static_cast<CRTP*>(this)->OnIQFeedConnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIBConnected( int e ) { // cross thread event
  m_bIBConnected = true;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderOn ) );
  static_cast<CRTP*>(this)->OnIBConnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleSimulatorConnected( int e ) { // cross thread event
  m_bSimConnected = true;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderOn ) );
  static_cast<CRTP*>(this)->OnSimulatorConnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIQFeedDisconnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderGoingOff ) );
  static_cast<CRTP*>(this)->OnIQFeedDisconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIBDisconnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderGoingOff ) );
  static_cast<CRTP*>(this)->OnIBDisconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleSimulatorDisconnecting( int e ) {  // cross thread event
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderGoingOff ) );
  static_cast<CRTP*>(this)->OnSimulatorDisconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIQFeedDisconnected( int e ) { // cross thread event
  m_bIQFeedConnected = false;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIQFeed, eProviderState_t::ProviderOff ) );
  static_cast<CRTP*>(this)->OnIQFeedDisconnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIBDisconnected( int e ) {  // cross thread event
  m_bIBConnected = false;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderIB, eProviderState_t::ProviderOff ) );
  static_cast<CRTP*>(this)->OnIBDisconnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleSimulatorDisconnected( int e ) {  // cross thread event
  m_bSimConnected = false;
  if ( 0 != m_pPanelProviderControl )
    m_pPanelProviderControl->QueueEvent( new UpdateProviderStatusEvent( EVT_ProviderSimulator, eProviderState_t::ProviderOff ) );
  static_cast<CRTP*>(this)->OnSimulatorDisconnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIBError( size_t e ) {
  std::cout << "HandleIBError: " << e << std::endl;
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleIQFeedError( size_t e ) {
  std::cout << "HandleIQFeedError: " << e << std::endl;
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleSimulatorError( size_t e ) {
  std::cout << "HandleSimulatorError: " << e << std::endl;
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData1Connecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnData1Connecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData1Connected(int e) {
  m_bData1Connected = true;
  static_cast<CRTP*>(this)->OnData1Connected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData1Disconnecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnData1Disconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData1Disconnected(int e) {
  m_bData1Connected = false;
  static_cast<CRTP*>(this)->OnData1Disconnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData2Connecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnData2Connecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData2Connected(int e) {
  m_bData2Connected = true;
  static_cast<CRTP*>(this)->OnData2Connected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData2Disconnecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnData2Disconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnData2Disconnected(int e) {
  m_bData2Connected = false;
  static_cast<CRTP*>(this)->OnData2Disconnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnExecConnecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnData1Connecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnExecConnected(int e) {
  m_bExecConnected = true;
  static_cast<CRTP*>(this)->OnExecConnected( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnExecDisconnecting( int e ) {  // cross thread event
  static_cast<CRTP*>(this)->OnExecDisconnecting( e );
}

template<typename CRTP>
void FrameWork01<CRTP>::HandleOnExecDisconnected(int e) {
  m_bExecConnected = false;
  static_cast<CRTP*>(this)->OnExecDisconnected( e );
}

template<typename CRTP>  // need an event to indicate D1 is changing
void FrameWork01<CRTP>::HandleProviderSelectD1( ou::tf::PanelProviderControl::Provider_t provider) {
  if ( 0 != m_pData1Provider.use_count() ) {
    m_pData1Provider->OnConnecting.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Connecting ) );
    m_pData1Provider->OnConnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Connected ) );
    m_pData1Provider->OnDisconnecting.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Disconnecting ) );
    m_pData1Provider->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Disconnected ) );
  }
  switch ( provider ) {
  case ou::tf::PanelProviderControl::Provider_t::EIQFeed:
    m_pData1Provider = m_iqfeed;
    break;
  case ou::tf::PanelProviderControl::Provider_t::EIB:
    m_pData1Provider = m_tws;
    break;
  case ou::tf::PanelProviderControl::Provider_t::ESim:
    m_pData1Provider = m_sim;
    break;
  }
  m_pData1Provider->OnConnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Connecting ) );
  m_pData1Provider->OnConnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Connected ) );
  m_pData1Provider->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Disconnecting ) );
  m_pData1Provider->OnDisconnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData1Disconnected ) );
  SetMode();
}

template<typename CRTP> // need an event to indicate D2 is changing
void FrameWork01<CRTP>::HandleProviderSelectD2( ou::tf::PanelProviderControl::Provider_t provider ) {
  if ( 0 != m_pData2Provider.use_count() ) {
    m_pData2Provider->OnConnecting.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Connecting ) );
    m_pData2Provider->OnConnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Connected ) );
    m_pData2Provider->OnConnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Disconnecting ) );
    m_pData2Provider->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Disconnected ) );
  }
  switch ( provider ) {
  case ou::tf::PanelProviderControl::Provider_t::EIQFeed:
    m_pData2Provider = m_iqfeed;
    break;
  case ou::tf::PanelProviderControl::Provider_t::EIB:
    m_pData2Provider = m_tws;
    break;
  case ou::tf::PanelProviderControl::Provider_t::ESim:
    m_pData2Provider = m_sim;
    break;
  }
  m_pData2Provider->OnConnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Connecting ) );
  m_pData2Provider->OnConnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Connected ) );
  m_pData2Provider->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Disconnecting ) );
  m_pData2Provider->OnDisconnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnData2Disconnected ) );
  SetMode();
}

template<typename CRTP> // need an event to indicate X is changing
void FrameWork01<CRTP>::HandleProviderSelectX( ou::tf::PanelProviderControl::Provider_t provider ) {
  if ( 0 != m_pExecutionProvider.use_count() ) {
    m_pExecutionProvider->OnConnecting.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecConnecting ) );
    m_pExecutionProvider->OnConnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecConnected ) );
    m_pExecutionProvider->OnDisconnecting.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecDisconnecting ) );
    m_pExecutionProvider->OnDisconnected.Remove( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecDisconnected ) );
  }
  switch ( provider ) {
  case ou::tf::PanelProviderControl::Provider_t::EIQFeed:
    m_pExecutionProvider = m_iqfeed;
    break;
  case ou::tf::PanelProviderControl::Provider_t::EIB:
    m_pExecutionProvider = m_tws;
    break;
  case ou::tf::PanelProviderControl::Provider_t::ESim:
    m_pExecutionProvider = m_sim;
    break;
  }
  m_pExecutionProvider->OnConnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecConnecting ) );
  m_pExecutionProvider->OnConnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecConnected ) );
  m_pExecutionProvider->OnDisconnecting.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecDisconnecting ) );
  m_pExecutionProvider->OnDisconnected.Add( MakeDelegate( this, &FrameWork01<CRTP>::HandleOnExecDisconnected ) );
  SetMode();
}

template<typename CRTP>
void FrameWork01<CRTP>::SetMode( void ) {
  m_mode = EModeUnknown;
  if ( ( 0 != m_pData1Provider.use_count() ) && ( 0 != m_pExecutionProvider.use_count() ) ) {
    if ( ( ou::tf::keytypes::EProviderSimulator == m_pData1Provider->ID() ) && ( ou::tf::keytypes::EProviderSimulator == m_pExecutionProvider->ID() ) ) {
      m_mode = EModeSimulation;
    }
    if ( 
      ( ( ou::tf::keytypes::EProviderIQF == m_pData1Provider->ID() ) || ( ou::tf::keytypes::EProviderIB == m_pData1Provider->ID() ) )
      && ( ou::tf::keytypes::EProviderIB == m_pExecutionProvider->ID() ) )
    {
      m_mode = EModeLive;
    }
  }
}

template<typename CRTP>
void FrameWork01<CRTP>::HandlePanelClosing( void ) {
  DelinkFromPanelProviderControl();
}

} // namespace tf
} // namespace ou

