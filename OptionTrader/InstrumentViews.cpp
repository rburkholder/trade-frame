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
#include <boost/lexical_cast.hpp>

#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/treectrl.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFIQFeed/OptionChainQuery.h>

#include <TFOptions/Chains.h>

#include <TFVuTrading/TreeItem.hpp>

#include "InstrumentViews.hpp"
#include "OptionChainView.hpp"
#include "OptionChainModel.hpp"

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
  m_pcurView = nullptr;
  m_pOptionChainView = nullptr;
  m_mapInstrument.clear();
  m_fBuildWatch = nullptr;
  m_fBuildOption = nullptr;
  m_pComposeInstrument.reset();
  m_pOptionEngine.reset();
}

void InstrumentViews::Init() {
  m_pcurView = nullptr;
  m_pOptionChainView = nullptr;
  m_pTreeCtrl = nullptr;
  m_pRootTreeItem = nullptr;
  m_fBuildWatch = nullptr;
  m_fBuildOption = nullptr;
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
  m_pTreeCtrl->Bind( wxEVT_TREE_ITEM_EXPANDED, &InstrumentViews::HandleTreeEventItemExpanded, this, m_pTreeCtrl->GetId() );
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

  m_pOptionChainView = new OptionChainView( this );
  itemBoxSizer1->Add( m_pOptionChainView, 1, wxALL | wxEXPAND, 0 );
  m_pOptionChainView->Show( false );

  Bind( wxEVT_DESTROY, &InstrumentViews::OnDestroy, this );

}

void InstrumentViews::HandleTreeEventItemGetToolTip( wxTreeEvent& event ) {
  event.SetToolTip( "to be fixed" );
  event.Skip();
}

void InstrumentViews::HandleTreeEventItemExpanded( wxTreeEvent& event ) {
  SizeTreeCtrl();
  event.Skip();
}

void InstrumentViews::Set(
  pComposeInstrument_t& pComposeInstrument
, fBuildWatch_t&& fBuildWatch
, fBuildOption_t&& fBuildOption
, pOptionEngine_t& pOptionEngine
) {

  assert( fBuildWatch );
  assert( fBuildOption );
  assert( pOptionEngine );

  m_fBuildWatch = std::move( fBuildWatch );
  m_fBuildOption = std::move( fBuildOption );
  m_pOptionEngine = pOptionEngine;

  m_pComposeInstrument = pComposeInstrument;
  for ( const setInstrumentName_t::value_type& vt: m_setInstrumentName ) {
    BuildInstrument( vt );
  }
  m_setInstrumentName.clear();
}

void InstrumentViews::DialogSymbol() {

  wxTextEntryDialog dialog( this, "Symbol Name:", "Add Symbol" );
  //dialog->ForceUpper(); // prints charters in reverse
  if ( wxID_OK == dialog.ShowModal() ) {
    std::string sIQFeedSymbolName = dialog.GetValue().Upper();
    if ( 0 < sIQFeedSymbolName.size() ) {
      BuildInstrument( sIQFeedSymbolName );
    }
  }

}

void InstrumentViews::BuildInstrument( const std::string& sIQFeedSymbolName ) {

  // todo: tool tip shows real symbol name, map is user requested iqfeed symbol

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
          CallAfter(
            [this,p=pInstrument]() mutable { // mutable on p, compiler wants it constant
              AddInstrument( p );
            } );
        }
      } );
  }
}

void InstrumentViews::AddInstrument( pInstrument_t& pInstrument ) {

  if ( pInstrument ) {

    const std::string& sNameGeneric( pInstrument->GetInstrumentName() );
    const std::string& sNameIQFeed(  pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );

    auto result = m_mapInstrument.emplace( sNameIQFeed, Instrument() );
    assert( result.second );
    mapInstrument_t::iterator iterInstrument = result.first;
    Instrument& instrument( iterInstrument->second );

    instrument.pInstrument = pInstrument;
    instrument.pti = m_pRootTreeItem->AppendChild(
      sNameGeneric,
      [this,&instrument,&sNameGeneric,&sNameIQFeed]( ou::tf::TreeItem* pti ){ // fClick_t
        // live chart?  bar chart?
      },
      [this,iterInstrument]( ou::tf::TreeItem* pti ){ // fOnBuildPopup_t
        pti->NewMenu();
        pti->AppendMenuItem(
          "option chain",
          [this,iterInstrument]( ou::tf::TreeItem* pti ){
            BuildOptionChains( iterInstrument );
          } );
      }
    );

    m_pRootTreeItem->Expand();
    SizeTreeCtrl();

    m_pRootTreeItem->SortChildren();

    SizeTreeCtrl();

  }
  else {
    BOOST_LOG_TRIVIAL(error) << "symbol/instrument not found";
  }

}

