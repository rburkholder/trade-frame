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

#include <fmt/core.h>

#include <wx/sizer.h>
#include <wx/textdlg.h>
#include <wx/treectrl.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/ComposeInstrument.hpp>

#include <TFIQFeed/BarHistory.h>
#include <TFIQFeed/OptionChainQuery.h>

#include <TFOptions/Chains.h>
#include <TFOptions/Engine.hpp>

#include <TFVuTrading/TreeItem.hpp>
#include "TFVuTrading/WinChartView.h"

#include "InstrumentViews.hpp"
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
  m_pOptionChainView = nullptr;
  m_pOptionChainModel = nullptr;
  m_mapInstrument.clear();
  m_fBuildWatch = nullptr;
  m_fBuildOption = nullptr;
  m_pComposeInstrument.reset();
  m_pOptionEngine.reset();
}

void InstrumentViews::Init() {
  m_pcurView = nullptr;
  m_pOptionChainView = nullptr;
  m_pOptionChainModel = nullptr;
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

  wxArrayString m_lbTagsStrings;
  m_clbTags = new wxCheckListBox(
    itemPanel1, wxID_ANY, wxDefaultPosition, wxDefaultSize,
    m_lbTagsStrings, wxLB_MULTIPLE|wxLB_EXTENDED|wxLB_NEEDED_SB //|wxLB_SORT
  );
  itemBoxSizer1->Add( m_clbTags, 0, wxGROW|wxALL, 1 );
  m_clbTags->SetMinClientSize( wxSize( 150, -1 ) );
  m_clbTags->Bind( wxEVT_CHECKLISTBOX, &InstrumentViews::HandleCheckListBoxEvent, this );

  m_pTreeCtrl = new wxTreeCtrl( itemPanel1, ID_TREECTRL, wxDefaultPosition, wxDefaultSize,
    wxTR_NO_LINES | wxTR_HAS_BUTTONS /*| wxTR_LINES_AT_ROOT | wxTR_HIDE_ROOT*/ | wxTR_SINGLE /*| wxTR_TWIST_BUTTONS*/ );
  ou::tf::TreeItem::Bind( this, m_pTreeCtrl );
  m_pTreeCtrl->Bind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &InstrumentViews::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() ); //wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP     wxEVT_TREE_ITEM_GETTOOLTIP
  m_pTreeCtrl->Bind( wxEVT_TREE_ITEM_EXPANDED, &InstrumentViews::HandleTreeEventItemExpanded, this, m_pTreeCtrl->GetId() );
  m_pTreeCtrl->ExpandAll();

  itemBoxSizer1->Add( m_pTreeCtrl, 0, wxGROW|wxALL, 1 );

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

  m_pOptionChainView = new OptionChainView( itemPanel1 );
  itemBoxSizer1->Add( m_pOptionChainView, 1, wxALL | wxEXPAND, 1 );
  m_pOptionChainView->Show();

  Layout();
  GetParent()->Layout();

  Bind( wxEVT_DESTROY, &InstrumentViews::OnDestroy, this );

  m_timerRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &InstrumentViews::HandleTimer, this, m_timerRefresh.GetId() );
  m_timerRefresh.Start( 500 );
}

