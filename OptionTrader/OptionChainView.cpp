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
 * File:    OptionChainView.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:21
 */

#include <boost/log/trivial.hpp>

#include <wx/icon.h>
#include <wx/menu.h>

#include "OptionChainView.hpp"
#include "OptionChainModel.hpp"
#include "OptionChainModel_impl.hpp"

OptionChainView::OptionChainView()
: wxGrid()
, m_fAddOrder( nullptr )
{
  Init();
}

OptionChainView::OptionChainView( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxGrid()
, m_fAddOrder( nullptr )
{
  Init();
  Create(parent, id, pos, size, style, name );
}

OptionChainView::~OptionChainView() {
}

void OptionChainView::Init() {
  m_pMenuAssignWatch = nullptr;
}

bool OptionChainView::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxGrid::Create(parent, id, pos, size, style );

  CreateControls();

  return true;
}

void OptionChainView::CreateControls() {

  Bind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this );

  Bind( wxEVT_GRID_CELL_LEFT_CLICK, &OptionChainView::OnGridCellLeftClick, this );
  Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &OptionChainView::OnGridCellRightClick, this );
  Bind( wxEVT_GRID_SELECT_CELL, &OptionChainView::OnGridSelectCell, this );

  //auto fRendererRight = []()->wxDataViewTextRenderer*{
  //  auto renderer = new wxDataViewTextRenderer();
  //  renderer->SetAlignment( wxALIGN_RIGHT );
  //  return renderer;
  //};

  SetDefaultColSize(50);
  SetDefaultRowSize(20);
  SetColLabelSize(22);
  SetRowLabelSize(50);

  EnableEditing( false );
  DisableDragRowSize();

  HideRowLabels();

  {
    // Todo: add intrinsic value, premium value for call & put, include last price?
    // Todo: use trades to indicate possible trade opportunities, as they may represent knowlegeable actors
    //       check if they correspond to peaks/valleys in the underlying
  }

  // the following does not work, will need to use wxGrid to provide column & header colours
  //wxColourDatabase cdb;

  //wxItemAttr attr1;
  //wxItemAttr attr2( attr1.GetTextColour(), attr1.GetBackgroundColour(), attr1.GetFont() );
  //attr2.SetBackgroundColour( cdb.Find( "RED" ) );
  //SetHeaderAttr( attr2 );
}

void OptionChainView::SetAddOrder( fAddOrder_t&& fAddOrder ) {
  m_fAddOrder = std::move( fAddOrder );
}

void OptionChainView::OnGridCellLeftClick( wxGridEvent& event ) { // zero based
  //BOOST_LOG_TRIVIAL(trace) << "Left Click " << event.GetRow() << ',' << event.GetCol();
  event.Skip();
}