void InstrumentViews::BuildOptionChains( mapInstrument_t::iterator iterInstrumentUnderlying ) {

  Instrument& instrumentUnderlying( iterInstrumentUnderlying->second );
  const std::string& sNameGeneric( instrumentUnderlying.pInstrument->GetInstrumentName() );
  const std::string& sNameIQFeed(  instrumentUnderlying.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );

  auto fProcessOptionList =
    [this,&sNameGeneric,iterInstrumentUnderlying]( const ou::tf::iqfeed::OptionChainQuery::OptionList& list ){
      auto nDownCount( list.vSymbol.size() );
      BOOST_LOG_TRIVIAL(info) << "chain request " << sNameGeneric << " has " << nDownCount << " options";

      for ( const std::string& sIQFeedSymbolName: list.vSymbol ) {
        --nDownCount;
        const bool bLastOne( 0 == nDownCount );
        m_pComposeInstrument->Compose(
          sIQFeedSymbolName,
          [this, bLastOne, iterInstrumentUnderlying]( pInstrument_t pInstrumentOption, bool bConstructed ){
            if ( bConstructed ) {
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrumentOption );  // is a CallAfter required, or can this run in a thread?
            }
            Instrument& instrumentUnderlying( iterInstrumentUnderlying->second );
            mapChains_t::iterator iterChains = ou::tf::option::GetChain( instrumentUnderlying.mapChains, pInstrumentOption );
            assert( instrumentUnderlying.mapChains.end() != iterChains );
            chain_t& chain( iterChains->second );

            // update put/call@strike with option
            Instance* pInstance
              = ou::tf::option::UpdateOption<chain_t,Instance>( chain, pInstrumentOption );
            pInstance->pInstrument = pInstrumentOption; // put / call as appropriate

            if ( bLastOne ) {
              CallAfter(
                [this,iterInstrumentUnderlying](){
                  PresentOptionChains( iterInstrumentUnderlying );
                } );
            }
          }
        );
      }
    };

  switch ( instrumentUnderlying.pInstrument->GetInstrumentType() ) {
    case ou::tf::InstrumentType::Future:
      m_pComposeInstrument->OptionChainQuery()->QueryFuturesOptionChain(
        sNameIQFeed,
        "pc", "", "", "1", // 1 near month
        std::move( fProcessOptionList )
      );
      break;
    case ou::tf::InstrumentType::Stock:
      m_pComposeInstrument->OptionChainQuery()->QueryEquityOptionChain(
        sNameIQFeed,
        "pc", "", "1", "0","0","0",  // 1 near month
        std::move( fProcessOptionList )
      );
      break;
    default:
      BOOST_LOG_TRIVIAL(warning)
        << "Symbol " << sNameGeneric
        << " (" << sNameIQFeed
        << ") is not stock or future, ignored";
      break;
  }

}

void InstrumentViews::PresentOptionChains( mapInstrument_t::iterator iterInstrumentUnderlying ) {
  Instrument& instrumentUnderlying( iterInstrumentUnderlying->second );
  for ( mapChains_t::value_type& vtChain: instrumentUnderlying.mapChains ) {
    const std::string sExpiry( ou::tf::Instrument::BuildDate( vtChain.first ) );

    ou::tf::TreeItem* ptiExpiry = instrumentUnderlying.pti->AppendChild(
      sExpiry
    , [this,&vtChain]( ou::tf::TreeItem* pti ){ // fOnClick_t
        OptionChainModel* model = new OptionChainModel( vtChain, m_fBuildOption );
        m_pOptionChainView->AssociateModel( model );
        model->DecRef();
        m_pOptionChainView->Show();
        Layout();
        GetParent()->Layout();    }
    );
    vtChain.second.Strikes(
      [ptiExpiry]( double strike, const chain_t::strike_t& entry ){
        ou::tf::TreeItem* ptiStrike = ptiExpiry->AppendChild( boost::lexical_cast<std::string>( strike ) );
        if ( entry.call.pInstrument ) {
          ptiStrike->AppendChild( entry.call.pInstrument->GetInstrumentName() );
        }
        if ( entry.put.pInstrument ) {
          ptiStrike->AppendChild( entry.put.pInstrument->GetInstrumentName() );
        }

      } );
  }
}

void InstrumentViews::OptionChainView_select() {

  if ( nullptr != m_pcurView ) { // todo: refactor this and the same below
    //BOOST_LOG_TRIVIAL(trace) << "chain hide2";
    m_pcurView->Hide();
    m_pcurView = nullptr;
    //GetSizer()->Remove()
  }

  // todo: set associate model

  m_pcurView = m_pOptionChainView;
  m_pcurView->Show();

}

void InstrumentViews::SizeTreeCtrl() {
  //const wxSize sizeClient( m_pTreeCtrl->GetClientSize() );
  //const wxSize sizeCurrent( m_pTreeCtrl->GetSize() );
  const wxSize sizeBest( m_pTreeCtrl->GetBestSize() );
  const wxSize sizeVirt( m_pTreeCtrl->GetVirtualSize() );
  //wxSize sizeWhat = m_pTreeCtrl->GetBestVirtualSize();
  const wxSize sizeSet( sizeVirt.x > sizeBest.x ? sizeVirt.x : sizeBest.x, -1 );
  //m_pTreeCtrl->SetSize( sizeSet );
  m_pTreeCtrl->SetMinSize( sizeSet );

  Layout();
  GetParent()->Layout();
}

void InstrumentViews::OnDestroy( wxWindowDestroyEvent& event ) {
  //TreeItem::UnBind( this, m_pTree ); // to be fixed
  m_pTreeCtrl->Unbind( wxEVT_TREE_ITEM_EXPANDED, &InstrumentViews::HandleTreeEventItemExpanded, this, m_pTreeCtrl->GetId() );
  m_pTreeCtrl->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &InstrumentViews::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() );
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
