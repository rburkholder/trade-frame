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
 * File:    GridOptionComboOrder_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/23 17:28:15
 */

#include "GridOptionComboOrder_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionComboOrder_impl::GridOptionComboOrder_impl( GridOptionComboOrder& details )
: wxGridTableBase()
, m_details( details ), m_bTimerActive( false ) {
}

GridOptionComboOrder_impl::~GridOptionComboOrder_impl() {
  m_details.SetTable( nullptr, false, wxGrid::wxGridSelectNone );
}

void GridOptionComboOrder_impl::CreateControls() {

  m_details.SetDefaultColSize(50);
  m_details.SetDefaultRowSize(22);
  m_details.SetColLabelSize(22);
  m_details.SetRowLabelSize(50);

  m_details.SetTable( this, false, wxGrid::wxGridSelectCells );

  // found in ModelCell_macros.h
  #ifdef GRID_EMIT_SetColSettings
  #undef GRID_EMIT_SetColSettings
  #endif

  #define GRID_EMIT_SetColSettings( z, n, VAR ) \
    /* m_details.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); */ \
    m_details.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_details.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  m_details.EnableDragCell( true );

//  m_details.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want
//  m_details.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  m_details.EnableEditing( false );

  m_vOptionComboOrderRow.resize( 5 );
  m_details.AppendRows( 5 ); // cells labelled empty when no order or summary is present ( fifth row is summary stats)

}

void GridOptionComboOrder_impl::Start() {
  if ( !m_bTimerActive ) {
    m_bTimerActive = true;
    // this GuiRefresh initialization should come after all else
    m_timerGuiRefresh.SetOwner( &m_details );
    m_details.Bind( wxEVT_TIMER, &GridOptionComboOrder_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
    m_timerGuiRefresh.Start( 300 );
  }
}

void GridOptionComboOrder_impl::Stop() {
  if ( m_bTimerActive ) {
    m_bTimerActive = false;
    m_timerGuiRefresh.Stop();
    m_timerGuiRefresh.DeletePendingEvents();
    m_details.Unbind( wxEVT_TIMER, &GridOptionComboOrder_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  }
}

void GridOptionComboOrder_impl::HandleGuiRefresh( wxTimerEvent& event ) {
  m_details.ForceRefresh();
}

void GridOptionComboOrder_impl::SetView( wxGrid *grid ) {
  wxGridTableBase::SetView( grid );
}

wxGrid* GridOptionComboOrder_impl::GetView() const {
  return wxGridTableBase::GetView();
}

int GridOptionComboOrder_impl::GetNumberRows() {
  //return m_mapOptionValueRow.size();
  return 0;
}

int GridOptionComboOrder_impl::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

bool GridOptionComboOrder_impl::IsEmptyCell( int row, int col ) {
  return ( m_vOptionComboOrderRow[ row ].m_bActive );
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
// wxGridStringTable::InsertRows
bool GridOptionComboOrder_impl::InsertRows( size_t pos, size_t numRows ) {
  //return wxGridTableBase::InsertRows( pos, numRows ); // don't do this
    if ( GetView() ) {
      wxGridTableMessage msg(
        this,
        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
        pos,
        numRows
      );
      GetView()->ProcessTableMessage( msg );
    }
  return true;
}

bool GridOptionComboOrder_impl::AppendRows( size_t numRows ) {
  if ( GetView() ) {
    wxGridTableMessage msg(
      this,
      wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
      numRows
    );
    GetView()->ProcessTableMessage( msg );
  }
return true;
}

void GridOptionComboOrder_impl::SetValue( int row, int col, const wxString &value ) {
  assert( false );  // not sure if this is used
}

wxString GridOptionComboOrder_impl::GetValue( int row, int col ) {
  wxString s;
  return s;
}

wxString GridOptionComboOrder_impl::GetColLabelValue( int col ) {

  wxString s;

  #define GRID_EMIT_SwitchGetColLabel( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      s = wxString( GRID_EXTRACT_COL_DETAILS(z, n, 1 ) ); \
      break;

  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColLabel, 0 )
  }

  return s;
}

wxGridCellAttr* GridOptionComboOrder_impl::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

  int align = wxALIGN_CENTER;

  #define GRID_EMIT_SwitchGetColAlign( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      align = GRID_EXTRACT_COL_DETAILS(z, n, 2 ); \
      break;

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  switch ( kind ) {
    case wxGridCellAttr::wxAttrKind::Cell:
    case wxGridCellAttr::wxAttrKind::Col:
      switch ( col ) {
        case COL_Quan:
        case COL_Price:
          switch ( col ) {
            BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColAlign, 0 )
          }
          pAttr->SetReadOnly( false );
          break;
        default:
          pAttr->SetReadOnly();
          break;
      }
      break;
    case wxGridCellAttr::wxAttrKind::Row:
      break;
    case wxGridCellAttr::wxAttrKind::Default:
      break;
  }

  pAttr->SetAlignment( align, wxALIGN_CENTER_VERTICAL );

  return pAttr;

}

void GridOptionComboOrder_impl::DestroyControls() {
//  m_details.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionOrder_impl::OnGridCellBeginDrag, this );
//  m_details.Unbind( wxEVT_MOTION, &GridOptionOrder_impl::OnMouseMotion, this );  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
