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

// started December 6, 2015, 1:26 PM

#include <vector>
#include <algorithm>

#include <boost/phoenix/core/argument.hpp>
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <wx/sizer.h>
#include <wx/icon.h>
#include <wx/menu.h>

#include <TFOptions/Option.h>

#include <TFVuTrading/WinChartView.h>
#include <TFVuTrading/NotebookOptionChains.h>

// need to check why this exists
//#include <wx/toplevel.h>
//#include <wx/sizer.h>
//#include <wx/list.h>

#include "PanelCharts.h"

// 2016/10/16 todo:
//   check serialization of instruments
//   save and retrieve data for charting
//   load data for charting
//   watch/unwatch live data on the charts
//   implement zoom/pan ability on chart

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelCharts::PanelCharts( void ): wxPanel() {
  Init();
}

PanelCharts::PanelCharts(
  wxWindow* parent, wxWindowID id,
  const wxPoint& pos,
  const wxSize& size,
  long style ): wxPanel()
{

  Init();
  Create(parent, id, pos, size, style);

}

PanelCharts::~PanelCharts() {
}

void PanelCharts::Init( void ) {

  m_panelSplitterRightPanel = nullptr;
  m_splitter = nullptr;
  m_sizerRight = nullptr;
  m_winRightDetail = nullptr;

  m_pTreeOps = nullptr;
  m_pInstrumentActions.reset( new InstrumentActions );

}

