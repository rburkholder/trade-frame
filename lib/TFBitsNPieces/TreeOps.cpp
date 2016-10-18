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

// started December 7, 2015, 9:39 PM

#include <vector>

#include <boost/phoenix/bind/bind_member_function.hpp>
#include <boost/phoenix/core/argument.hpp>

#include <wx/wx.h>
//#include <wx-3.0/wx/window.h>

#include "TreeOps.h"

namespace ou {
namespace tf {

//IMPLEMENT_DYNAMIC_CLASS( TreeOps, wxTreeCtrl )

TreeOps::TreeOps() {
  Init();
}

TreeOps::TreeOps( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool TreeOps::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxTreeCtrl::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

TreeOps::~TreeOps() {
  // m_resources.pAudio.reset();  // implied now
}

void TreeOps::Init() {
  //m_pResources.reset( new TreeItemResources( *this ) );
  //m_resources.pAudio = 0; // implied now
  //m_resources.sCurrentPath = std::string( ::wxGetCwd() );
}

void TreeOps::CreateControls() {    
  
  TreeOps* item = this;
  
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_MENU, &TreeOps::HandleContextMenu, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGED, &TreeOps::HandleSelectionChanged, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGING, &TreeOps::HandleSelectionChanging, this );
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_ACTIVATED, &TreeOps::HandleItemActivated, this );
  wxTreeCtrl::Bind( wxEVT_TREE_DELETE_ITEM, &TreeOps::HandleItemDeleted, this );
  
  //wxTreeItemId id = wxTreeCtrl::AddRoot( "Root" );  // can be renamed
  //m_pTreeItemRoot.reset( new TreeItemRoot( id, m_resources ) );
  //m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), m_pTreeItemRoot ) );
  
}

void TreeOps::Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase ) {
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), pTreeItemBase ) );
}

void TreeOps::Delete( wxTreeItemId id ) {
  if ( 0 == GetChildrenCount( id ) ) {
    
    // need to detect that it is deleting itself, and that it is currently selected
    if ( m_idOld.IsOk() ) {
      if ( m_idOld = id ) {
        //m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
      }
    }
    
    wxTreeItemId idParent = wxTreeCtrl::GetItemParent( id );
    assert( idParent.IsOk() );
    typename mapDecoder_t::iterator iterParent = m_mapDecoder.find( idParent.GetID() );
    assert( m_mapDecoder.end() != iterParent );
    iterParent->second->DeletingChild( id );
    
    typename mapDecoder_t::iterator iterChild = m_mapDecoder.find( id.GetID() );
    assert( m_mapDecoder.end() != iterChild );
    
    wxTreeCtrl::Delete( id );
    m_idOld.Unset();
    m_mapDecoder.erase( iterChild );
  }
  else {
    std::cout << "item has children" << std::endl;
  }
}

void TreeOps::HandleContextMenu( wxTreeEvent& event ) {
  m_mapDecoder[ event.GetItem().GetID() ]->ShowContextMenu();
}

void TreeOps::RemoveSelectOld( void ) {
  //std::cout << "HandleSelectionChanging " << event.GetItem().GetID() << std::endl;
  //if ( m_idOld.IsOk() ) m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
  m_idOld.Unset();
}

void TreeOps::HandleSelectionChanging( wxTreeEvent& event ) {
  //std::cout << "HandleSelectionChanging " << event.GetItem().GetID() << std::endl; // triggered first (object b)
  //std::cout << "HandleSelectionChanging " << event.GetId() << "," << event.GetItem().GetID() << std::endl; // triggered first (object b)
  signalChanging( event.GetItem().GetID() );
  RemoveSelectOld();
}

void TreeOps::HandleSelectionChanged( wxTreeEvent& event ) {
  //std::cout << "HandleSelectionChanged " << event.GetId() << "," << event.GetItem().GetID() << std::endl;  // triggered second (object b))
  m_idOld = event.GetItem();
  //m_mapDecoder[ m_idOld.GetID() ]->SetSelected( m_guiElements );
  
}

void TreeOps::HandleItemActivated( wxTreeEvent& event ) {
  //std::cout << "HandleItemActivated" << std::endl; // doesn't appear to be triggered
}

void TreeOps::HandleItemDeleted( wxTreeEvent& event ) {
  //std::cout << "HandleItemDeleted" << std::endl;
}

wxBitmap TreeOps::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeOps::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

//void TreeOps::Save( boost::archive::text_oarchive& oa ) {
  //size_t n( m_pResources->vpSceneManager.size() );
  //oa & n;

//  TreeItemBase* pBase = m_pTreeItemRoot.get();
  //const TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  //oa & *m_pTreeItemRoot;
//}

//void TreeOps::Load( boost::archive::text_iarchive& ia ) {
  //size_t n;
  //ia & n;
  //assert( m_pResources->vpSceneManager.size() == n );

//  TreeItemBase* pBase = m_pTreeItemRoot.get();
  //TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  //ia & *m_pTreeItemRoot;
//}

wxString TreeOps::GetInput( const wxString& sPrompt, const wxString& sDefault ) {
  wxString s( sDefault );
  wxTextEntryDialog* p = new wxTextEntryDialog( this, sPrompt, sPrompt, sDefault );
  if ( wxID_OK == p->ShowModal() ) {
    s = p->GetValue();
  }
  assert( p->Close() );
  return s;
}

void TreeOps::PopulateResources( TreeItemResources& resources ) {
  namespace args = boost::phoenix::arg_names;
  resources.signalAdd.connect( boost::phoenix::bind( &TreeOps::Add, this, args::arg1, args::arg2 ) );
  resources.signalDelete.connect( boost::phoenix::bind( &TreeOps::Delete, this, args::arg1 ) );
  resources.signalGetInput.connect( boost::phoenix::bind( &TreeOps::GetInput, this, args::arg1, args::arg2 ) );
  resources.signalGetItemText.connect( boost::phoenix::bind( &TreeOps::GetItemText, this, args::arg1 ) );
  resources.signalPopupMenu.connect( boost::phoenix::bind( &TreeOps::PopupMenu, this, args::arg1, wxDefaultPosition ) );
  resources.signalSetItemText.connect( boost::phoenix::bind( &TreeOps::SetItemText, this, args::arg1, args::arg2 ) );
  resources.signalAppendItem.connect( boost::phoenix::bind( &TreeOps::AppendItem, this, args::arg1, args::arg2, -1, -1, (wxTreeItemData*)0 ) );
  resources.signalEnsureVisible.connect( boost::phoenix::bind( &TreeOps::EnsureVisible, this, args::arg1 ) );
}

} // namespace tf
} // namespace ou