void InstrumentViews::HandleTimer( wxTimerEvent& event ) {
  if ( ( nullptr != m_pOptionChainView ) && ( nullptr != m_pOptionChainModel ) ) {
    wxDataViewItem dviTopItem = m_pOptionChainView->GetTopItem();
    int nRows = m_pOptionChainView->GetCountPerPage();
    m_pOptionChainModel->HandleTimer( dviTopItem, nRows );
  }
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
, pBarHistory_t&& pBarHistory
, ou::tf::WinChartView* pWinChartView_session
, ou::tf::WinChartView* pWinChartView_daily
, fUpdateDividendFields_t&& fUpdateDividendFields
) {

  assert( fBuildWatch );
  assert( fBuildOption );
  assert( pOptionEngine );

  assert( pBarHistory );

  assert( pWinChartView_session );
  assert( pWinChartView_daily );

  assert( fUpdateDividendFields );

  m_fBuildWatch = std::move( fBuildWatch );
  m_fBuildOption = std::move( fBuildOption );
  m_pOptionEngine = pOptionEngine;

  m_pBarHistory = std::move( pBarHistory );

  m_pWinChartView_session = pWinChartView_session;
  m_pWinChartView_daily = pWinChartView_daily;

  m_fUpdateDividendFields = std::move( fUpdateDividendFields );

  m_pComposeInstrument = pComposeInstrument;
  for ( const mapStateCache_t::value_type& vt: m_mapStateCache ) {
    BuildInstrument( vt.first );
  }
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
    if ( !result.second ) {
      BOOST_LOG_TRIVIAL(warning) << "AddInstrument - already added - " << sNameGeneric << " - " << sNameIQFeed;
    }
    else {
      assert( result.second );
      mapInstrument_t::iterator iterInstrument = result.first;
      Instrument& instrument( iterInstrument->second );

      mapStateCache_t::iterator iterCache = m_mapStateCache.find( sNameIQFeed );
      if ( m_mapStateCache.end() != iterCache ) {
        instrument.notesDividend.sNotes = iterCache->second.sDvidendNotes;
        m_mapStateCache.erase( iterCache );
      }

      instrument.pInstrument = pInstrument;
      instrument.mdbm.Set( instrument.pInstrument->GetInstrumentType() );

      pWatch_t pWatch = m_fBuildWatch( pInstrument );
      instrument.Set( pWatch );

      AddInstrumentToTree( instrument );
    }
  }
  else {
    BOOST_LOG_TRIVIAL(error) << "symbol/instrument not found";
  }

}

void InstrumentViews::UpdateDividendNotes( Instrument& instrument ) {

  const std::string& sNameGeneric( instrument.pInstrument->GetInstrumentName() );
  const std::string& sNameIQFeed(  instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );

  DividendNotes& dn( instrument.notesDividend );

  //std::cout
  //<< iterSymbolInfo->first
  //<< ",yld=" << ki.dblYield
  //<< ",lst=" << ki.dblLast
  //<< ",rate=" << ki.dblRate
  //<< "," << ki.sCompanyName
  //<< std::endl;

  ou::tf::PanelDividendNotes::Fields fields;
  fields.sYield  = fmt::format( "{:.{}f}", dn.dblYield, 2 );
  fields.sLast   = fmt::format( "{:.{}f}", dn.dblLast, 2 );
  fields.sAmount = fmt::format( "{:.{}f}", dn.dblAmount, 2 );
  fields.sRate   = fmt::format( "{:.{}f}", dn.dblRate, 2 );

  if ( 0.0 < dn.dblYield ) {
    fields.sExDiv = ou::tf::Instrument::BuildDate( dn.dateExDividend );
    fields.sPayed = ou::tf::Instrument::BuildDate( dn.datePayed );
    }
  else {
    fields.sExDiv = std::string();
    fields.sPayed = std::string();
    }
  fields.sNotes = dn.sNotes;
  fields.sSymbol = sNameGeneric;
  //fields.sName = boost::lexical_cast<std::string>( dn.sCompanyName );
  fields.sName = dn.sCompanyName;
  fields.fBtnUndo =
    [&instrument]()->std::string{
      return instrument.notesDividend.sNotes;
    };
  fields.fBtnSave =
    [&instrument]( const std::string& sNotes ){
      instrument.notesDividend.sNotes = sNotes;
    };

  wxArrayString rTag;
  m_TagSymbolMap.TagListBySymbol(
    sNameIQFeed,
    [this,&rTag]( const TagSymbolMap::sTag_t& sTag ){
      rTag.Add( sTag );
    } );

  m_fUpdateDividendFields( fields, rTag );
}

