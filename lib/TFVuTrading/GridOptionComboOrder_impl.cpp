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

GridOptionComboOrder_impl::GridOptionComboOrder_impl( GridOptionComboOrder& grid )
: wxGridTableBase()
, m_grid( grid ) {
}

GridOptionComboOrder_impl::~GridOptionComboOrder_impl() {
  m_grid.SetTable( nullptr, false, wxGrid::wxGridSelectNone );
}

void GridOptionComboOrder_impl::CreateControls() {

  m_grid.SetDefaultColSize(50);
  m_grid.SetDefaultRowSize(22);
  m_grid.SetColLabelSize(22);
  m_grid.SetRowLabelSize(50);

  m_grid.SetTable( this, false, wxGrid::wxGridSelectCells );

  // found in ModelCell_macros.h
  #ifdef GRID_EMIT_SetColSettings
  #undef GRID_EMIT_SetColSettings
  #endif

  #define GRID_EMIT_SetColSettings( z, n, VAR ) \
    /* m_grid.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); */ \
    m_grid.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_grid.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  m_grid.EnableDragCell( true );

//  m_grid.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want
//  m_grid.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  m_grid.EnableEditing( false );

  m_vOptionComboOrderRow.resize( 5 );
  m_grid.AppendRows( 5 ); // cells labelled empty when no order or summary is present ( fifth row is summary stats)

}

void GridOptionComboOrder_impl::Add( ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sName ) {

  bool bSymbolFound( false );
  for (vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
    if ( OptionComboOrderRow::EType::item == row.m_type ) {
      if ( boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue() == sName ) {
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        boost::fusion::at_c<COL_Price>( row.m_vModelCells ).SetValue( price );
        bSymbolFound = true;
        break;
      }
    }
  }

  if ( bSymbolFound ) {
    m_grid.ForceRefresh();
  }
  else {
    bool bEmptyFilled( false );
    for (vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
      if ( OptionComboOrderRow::EType::empty == row.m_type ) {
        row.m_type = OptionComboOrderRow::EType::item;
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        boost::fusion::at_c<COL_Price>( row.m_vModelCells ).SetValue( price );
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).SetValue( sName );
        bEmptyFilled = true;
        break; // exit for
      }
    }

    if ( bEmptyFilled ) {
      m_grid.ForceRefresh();
    }
    else {
      std::cout << "no empty slot found for " << sName << std::endl;
    }
  }

}

void GridOptionComboOrder_impl::Refresh() {
  m_grid.ForceRefresh();
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
  return ( OptionComboOrderRow::EType::empty == m_vOptionComboOrderRow[ row ].m_type );
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

wxString GridOptionComboOrder_impl::GetValue( int ixRow, int ixCol ) {
  wxString s;
  assert( 0 <= ixRow );
  assert( m_vOptionComboOrderRow.size() > ixRow );
  OptionComboOrderRow& row( m_vOptionComboOrderRow[ ixRow ] );
  if ( OptionComboOrderRow::EType::empty == row.m_type ) {
    s = "";
  }
  else {
    switch ( ixCol ) {
      case COL_OrderSide:
        switch ( boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).GetValue() ) {
          case ou::tf::OrderSide::Buy:
            s = "Buy";
            break;
          case ou::tf::OrderSide::Sell:
            s = "Sell";
            break;
          default:
            assert( false );
            break;
        }
        break;
      case COL_Quan:
        s = boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).GetText();
        break;
      case COL_Price:
        s = boost::fusion::at_c<COL_Price>( row.m_vModelCells ).GetText();
        break;
      case COL_Name:
        s = boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetText();
        break;
      default:
        s = "";
        break;
    }
  }
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
// m_grid.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionOrder_impl::OnGridCellBeginDrag, this );
// m_grid.Unbind( wxEVT_MOTION, &GridOptionOrder_impl::OnMouseMotion, this );
// m_grid.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
