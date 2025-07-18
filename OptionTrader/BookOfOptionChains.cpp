/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    BookOfOptionChains.vpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/treectrl.h>

#include <TFVuTrading/TreeItem.hpp>

#include "OptionChainView.hpp"
#include "BookOfOptionChains.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

BookOfOptionChains::BookOfOptionChains(): wxPanel() {
  Init();
}

BookOfOptionChains::BookOfOptionChains( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel()
{
  Init();
  Create( parent, id, pos, size, style, name );
}

BookOfOptionChains::~BookOfOptionChains() {
}

void BookOfOptionChains::Init() {
  m_pTreeCtrl = nullptr;
  m_pRootTreeItem = nullptr;
  m_fOnPageChanged = nullptr;
  m_fOnPageChanging = nullptr;
  m_fOnNodeCollapsed = nullptr;
  m_fOnNodeExpanded = nullptr;
}

bool BookOfOptionChains::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

    SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if ( GetSizer() ) {
      GetSizer()->SetSizeHints( this );
    }

  return true;
}

void BookOfOptionChains::CreateControls() {

  BookOfOptionChains* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
  itemPanel1->SetSizer( itemBoxSizer1 );

  m_pTreeCtrl = new wxTreeCtrl( itemPanel1, ID_TREECTRL, wxDefaultPosition, wxDefaultSize,
    wxTR_NO_LINES | wxTR_HAS_BUTTONS /*| wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT*/ | wxTR_SINGLE /*| wxTR_TWIST_BUTTONS*/ );
  m_pTreeCtrl->Bind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &BookOfOptionChains::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTreeCtrl->ExpandAll();

  itemBoxSizer1->Add( m_pTreeCtrl, 0, wxGROW|wxALL, 1 );

  ou::tf::TreeItem::Bind( this, m_pTreeCtrl );

  m_pRootTreeItem = new ou::tf::TreeItem( m_pTreeCtrl, "Symbols" );
  //m_pRootTreeItem->SetOnClick(
  //  [this]( TreeItem* pti ){
  //  });
  m_pRootTreeItem->SetOnBuildPopUp(
    [this]( ou::tf::TreeItem* pti ) {
      pti->NewMenu();
      pti->AppendMenuItem(
        "Add Symbol",
        [this]( ou::tf::TreeItem* pti ) {
          AddSymbol();
        }
      );
    }
  );

  if ( BookOfOptionChains::ShowToolTips() ) {
    m_pTreeCtrl->SetToolTip(_( "Symbols / Actions" ) );
  }

  Bind( wxEVT_DESTROY, &BookOfOptionChains::OnDestroy, this );

}

void BookOfOptionChains::HandleTreeEventItemGetToolTip( wxTreeEvent& event ) {
  event.SetToolTip( "to be fixed" );
  event.Skip();
}

void BookOfOptionChains::AddSymbol() {

  wxTextEntryDialog dialog( this, "Symbol Name:", "Add Symbol" );
  //dialog->ForceUpper(); // prints charters in reverse
  if ( wxID_OK == dialog.ShowModal() ) {
    std::string sSymbolName = dialog.GetValue().Upper();
    if ( 0 < sSymbolName.size() ) {
      ou::tf::TreeItem* pti = m_pRootTreeItem->AppendChild( sSymbolName );

      m_pRootTreeItem->Expand();
      wxSize sizeBest = m_pTreeCtrl->GetVirtualSize();
      //sizeBest.SetHeight( -1 );
      m_pTreeCtrl->SetSize( sizeBest );
      GetSizer()->SetSizeHints( this );

      //mapSymbolInfo_t::iterator iterSymbolInfo = m_mapSymbolInfo.find( sSymbolName );
      //assert( m_mapSymbolInfo.end() == iterSymbolInfo ); // symbols are unique across groups
      //if ( m_mapSymbolInfo.end() != iterSymbolInfo ) {
      //  BOOST_LOG_TRIVIAL(warning) << "symbol " << sSymbolName << " exists";
      //}
      //else {
      //  auto result = m_mapSymbolInfo.emplace( sSymbolName, SymbolInfo() );
      //  assert( result.second );
      //  iterSymbolInfo = result.first;
      //  AddSymbolToTree( sSymbolName, pti );
      //  m_ptiRoot->SortChildren();
      //}
      //OnSymbolClick( iterSymbolInfo );

      OptionChainView* pOptionChainView = new OptionChainView( this );
      GetSizer()->Add( pOptionChainView, 1, wxALL | wxEXPAND, 0 );
      GetSizer()->SetSizeHints( this );
    }
  }

}

void BookOfOptionChains::Set(
  fOnPageEvent_t&& fOnPageChanging // departed
, fOnPageEvent_t&& fOnPageChanged  // arrival
, fOnNodeEvent_t&& fOnNodeCollapsed
, fOnNodeEvent_t&& fOnNodeExpanded
) {
  m_fOnPageChanging = std::move( fOnPageChanging );
  m_fOnPageChanged  = std::move( fOnPageChanged );
  m_fOnNodeCollapsed = std::move( fOnNodeCollapsed );
  m_fOnNodeExpanded = std::move( fOnNodeExpanded );
}

void BookOfOptionChains::OnDestroy( wxWindowDestroyEvent& event ) {
  //TreeItem::UnBind( this, m_pTree ); // to be fixed
  m_pTreeCtrl->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &BookOfOptionChains::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTreeCtrl->DeleteAllItems();
  assert( Unbind( wxEVT_DESTROY, &BookOfOptionChains::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap BookOfOptionChains::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

} // namespace tf
} // namespace ou
