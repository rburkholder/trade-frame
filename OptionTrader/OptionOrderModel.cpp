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
 * File:    OptionOrderModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/05 10:38:02
 */

#include "OptionOrderModel.hpp"

namespace {
  static const int c_nDefaultRows( 1 ); // summary line only
}

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionOrderModel::OptionOrderModel()
: wxGridTableBase()
{
}

OptionOrderModel::~OptionOrderModel() {
}

void OptionOrderModel::CreateControls() {

  //m_grid.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );


//  m_grid.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want
//  m_grid.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  assert( 1 ==c_nDefaultRows );
  m_vOptionOrderRow.resize( c_nDefaultRows );
  //m_grid.AppendRows( c_nDefaultRows ); // cells labelled empty when no order or summary is present ( fifth row is summary stats)

  m_vOptionOrderRow[ c_nDefaultRows - 1 ].m_type = OptionOrderRow::EType::summary;

}

void OptionOrderModel::Add( ou::tf::OrderSide::EOrderSide side, int quan, const std::string& sIQFeedName ) {

  bool bSymbolFound( false );
  for ( vOptionOrderRow_t::value_type& row: m_vOptionOrderRow ) {
    if ( OptionOrderRow::EType::option == row.m_type ) {
      if (
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue() == sIQFeedName )
      {
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        bSymbolFound = true;
        break; // exit for
      }
    }
  }

  if ( bSymbolFound ) {
    //m_grid.ForceRefresh();
  }
  else {
    bool bEmptyFilled( false );
    for ( vOptionOrderRow_t::value_type& row: m_vOptionOrderRow ) {
      if ( OptionOrderRow::EType::empty == row.m_type ) {

        row.m_type = OptionOrderRow::EType::option;
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quan );
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).SetValue( sIQFeedName );

        bEmptyFilled = true;
        break; // exit for
      }
    }

    if ( bEmptyFilled ) {
      //m_grid.ForceRefresh();
    }
    else {
      std::cout << "no empty slot found for " << sIQFeedName << std::endl;
    }
  }

}

void OptionOrderModel::Refresh() {

  OptionOrderRow& summary( m_vOptionOrderRow[ c_nDefaultRows - 1] );

  boost::fusion::at_c<COL_Quan>(  summary.m_vModelCells ).SetValue(   0 );
  boost::fusion::at_c<COL_Last>(  summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_Bid>(   summary.m_vModelCells ).SetValue( 0.0 ); // best - no spread - lower amount
  boost::fusion::at_c<COL_Ask>(   summary.m_vModelCells ).SetValue( 0.0 ); // full - full spread - higher amount
  boost::fusion::at_c<COL_Delta>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_Gamma>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_IV>(    summary.m_vModelCells ).SetValue( 0.0 );

  int sumQuan_int {};
  double sumQuan_dbl {};
  double sumLast {};
  double sumBid {};
  double sumAsk {};
  double sumDelta {};
  double sumGamma {};
  double sumIV {};

  for ( vOptionOrderRow_t::value_type& item: m_vOptionOrderRow ) {
    if ( OptionOrderRow::EType::option == item.m_type ) {

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

      // TODO: use bid ask to define spread
      //double price = boost::fusion::at_c<COL_Price>( item.m_vModelCells ).GetValue();
      //sumPrice += multiplier * price;
      //boost::fusion::at_c<COL_Price>( summary.m_vModelCells ).SetValue( sumPrice );

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

void OptionOrderModel::SetView( wxGrid *grid ) {
  wxGridTableBase::SetView( grid );
}

wxGrid* OptionOrderModel::GetView() const {
  return wxGridTableBase::GetView();
}

int OptionOrderModel::GetNumberRows() {
  //return m_mapOptionValueRow.size();
  return 0;
}

int OptionOrderModel::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

bool OptionOrderModel::IsEmptyCell( int row, int col ) {
  return ( OptionOrderRow::EType::empty == m_vOptionOrderRow[ row ].m_type );
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
// wxGridStringTable::InsertRows
bool OptionOrderModel::InsertRows( size_t pos, size_t numRows ) {
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

bool OptionOrderModel::AppendRows( size_t numRows ) {
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

void OptionOrderModel::SetValue( int row, int col, const wxString &value ) {
  assert( false );  // not sure if this is used
}

wxString OptionOrderModel::GetValue( int ixRow, int ixCol ) {

  assert( 0 <= ixRow );
  assert( m_vOptionOrderRow.size() > ixRow );

  wxString s;
  OptionOrderRow& row( m_vOptionOrderRow[ ixRow ] );

  switch ( row.m_type ) {
    case OptionOrderRow::EType::empty:
      break;
    case OptionOrderRow::EType::option:
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
        case COL_Last:
          s = boost::fusion::at_c<COL_Last>( row.m_vModelCells ).GetText();
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
    case OptionOrderRow::EType::summary:
      assert( ( c_nDefaultRows - 1 ) == ixRow );
      switch ( ixCol ) {
        case COL_OrderSide:
          // empty
          break;
        case COL_Quan:
          s = boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).GetText();
          break;
        case COL_Last:
          s = boost::fusion::at_c<COL_Last>( row.m_vModelCells ).GetText();
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

wxString OptionOrderModel::GetColLabelValue( int col ) {

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

wxGridCellAttr* OptionOrderModel::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

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

void OptionOrderModel::Set( fGatherOrderLegs_t&& fGatherOrderLegs ) {
  m_fGatherOrderLegs = std::move( fGatherOrderLegs );
  assert( m_fGatherOrderLegs );
}


OptionOrderModel::fOrderLeg_t OptionOrderModel::FactoryAddComboOrderLeg() {
  fOrderLeg_t f = [this](ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sName ){
    //m_pimpl->Add( side, quan, price, sName );
  };
  return std::move( f );
}

void OptionOrderModel::PlaceComboOrder() {
  m_fGatherOrderLegs(
    [this]( OptionOrderModel::fOrderLeg_t&& fOrderLeg ){
      for ( vOptionOrderRow_t::value_type& row: m_vOptionOrderRow ) {
        switch (row.m_type ) {
          case OptionOrderRow::EType::option:
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

void OptionOrderModel::ClearRows() {
  for ( vOptionOrderRow_t::value_type& row: m_vOptionOrderRow ) {
    switch ( row.m_type ) {
      case OptionOrderRow::EType::option:
        //if ( m_grid.m_fOptionDelegates_Detach ) {
        //}
        row.m_type = OptionOrderRow::EType::empty;
        break;
      case OptionOrderRow::EType::empty:
        break;
      case OptionOrderRow::EType::summary:
        break;
    }
  }

  //m_grid.ForceRefresh();
}

void OptionOrderModel::DestroyControls() {

  ClearRows();

// m_grid.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionOrder_impl::OnGridCellBeginDrag, this );
// m_grid.Unbind( wxEVT_MOTION, &GridOptionOrder_impl::OnMouseMotion, this );
// m_grid.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