void OptionChainView::OnGridCellRightClick( wxGridEvent& event ) { // zero based, pop up menu
  //BOOST_LOG_TRIVIAL(trace) << "Right Click " << event.GetRow() << ',' << event.GetCol();

  const auto row = event.GetRow();
  const auto col = event.GetCol();

  std::string sMessage;
  ou::tf::OrderSide::EOrderSide side( ou::tf::OrderSide::Unknown );

  switch ( col ) {
    case OptionChainModel_impl::col_CallAsk:
      sMessage = "buy call @ ask";
      side =  ou::tf::OrderSide::Buy;
      break;
    case OptionChainModel_impl::col_CallBid:
      sMessage = "sell call @ bid";
      side =  ou::tf::OrderSide::Sell;
      break;
    case OptionChainModel_impl::col_PutAsk:
      sMessage = "buy put @ ask";
      side =  ou::tf::OrderSide::Buy;
      break;
    case OptionChainModel_impl::col_PutBid:
      sMessage = "sell put @ bid";
      side =  ou::tf::OrderSide::Sell;
      break;
    default:
      break;
  }

  if ( nullptr != m_pMenuAssignWatch ) {
    delete m_pMenuAssignWatch;
    m_pMenuAssignWatch = nullptr;
  }

  m_pMenuAssignWatch = new wxMenu();

  if ( 0 < sMessage.size() ) {
    wxMenuItem* pMenuItemBuy = m_pMenuAssignWatch->Append( wxID_ANY, sMessage );
    m_pMenuAssignWatch->Bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this, row, col, side ]( wxCommandEvent& event ){
        //BOOST_LOG_TRIVIAL(trace) << "menu" << ',' << row << ',' << col;
        OptionChainModel* p = reinterpret_cast<OptionChainModel*>( GetTable() );
        pOption_t pOption = p->GetOption( row, col );
        assert( m_fAddOrder );
        m_fAddOrder( pOption, side, 1 );
      },
      pMenuItemBuy->GetId()
    );

    //wxMenuItem* pMenuItemSell = m_pMenuAssignWatch->Append( wxID_ANY, "Sell " + sType + "@" + sSide );
    //m_pMenuAssignWatch->Bind(
    //  wxEVT_COMMAND_MENU_SELECTED,
    //  [this, row=event.GetRow(), col = event.GetCol() ]( wxCommandEvent& event ){
    //    BOOST_LOG_TRIVIAL(trace) << "Menu Sell" << ',' << row << ',' << col;
    //  },
    //  pMenuItemSell->GetId()
    //);
  }
  else {
    wxMenuItem* pMenuItemInfo = m_pMenuAssignWatch->Append( wxID_ANY, "Info" );
    m_pMenuAssignWatch->Bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [this, row=event.GetRow(), col = event.GetCol() ]( wxCommandEvent& event ){
        BOOST_LOG_TRIVIAL(trace) << "Menu Info" << ',' << row << ',' << col;
      },
      pMenuItemInfo->GetId()
    );
  }

  PopupMenu( m_pMenuAssignWatch );
  event.Skip();
}

void OptionChainView::OnGridSelectCell( wxGridEvent& event ) { // labelled row/column, cursoring & tabbing
  //BOOST_LOG_TRIVIAL(trace) << "Select Cell " << event.GetRow() << ',' << event.GetCol();
  event.Skip();
}

int OptionChainView::GetFirstVisibleRow() const {
  return GetFirstFullyVisibleRow();
}

int OptionChainView::GetVisibleRowCount() const {
  const int ixFirstRow( GetFirstFullyVisibleRow() );
  int nRows {};
  if ( -1 != ixFirstRow ) {
    int ixRow( ixFirstRow );
    const int cntRows( GetTable()->GetNumberRows() );
    while (
      ( IsVisible( ixRow, OptionChainModel_impl::col_Strike, false ) )
      && ( cntRows > ixRow )
    ) {
      nRows++;
      ixRow++;
    }
  }
  return nRows;
}

void OptionChainView::SetVisible( int ixRow ) {
  MakeCellVisible( ixRow, OptionChainModel_impl::col_Strike );
}

int OptionChainView::GetColumnCount() const {
  return GRID_ARRAY_COL_COUNT;
}

void OptionChainView::OnDestroy( wxWindowDestroyEvent& event ) {

  assert( Unbind( wxEVT_GRID_CELL_LEFT_CLICK, &OptionChainView::OnGridCellLeftClick, this ) );
  assert( Unbind( wxEVT_GRID_CELL_RIGHT_CLICK, &OptionChainView::OnGridCellRightClick, this ) );
  assert( Unbind( wxEVT_GRID_SELECT_CELL, &OptionChainView::OnGridSelectCell, this ) );

  assert( Unbind( wxEVT_DESTROY, &OptionChainView::OnDestroy, this ) );

  if ( nullptr != m_pMenuAssignWatch ) {
    delete m_pMenuAssignWatch;
    m_pMenuAssignWatch = nullptr;
  }

  event.Skip( true );  // auto followed by Destroy();
}

wxBitmap OptionChainView::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon OptionChainView::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}
