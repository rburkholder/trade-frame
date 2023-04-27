/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    PanelComboOrder.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: April 10, 2023 13:10:29
 */

#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/tglbtn.h>
#include <wx/listbook.h>

#include "PanelComboOrder.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelComboOrder::PanelComboOrder(): wxPanel() {
  Init();
}

PanelComboOrder::PanelComboOrder( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel()
{
  Init();
  Create(parent, id, pos, size, style, name );
}

PanelComboOrder::~PanelComboOrder() {
}

void PanelComboOrder::Init() {

  m_pgcsGridOptionChain = nullptr;

  m_pBookOptionChains = nullptr;
  m_pGridOptionComboOrder = nullptr;
  m_pGridOptionChain_Current = nullptr;

  m_btnUpgdateGreeks = nullptr;
  m_btnClearOrder = nullptr;
  m_btnPlaceOrder = nullptr;

  m_fOnPageChanged = nullptr;
  m_fOnPageChanging = nullptr;

  m_fOptionDelegates_Attach = nullptr;
  m_fOptionDelegates_Detach = nullptr;
}

bool PanelComboOrder::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

////@begin PanelComboOrder creation
    SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    //Centre();
////@end PanelComboOrder creation
    return true;
}

void PanelComboOrder::CreateControls() {

    PanelComboOrder* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_pBookOptionChains = new wxListbook( itemPanel1, ID_BOOK_OptionChains, wxDefaultPosition, wxDefaultSize, wxBK_LEFT|wxNO_BORDER );
    itemBoxSizer2->Add(m_pBookOptionChains, 1, wxGROW|wxALL, 1);

    m_pGridOptionComboOrder = new GridOptionComboOrder( itemPanel1, ID_GRID_ComboOrder, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxVSCROLL );
    itemBoxSizer2->Add(m_pGridOptionComboOrder, 0, wxGROW|wxALL, 0);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    m_btnUpgdateGreeks = new wxToggleButton( itemPanel1, ID_BTN_UpdateGreeks, _("Update Greeks"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnUpgdateGreeks->SetValue(false);
    itemBoxSizer4->Add(m_btnUpgdateGreeks, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_btnClearOrder = new wxButton( itemPanel1, ID_BTN_ClearOrder, _("Clear Order"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer4->Add(m_btnClearOrder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_btnPlaceOrder = new wxButton( itemPanel1, ID_BTN_PlaceOrder, _("Place Order"), wxDefaultPosition, wxDefaultSize, 0 );
    m_btnPlaceOrder->SetDefault();
    itemBoxSizer4->Add(m_btnPlaceOrder, 0, wxALIGN_CENTER_VERTICAL|wxALL, 1);

  BindEvents();
}

void PanelComboOrder::BindEvents() {
  m_pBookOptionChains->Bind( wxEVT_DESTROY, &PanelComboOrder::OnDestroy_Book, this, ID_BOOK_OptionChains );

  // Page Change events cause issues during OnDestroy
  Bind( wxEVT_BUTTON, &PanelComboOrder::OnBTNClearOrderClick, this, ID_BTN_ClearOrder );
  Bind( wxEVT_BUTTON, &PanelComboOrder::OnBTNPlaceOrderClick, this, ID_BTN_PlaceOrder );
  Bind( wxEVT_LISTBOOK_PAGE_CHANGING, &PanelComboOrder::OnBOOKOptionChainsPageChanging, this );
  Bind( wxEVT_LISTBOOK_PAGE_CHANGED, &PanelComboOrder::OnBOOKOptionChainsPageChanged, this );
  Bind( wxEVT_DESTROY, &PanelComboOrder::OnDestroy_Panel, this );

  StartRefresh();
}

void PanelComboOrder::UnbindEvents() {

  StopRefresh();

  assert( m_pBookOptionChains->Unbind( wxEVT_DESTROY, &PanelComboOrder::OnDestroy_Book, this, ID_BOOK_OptionChains ) );

  // Page change events occur during Deletion of Pages, causing problems
  assert( Unbind( wxEVT_DESTROY, &PanelComboOrder::OnDestroy_Panel, this ) );
  assert( Unbind( wxEVT_LISTBOOK_PAGE_CHANGING, &PanelComboOrder::OnBOOKOptionChainsPageChanging, this ) );
  assert( Unbind( wxEVT_LISTBOOK_PAGE_CHANGED, &PanelComboOrder::OnBOOKOptionChainsPageChanged, this ) );
  assert( Unbind( wxEVT_BUTTON, &PanelComboOrder::OnBTNClearOrderClick, this, ID_BTN_ClearOrder ) );
  assert( Unbind( wxEVT_BUTTON, &PanelComboOrder::OnBTNPlaceOrderClick, this, ID_BTN_PlaceOrder ) );
}

void PanelComboOrder::Set(
  fOnPageEvent_t&& fOnPageChanging // departed
, fOnPageEvent_t&& fOnPageChanged  // arrival
) {
  m_fOnPageChanging = std::move( fOnPageChanging );
  m_fOnPageChanged  = std::move( fOnPageChanged );
}

void PanelComboOrder::Set(
  fOptionDelegates_t&& fOptionDelegates_Attach
, fOptionDelegates_t&& fOptionDelegates_Detach
) {
  m_fOptionDelegates_Attach = std::move( fOptionDelegates_Attach );
  m_fOptionDelegates_Detach = std::move( fOptionDelegates_Detach );

  for ( mapOptionExpiry_t::value_type& expiry: m_mapOptionExpiry ) {
    Tab& tab( expiry.second );
    tab.pGridOptionChain->Set( m_fOptionDelegates_Attach, m_fOptionDelegates_Detach ); // make copies of the lambdas
  }

  m_pGridOptionComboOrder->Set( m_fOptionDelegates_Attach, m_fOptionDelegates_Detach ); // make copies of the lambdas
}

void PanelComboOrder::StartRefresh() {
  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( this );
  Bind( wxEVT_TIMER, &PanelComboOrder::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 300 );
}

void PanelComboOrder::StopRefresh() {
  m_timerGuiRefresh.Stop();
  m_timerGuiRefresh.DeletePendingEvents();
  Unbind( wxEVT_TIMER, &PanelComboOrder::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
}

void PanelComboOrder::HandleGuiRefresh( wxTimerEvent& event ) {
  if ( m_pGridOptionChain_Current ) m_pGridOptionChain_Current->Refresh();
  if ( m_pGridOptionComboOrder ) m_pGridOptionComboOrder->Refresh();
}

void PanelComboOrder::MakeRowVisible( boost::gregorian::date date, double strike ) {
  // what happens if grid is not visible, different date is showing?
  m_mapOptionExpiry[ date ].pGridOptionChain->MakeRowVisible( strike );
}

void PanelComboOrder::Update( boost::gregorian::date date, double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Quote& quote ) {
  m_mapOptionExpiry[ date ].pGridOptionChain->Update( strike, side, quote );
}

void PanelComboOrder::Update( boost::gregorian::date date, double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Trade& trade ) {
  m_mapOptionExpiry[ date ].pGridOptionChain->Update( strike, side, trade );
}

void PanelComboOrder::Update( boost::gregorian::date date, double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Greek& greek ) {
  m_mapOptionExpiry[ date ].pGridOptionChain->Update( strike, side, greek );
}

void PanelComboOrder::Clear( boost::gregorian::date date, double strike ) {
  m_mapOptionExpiry[ date ].pGridOptionChain->Clear( strike );
}

// add specific put/call-at-strike pair to Notebook of OptionChaines
void PanelComboOrder::Add( boost::gregorian::date date, double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sIQFeedSymbolName ) {

  //[this,pti=uws.pti]( boost::gregorian::date date ){
    //  ou::tf::Instrument::BuildDate( date ),

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
    auto* pGridOptionChain = new GridOptionChain( pPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, sIQFeedSymbolName );
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

    pGridOptionChain->Set( std::move( m_pGridOptionComboOrder->FunctionAddComboOrder() ) );

    auto pair = m_mapOptionExpiry.emplace( mapOptionExpiry_t::value_type( date, Tab( sDate, pPanel, pGridOptionChain ) ) );
    assert( pair.second );
    iterExpiry = pair.first;

    iterExpiry->second.pGridOptionChain->Set( m_fOptionDelegates_Attach, m_fOptionDelegates_Detach ); // make copies of the lambdas

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

    m_pBookOptionChains->InsertPage( iterExpiry->second.ixTab, pPanel, sDate );

    m_pBookOptionChains->SetSelection( 0 );
    m_pGridOptionChain_Current = m_mapOptionExpiry.begin()->second.pGridOptionChain;
  } // end add panel

  mapStrike_t& mapStrike( iterExpiry->second.mapStrike ); // assumes single thread
  mapStrike_t::iterator iterStrike = mapStrike.find( strike );

  if ( mapStrike.end() == iterStrike ) {
    iterStrike = mapStrike.insert( mapStrike.begin(), mapStrike_t::value_type( strike, Row( mapStrike.size() ) ) );
  }

  switch ( side ) {
    case ou::tf::OptionSide::Call:
      assert( "" == iterStrike->second.sCall );
      iterStrike->second.sCall = sIQFeedSymbolName;
      break;
    case ou::tf::OptionSide::Put:
      assert( "" == iterStrike->second.sPut );
      iterStrike->second.sPut = sIQFeedSymbolName;
      break;
  }

  // add option set to the expiry panel
  iterExpiry->second.pGridOptionChain->Add( strike, side, sIQFeedSymbolName );

}

void PanelComboOrder::OnDestroy_Book( wxWindowDestroyEvent& event ) {

  std::for_each(
    m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(),
    [](mapOptionExpiry_t::value_type& value){
     value.second.pGridOptionChain->PreDestroy();
     value.second.pGridOptionChain->Destroy();
     value.second.pPanel->Destroy();
   });

  while ( 0 != m_pBookOptionChains->GetPageCount() ) {
   m_pBookOptionChains->RemovePage( 0 );
  }

  event.Skip();
}

void PanelComboOrder::OnDestroy_Panel( wxWindowDestroyEvent& event ) {

  UnbindEvents();

  m_pGridOptionComboOrder = nullptr;
  m_pBookOptionChains = nullptr;

  event.Skip();
}

void PanelComboOrder::OnBOOKOptionChainsPageChanging( wxListbookEvent& event ) {
  int ixTab = event.GetOldSelection();
  if ( -1 != ixTab ) {
    //std::cout << "page changing: " << ixTab << std::endl;
    mapOptionExpiry_t::iterator iter
      = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
        return ixTab == vt.second.ixTab;
    });
    if ( m_mapOptionExpiry.end() == iter ) {
      std::cout << "PanelComboOrder::OnPageChanging: couldn't find tab index: " << ixTab << std::endl;
    }
    else {
      //iter->second.pGridOptionChain->Stop();
      m_pGridOptionChain_Current = nullptr;
      if ( nullptr != m_pgcsGridOptionChain ) {
        iter->second.pGridOptionChain->SaveColumnSizes( *m_pgcsGridOptionChain );
      }
      if ( nullptr != m_fOnPageChanging ) {
        m_fOnPageChanging( iter->first );
      }
    }
  }
  event.Skip();
}

void PanelComboOrder::OnBOOKOptionChainsPageChanged( wxListbookEvent& event ) {
  int ixTab = event.GetSelection();
  //std::cout << "page changed: " << ixTab << std::endl;
  mapOptionExpiry_t::iterator iter
    = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
      return ixTab == vt.second.ixTab;
      });
  if ( m_mapOptionExpiry.end() == iter ) {
    std::cout << "PanelComboOrder::OnPageChanged: couldn't find tab index: " << ixTab << std::endl;
  }
  else {
    if ( nullptr != m_pgcsGridOptionChain ) {
      iter->second.pGridOptionChain->SetColumnSizes( *m_pgcsGridOptionChain );
    }
    //iter->second.pGridOptionChain->Start();
    m_pGridOptionChain_Current = iter->second.pGridOptionChain;
    if ( nullptr != m_fOnPageChanged ) {
      m_fOnPageChanged( iter->first );
    }
  }
  event.Skip();
}

void PanelComboOrder::OnBTNUpdateGreeksClick( wxCommandEvent& event ) {
  event.Skip();
}

void PanelComboOrder::OnBTNClearOrderClick( wxCommandEvent& event ) {
  m_pGridOptionComboOrder->ClearOrders();
  event.Skip();
}

void PanelComboOrder::OnBTNPlaceOrderClick( wxCommandEvent& event ) {
  m_pGridOptionComboOrder->PlaceComboOrder();
  event.Skip();
}

void PanelComboOrder::SetGridOptionChain_ColumnSaver( ou::tf::GridColumnSizer* pgcs ) {
  m_pgcsGridOptionChain = pgcs;
  int ixTab = m_pBookOptionChains->GetSelection();
  mapOptionExpiry_t::iterator iter
    = std::find_if( m_mapOptionExpiry.begin(), m_mapOptionExpiry.end(), [ixTab,this](mapOptionExpiry_t::value_type& vt) {
      return ixTab == vt.second.ixTab;
      });
  if ( m_mapOptionExpiry.end() == iter ) {
    std::cout << "PanelComboOrder::SetGridOptionChain_ColumnSaver: couldn't find tab index: " << ixTab << std::endl;
  }
  else {
    if ( nullptr != m_pgcsGridOptionChain ) {
      iter->second.pGridOptionChain->SetColumnSizes( *m_pgcsGridOptionChain );
    }
  }
}

wxBitmap PanelComboOrder::GetBitmapResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullBitmap;
}

wxIcon PanelComboOrder::GetIconResource( const wxString& name ) {
    wxUnusedVar(name);
    return wxNullIcon;
}


} // namespace tf
} // namespace ou
