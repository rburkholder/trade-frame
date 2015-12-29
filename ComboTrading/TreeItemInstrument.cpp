/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 13, 2015, 8:16 PM

#include "TreeItemInstrument.h"

void TreeItemInstrument::Init() {
  /*
   * work on loading instrument list in background
   * confirm that subsets can be saved and loaded
   * need instrument list
   */
  m_pInstrument = m_resources.signalNewInstrument(); 
  if ( 0 != m_pInstrument.get() ) {
    m_baseResources.signalSetItemText( m_id, m_pInstrument->GetInstrumentName() );
    // set watch
  }
}

void TreeItemInstrument::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  pMenu->Append( MILiveChart, "Live Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleLiveChart, this, MILiveChart );
  pMenu->Append( MIDailyChart, "Daily Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDailyChart, this, MIDailyChart );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDelete, this, MIDelete );
}

void TreeItemInstrument::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Delete: TreeItemInstrument" << std::endl;
  m_baseResources.signalDelete( this->m_id );
}

void TreeItemInstrument::HandleLiveChart( wxCommandEvent& event ) {
  
}

void TreeItemInstrument::HandleDailyChart( wxCommandEvent& event ) {
  
}
