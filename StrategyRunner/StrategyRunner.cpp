/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// StrategyRunner.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <wx/bitmap.h>

#include "StrategyRunner.h"

IMPLEMENT_APP(AppStrategyRunner)

bool AppStrategyRunner::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Strategy Simulator" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
//  m_pFrameMain->SetSize( 4400, 1500 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxHORIZONTAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxVERTICAL );
  m_sizerMain->Add( m_sizerControls, 0, wxALL, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_TOP|wxBOTTOM, 5);
  m_pPanelProviderControl->Show( true );

  m_pFrameMain->Show( true );

  return 1;

}

int AppStrategyRunner::OnExit() {

  return 0;
}

