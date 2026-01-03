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

namespace {
  static const int c_nDefaultRows( 5 );
}

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
  m_grid.SetDefaultRowSize(20);
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

  assert( 2 < c_nDefaultRows );
  m_vOptionComboOrderRow.resize( c_nDefaultRows );
  m_grid.AppendRows( c_nDefaultRows ); // cells labelled empty when no order or summary is present ( fifth row is summary stats)

  m_vOptionComboOrderRow[ c_nDefaultRows - 1 ].m_type = OptionComboOrderRow::EType::summary;

}

void GridOptionComboOrder_impl::Add( ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sIQFeedName ) {

  bool bSymbolFound( false );
  for ( vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
    if ( OptionComboOrderRow::EType::item == row.m_type ) {
      if (
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue() == sIQFeedName )
      {
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        boost::fusion::at_c<COL_Price>( row.m_vModelCells ).SetValue( price );
        bSymbolFound = true;
        break; // exit for
      }
    }
  }

  if ( bSymbolFound ) {
    m_grid.ForceRefresh();
  }
  else {
    bool bEmptyFilled( false );
    for ( vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
      if ( OptionComboOrderRow::EType::empty == row.m_type ) {

        row.m_type = OptionComboOrderRow::EType::item;
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        boost::fusion::at_c<COL_Price>( row.m_vModelCells ).SetValue( price );
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).SetValue( sIQFeedName );

        if ( m_grid.m_fOptionDelegates_Attach ) {
          ou::tf::option::Delegates delegates;
          delegates.sSymbolName = sIQFeedName;
          delegates.fdQuote = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateQuote );
          delegates.fdGreek = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateGreeks );
          delegates.fdTrade = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateTrade );
          m_grid.m_fOptionDelegates_Attach( delegates );
        }

        bEmptyFilled = true;
        break; // exit for
      }
    }

    if ( bEmptyFilled ) {
      m_grid.ForceRefresh();
    }
    else {
      std::cout << "no empty slot found for " << sIQFeedName << std::endl;
    }
  }

}

void GridOptionComboOrder_impl::Refresh() {

  OptionComboOrderRow& summary( m_vOptionComboOrderRow[ c_nDefaultRows - 1] );

  boost::fusion::at_c<COL_Quan>(  summary.m_vModelCells ).SetValue(   0 );
  boost::fusion::at_c<COL_Price>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_Bid>(   summary.m_vModelCells ).SetValue( 0.0 ); // best - no spread - lower amount
  boost::fusion::at_c<COL_Ask>(   summary.m_vModelCells ).SetValue( 0.0 ); // full - full spread - higher amount
  boost::fusion::at_c<COL_Delta>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_Gamma>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_IV>(    summary.m_vModelCells ).SetValue( 0.0 );

  int sumQuan_int {};
  double sumQuan_dbl {};
  double sumPrice {};
  double sumBid {};
  double sumAsk {};
  double sumDelta {};
  double sumGamma {};
  double sumIV {};

  for ( vOptionComboOrderRow_t::value_type& item: m_vOptionComboOrderRow ) {
    if ( OptionComboOrderRow::EType::item == item.m_type ) {

      double spread_none {};
      double spread_full {};

      int quan_int = boost::fusion::at_c<COL_Quan>( item.m_vModelCells ).GetValue();
      double quan_dbl = quan_int;
      sumQuan_int += quan_int;
      sumQuan_dbl += quan_dbl;
      boost::fusion::at_c<COL_Quan>( summary.m_vModelCells ).SetValue( sumQuan_int );

      double multiplier {};
      switch ( boost::fusion::at_c<COL_OrderSide>( item.m_vModelCells ).GetValue() ) {
        case ou::tf::OrderSide::Buy:
          multiplier = 1.0 * quan_dbl;
          spread_full = boost::fusion::at_c<COL_Ask>( item.m_vModelCells ).GetValue();
          spread_none = boost::fusion::at_c<COL_Bid>( item.m_vModelCells ).GetValue();
          break;
        case ou::tf::OrderSide::Sell:
          multiplier = -1.0 * quan_dbl;
          spread_full = boost::fusion::at_c<COL_Bid>( item.m_vModelCells ).GetValue();
          spread_none = boost::fusion::at_c<COL_Ask>( item.m_vModelCells ).GetValue();
          break;
        default:
          assert( false );
          break;
      }

      double price = boost::fusion::at_c<COL_Price>( item.m_vModelCells ).GetValue();
      sumPrice += multiplier * price;
      boost::fusion::at_c<COL_Price>( summary.m_vModelCells ).SetValue( sumPrice );

      sumBid += multiplier * spread_none;
      //sumBid += quan_dbl * spread_none;
      boost::fusion::at_c<COL_Bid>( summary.m_vModelCells ).SetValue( sumBid );

      sumAsk += multiplier * spread_full;
      //sumAsk += quan_dbl * spread_full;
      boost::fusion::at_c<COL_Ask>( summary.m_vModelCells ).SetValue( sumAsk );

      double delta = boost::fusion::at_c<COL_Delta>( item.m_vModelCells ).GetValue();
      sumDelta += multiplier * delta;
      boost::fusion::at_c<COL_Delta>( summary.m_vModelCells ).SetValue( sumDelta );

      double gamma = boost::fusion::at_c<COL_Gamma>( item.m_vModelCells ).GetValue();
      sumGamma += multiplier * gamma;
      boost::fusion::at_c<COL_Gamma>( summary.m_vModelCells ).SetValue( sumGamma );

      double iv = boost::fusion::at_c<COL_IV>( item.m_vModelCells ).GetValue();
      sumIV += quan_dbl * iv;
      boost::fusion::at_c<COL_IV>( summary.m_vModelCells ).SetValue( sumIV / sumQuan_dbl );

    }
  }

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

  assert( 0 <= ixRow );
  assert( m_vOptionComboOrderRow.size() > ixRow );

  wxString s;
  OptionComboOrderRow& row( m_vOptionComboOrderRow[ ixRow ] );

  switch ( row.m_type ) {
    case OptionComboOrderRow::EType::empty:
      break;
    case OptionComboOrderRow::EType::item:
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
        case COL_Bid:
          s = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetText();
          break;
        case COL_Ask:
          s = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetText();
          break;
        case COL_Delta:
          s = boost::fusion::at_c<COL_Delta>( row.m_vModelCells ).GetText();
          break;
        case COL_Gamma:
          s = boost::fusion::at_c<COL_Gamma>( row.m_vModelCells ).GetText();
          break;
        case COL_IV:
          s = boost::fusion::at_c<COL_IV>( row.m_vModelCells ).GetText();
          break;
        default:
          s = "n/a";
          break;
      }
      break;
    case OptionComboOrderRow::EType::summary:
      assert( ( c_nDefaultRows - 1 ) == ixRow );
      switch ( ixCol ) {
        case COL_OrderSide:
          // empty
          break;
        case COL_Quan:
          s = boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).GetText();
          break;
        case COL_Price:
          s = boost::fusion::at_c<COL_Price>( row.m_vModelCells ).GetText();
          break;
        case COL_Name:
          // empty
          break;
        case COL_Bid:
          s = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetText();
          break;
        case COL_Ask:
          s = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetText();
          break;
        case COL_Delta:
          s = boost::fusion::at_c<COL_Delta>( row.m_vModelCells ).GetText();
          break;
        case COL_Gamma:
          s = boost::fusion::at_c<COL_Gamma>( row.m_vModelCells ).GetText();
          break;
        case COL_IV:
          s = boost::fusion::at_c<COL_IV>( row.m_vModelCells ).GetText();
          break;
        default:
          s = "n/a";
          break;
      }
      break;
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
    case wxGridCellAttr::wxAttrKind::Any:
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

