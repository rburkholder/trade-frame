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
#include "TreeItem_ItemData.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

/* public root */
TreeItem::TreeItem( wxTreeCtrl* tree, const std::string& sText )
: TreeItem(
    tree, sText,
    []( TreeItem* pti )->ou::tf::CustomItemData_Base*{
      return new ou::tf::CustomItemData_Base( pti );
    } )
{}

/* public root */
TreeItem::TreeItem( wxTreeCtrl* tree, const std::string& sText, fCustomItemData_Factory_t&& f )
: m_pTreeCtrl( tree )
, m_ptiParent( nullptr ), m_pMenuPopup( nullptr )
, m_fOnClick( nullptr ), m_fOnBuildPopUp( nullptr ), m_fOnDeleted( nullptr )
{
  assert( nullptr != tree );
  m_pMenuPopup = new wxMenu();
  m_idSelf = m_pTreeCtrl->AddRoot( sText, -1, -1, f( this ) );
}

/* private children */
TreeItem::TreeItem( wxTreeCtrl* tree, TreeItem* ptiParent, const std::string& sText )
: m_pTreeCtrl( tree ), m_ptiParent( ptiParent )
, m_fOnClick( nullptr ), m_fOnBuildPopUp( nullptr ), m_fOnDeleted( nullptr )
{
  assert( nullptr != tree );
  m_pMenuPopup = new wxMenu();
  m_idSelf = m_pTreeCtrl->AppendItem( ptiParent->m_idSelf, sText, -1, -1, new CustomItemData_Base( this ) );
}

/* private children */
TreeItem::TreeItem( wxTreeCtrl* tree, TreeItem* ptiParent, const std::string& sText, fCustomItemData_Factory_t&& f )
: m_pTreeCtrl( tree ), m_ptiParent( ptiParent )
, m_fOnClick( nullptr ), m_fOnBuildPopUp( nullptr ), m_fOnDeleted( nullptr )
{
  assert( nullptr != tree );
  m_pMenuPopup = new wxMenu();
  m_idSelf = m_pTreeCtrl->AppendItem( ptiParent->m_idSelf, sText, -1, -1, f( this ) );
}

TreeItem::~TreeItem() {
  if ( m_pMenuPopup ) {
    delete m_pMenuPopup;
    m_pMenuPopup = nullptr;
  }
  m_pTreeCtrl = nullptr;
  //std::cout << m_idSelf << std::endl;
}

TreeItem* TreeItem::AppendChild( const std::string& sText ) {
  TreeItem* pTreeItem = new TreeItem( m_pTreeCtrl, this, sText );
  return pTreeItem;
}

TreeItem* TreeItem::AppendChild( const std::string& sText, fOnClick_t&& fOnClick ) {
  TreeItem* pTreeItemChild = AppendChild( sText );
  pTreeItemChild->SetOnClick( std::move( fOnClick ) );
  return pTreeItemChild;
}

TreeItem* TreeItem::AppendChild( const std::string& sText, fOnClick_t&& fOnClick, fOnBuildPopUp_t&& fOnBuildPopUp ) {
  TreeItem* pTreeItemChild = AppendChild( sText );
  pTreeItemChild->SetOnClick( std::move( fOnClick ) );
  pTreeItemChild->SetOnBuildPopUp( std::move( fOnBuildPopUp ) );
  return pTreeItemChild;
}

TreeItem* TreeItem::AppendChild(
  const std::string& sText
, fOnClick_t&& fOnClick, fOnBuildPopUp_t&& fOnBuildPopUp
, fCustomItemData_Factory_t&& fCustomItemData_Factory
) {
  TreeItem* pTreeItemChild = new TreeItem( m_pTreeCtrl, this, sText, std::move( fCustomItemData_Factory ) );
  //TreeItem* pTreeItemChild = AppendChild( sText );
  pTreeItemChild->SetOnClick( std::move( fOnClick ) );
  pTreeItemChild->SetOnBuildPopUp( std::move( fOnBuildPopUp ) );
  return pTreeItemChild;
}

void TreeItem::SortChildren() {
  m_pTreeCtrl->SortChildren( m_idSelf );
}

void TreeItem::IterateChildren( fIterateChildren_t&& f ) {
  wxTreeItemIdValue tiv {}; // cookie
  wxTreeItemId ti = m_pTreeCtrl->GetFirstChild( m_idSelf, tiv );
  while ( ti.IsOk() ) {
    assert( ti != m_idSelf );
    wxTreeItemData* pData = m_pTreeCtrl->GetItemData( ti );
    assert( pData );
    CustomItemData_Base* pCustom = dynamic_cast<CustomItemData_Base*>( pData );
    bool bContinue = f( pCustom->GetTreeItem() );
    if ( bContinue ) {
      ti = m_pTreeCtrl->GetNextChild( m_idSelf, tiv );
    }
    else {
      break;
    }
  }
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

std::string TreeItem::GetText() const {
  return m_pTreeCtrl->GetItemText( m_idSelf );
}

void TreeItem::Delete() {
  // everything should self delete
  if ( m_pTreeCtrl ) {
    m_pTreeCtrl->Delete( m_idSelf );
    // A Deleted event should occur
    //  will need to verify sequence of events
  }
}

void TreeItem::DeleteChildren() {
  if ( m_pTreeCtrl ) {
    m_pTreeCtrl->DeleteChildren( m_idSelf );
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

CustomItemData_Base* TreeItem::CustomItemData() {
  wxTreeItemData* pData = m_pTreeCtrl->GetItemData( m_idSelf );
  if ( nullptr == pData ) {
    return nullptr;
  }
  else {
    CustomItemData_Base* pCustom = dynamic_cast<CustomItemData_Base*>( pData );
    return pCustom;
  }
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
