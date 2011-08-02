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

#include <wx/wx.h>
#include <wx/splitter.h>

#include "VuPortfolios.h"
#include "VuPositions.h"
#include "VuOrders.h"
#include "VuExecutions.h"

class FramePortfolioPositionOrderExec : public wxFrame {
public:
  FramePortfolioPositionOrderExec(const wxString& title, const wxPoint& pos, const wxSize& size);
  ~FramePortfolioPositionOrderExec(void);
protected:
   // Do we really need to expose the implementation detail? I guess not.
   void OnQuit(wxCommandEvent& event);
   void OnAbout(wxCommandEvent& event);
private:
   enum { ID_Quit=wxID_HIGHEST + 1, ID_About, 
     ID_ConnectIB, ID_ConnectIQFeed, ID_ConnectSim,
     ID_DisConnectIB, ID_DisConnectIQFeed, ID_DisConnectSim,
     ID_AddPortfolio
   };
};