void GridOptionComboOrder_impl::PlaceComboOrder() {
  m_grid.m_fGatherOrderLegs(
    [this]( GridOptionComboOrder::fOrderLeg_t&& fOrderLeg ){
      for ( vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
        switch (row.m_type ) {
          case OptionComboOrderRow::EType::item:
            {
              int side = boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).GetValue();
              double price {};
              switch ( side ) {
                case ou::tf::OrderSide::Buy:
                  price = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetValue(); // start on low side
                  break;
                case ou::tf::OrderSide::Sell:
                  price = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetValue(); // start on high side
                  break;
                default:
                  assert( false );
                  break;
              }
              fOrderLeg(
                (ou::tf::OrderSide::EOrderSide) side,
                boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).GetValue(),
                price, //boost::fusion::at_c<COL_Price>( row.m_vModelCells ).GetValue(),
                boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue()
              );
            }
            break;
          default:
            break;
        }
      }
    }
  );
  ClearRows();
}

void GridOptionComboOrder_impl::ClearRows() {
  for ( vOptionComboOrderRow_t::value_type& row: m_vOptionComboOrderRow ) {
    switch ( row.m_type ) {
      case OptionComboOrderRow::EType::item:
        if ( m_grid.m_fOptionDelegates_Detach ) {
          ou::tf::option::Delegates delegates;
          delegates.sSymbolName =  boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue();
          delegates.fdQuote = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateQuote );
          delegates.fdGreek = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateGreeks );
          delegates.fdTrade = fastdelegate::MakeDelegate( &row, &OptionComboOrderRow::UpdateTrade );
          m_grid.m_fOptionDelegates_Detach( delegates );
        }
        row.m_type = OptionComboOrderRow::EType::empty;
        break;
      case OptionComboOrderRow::EType::empty:
        break;
      case OptionComboOrderRow::EType::summary:
        break;
    }
  }

  m_grid.ForceRefresh();
}

void GridOptionComboOrder_impl::DestroyControls() {

  ClearRows();

// m_grid.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionOrder_impl::OnGridCellBeginDrag, this );
// m_grid.Unbind( wxEVT_MOTION, &GridOptionOrder_impl::OnMouseMotion, this );
// m_grid.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