void InstrumentViews::AddInstrumentToTree( Instrument& instrument ) {

  const std::string& sNameGeneric( instrument.pInstrument->GetInstrumentName() );

  instrument.pti = m_pRootTreeItem->AppendChild(
    sNameGeneric,
    [this,&instrument,&sNameGeneric]( ou::tf::TreeItem* pti ){ // fClick_t
      //m_pWinChartView_session->SetLive_trail(); // todo: revert to this after testing
      m_pWinChartView_session->SetLive_review();
      m_pWinChartView_session->SetChartDataView( instrument.sbm.GetChartDataView() );

      m_pWinChartView_daily->SetLive_review();
      m_pWinChartView_daily->SetChartDataView( instrument.mdbm.GetChartDataView() );

      UpdateDividendNotes( instrument );

      if ( instrument.sbm.IsWatching() ) {}
      else {
        BuildDailyBarModel( instrument );
      }
    },
    [this,&instrument]( ou::tf::TreeItem* pti ){ // fOnBuildPopup_t
      pti->NewMenu();
      pti->AppendMenuItem(
        "option chain",
        [this,&instrument]( ou::tf::TreeItem* pti ){
          BuildOptionChains( instrument );
        } );
      pti->AppendMenuItem(
        "add tag",
        [this,&instrument]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog dialog( this, "Tag Name:", "Add Tag" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog.ShowModal() ) {
            //const std::string& sSymbolName( instrument.pInstrument->GetInstrumentName( ) );
            const std::string& sNameIQFeed( instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );
            std::string sTag = dialog.GetValue().Upper();
            if ( 0 < sTag.size() ) {
              AddTag( sTag, sNameIQFeed );
              //CallAfter(
              //  [this](){
              //    FilterByTag();
              //} );
            }
          }
        } );
      pti->AppendMenuItem(
        "delete tag",
        [this,&instrument]( ou::tf::TreeItem* pti ){
          wxTextEntryDialog dialog( this, "Tag Name:", "Delete Tag" );
          //dialog->ForceUpper(); // prints charters in reverse
          if ( wxID_OK == dialog.ShowModal() ) {
            //const std::string& sSymbolName( instrument.pInstrument->GetInstrumentName( ) );
            const std::string& sNameIQFeed( instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );
            std::string sTag = dialog.GetValue().Upper();
            if ( 0 < sTag.size() ) {
              DelTag( sTag, sNameIQFeed );
              //CallAfter(
              //  [this](){
              //    FilterByTag();
              //} );
            }
          }
        } );
      pti->AppendMenuItem(
        "delete symbol",
        [this,&instrument]( ou::tf::TreeItem* pti ){
          const std::string& sNameIQFeed( instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );
          mapInstrument_t::iterator iterInstrument = m_mapInstrument.find( sNameIQFeed );
          assert( m_mapInstrument.end() != iterInstrument );
          CallAfter(
            [this,&instrument,iterInstrument,&sNameIQFeed](){
              // todo: need to clear gui chains if exists
              m_TagSymbolMap.DelTagsForSymbol(
                sNameIQFeed,
                [this]( const TagSymbolMap::sTag_t& sTag ){
                  int n = m_clbTags->FindString( sTag );
                  m_clbTags->Delete( n );
                } );
              assert( nullptr != instrument.pti );
              instrument.pti->Delete();
              m_mapInstrument.erase( iterInstrument );
            } );
        } );
    }
  );

  m_pRootTreeItem->Expand();
  SizeTreeCtrl();

  m_pRootTreeItem->SortChildren();

  //SizeTreeCtrl();

}

void InstrumentViews::AddTag( const TagSymbolMap::sTag_t& sTag, const TagSymbolMap::sSymbol_t& sSymbol ) {

  m_TagSymbolMap.AddTag(
    sTag, sSymbol,
    [this]( const TagSymbolMap::sTag_t& sTag ){
      wxArrayString rTag;
      rTag.Add( sTag );
      CallAfter(
        [this,rTag_=std::move(rTag)](){
          m_clbTags->InsertItems( rTag_, 0 );
        } );
    } );

}

void InstrumentViews::DelTag( const TagSymbolMap::sTag_t& sTag, const TagSymbolMap::sSymbol_t& sSymbol ) {

  m_TagSymbolMap.DelTag(
    sTag, sSymbol,
    [this]( const TagSymbolMap::sTag_t& sTag ){
      CallAfter(
        [this,sTag](){
          int n = m_clbTags->FindString( sTag );
          m_clbTags->Delete( n );
        } );
    } );

}

