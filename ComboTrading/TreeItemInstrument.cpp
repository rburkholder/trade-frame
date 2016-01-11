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

TreeItemInstrument::~TreeItemInstrument( void ) {
  if ( 0 != m_pWatch ) {
    delete m_pWatch;
  }
}

void TreeItemInstrument::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Delete: TreeItemInstrument" << std::endl;
  m_baseResources.signalDelete( this->m_id );
}

void TreeItemInstrument::HandleNewInstrument( wxCommandEvent& event ) {
  NewInstrumentViaDialog();
}

void TreeItemInstrument::NewInstrumentViaDialog( void ) {
  if ( 0 == m_pInstrument.use_count() ) {
    m_pInstrument = m_resources.signalNewInstrumentViaDialog(); // call dialog
    if ( 0 != m_pInstrument.get() ) {
      m_baseResources.signalSetItemText( m_id, m_pInstrument->GetInstrumentName() );
      Watch();
    }
  }
  else {
    std::cout << "instrument already assigned" << std::endl;
  }
  
}

void TreeItemInstrument::Watch( void ) {
  if ( 0 == m_pWatch ) {
    m_pWatch = new ou::tf::Watch( m_pInstrument, m_resources.pData1Provider );
    m_pWatch->StartWatch();
  }
}

void TreeItemInstrument::HandleLiveChart( wxCommandEvent& event ) {
  
}

void TreeItemInstrument::HandleDailyChart( wxCommandEvent& event ) {
  
}

void TreeItemInstrument::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  if ( 0 == m_pInstrument.use_count() ) {
    pMenu->Append( MINewInstrument, "New Instrument" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleNewInstrument, this, MINewInstrument );
  }
  pMenu->Append( MIEmit, "Emit" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleEmit, this, MIEmit );
  pMenu->Append( MILiveChart, "Live Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleLiveChart, this, MILiveChart );
  pMenu->Append( MIDailyChart, "Daily Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDailyChart, this, MIDailyChart );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDelete, this, MIDelete );
}

void TreeItemInstrument::HandleEmit( wxCommandEvent& event ) {
  if ( 0 != m_pWatch ) {
    m_pWatch->EmitValues();
  }
}

void TreeItemInstrument::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemInstrument::BuildContextMenu( m_pMenu );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}
