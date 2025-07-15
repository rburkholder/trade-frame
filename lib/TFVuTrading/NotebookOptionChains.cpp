/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   NotebookOptionChains.cpp
 * Author: raymond@burkholder.net
 *
 * Created on July 2, 2017, 8:16 PM
 */

#include <algorithm>

#include <boost/lexical_cast.hpp>

#include "NotebookOptionChains.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

NotebookOptionChains::NotebookOptionChains(): wxListbook() {
  Init();
}

NotebookOptionChains::NotebookOptionChains( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxListbook()
{
  Init();
  Create(parent, id, pos, size, style, name );
}

NotebookOptionChains::~NotebookOptionChains() {
}

void NotebookOptionChains::Init() {
  m_bBound = false;

  m_pgcsGridOptionChain = nullptr;

  m_fOnRowClicked = nullptr;
  m_fOnPageChanged = nullptr;
  m_fOnPageChanging = nullptr;
  m_fOnOptionUnderlyingRetrieve = nullptr;
}

bool NotebookOptionChains::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxListbook::Create(parent, id, pos, size, style, name );

  CreateControls();

  return true;
}

void NotebookOptionChains::CreateControls() {

  //Bind( wxEVT_CLOSE_WINDOW, &WinChartView::OnClose, this );  // not called for child windows
  Bind( wxEVT_DESTROY, &NotebookOptionChains::OnDestroy, this );

  BindEvents();

}

void NotebookOptionChains::BindEvents() {
  if ( !m_bBound ) {
    // Page Change events cause issues during OnDestroy
    Bind( wxEVT_LISTBOOK_PAGE_CHANGING, &NotebookOptionChains::OnPageChanging, this );
    Bind( wxEVT_LISTBOOK_PAGE_CHANGED, &NotebookOptionChains::OnPageChanged, this );

    //Bind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
    //Bind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );

    //Bind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
    //Bind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
    //Bind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
    //Bind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

    m_bBound = true;
  }
}

void NotebookOptionChains::UnbindEvents() {
  if ( m_bBound ) {
    // Page change events occur during Deletion of Pages, causing problems
    assert( Unbind( wxEVT_LISTBOOK_PAGE_CHANGING, &NotebookOptionChains::OnPageChanging, this ) );
    assert( Unbind( wxEVT_LISTBOOK_PAGE_CHANGED, &NotebookOptionChains::OnPageChanged, this ) );

    //Unbind( wxEVT_PAINT, &WinChartView::HandlePaint, this );
    //Unbind( wxEVT_SIZE, &GridOptionDetails::HandleSize, this );

    //Unbind( wxEVT_MOTION, &WinChartView::HandleMouse, this );
    //Unbind( wxEVT_MOUSEWHEEL, &WinChartView::HandleMouseWheel, this );
    //Unbind( wxEVT_ENTER_WINDOW, &WinChartView::HandleMouseEnter, this );
    //Unbind( wxEVT_LEAVE_WINDOW, &WinChartView::HandleMouseLeave, this );

    m_bBound = false;
  }
}

void NotebookOptionChains::Set(
  fOnPageEvent_t&& fOnPageChanging // departed
, fOnPageEvent_t&& fOnPageChanged  // arrival
) {
  m_fOnPageChanging = std::move( fOnPageChanging );
  m_fOnPageChanged  = std::move( fOnPageChanged );
}

// start leaving old page
void NotebookOptionChains::OnPageChanging( wxBookCtrlEvent& event ) {
  int ixTab = event.GetOldSelection();
  if ( -1 != ixTab ) {
    //std::cout << "page changing: " << ixTab << std::endl;
    mapOptionExpiry_t::iterator iter
     = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
       return ixTab == vt.second.ixTab;
    });
    if ( m_mapOptionExpiry.end() == iter ) {
      std::cout << "NotebookOptionChains::OnPageChanging: couldn't find tab index: " << ixTab << std::endl;
    }
    else {
      //iter->second.pWinOptionChain->Stop();
      assert( false );  // TODO: refer to PanelComboOrder::OnBOOKOptionChainsPageChanging for guidance
      if ( nullptr != m_pgcsGridOptionChain ) {
        iter->second.pWinOptionChain->SaveColumnSizes( *m_pgcsGridOptionChain );
      }
      if ( nullptr != m_fOnPageChanging ) {
        m_fOnPageChanging( iter->first );
      }
    }
  }
  event.Skip();
}

// finishing arriving at new page
void NotebookOptionChains::OnPageChanged( wxBookCtrlEvent& event ) {
  int ixTab = event.GetSelection();
  //std::cout << "page changed: " << ixTab << std::endl;
  mapOptionExpiry_t::iterator iter
    = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
      return ixTab == vt.second.ixTab;
      });
  if ( m_mapOptionExpiry.end() == iter ) {
    std::cout << "NotebookOptionChains::OnPageChanged: couldn't find tab index: " << ixTab << std::endl;
  }
  else {
    if ( nullptr != m_pgcsGridOptionChain ) {
      iter->second.pWinOptionChain->SetColumnSizes( *m_pgcsGridOptionChain );
    }
    //iter->second.pWinOptionChain->Start();
    assert( false );   // TODO: refer to PanelComboOrder::OnBOOKOptionChainsPageChanged for guidance
    if ( nullptr != m_fOnPageChanged ) {
      m_fOnPageChanged( iter->first );
    }
  }
  event.Skip();
}