void InstrumentViews::BuildOptionChains( Instrument& instrumentUnderlying ) {

  const std::string& sNameGeneric( instrumentUnderlying.pInstrument->GetInstrumentName() );
  const std::string& sNameIQFeed(  instrumentUnderlying.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );

  auto fProcessOptionList =
    [this,&sNameGeneric,&instrumentUnderlying]( const ou::tf::iqfeed::OptionChainQuery::OptionList& list ){
      auto nDownCount( list.vSymbol.size() );
      BOOST_LOG_TRIVIAL(info) << "chain request " << sNameGeneric << " has " << nDownCount << " options";

      for ( const std::string& sIQFeedSymbolName: list.vSymbol ) {
        --nDownCount;
        const bool bLastOne( 0 == nDownCount );
        m_pComposeInstrument->Compose(
          sIQFeedSymbolName,
          [this, bLastOne, &instrumentUnderlying]( pInstrument_t pInstrumentOption, bool bConstructed ){
            if ( bConstructed ) {
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance() );
              im.Register( pInstrumentOption );  // is a CallAfter required, or can this run in a thread?
            }
            mapChains_t::iterator iterChains = ou::tf::option::GetChain( instrumentUnderlying.mapChains, pInstrumentOption );
            assert( instrumentUnderlying.mapChains.end() != iterChains );
            chain_t& chain( iterChains->second );

            // update put/call@strike with option
            Instance* pInstance
              = ou::tf::option::UpdateOption<chain_t,Instance>( chain, pInstrumentOption );
            pInstance->pInstrument = pInstrumentOption; // put / call as appropriate

            if ( bLastOne ) {
              CallAfter(
                [this, &instrumentUnderlying](){
                  PresentOptionChains( instrumentUnderlying );
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

void InstrumentViews::PresentOptionChains( Instrument& underlying ) {
  for ( mapChains_t::value_type& vtChain: underlying.mapChains ) {
    const std::string sExpiry( ou::tf::Instrument::BuildDate( vtChain.first ) );

    ou::tf::TreeItem* ptiExpiry = underlying.pti->AppendChild(
      sExpiry
    , [this,&vtChain,&underlying]( ou::tf::TreeItem* pti ){ // fOnClick_t
        if ( nullptr != m_pOptionChainModel ) {
          m_pOptionChainModel->DecRef();
          m_pOptionChainModel = nullptr;
        }
        m_pOptionChainModel = new OptionChainModel(
          vtChain, m_fBuildOption
        , [this,p=underlying.pWatch]( pOption_t pOption ){ // fOptionEngineStart_t
            try {
              m_pOptionEngine->Add( pOption, p );
            }
            catch ( const std::runtime_error& e ) {
              BOOST_LOG_TRIVIAL(error) << "engine start: " << e.what();
            }
          }
        , [this,p=underlying.pWatch]( pOption_t pOption ){ // fOptionEngineStop_t
            try {
              m_pOptionEngine->Remove( pOption, p );
            }
            catch ( const std::runtime_error& e ) {
              BOOST_LOG_TRIVIAL(error) << "engine stop: " << e.what();
            }
          }
        );
        wxDataViewItem item( m_pOptionChainModel->ClosestStrike( underlying.pWatch->LastQuote().Ask() ) );
        m_pOptionChainView->AssociateModel( m_pOptionChainModel );
        m_pOptionChainView->EnsureVisible( item );
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

// currently not used
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

void InstrumentViews::BuildSessionBarModel( Instrument& instrument ) {
  const std::string& sIQFeedSymbolName( instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );
  m_pBarHistory->Set(
    [&instrument]( const ou::tf::Bar& bar ){ // fHistory_Bar_t
      instrument.sbm.OnHistoryBar( bar );
    }
  , [this,&instrument](){ // fHistory_Done_t
      instrument.sbm.OnHistoryDone();
    }
  );
  m_pBarHistory->RequestNDaysOfBars( sIQFeedSymbolName, 60, 2 ); // 60 second bars over 2 days
}

// order of execution, chained together:
// BuildDailyBarModel
// BuildPivotModel
// BuildSessionBarModel

// note: makes use of sdm.IsWatching to be single entry here
void InstrumentViews::BuildDailyBarModel( Instrument& instrument ) {
  const std::string& sIQFeedSymbolName( instrument.pInstrument->GetInstrumentName( keytypes::eidProvider_t::EProviderIQF ) );
  //BOOST_LOG_TRIVIAL(info) << "BuildDailyBarModel " << sIQFeedSymbolName;
  m_pBarHistory->Set(
    [&instrument]( const ou::tf::Bar& bar ){ // fHistory_Bar_t
      instrument.mdbm.OnHistoryIntraBar( bar );
    }
  , [this,&instrument](){ // fHistory_Done_t
      instrument.mdbm.OnHistoryDone();
      BuildSessionBarModel( instrument );
  } );

  m_pBarHistory->RequestNDaysOfBars( sIQFeedSymbolName, 30 * 60 /* 30 minutes */, 300 /* days */ ); // provides about 203 days
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

void InstrumentViews::HandleCheckListBoxEvent( wxCommandEvent& event ) {
  auto id = event.GetSelection();
  bool b( m_clbTags->IsChecked( id ) );
  //std::cout << "selection " << id << ',' << b << std::endl;
  FilterByTag();
  event.Skip();
}

void InstrumentViews::FilterByTag() {

  m_pRootTreeItem->DeleteChildren();

  wxArrayInt rChecked;
  unsigned int nChecked = m_clbTags->GetCheckedItems( rChecked );

  if ( 0 == nChecked ) { // show all
    for ( mapInstrument_t::value_type& vt: m_mapInstrument ) {
      //AddSymbolToTree( vt.first, m_pRootTreeItem );
      AddInstrumentToTree( vt.second );
    }
  }
  else { // show subset

    using sTag_t = TagSymbolMap::sTag_t;
    using sSymbol_t = TagSymbolMap::sSymbol_t;

    using setTag_t = std::set<sTag_t>;
    setTag_t setTag;

    for ( wxArrayInt::value_type vt: rChecked ) {
      const sTag_t sTag( m_clbTags->GetString( vt ) );
      setTag.insert( sTag );
    }

    m_TagSymbolMap.SymbolListByTagSet(
      setTag,
      [this]( const sSymbol_t& sSymbol ){
        mapInstrument_t::iterator iterSymbolInfo = m_mapInstrument.find( sSymbol );
        if ( m_mapInstrument.end() == iterSymbolInfo ) {
          BOOST_LOG_TRIVIAL(error) << "FilterByTag symbol " << sSymbol << " not found";
        }
        else {
          AddInstrumentToTree( iterSymbolInfo->second );
        }
      } );
  }
}

void InstrumentViews::OnDestroy( wxWindowDestroyEvent& event ) {

  BOOST_LOG_TRIVIAL(trace) << "InstrumentViews::OnDestroy top";

  m_clbTags->Unbind( wxEVT_CHECKLISTBOX, &InstrumentViews::HandleCheckListBoxEvent, this );

  if ( m_timerRefresh.IsRunning() ) {
    m_timerRefresh.Stop();
    Unbind( wxEVT_TIMER, &InstrumentViews::HandleTimer, this, m_timerRefresh.GetId() );
  }

  BOOST_LOG_TRIVIAL(trace) << "InstrumentViews::OnDestroy 1";

  m_pcurView = nullptr;
  if ( nullptr != m_pOptionChainView ) {
    m_pOptionChainView->AssociateModel( nullptr );
  }
  if ( nullptr != m_pOptionChainModel ) {
    m_pOptionChainModel->DecRef();
    m_pOptionChainModel = nullptr;
  }

  BOOST_LOG_TRIVIAL(trace) << "InstrumentViews::OnDestroy 2";

  //TreeItem::UnBind( this, m_pTree ); // to be fixed
  m_pTreeCtrl->Unbind( wxEVT_TREE_ITEM_EXPANDED, &InstrumentViews::HandleTreeEventItemExpanded, this, m_pTreeCtrl->GetId() );
  m_pTreeCtrl->Unbind( wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, &InstrumentViews::HandleTreeEventItemGetToolTip, this, m_pTreeCtrl->GetId() );
  m_pTreeCtrl->DeleteAllItems();  // time consuming with option chains
  assert( Unbind( wxEVT_DESTROY, &InstrumentViews::OnDestroy, this ) );
  event.Skip( true );  // auto followed by Destroy();

  BOOST_LOG_TRIVIAL(trace) << "InstrumentViews::OnDestroy btm";
}

wxBitmap InstrumentViews::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

} // namespace tf
} // namespace ou
