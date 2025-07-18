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
 * File:    InstrumentViews.vpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 10:26:28
 */

#include <boost/log/trivial.hpp>

#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/treectrl.h>

#include <TFTrading/InstrumentManager.h>

#include <TFVuTrading/TreeItem.hpp>

#include "InstrumentViews.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

InstrumentViews::InstrumentViews(): wxPanel() {
  Init();
}

InstrumentViews::InstrumentViews( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel()
{
  Init();
  Create( parent, id, pos, size, style, name );
}

InstrumentViews::~InstrumentViews() {
}

void InstrumentViews::Init() {
  m_pTreeCtrl = nullptr;
  m_pRootTreeItem = nullptr;
}

bool InstrumentViews::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

    SetExtraStyle( wxWS_EX_VALIDATE_RECURSIVELY );
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if ( GetSizer() ) {
      GetSizer()->SetSizeHints( this );
    }

  return true;
}

void InstrumentViews::CreateControls() {

  InstrumentViews* itemPanel1 = this;

  wxBoxSizer* itemBoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
  itemPanel1->SetSizer( itemBoxSizer1 );

  m_pTreeCtrl = new wxTreeCtrl( itemPanel1, ID_TREECTRL, wxDefaultPosition, wxDefaultSize,
    wxTR_NO_LINES | wxTR_HAS_BUTTONS /*| wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT*/ | wxTR_SINGLE /*| wxTR_TWIST_BUTTONS*/ );
  m_pTreeCtrl->Bind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &InstrumentViews::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
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
          DialogSymbol();
        }
      );
    }
  );

  if ( InstrumentViews::ShowToolTips() ) {
    m_pTreeCtrl->SetToolTip(_( "Symbols / Actions" ) );
  }

  Bind( wxEVT_DESTROY, &InstrumentViews::OnDestroy, this );

}

void InstrumentViews::HandleTreeEventItemGetToolTip( wxTreeEvent& event ) {
  event.SetToolTip( "to be fixed" );
  event.Skip();
}

void InstrumentViews::Set( ou::tf::iqfeed::Provider::pProvider_t& piqf ) {

  assert( piqf );
  assert( piqf->Connected() );
  m_piqf = piqf;

  m_pComposeInstrument = std::make_unique<ou::tf::ComposeInstrument>(
    piqf,
    [this](){
      for ( const setInstrumentName_t::value_type& vt: m_setInstrumentName ) {
        AddSymbol( vt );
      }
      m_setInstrumentName.clear();
    } );

}

void InstrumentViews::DialogSymbol() {

  wxTextEntryDialog dialog( this, "Symbol Name:", "Add Symbol" );
  //dialog->ForceUpper(); // prints charters in reverse
  if ( wxID_OK == dialog.ShowModal() ) {
    std::string sIQFeedSymbolName = dialog.GetValue().Upper();
    if ( 0 < sIQFeedSymbolName.size() ) {
      AddSymbol( sIQFeedSymbolName );
    }
  }

}

void InstrumentViews::AddSymbol( const std::string& sIQFeedSymbolName ) {

  // todo: tool tip shows real symbol name, map is user requested symbol

  mapInstrument_t::iterator iterInstrument = m_mapInstrument.find( sIQFeedSymbolName );
  if ( m_mapInstrument.end() != iterInstrument ) {
    BOOST_LOG_TRIVIAL(warning) << "symbol " << sIQFeedSymbolName << " exists";
  }
  else {
    // need the database here for persistence
    // need to change bool to tri-state:  constructed, cached, does not exist
    // don't add to map prior to this, need to determine if instrument exists first
    m_pComposeInstrument->Compose(
      sIQFeedSymbolName,
      [this]( pInstrument_t pInstrument, bool bConstructed ){
        if ( pInstrument ) {
          if ( bConstructed ) {
            ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
            im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
          }
          BuildView( pInstrument );
        }
      } );
  }

}

void InstrumentViews::BuildView( pInstrument_t& pInstrument ) {

  if ( pInstrument ) {

    const std::string& sNameGeneric( pInstrument->GetInstrumentName() );
    const std::string& sNameIQFeed(  pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );

    auto result = m_mapInstrument.emplace( sNameIQFeed, Instrument() );
    assert( result.second );
    mapInstrument_t::iterator iterInstrument = result.first;
    Instrument& instrument( iterInstrument->second );

    instrument.pti = m_pRootTreeItem->AppendChild( sNameGeneric );
    instrument.pInstrument = pInstrument;

    m_pRootTreeItem->Expand();
    wxSize sizeBest = m_pTreeCtrl->GetBestSize();
    wxSize sizeVirt = m_pTreeCtrl->GetVirtualSize();
    //wxSize sizeWhat = m_pTreeCtrl->GetBestVirtualSize();
    wxSize sizeSet( sizeVirt.x > sizeBest.x ? sizeVirt.x : sizeBest.x, -1 );
    //m_pTreeCtrl->SetSize( sizeSet );
    m_pTreeCtrl->SetMinSize( sizeSet );
    //GetSizer()->SetSizeHints( this );
    Layout();
    //GetParent()->GetSizer()->SetSizeHints( GetParent() );
    GetParent()->Layout();

    m_pRootTreeItem->SortChildren();
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "symbol/instrument not found";
  }



  //OptionChainView* pOptionChainView = new OptionChainView( this );
  //GetSizer()->Add( pOptionChainView, 1, wxALL | wxEXPAND, 0 );
}

void InstrumentViews::OnDestroy( wxWindowDestroyEvent& event ) {
  //TreeItem::UnBind( this, m_pTree ); // to be fixed
  m_pTreeCtrl->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &InstrumentViews::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTreeCtrl->DeleteAllItems();
  assert( Unbind( wxEVT_DESTROY, &InstrumentViews::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap InstrumentViews::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

} // namespace tf
} // namespace ou
