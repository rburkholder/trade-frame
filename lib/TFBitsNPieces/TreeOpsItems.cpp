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

// started December 12, 2015, 2:11 PM

#include "TreeOpsItems.h"

namespace ou {
namespace tf {  

void TreeItemBase::AddMember( unsigned int type, wxTreeItemId id, pTreeItemBase_t p ) {
  assert( 0 != id.GetID() );
  assert( 0 != p.get() );
  m_vMembers.push_back( member_t( type, id.GetID(), p ) );
  m_mapMembers.insert( mapMembers_t::value_type( id.GetID(), p ) );
}

void TreeItemBase::DeleteMember( wxTreeItemId id ) {
  
  struct matchId {
    bool operator()( const TreeItemBase::member_t& member ) const { return member.m_void == m_v; }
    matchId( void* v ): m_v( v ) {};
    void* m_v;
  };
  
  mapMembers_t::const_iterator iterMap = m_mapMembers.find( id.GetID() );
  if ( m_mapMembers.end() != iterMap ) {
    m_mapMembers.erase( iterMap );
  }
  else assert( 0 );
  vMembers_t::iterator iterVec = std::find_if( m_vMembers.begin(), m_vMembers.end(), matchId( id.GetID() ) );
  assert( m_vMembers.end() != iterVec );
  m_vMembers.erase( iterVec );
}

void TreeItemBase::HandleRename( wxCommandEvent& event ) { 
  Rename();
}

void TreeItemBase::Rename( const wxString& sPrompt ) { 
  Rename( sPrompt, m_resources.signalGetItemText( m_id ) );
}

void TreeItemBase::Rename( const wxString& sPrompt, const wxString& sDefault ) { 
  wxString s = m_resources.signalGetInput( sPrompt, sDefault );
  m_resources.signalSetItemText( m_id, s );
}

wxTreeItemId TreeItemBase::AppendSubItem( const std::string& sLabel ) {
  wxTreeItemId id = m_resources.signalAppendItem( m_id, sLabel );
  m_resources.signalEnsureVisible( id );
  return id;
}

TreeItemBase::pTreeItemBase_t TreeItemBase::AppendSubItem( wxTreeItemId id, TreeItemBase* p ) {
  pTreeItemBase_t pTreeItemBase( p );
  m_resources.signalAdd( id, pTreeItemBase );
  return pTreeItemBase;
}

// ====================================
    
void TreeItemGroup::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddSubGroup, "Add Sub &Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleAddGroup, this, MIAddSubGroup );
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleDelete, this, MIDelete );
  m_resources.signalPopupMenu( pMenu );
}

void TreeItemGroup::HandleAddGroup( wxCommandEvent& event ) { 
  //TreeItemGroup* p = AddGroup();
  TreeItemGroup* p = AddTreeItem<TreeItemGroup>( "Group", IdGroup );
  p->Rename();
}

void TreeItemGroup::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemGroup Delete" << std::endl;
  m_resources.signalDelete( this->m_id );
}

// ================

void TreeItemRoot::ShowContextMenu( void ) {
  //wxMenu* pMenu = new wxMenu( "Surfaces");
  wxMenu* pMenu = new wxMenu();  // menu does not get deleted, so may need to reclaim afterwards.  put into a list somewhere?
  pMenu->Append( MIAddGroup, "&Add Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemRoot::HandleAddGroup, this, MIAddGroup );
  m_resources.signalPopupMenu( pMenu );
}

void TreeItemRoot::HandleAddGroup( wxCommandEvent& event ) {
  TreeItemGroup* pGroup = TreeItemGroup::AddTreeItem<TreeItemGroup>( "Group", TreeItemGroup::IdGroup );
  pGroup->Rename();
}

} // namespace tf
} // namespace ou
