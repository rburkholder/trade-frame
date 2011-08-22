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

#include <OUCommon/Delegate.h>

#include "FrameManualOrder.h"

class FrameMain : public wxFrame {
public:

  FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~FrameMain(void);

  ou::Delegate<void*> OnBtnClickedConnectToIb;
  ou::Delegate<void*> OnBtnClickedDisconnectFromIb;
  ou::Delegate<void*> OnBtnClickedConnectToIQFeed;
  ou::Delegate<void*> OnBtnClickedDisconnectFromIQFeed;
  ou::Delegate<void*> OnBtnClickedConnectToSimulator;
  ou::Delegate<void*> OnBtnClickedDisconnectFromSimulator;
  ou::Delegate<void*> OnBtnClickedStartTrading;
  ou::Delegate<void*> OnBtnClickedStopTrading;
  ou::Delegate<void*> OnBtnClickedStartWatch;
  ou::Delegate<void*> OnBtnClickedStopWatch;
  ou::Delegate<void*> OnBtnClickedSaveSeries;
  ou::Delegate<void*> OnBtnClickedEmitStats;
  ou::Delegate<void*> OnBtnClickedStartSimulation;
  ou::Delegate<void*> OnBtnClickedStopSimulation;

protected:

  // Do we really need to expose the implementation detail? I guess not.
  void OnQuit(wxCommandEvent& event);
  void OnAbout(wxCommandEvent& event);
  void OnClose( wxCommandEvent& event );
  void OnOpenFrameManualOrder(wxCommandEvent& event);
  void OnCloseThis( wxCloseEvent& event );

private:

  enum { ID_Quit=wxID_HIGHEST + 1, ID_About, ID_CloseWindow,
    ID_ConnectIB, ID_ConnectIQFeed, ID_ConnectSim,
    ID_DisConnectIB, ID_DisConnectIQFeed, ID_DisConnectSim,
    ID_AddPortfolio, ID_ManualTrade
  };

  typedef std::vector<FrameManualOrder*> vFrameManualOrder_t;
  vFrameManualOrder_t m_vFrameManualOrders;

  void CleanUpForExit( void );


};
