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

// 20161003 
//  manual control of watch/unwatch in menu at some point
//  set colour on menu item for watch/unwatch mode
//  then start the watch chart
//  then provide shift/zoom capability on the chart

// 20161009
//   should override rename so it always has proper instrument name
//   is rename actually in the menu, if not, then nothing to do

#include "TreeItemInstrument.h"

TreeItemInstrument::TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
  TreeItemResources( id, baseResources, resources ) {
}
  
TreeItemInstrument::~TreeItemInstrument( void ) {
}

void TreeItemInstrument::HandleDelete( wxCommandEvent& event ) {
  std::cout << "Delete: TreeItemInstrument" << std::endl;
  m_baseResources.signalDelete( this->m_id );
}

void TreeItemInstrument::HandleLiveChart( wxCommandEvent& event ) {
  
}

void TreeItemInstrument::HandleDailyChart( wxCommandEvent& event ) {
  
}

void TreeItemInstrument::HandleSaveData( wxCommandEvent& event ) {
  //m_pInstrumentWatch->SaveSeries()
}

void TreeItemInstrument::BuildContextMenu( wxMenu* pMenu ) {
  assert( 0 != pMenu );
  if ( 0 == m_pInstrumentWatch.use_count() ) {  // is this actually used?
    pMenu->Append( MINewInstrument, "New Instrument" );
    pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuNewInstrument, this, MINewInstrument );
  }
  else {
    if ( m_pInstrumentWatch->GetInstrument()->IsFuture() ) {
      // can then use underlying to calc implied volatility
      pMenu->Append( MINewFuturesOption, "New Futures Option" );
      pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddFuturesOption, this, MINewFuturesOption );
    }
    else {
      if ( m_pInstrumentWatch->GetInstrument()->IsStock() ) {
      // can then use underlying to calc implied volatility
        pMenu->Append( MINewOption, "New Option" );
        pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleMenuAddOption, this, MINewOption );
      }
    }
  }
  // add watch/unwatch menu item?
  pMenu->Append( MIEmit, "Emit" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleEmit, this, MIEmit );
  pMenu->Append( MILiveChart, "Live Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleLiveChart, this, MILiveChart );
  pMenu->Append( MIDailyChart, "Daily Chart" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDailyChart, this, MIDailyChart );
  pMenu->Append( MISaveData, "Save Data" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleSaveData, this, MISaveData );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemInstrument::HandleDelete, this, MIDelete );
}

void TreeItemInstrument::HandleEmit( wxCommandEvent& event ) {
  m_pInstrumentWatch->EmitValues();
}

void TreeItemInstrument::ShowContextMenu( void ) {
  if ( 0 == m_pMenu ) {
    m_pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
    TreeItemInstrument::BuildContextMenu( m_pMenu );
  }
  m_baseResources.signalPopupMenu( m_pMenu );
}

void TreeItemInstrument::InstrumentViaDialog( Resources::ENewInstrumentLock lock, const std::string& sPrompt ) {
  TreeItemInstrument* p = AddTreeItem<TreeItemInstrument>( sPrompt, IdInstrument, m_resources );
  p->NewInstrumentViaDialog( lock );
  if ( 0 == p->m_pInstrumentWatch.get() ) {
    this->m_baseResources.signalDelete( p->GetTreeItemId() );
    //DeleteMember( p->GetTreeItemId() );
  }
//  else {
//  }
}

void TreeItemInstrument::NewInstrumentViaDialog( Resources::ENewInstrumentLock lock ) {
  if ( 0 == m_pInstrumentWatch.use_count() ) {
    m_pInstrumentWatch = m_resources.signalNewInstrumentViaDialog( lock ); // call dialog
    if ( 0 != m_pInstrumentWatch.get() ) {
      m_baseResources.signalSetItemText( m_id, m_pInstrumentWatch->GetInstrument()->GetInstrumentName() );
      m_pInstrumentWatch->StartWatch();
    }
  }
  else {
    std::cout << "InstrumentWatch already assigned" << std::endl;
  }
}

void TreeItemInstrument::HandleMenuNewInstrument( wxCommandEvent& event ) {
  NewInstrumentViaDialog( Resources::NoLock );
}

/* todo:  
 *   for following two handlers:
 *   the lock should be forcing the gui to show options only for the underlying instrument
 *     when adding sub-menus to the tree
 *   the dialog needs a lock for FuturesOption and Option
 *   then need NewOptionViaDialog, NewFuturesOptionViaDialog to force that setting in the dialog
 */

// from tree menu popup
void TreeItemInstrument::HandleMenuAddOption( wxCommandEvent& event ) { 
  InstrumentViaDialog( Resources::LockOption, "Option" );
}

// from tree menu popup
void TreeItemInstrument::HandleMenuAddFuturesOption( wxCommandEvent& event ) { 
  InstrumentViaDialog( Resources::LockFuturesOption, "FuturesOption" );
}