bool PanelCharts::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {

  wxPanel::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer())     {
      GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

/*
   m_sizerPM = new wxBoxSizer(wxVERTICAL);
  SetSizer(m_sizerPM);

  //m_scrollPM = new wxScrolledWindow( m_pFPPOE, wxID_ANY, wxDefaultPosition, wxSize(200, 400), wxVSCROLL );
  m_scrollPM = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxVSCROLL );
  m_sizerPM->Add(m_scrollPM, 1, wxGROW|wxALL, 5);
  m_scrollPM->SetScrollbars(1, 1, 0, 0);

  m_sizerScrollPM = new wxBoxSizer(wxVERTICAL);
  m_scrollPM->SetSizer( m_sizerScrollPM );
*/

void PanelCharts::CreateControls() {

  PanelCharts* itemPanel1 = this;

  wxBoxSizer* sizerMain = new wxBoxSizer(wxVERTICAL);
  itemPanel1->SetSizer(sizerMain);

  m_splitter = new wxSplitterWindow( this );
  m_splitter->SetMinimumPaneSize(10);
  m_splitter->SetSashGravity(0.2);

  // tree
  //wxTreeCtrl* tree;
//  m_pTreeSymbols = new wxTreeCtrl( splitter );
  //wxTreeItemId idRoot = m_pTreeSymbols->AddRoot( "/", -1, -1, new CustomItemData( CustomItemData::Root, m_eLatestDatumType ) );
//  wxTreeItemId idRoot = m_pTreeSymbols->AddRoot( "Collections", -1, -1, new CustomItemBase );
  //m_pHdf5Root->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &PanelChartHdf5::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId() );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
//  m_pFrameMain->Bind( wxEVT_COMMAND_TREE_SEL_CHANGED, &AppLiveChart::HandleTreeEventItemActivated, this, m_pHdf5Root->GetId()  );
  //m_pTreeSymbols->Bind( wxEVT_COMMAND_TREE_ITEM_MENU, &PanelCharts::HandleTreeItemMenu, this, m_pTreeSymbols->GetId()  );
  //m_pTreeSymbols->AppendItem( idRoot, "equities" );
  //m_pTreeSymbols->AppendItem( idRoot, "futures" );
  //m_pTreeSymbols->AppendItem( idRoot, "foptions" );
  //m_pTreeSymbols->AppendItem( idRoot, "portfolios" );

  m_pTreeOps = new ou::tf::TreeOps( m_splitter );
  m_pTreeOps->PopulateResources( m_baseResources );

  wxTreeItemId item = m_pTreeOps->AddRoot( "Instruments" );  // can be renamed
  boost::shared_ptr<TreeItemRoot> p( new TreeItemRoot( item, m_baseResources, m_resources ) );
  m_pTreeOps->SetRoot( p );
  //m_pTreeItemRoot.reset( new TreeItemRoot( id, m_resources ) );
  //m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), m_pTreeItemRoot ) );

  // panel for right side of m_splitter
  m_panelSplitterRightPanel = new wxPanel( m_splitter );

  m_splitter->SplitVertically( m_pTreeOps, m_panelSplitterRightPanel, 0 );
  sizerMain->Add( m_splitter, 1, wxGROW|wxALL, 2 );

  // sizer for right side of m_splitter
  m_sizerRight = new wxBoxSizer( wxVERTICAL );
  m_panelSplitterRightPanel->SetSizer( m_sizerRight );

  // initialize the tree
  //m_pHdf5Root->DeleteChildren( m_pHdf5Root->GetRootItem() );

  Bind( wxEVT_DESTROY, &PanelCharts::OnWindowDestroy, this );

  // maybe use std::bind now (sometimes std::bind doesn't work with boost aspects)
  namespace args = boost::phoenix::arg_names;

  m_connGetInstrumentActions = m_resources.signalGetInstrumentActions.connect( boost::phoenix::bind( &PanelCharts::HandleGetInstrumentActions, this, args::arg1 ) );

  m_connNewInstrument = m_pInstrumentActions->signalNewInstrument.connect( boost::phoenix::bind( &PanelCharts::HandleNewInstrumentRequest, this, args::arg1, args::arg2, args::arg3 ) );
  m_connLoadInstrument = m_pInstrumentActions->signalLoadInstrument.connect( boost::phoenix::bind( &PanelCharts::HandleLoadInstrument, this, args::arg1, args::arg2, args::arg3 ) );
  m_connEmitValues = m_pInstrumentActions->signalEmitValues.connect( boost::phoenix::bind( &PanelCharts::HandleEmitValues, this, args::arg1 ) );
  m_connLiveChart = m_pInstrumentActions->signalLiveChart.connect( boost::phoenix::bind( &PanelCharts::HandleInstrumentLiveChart, this, args::arg1 ) );
  m_connOptionList = m_pInstrumentActions->signalOptionList.connect( boost::phoenix::bind( &PanelCharts::HandleOptionChainList, this, args::arg1 ) );
  m_connDelete = m_pInstrumentActions->signalDelete.connect( boost::phoenix::bind( &PanelCharts::HandleMenuItemDelete, this, args::arg1 ) );
  m_connChanging = m_pTreeOps->signalChanging.connect( boost::phoenix::bind( &PanelCharts::HandleTreeOpsChanging, this, args::arg1 ) );

}

// this isn't going to work very well anymore.  many of the provider settings are now in the caller via function callbacks
// in addition, other panels have provider issues as well.
//void PanelCharts::SetProviders( pProvider_t pData1Provider, pProvider_t pData2Provider, pProvider_t pExecutionProvider ) {
//  bool bChangedData1Provider( m_pData1Provider.get() != pData1Provider.get() );
//  m_pData1Provider = pData1Provider;
//  m_pData2Provider = pData2Provider;
//  m_pExecutionProvider = pExecutionProvider;
//  if ( bChangedData1Provider ) {
//    std::for_each( m_mapInstrumentEntry.begin(), m_mapInstrumentEntry.end(),
//      [this](mapInstrumentEntry_t::value_type& vt){
//        vt.second.m_pWatchInfo.reset();  // clear out existing WatchInfo, as it depends upon provider type
//        vt.second.m_mapSelectedChainOptions.clear();  // clear out existing chains, as they depend upon provider type
//        vt.second.m_pWatch->SetProvider( m_pData1Provider ); // set new provider
//        vt.second.m_pWatchInfo.reset( new WatchInfo( vt.second.m_pWatch ) ); // build new WatchInfo
//      }
//      );
//  }
//}

void PanelCharts::RemoveRightDetail() {
  auto winRightDetail = m_winRightDetail;
  if ( 0 != winRightDetail ) {
    //dynamic_cast<InterfaceBoundEvents*>( winRightDetail )->UnbindEvents();
    // perform this afterwards as the DynamicEventTable seems to have problems
    CallAfter([this,winRightDetail](){
      assert( 0 != winRightDetail );
      m_sizerRight->Detach( winRightDetail );
      assert( winRightDetail->Destroy() );
    } );
    m_winRightDetail = 0;
  }
}


void PanelCharts::ReplaceRightDetail( wxWindow* pWindow ) {
  assert( 0 != pWindow );
  CallAfter([this,pWindow](){ // Delayed actions in RemoveRightDetail, so same delay here
    assert( 0 == m_winRightDetail );
    m_winRightDetail = pWindow;
    m_sizerRight->Add( pWindow, 1, wxALL|wxEXPAND, 5);
    m_sizerRight->Layout();
  });
}

void PanelCharts::HandleTreeOpsChanging( wxTreeItemId item ) {
  RemoveRightDetail();
}

// TODO: has various indirect and side actions.  can this be cleaned up?
// TOOD: does need fix ups as WatchInfo is no longer initialized
//   based upon signalGetInstrumentActions
PanelCharts::pInstrumentActions_t PanelCharts::HandleGetInstrumentActions( const wxTreeItemId& item ) {
  mapItemToInstrument_t::iterator iter = m_mapItemToInstrument.find( item.GetID() );
  // create an entry for the menu item with its specific instrument and watch and indicators
  if ( m_mapItemToInstrument.end() == iter ) {
    //pWatchInfo_t pWatchInfo( new WatchInfo );
//    m_mapGuiEntry.insert( mapGuiEntry_t::value_type( item.GetID(), GuiEntry( new WatchInfo ) ) );
  }
  else {
    std::cout << "PanelCharts::HandleGetInstrumentActions: menu item already in existence" << std::endl;
  }
  return m_pInstrumentActions;  // TODO: maybe turn this into a lambda now
}

// signaled in TreeItemInstrument::~TreeItemInstrument
void PanelCharts::HandleMenuItemDelete( const wxTreeItemId& item ) {
  mapItemToInstrument_t::iterator iterIdItem = m_mapItemToInstrument.find( item.GetID() );
  if ( m_mapItemToInstrument.end() == iterIdItem ) {
    // this might be ok if deleting a group
    std::cout << "PanelCharts::HandleMenuItemDelete: no menuitem to delete" << std::endl;
  }
  else {
    mapInstrumentEntry_t::iterator iterIdInstrument = m_mapInstrumentEntry.find( iterIdItem->second );
    if ( m_mapInstrumentEntry.end() == iterIdInstrument ) {
      std::cout << "PanelCharts::HandleMenuItemDelete: no instrument found: " << iterIdItem->second << std::endl;;
    }
    else {
      InstrumentEntry& entry( iterIdInstrument->second );
      if ( 0 == entry.m_cntMenuDependents ) {
        std::cout << "PanelCharts::HandleMenuItemDelete: inconsistent m_cntMenuDependents" << std::endl;
      }
      else {
        if ( 2 < entry.m_pWatch.use_count() ) {
          std::cout << "PanelCharts::HandleMenuItemDelete use_count: not ready for removal:" << iterIdItem->second << "," << entry.m_pWatch.use_count() << std::endl;
        }
        else {
          if ( entry.m_bAddedToEngine ) {
            m_fCalcOptionGreek_Remove( std::dynamic_pointer_cast<ou::tf::option::Option>( entry.m_pWatch ), entry.m_pWatchUnderlying ); // any error result?
            entry.m_bAddedToEngine = false;
          }
          entry.m_cntMenuDependents--;
          if ( 0 == entry.m_cntMenuDependents ) {
            m_mapInstrumentEntry.erase( iterIdInstrument );
          }
        }
      }
    }
    RemoveRightDetail();
  }
}

void PanelCharts::HandleInstrumentLiveChart( const wxTreeItemId& item ) {

  mapItemToInstrument_t::iterator iterIdItem = m_mapItemToInstrument.find( item.GetID() );
  if ( m_mapItemToInstrument.end() == iterIdItem ) {
    std::cout << "PanelCharts::HandleInstrumentLiveChart: no menuitem" << std::endl;
  }
  else {

    RemoveRightDetail();  // remove old content to build new content

    m_ChartDataView.Clear();
    mapInstrumentEntry_t::iterator iterIdInstrument = m_mapInstrumentEntry.find( iterIdItem->second );
    if ( m_mapInstrumentEntry.end() == iterIdInstrument ) {
      std::cout << "PanelCharts::HandleInstrumentLiveChart: no instrument " << iterIdItem->second << std::endl;
    }
    else {
      iterIdInstrument->second.m_pWatchInfo->ApplyDataTo( &m_ChartDataView );

      WinChartView* pWinChartView = nullptr;
      pWinChartView = new WinChartView( m_panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );

      // has postponed gui related assignments
      ReplaceRightDetail( pWinChartView );

      // starts update thread
      CallAfter( [this,pWinChartView](){ pWinChartView->SetChartDataView( &m_ChartDataView ); } );
    }
  }
}

void PanelCharts::HandleOptionChainList( const wxTreeItemId& item ) {

  RemoveRightDetail();
  auto pNotebookOptionChains = new NotebookOptionChains( m_panelSplitterRightPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT, _T( "a name" ) );
  pNotebookOptionChains->SetGridOptionChain_ColumnSaver( &m_gcsGridOptionChain );
  ReplaceRightDetail( pNotebookOptionChains );

  mapItemToInstrument_t::iterator iterIdItem = m_mapItemToInstrument.find( item.GetID() );
  if ( m_mapItemToInstrument.end() == iterIdItem ) {
    std::cout << "PanelCharts::HandleOptionChainList: no menuitem" << std::endl;
  }
  else {
    mapInstrumentEntry_t::iterator iterIdUnderlyingInstrument = m_mapInstrumentEntry.find( iterIdItem->second );
    if ( m_mapInstrumentEntry.end() == iterIdUnderlyingInstrument ) {
      std::cout << "PanelCharts::HandleOptionChainList: no " << iterIdItem->second << std::endl;
    }
    else {
      CallAfter([this, pNotebookOptionChains, iterIdUnderlyingInstrument](){ // ensure iterIdUnderlyingInstrument is not invalidated in the meantime
        InstrumentEntry& entry( iterIdUnderlyingInstrument->second );
        // obtain instrument name (future requires special handling)
        ou::tf::Instrument::pInstrument_t pUnderlyingInstrument = entry.m_pWatch->GetInstrument();
        std::string sSymbol;
        switch ( pUnderlyingInstrument->GetInstrumentType() ) {
          case ou::tf::InstrumentType::Stock:
            sSymbol = pUnderlyingInstrument->GetInstrumentName( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF );
            break;
          case ou::tf::InstrumentType::Future: {
            //why not the Provider name here as well?
            //   -- because it supplies something like QGCZ17, where we want only QGC for the seach
            //sSymbol = p->GetInstrumentName( ou::tf::ProviderInterfaceBase::eidProvider_t::EProviderIQF );
            std::string sTemp = pUnderlyingInstrument->GetInstrumentName();
            size_t pos = sTemp.find( '-' );
            assert( 0 != pos );
            sSymbol = sTemp.substr( 0, pos );
            }
            break;
          default:
            assert(0);
            break;
        }
        assert( 0 != sSymbol.length() );

        pNotebookOptionChains->SetName( sSymbol );

        // populate tabs of notebook
        signalRetrieveOptionList(
          sSymbol,
          [pNotebookOptionChains]( const ou::tf::iqfeed::MarketSymbol::TableRowDef& row ) {
            boost::gregorian::date date( row.nYear, row.nMonth, row.nDay );
            pNotebookOptionChains->Add( date, row.dblStrike, row.eOptionSide, row.sSymbol );
          }
          );

        namespace args = std::placeholders;
        pNotebookOptionChains->Set(
          std::bind( &PanelCharts::OnOptionChainPageChanging, this, args::_1 )
        , std::bind( &PanelCharts::OnOptionChainPageChanged, this, args::_1 )
        );
        pNotebookOptionChains->m_fOnRowClicked
          = std::bind( &PanelCharts::HandleGridClick, this, iterIdUnderlyingInstrument->first, args::_1, args::_2, args::_3, args::_4, args::_5 );
        pNotebookOptionChains->m_fOnOptionUnderlyingRetrieve
          = [this, pUnderlyingInstrument](const std::string& sIQFeedOptionName, boost::gregorian::date date, double strike, GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t f){
              if ( nullptr != m_fBuildOptionInstrument ) {
                m_fBuildOptionInstrument( pUnderlyingInstrument, sIQFeedOptionName, date, strike, [pUnderlyingInstrument, f](pInstrument_t pOptionInstrument){
                  if ( nullptr != f ) {
                    f( pOptionInstrument, pUnderlyingInstrument );
                  }
                });
          }
        };
      });
    }
  }
}

void PanelCharts::OnOptionChainPageChanging( boost::gregorian::date date ) {
}

void PanelCharts::OnOptionChainPageChanged( boost::gregorian::date date ) {
}

void PanelCharts::HandleGridClick(
  idInstrument_t idInstrumentUnderlying,
  boost::gregorian::date date, double strike, bool bSelected,
  const ou::tf::option::Delegates& call,
  const ou::tf::option::Delegates& put )
{
  std::cout << "GridClick: " << date << "," << strike << "," << call.sSymbolName << "," << put.sSymbolName << std::endl;
//  if ( ou::tf::keytypes::EProviderIQF != m_pData1Provider->ID() ) {
//    std::cout << funcCall.sSymbolName << "," << funcPut.sSymbolName << ": IQFeed provider not available" << std::endl;
//  }
//  else {
    mapInstrumentEntry_t::iterator iterInstrumentUnderlying = m_mapInstrumentEntry.find( idInstrumentUnderlying );
    if ( m_mapInstrumentEntry.end() == iterInstrumentUnderlying ) {
      std::cout << "PanelCharts::HandleGridClick: " << idInstrumentUnderlying << " not found" << std::endl;
    }
    else {
      InstrumentEntry& entryUnderlying( iterInstrumentUnderlying->second );
      std::vector<const ou::tf::option::Delegates*> vFuncs = { &call, &put };
      std::for_each( vFuncs.begin(), vFuncs.end(),
        [this, &entryUnderlying, bSelected](const ou::tf::option::Delegates* delegates) {

          mapOption_t::iterator iterOption = entryUnderlying.m_mapSelectedChainOptions.find( delegates->sSymbolName );
          if ( entryUnderlying.m_mapSelectedChainOptions.end() == iterOption ) {
            pInstrument_t pInstrument = m_fBuildInstrumentFromIqfeed( delegates->sSymbolName );
            assert( pInstrument->IsOption() || pInstrument->IsFuturesOption() );
            //ou::tf::option::Option::pOption_t pOption( new ou::tf::option::Option( pInstrument, m_pData1Provider ) );
            ou::tf::option::Option::pOption_t pOption;
            m_fBuildOption( pInstrument, pOption );
            iterOption
              = entryUnderlying.m_mapSelectedChainOptions.insert(
                entryUnderlying.m_mapSelectedChainOptions.begin(), mapOption_t::value_type( delegates->sSymbolName, pOption ) );
          }
          // TODO: do/should these be cleaned up on page changes?, or cleaned up upon tree removal?
          ou::tf::option::Option* pOption( iterOption->second.get() );
          assert( 0 != pOption );
          assert( pOption->GetInstrument()->GetInstrumentName() == delegates->sSymbolName );
          if ( bSelected != pOption->Watching() ) {
            if ( bSelected ) {
              assert( !pOption->Watching() );
              pOption->OnQuote.Add( delegates->fdQuote );
              pOption->OnTrade.Add( delegates->fdTrade );
              pOption->OnGreek.Add( delegates->fdGreek );
              pOption->StartWatch(); // verify if start watch happens in the engine
              m_fCalcOptionGreek_Add( iterOption->second, entryUnderlying.m_pWatch );
            }
            else {
              assert( pOption->Watching() );
              m_fCalcOptionGreek_Remove( iterOption->second, entryUnderlying.m_pWatch );
              pOption->StopWatch(); // verify if stop watch happens in the engine
              pOption->OnQuote.Remove( delegates->fdQuote );
              pOption->OnTrade.Remove( delegates->fdTrade );
              pOption->OnGreek.Remove( delegates->fdGreek );
            }
          }
        });
    }
//  }
}

void PanelCharts::HandleEmitValues( const wxTreeItemId& item ) {
  mapItemToInstrument_t::iterator iterIdItem = m_mapItemToInstrument.find( item.GetID() );
  if ( m_mapItemToInstrument.end() == iterIdItem ) {
    std::cout << "PanelCharts::HandleEmitValues: no menuitem" << std::endl;
  }
  else {
    mapInstrumentEntry_t::iterator iterIdInstrument = m_mapInstrumentEntry.find( iterIdItem->second );
    if ( m_mapInstrumentEntry.end() == iterIdInstrument ) {
      std::cout << "PanelCharts::HandleEmitValues no " << iterIdItem->second << " found" << std::endl;
    }
    else {
      iterIdInstrument->second.m_pWatch->EmitValues();
    }
  }
}

// constructs entry in m_mapInstrumentEntry, pInstrument may be equity, future, option, futuresoption
void PanelCharts::ConstructInstrumentEntry( const wxTreeItemId& item, pInstrument_t pInstrument, const std::string& sUnderlying ) {

  mapItemToInstrument_t::iterator iterIdItem = m_mapItemToInstrument.find( item.GetID() );
  if ( m_mapItemToInstrument.end() != iterIdItem ) {
    std::cout << "PanelCharts::ConstructInstrumentEntry menu item already exists" << std::endl;
  }
  else {
    const ou::tf::Instrument::idInstrument_t idInstrument( pInstrument->GetInstrumentName() );
    mapInstrumentEntry_t::iterator iterInstrument = m_mapInstrumentEntry.find( idInstrument );
    if ( m_mapInstrumentEntry.end() != iterInstrument ) {
      std::cout << "PanelCharts::ConstructInstrumentEntry: " << idInstrument << " exists." << std::endl;
    }
    else {
      signalRegisterInstrument( pInstrument );  // not needed if signalLoadInstrument has been called, does the Dialog do a registration, maybe do the registration there based upon flag from here

      pWatch_t pWatch;  // for instrument
      pWatch_t pWatchUnderlying; // set if pWatch turns out to be option
      if ( pInstrument->IsOption() || pInstrument->IsFuturesOption() ) {
        // need to perform outside option build call
        pOption_t pOption;
        m_fBuildOption( pInstrument, pOption );
        //pOption.reset( new ou::tf::option::Option( pInstrument, m_pData1Provider ) );
        //pWatch = boost::dynamic_pointer_cast<ou::tf::Watch>( pOption );
        pWatch = pOption;
        mapInstrumentEntry_t::iterator iterUnderlying = m_mapInstrumentEntry.find( sUnderlying );
        if ( m_mapInstrumentEntry.end() == iterUnderlying ) {
          std::cout << "PanelCharts::ConstructInstrumentEntry: didn't find underlying (" << sUnderlying << ")" << std::endl;
        }
        else {
          pWatchUnderlying = iterUnderlying->second.m_pWatch;
          //pOption_t pOption;
          //pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
          m_fCalcOptionGreek_Add( pOption, pWatchUnderlying );
        }
      }
      else { // is equity or future
        m_fBuildWatch( pInstrument, pWatch );
        //pWatch.reset( new ou::tf::Watch( pInstrument, m_pData1Provider ) );
      }

      pWatchInfo_t pWatchInfo( new WatchInfo( pWatch ) );
      iterInstrument
        = m_mapInstrumentEntry.insert( m_mapInstrumentEntry.begin(), mapInstrumentEntry_t::value_type( idInstrument, InstrumentEntry( pWatch, pWatchInfo ) ) );
      if ( 0 != pWatchUnderlying.use_count() )
        iterInstrument->second.m_pWatchUnderlying = pWatchUnderlying;

      m_mapItemToInstrument.insert( mapItemToInstrument_t::value_type( item.GetID(), idInstrument ) );
    }
  }
}

// called by anything more than the serialization in TreeItemInstrument?
void PanelCharts::HandleLoadInstrument(
  const wxTreeItemId& item, const std::string& sName, const std::string& sUnderlying
) {
  // need to set values/locks somewhere
  ConstructInstrumentEntry( item, signalLoadInstrument( sName ), sUnderlying );
}


InstrumentActions::values_t PanelCharts::HandleNewInstrumentRequest(
  const wxTreeItemId& item,
  const ou::tf::Allowed::EInstrument selector,
  const wxString& wxsUnderlying // optional
) {

  // the item coming in represents the existing menu item
  //   which might be a group item, or an instrument item

  InstrumentActions::values_t values;

  // TODO: turn this into a std::function call
  pInstrument_t pInstrument = m_fSelectInstrument( selector, wxsUnderlying );

  if ( nullptr != pInstrument.get() ) {

    const std::string sUnderlying( wxsUnderlying );
    ConstructInstrumentEntry( item, pInstrument, sUnderlying );

    Instrument::idInstrument_cref idInstrument( pInstrument->GetInstrumentName() );
    values.name_ = idInstrument;

    // are these selector types propagated properly?
    //  ie, on load from file, are they set there?
    if ( pInstrument->IsStock() )         values.selector = ou::tf::Allowed::Options;
    if ( pInstrument->IsFuture() )        values.selector = ou::tf::Allowed::FuturesOptions;
    if ( pInstrument->IsOption() )        values.selector = ou::tf::Allowed::None;
    if ( pInstrument->IsFuturesOption() ) values.selector = ou::tf::Allowed::None;

  }

  return values;
}

void PanelCharts::OnWindowDestroy( wxWindowDestroyEvent& event ) {

  //m_pInstrumentActions.reset();

  int id1 = event.GetId();
  wxWindowID id2 = this->GetId();

  if ( id1 == id2 ) { // ignores 'destroy' of any child window
    m_connGetInstrumentActions.disconnect();

    m_connNewInstrument.disconnect();
    m_connLoadInstrument.disconnect();
    m_connEmitValues.disconnect();
    m_connLiveChart.disconnect();
    m_connDelete.disconnect();

    m_connChanging.disconnect();

    Unbind( wxEVT_DESTROY, &PanelCharts::OnWindowDestroy, this );

  }

  event.Skip();

}

void PanelCharts::SaveSeries( const std::string& sPrefix, const std::string& sDaily ) {
  std::for_each( m_mapInstrumentEntry.begin(), m_mapInstrumentEntry.end(),
    [&sPrefix,&sDaily](mapInstrumentEntry_t::value_type& vt) {
      InstrumentEntry& entry( vt.second );
      entry.m_pWatch->EmitValues();
      entry.m_pWatch->SaveSeries( sPrefix, sDaily );
    });
}

wxBitmap PanelCharts::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelCharts::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
