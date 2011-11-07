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

// Strategy1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "Strategy1.h"

IMPLEMENT_APP(AppStrategy1)

bool AppStrategy1::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  m_pFrameMain->Center();
  m_pFrameMain->Show( true );
  SetTopWindow( m_pFrameMain );

  m_pPanelSimulationControl = new PanelSimulationControl( m_pFrameMain, wxID_ANY );
  m_pPanelSimulationControl->Show( true );

  m_pPanelSimulationControl->SetOnStartSimulation( MakeDelegate( this, &AppStrategy1::HandleBtnSimulationStart) );

  m_pStrategy = new Strategy;

  return 1;

}

int AppStrategy1::OnExit() {
//  m_pPanelSimulationControl->SetOnStartSimulation(0 );
//  delete m_pStrategy;
  return 0;
}

void AppStrategy1::HandleBtnSimulationStart( void ) {
  m_pStrategy->Start( "" );
}