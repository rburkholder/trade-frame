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

#pragma once

#include <vector>

#include <wx/wx.h>
#include <wx/timer.h>

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/ProviderManager.h>

#include <TFVuTrading/PanelManualOrder.h>  // need to put this is a frame for local use

#include <TFIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <TFIQFeed/IQFeedProvider.h>  // includes CPortfolio and CPosition

#include <TFInteractiveBrokers/IBTWS.h>

#include <TFSimulation/SimulationProvider.h>

// may need to inherit and add more functionality to the class:
#include <TFTrading/DBOps.h>

//#include "ThreadMain.h"
#include "FrameMain.h"
#include "FrameProviderControl.h"
//#include "DialogManualOrder.h"
#include "FrameInstrumentStatus.h"

#include "InstrumentData.h"

using namespace ou::tf;

//wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

class AppSemiAuto : 
  public wxApp, 
  public wxTimer,
  public wxGridTableBase
{

  enum enumMode {
    EModeSimulation,
    EModeLive
  } m_eMode;

  virtual bool OnInit();
  virtual int OnExit();

  // wxGridTableBase
  int GetNumberRows();
  int GetNumberCols();
  wxString GetValue(int row, int col);
  void SetValue(int row, int col, const wxString &value);
  double GetValueAsDouble( int row, int col );
  bool CanGetValueAs( int row, int col, const wxString& name );
  wxString GetRowLabelValue(int row);
  wxString GetColLabelValue(int col);
  wxGridCellAttr*	GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);

protected:
private:

  enum enumAcquisitionStates {
    EStartUp, 
    EWaitForMarketOpen, EWaitForMarketClose, 
    EWriteData, EResetStructures, EWaitToStartLogging, EQuiescent
  } m_stateAcquisition;

  typedef CPortfolio::pPortfolio_t pPortfolio_t;
  typedef CPosition::pPosition_t pPosition_t;

  typedef ou::tf::keytypes::idInstrument_t idInstrument_t;
  typedef CInstrument::pInstrument_t pInstrument_t;

  typedef CProviderInterfaceBase::pProvider_t pProvider_t;

  typedef IBTWS::pProvider_t pProviderIBTWS_t;
  typedef CIQFeedProvider::pProvider_t pProviderIQFeed_t;
  typedef SimulationProvider::pProvider_t pProviderSim_t;

  //typedef FrameProviderControl::eProviderState_t eProviderState_t;

  wxGridCellAttr* m_pattrCell;

  typedef ou::tf::PanelManualOrder::Order_t ManualOrder_t;

  FrameMain* m_FrameMain;
  FrameProviderControl* m_FrameProviderControl;
  FrameInstrumentStatus* m_FrameGridInstrumentData;

  DBOps m_db;

//  ThreadMain* m_pThreadMain;

  bool m_bWatchingOptions;
  bool m_bTrading;

  typedef std::vector<InstrumentData> vInstrumentData_t;
  typedef vInstrumentData_t::iterator vInstrumentData_iter_t;
  vInstrumentData_t m_vInstruments;

  bool m_bExecConnected;
  bool m_bData1Connected;
  bool m_bData2Connected;

  pProviderIBTWS_t m_tws;
  bool m_bIBConnected;

  pProviderIQFeed_t m_iqfeed;
  bool m_bIQFeedConnected;

  pProviderSim_t m_sim;
  bool m_bSimConnected;

  pProvider_t m_pExecutionProvider;
  pProvider_t m_pData1Provider;
  pProvider_t m_pData2Provider;

  unsigned int m_curDialogManualOrder;
  struct structManualOrder {
    ou::tf::PanelManualOrder* pDialogManualOrder;
    ou::tf::IBTWS::ContractDetails details;
    pInstrument_t pInstrument;
  };

  typedef std::vector<structManualOrder> vManualOrder_t;
  vManualOrder_t m_vManualOrders;

  ou::tf::keytypes::idPortfolio_t m_idPortfolio;

  pPosition_t m_pPosition;

  std::string m_sTSDataStreamOpened;

  void Notify( void );  // override of wxTimer::Notify

  void HandlePopulateDatabase( void );

  void HandleOnData1Connected( int );
  void HandleOnData1Disconnected( int );

  void HandleOnData2Connected( int );
  void HandleOnData2Disconnected( int );

  void HandleOnExecConnected( int );  // need to test for connection failure, when ib is not running
  void HandleOnExecDisconnected( int );

  void HandleIBStateChangeRequest( eProviderState_t );
  void HandleIQFeedStateChangeRequest( eProviderState_t );
  void HandleSimulatorStateChangeRequest( eProviderState_t );

  void HandleStateChangeRequest( eProviderState_t, bool&, pProvider_t );

  void HandleIBConnected( int );
  void HandleIQFeedConnected( int );
  void HandleSimulatorConnected( int );

  void HandleIBDisConnected( int );
  void HandleIQFeedDisConnected( int );
  void HandleSimulatorDisConnected( int );

  void HandleCreateNewFrameManualOrder( void );
  void HandleFrameManualOrderFocus( unsigned int );
  void HandleManualOrder( const ManualOrder_t& );

  void HandleOnCleanUpForExitForFrameMain( int );

  void HandleCheckSymbolNameAgainstIB( const std::string& );
  void HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails&, pInstrument_t& pInstrument );
  void HandleIBContractDetailsDone( void );

  void HandleSaveSeriesEvent( void );

};
 
// Implements MyApp& wxGetApp()
DECLARE_APP(AppSemiAuto)