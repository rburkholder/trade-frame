/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    Item.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: March 31, 2022 09:50
 */

#include <wx/menu.h>
#include <wx/treectrl.h>

#include "TreeItem.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CustomItemData_Base: public wxTreeItemData {
public:

  CustomItemData_Base( TreeItem* pTreeItem ) // self?
  : wxTreeItemData(), m_pTreeItem( pTreeItem ) {}

  ~CustomItemData_Base() {
    // assumes binds are cleared as well
    if ( m_pTreeItem ) {
      delete m_pTreeItem;
      m_pTreeItem = nullptr;
    }
  }

  TreeItem* GetTreeItem() { return m_pTreeItem; }

protected:
private:
  TreeItem* m_pTreeItem;
};

// ================================

TreeItem::TreeItem( wxTreeCtrl* tree, const std::string& sText )
: m_pTreeCtrl( tree )
, m_fOnClick( nullptr ), m_fOnBuildPopUp( nullptr ), m_fOnDeleted( nullptr )
{
  assert( nullptr != tree );
  m_pMenuPopup = new wxMenu();
  m_idSelf = m_pTreeCtrl->AddRoot( sText, -1, -1, new CustomItemData_Base( this ) );
}

TreeItem::TreeItem( wxTreeCtrl* tree, wxTreeItemId idParent, const std::string& sText ) // private constructor
: m_pTreeCtrl( tree ), m_idParent( idParent )
, m_fOnClick( nullptr ), m_fOnBuildPopUp( nullptr ), m_fOnDeleted( nullptr )
{
  assert( nullptr != tree );
  m_pMenuPopup = new wxMenu();
  m_idSelf = m_pTreeCtrl->AppendItem( idParent, sText, -1, -1, new CustomItemData_Base( this ) );
}

TreeItem::~TreeItem() {
  if ( m_pMenuPopup ) {
    delete m_pMenuPopup;
    m_pMenuPopup = nullptr;
  }
  m_pTreeCtrl = nullptr;
  //std::cout << m_idSelf << std::endl;
}

TreeItem* TreeItem::AppendChild( const std::string& sText, fOnClick_t&& fOnClick, fOnBuildPopUp_t&& fOnBuildPopUp ) {
  TreeItem* pTreeItemChild = AppendChild( sText );
  pTreeItemChild->SetOnClick( std::move( fOnClick ) );
  pTreeItemChild->SetOnBuildPopUp( std::move( fOnBuildPopUp ) );
  return pTreeItemChild;
}

TreeItem* TreeItem::AppendChild( const std::string& sText, fOnClick_t&& fOnClick ) {
  TreeItem* pTreeItemChild = AppendChild( sText );
  pTreeItemChild->SetOnClick( std::move( fOnClick ) );
  return pTreeItemChild;
}

TreeItem* TreeItem::AppendChild( const std::string& sText ) {
  TreeItem* pTreeItem = new TreeItem( m_pTreeCtrl, m_idSelf, sText );
  return pTreeItem;
}

void TreeItem::SortChildren() {
  m_pTreeCtrl->SortChildren( m_idSelf );
}

void TreeItem::HandleTreeEventItemChanged() {
  assert( m_pTreeCtrl );
  if ( m_fOnClick ) {
    m_fOnClick( this );
  }
}

void TreeItem::HandleTreeEventItemMenu() {
  assert( m_pTreeCtrl );
  if ( m_fOnBuildPopUp ) {
    m_fOnBuildPopUp( this );
  }
}

void TreeItem::UpdateText( const std::string& sText ) {
  assert( m_pTreeCtrl );
  m_pTreeCtrl->SetItemText( m_idSelf, sText );
}

void TreeItem::Delete() {
  //  if ( 0 < m_pTree->GetChildrenCount( id ) ) throw std::runtime_error( "item has children" );
  // everything should self delete
  if ( m_pTreeCtrl ) {
    m_pTreeCtrl->Delete( m_idSelf );
    // A Deleted event should occur
    //  will need to verify sequence of events
  }
}

// when the associated menu item has been deleted
void TreeItem::Deleted( const wxTreeItemId& id ) {
  if ( m_fOnDeleted ) {
    m_fOnDeleted();
  }
  if ( id == m_idSelf ) {
    m_pTreeCtrl = nullptr;
  }
}

void TreeItem::NewMenu() {
  if ( m_pMenuPopup ) {
    delete m_pMenuPopup;
  }
  m_pMenuPopup = new wxMenu();
}

void TreeItem::AppendMenuItem( const std::string& sText, fOnClick_t&& fOnClick ) {
  wxMenuItem* pMenuItem = m_pMenuPopup->Append( wxID_ANY, sText );
  int idPopUp = pMenuItem->GetId();
  m_pMenuPopup->Bind(
    wxEVT_COMMAND_MENU_SELECTED,
    [this,fOnClick_=std::move( fOnClick )]( wxCommandEvent& event ){
      fOnClick_( this );
    },
    idPopUp
    );
}

/* static */
void TreeItem::Bind( wxWindow* pWindow, wxTreeCtrl* pTree ) {

  assert( pWindow );
  assert( pTree );

  pTree->Bind(
    wxEVT_TREE_SEL_CHANGED,
    [pTree]( wxTreeEvent& event ){
      wxTreeItemData* pData = pTree->GetItemData( event.GetItem() );
      if ( nullptr != pData ) {
        CustomItemData_Base* pCustom = dynamic_cast<CustomItemData_Base*>( pData );
        pCustom->GetTreeItem()->HandleTreeEventItemChanged();
      }
    },
    pTree->GetId()
    );

  pTree->Bind(
    wxEVT_TREE_ITEM_MENU, //&AppIndicatorTrading::HandleTreeEventItemMenu, this, m_ptreeTradables->GetId() );
    [pTree,pWindow]( wxTreeEvent& event ){
      wxTreeItemData* pData = pTree->GetItemData( event.GetItem() );
      assert( nullptr != pData );
      CustomItemData_Base* pCustom = dynamic_cast<CustomItemData_Base*>( pData );
      pCustom->GetTreeItem()->HandleTreeEventItemMenu(); // build a new menu if needed
      wxMenu* pMenuPopup = pCustom->GetTreeItem()->m_pMenuPopup;
      if ( pMenuPopup ) {
        pWindow->PopupMenu( pMenuPopup, event.GetPoint() );
      }
    },
    pTree->GetId()
    );

  pTree->Bind(
    wxEVT_TREE_DELETE_ITEM, // seems to come during the deletion request, prior to destructor, so looks safe
    [pTree]( wxTreeEvent& event ){
      wxTreeItemData* pData = pTree->GetItemData( event.GetItem() );
      assert( nullptr != pData );
      CustomItemData_Base* pCustom = dynamic_cast<CustomItemData_Base*>( pData );
      pCustom->GetTreeItem()->Deleted( event.GetItem() );
    },
    pTree->GetId()
  );

}

/* static */
//void TreeItem::UnBind( wxWindow* pWindow, wxTreeCtrl* pTree ) {

//  assert( pWindow );
//  assert( pTree );

  //pTree->Unbind( wxEVT_TREE_SEL_CHANGED, nullptr, pTree->GetId() );
  //pTree->Unbind( wxEVT_TREE_ITEM_MENU, nullptr, pTree->GetId() );
  //pTree->Unbind( wxEVT_TREE_DELETE_ITEM, nullptr, pTree->GetId() );

//}

} // namespace tf
} // namespace ou