void NotebookOptionChains::SetGridOptionChain_ColumnSaver( ou::tf::GridColumnSizer* pgcs ) {
  m_pgcsGridOptionChain = pgcs;
  int ixTab = GetSelection();
  mapOptionExpiry_t::iterator iter
    = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
      return ixTab == vt.second.ixTab;
      });
  if ( m_mapOptionExpiry.end() == iter ) {
    std::cout << "NotebookOptionChains::SetGridOptionChain_ColumnSaver: couldn't find tab index: " << ixTab << std::endl;
  }
  else {
    if ( nullptr != m_pgcsGridOptionChain ) {
      iter->second.pWinOptionChain->SetColumnSizes( *m_pgcsGridOptionChain );
    }
  }
}

void NotebookOptionChains::SetName( const std::string& sName ) {
  wxListbook::SetName( sName );
  m_sName = sName;
}

// add specific put/call-at-strike pair to Notebook of OptionChaines
void NotebookOptionChains::Add( boost::gregorian::date date, double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol ) {

  mapOptionExpiry_t::iterator iterExpiry = m_mapOptionExpiry.find( date );

  if ( m_mapOptionExpiry.end() == iterExpiry ) {
    // add another panel
    std::string sDate = boost::lexical_cast<std::string>( date.year() );
    sDate += std::string( "/" )
      + ( date.month().as_number() < 10 ? "0" : "" )
      + boost::lexical_cast<std::string>( date.month().as_number() );
    sDate += std::string( "/" ) + ( date.day()   < 10 ? "0" : "" ) + boost::lexical_cast<std::string>( date.day() );

    auto* pPanel = new wxPanel( this, wxID_ANY );
    auto* pSizer = new wxBoxSizer(wxVERTICAL);
    pPanel->SetSizer( pSizer );
    auto* pGridOptionChain = new GridOptionChain( pPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, sSymbol );
    pSizer->Add( pGridOptionChain, 1, wxALL|wxEXPAND, 1 );

    // a control right click will signal through that strike should watch/unwatch
    // TODO: maybe the signal through should return a boolean of whether it turned out to be watch or unwatch
    pGridOptionChain->m_fOnRowClicked =
      [this, date](double strike, bool bSelected, const ou::tf::option::Delegates& call, const ou::tf::option::Delegates& put  ){
        if ( nullptr != m_fOnRowClicked) {
          m_fOnRowClicked( date, strike, bSelected, call, put );
        }
    };
    pGridOptionChain->m_fOnOptionUnderlyingRetrieveInitiate =
      [this, date]( const std::string& sIQFeedOptionName, double strike, GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t&& f ){
        if ( nullptr != m_fOnOptionUnderlyingRetrieve ) {
          m_fOnOptionUnderlyingRetrieve(sIQFeedOptionName, date, strike, f );
        }
    };

    iterExpiry = m_mapOptionExpiry.insert(
      m_mapOptionExpiry.begin(), mapOptionExpiry_t::value_type( date, Tab( sDate, pPanel, pGridOptionChain ) ) );

    struct Reindex {
      size_t ix;
      Reindex(): ix{} {}
      void operator()( Tab& tab ) { tab.ixTab = ix; ix++; }
    };

    // renumber the pages
    Reindex reindex;
    std::for_each(
      m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(),
        [&reindex](mapOptionExpiry_t::value_type& v){ reindex( v.second ); } );

    InsertPage( iterExpiry->second.ixTab, pPanel, sDate );

    SetSelection( 0 );
  } // end add panel

  mapStrike_t& mapStrike( iterExpiry->second.mapStrike ); // assumes single thread
  mapStrike_t::iterator iterStrike = mapStrike.find( strike );

  if ( mapStrike.end() == iterStrike ) {
    iterStrike = mapStrike.insert( mapStrike.begin(), mapStrike_t::value_type( strike, Row( mapStrike.size() ) ) );
  }

  switch ( side ) {
    case ou::tf::OptionSide::Call:
      assert( "" == iterStrike->second.sCall );
      iterStrike->second.sCall = sSymbol;
      break;
    case ou::tf::OptionSide::Put:
      assert( "" == iterStrike->second.sPut );
      iterStrike->second.sPut = sSymbol;
      break;
  }

  // add option set to the expiry panel
  iterExpiry->second.pWinOptionChain->Add( strike, side, sSymbol );

}

void NotebookOptionChains::OnDestroy( wxWindowDestroyEvent& event ) {

  UnbindEvents();

  std::for_each(
    m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(),
    [](mapOptionExpiry_t::value_type& value){
     value.second.pWinOptionChain->PreDestroy();
     value.second.pWinOptionChain->Destroy();
     value.second.pPanel->Destroy();
   });

  //DeleteAllPages();
  while ( 0 != GetPageCount() ) {
   //DeletePage( 0 );
   RemovePage( 0 );
  }

  assert( Unbind( wxEVT_DESTROY, &NotebookOptionChains::OnDestroy, this ) );

  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap NotebookOptionChains::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon NotebookOptionChains::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}

} // namespace tf
} // namespace ou
