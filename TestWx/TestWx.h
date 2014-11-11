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

#pragma once

// Started 2013/12/12

#include <TFVuTrading/FrameMain.h>
#include <TFVuTrading/PanelLogging.h>

class AppTestWx: public wxApp {
public:
protected:
private:
  FrameMain* m_pFrameMain;
  ou::tf::PanelLogging* m_pPanelLogging;
  
  wxPanel* m_pPanel;

  void TestChart( void );

  virtual bool OnInit();
  virtual int OnExit();
  void OnClose( wxCloseEvent& event );
};

DECLARE_APP(AppTestWx)