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

#include "SemiAuto.h"

IMPLEMENT_APP(AppSemiAuto)

bool AppSemiAuto::OnInit() {

//  m_pThreadMain = new ThreadMain( EModeLive );

  m_FrameMain = new FrameMain("Semi-Auto Trading", wxPoint(50,50), wxSize(600,900));
  m_FrameMain->Show(TRUE);
  SetTopWindow(m_FrameMain);

  m_FrameProviderControl = new FrameProviderControl( m_FrameMain );
  m_FrameProviderControl->Show( true );

  return TRUE;
}

int AppSemiAuto::OnExit() {
//  delete m_FrameMain;
//  delete m_pThreadMain;
  return 0;
}
 