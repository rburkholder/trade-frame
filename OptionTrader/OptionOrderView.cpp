/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    OptionOrderView.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/05 10:38:02
 */

#include <boost/log/trivial.hpp>

#include <wx/menu.h>

#include "OptionOrderModel.hpp"
#include "OptionOrderView.hpp"
#include "OptionOrderModel_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionOrderView::OptionOrderView()
: wxGrid()
, m_pMenuRightClick( nullptr )
{
  Init();
}

OptionOrderView::OptionOrderView(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& sTitle )
: wxGrid()
, m_pMenuRightClick( nullptr )
{
  Init();
  Create(parent, id, pos, size, style, sTitle );
}

OptionOrderView::~OptionOrderView() {
}

void OptionOrderView::Init() {
}

bool OptionOrderView::Create(
  wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) {

  wxGrid::Create( parent, id, pos, size, style, name );
  //m_pimpl = std::make_unique<GridOptionComboOrder_impl>( *this );

  CreateControls();

  return true;
}

void OptionOrderView::CreateControls() {

  SetDefaultColSize(50);
  SetDefaultRowSize(20);
  SetColLabelSize(22);
  SetRowLabelSize(50);

  EnableDragCell( false );
  EnableEditing( false );
  DisableDragRowSize();

  // commented out, too early, needs to come after grid drawn, or provide in the attributes
  // found in ModelCell_macros.h
  //#ifdef GRID_EMIT_SetColSettings
  //#undef GRID_EMIT_SetColSettings
  //#endif

  //#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  //  /* m_grid.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); */ \
  //  SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  //int ix( 0 );
  //BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_grid.SetTable( this, false, wxGrid::wxGridSelectCells );

  Bind( wxEVT_DESTROY, &OptionOrderView::OnDestroy, this );

  Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &OptionOrderView::OnGridCellRightClick, this );

  // this GuiRefresh initialization should come after all else
  //m_timerGuiRefresh.SetOwner( this );
  //Bind( wxEVT_TIMER, &GridOptionDetails::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  //m_timerGuiRefresh.Start( 250 );

  //m_pimpl->CreateControls();
}

void OptionOrderView::Refresh() {
  //m_pimpl->Refresh();
  ForceRefresh();
}

void OptionOrderView::OnGridCellRightClick( wxGridEvent& event ) { // zero based, pop up menu

  if ( nullptr != m_pMenuRightClick ) {
    delete m_pMenuRightClick;
    m_pMenuRightClick = nullptr;
  }

  m_pMenuRightClick = new wxMenu();

  OptionOrderModel* pOptionOrderModel = reinterpret_cast<OptionOrderModel*>( GetTable() );
  const int cntRow = pOptionOrderModel->GetRowsCount();
  if ( ( cntRow - 1 ) == event.GetRow() ) {
    wxMenuItem* pMenuItemPlaceOrder = m_pMenuRightClick->Append( ID_MENUITEM_PlaceOrder, "place basket order" );
    m_pMenuRightClick->Bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [pOptionOrderModel]( wxCommandEvent& event ){
        BOOST_LOG_TRIVIAL(trace) << "menu item: place basket order";
        pOptionOrderModel->PlaceComboOrder();
      },
      pMenuItemPlaceOrder->GetId()
    );
  }
  else {
    wxMenuItem* pMenuItemDelete = m_pMenuRightClick->Append( ID_MENUITEM_DeleteOrder, "delete order" );
    m_pMenuRightClick->Bind(
      wxEVT_COMMAND_MENU_SELECTED,
      [pOptionOrderModel, row=event.GetRow()]( wxCommandEvent& event ){
        BOOST_LOG_TRIVIAL(trace) << "menu item: delete order " << row;
        pOptionOrderModel->DeleteOrder( row );
      },
      pMenuItemDelete->GetId()
    );
  }

  wxMenuItem* pMenuItemClearOrders = m_pMenuRightClick->Append( ID_MENUITEM_ClearOrders, "clear orders" );
  m_pMenuRightClick->Bind(
    wxEVT_COMMAND_MENU_SELECTED,
    [pOptionOrderModel, row=event.GetRow()]( wxCommandEvent& event ){
      BOOST_LOG_TRIVIAL(trace) << "menu item: clear orders";
      pOptionOrderModel->ClearOrders();
    },
    pMenuItemClearOrders->GetId()
  );


  PopupMenu( m_pMenuRightClick );

  event.Skip();
}

void OptionOrderView::ClearOrders() {
  //m_pimpl->ClearRows();
}

void OptionOrderView::PlaceComboOrder() {
  //m_pimpl->PlaceComboOrder();
}

int OptionOrderView::GetColumnCount() const {
  return GRID_ARRAY_COL_COUNT;
}

void OptionOrderView::OnDestroy( wxWindowDestroyEvent& event ) {

  //m_timerGuiRefresh.Stop();
  //Unbind( wxEVT_TIMER, &WinChartView::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  //m_pimpl->StopWatch();
  //m_pimpl->DestroyControls();
  //m_pimpl.reset();

  assert( Unbind( wxEVT_GRID_CELL_RIGHT_CLICK, &OptionOrderView::OnGridCellRightClick, this ) );
  assert( Unbind( wxEVT_DESTROY, &OptionOrderView::OnDestroy, this ) );

  if ( m_pMenuRightClick ) {
    delete m_pMenuRightClick;
    m_pMenuRightClick = nullptr;
  }

  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou
