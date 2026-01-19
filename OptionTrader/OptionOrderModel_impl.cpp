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
 * File:    OptionOrderModel_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/08 19:56:36
 */

#include <boost/log/trivial.hpp>

#include <TFTrading/Order.h>
#include <TFTrading/OrderManager.h>

#include "OptionOrderModel_impl.hpp"

namespace {
  const int c_nDefaultRows( 1 ); // summary line only
} // namespace

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionOrderModel_impl::OptionOrderModel_impl()
: m_pGrid( nullptr )
, m_fPlaceBagOrder( nullptr )
{
  m_vOptionOrderRow.reserve( 10 );
  assert( 1 ==c_nDefaultRows );
  m_vOptionOrderRow.emplace_back( std::make_unique<OptionOrderRow>() );
}

OptionOrderModel_impl::~OptionOrderModel_impl() {
  m_fPlaceBagOrder = nullptr;
  m_vOptionOrderRow.clear();
  m_pGrid = nullptr;
}

// private
void OptionOrderModel_impl::Add( OptionOrderRow::EType type, const std::string& sName, ou::tf::OrderSide::EOrderSide side, int quantity, fAdd_t&& f ) {

  assert( 0 < m_vOptionOrderRow.size() );

  bool bSymbolFound( false );

  for ( vOptionOrderRow_t::value_type& p: m_vOptionOrderRow ) {
    OptionOrderRow& row( *p );
    if ( type == row.m_type ) {
      if (
        boost::fusion::at_c<COL_Name>( row.m_vModelCells ).GetValue() == sName )
      {
        boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).SetValue( side );
        boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).SetValue( quantity );
        bSymbolFound = true;
        break; // exit for
      }
    }
  }

  if ( bSymbolFound ) {}
  else {

    vOptionOrderRow_t::iterator last = m_vOptionOrderRow.end();
    --last; // access the summary record

    m_vOptionOrderRow.emplace( last, std::move( f() ) );

    assert( m_pGrid );
    m_pGrid->InsertRows( m_vOptionOrderRow.size() - 1, 1, false );

  }

  assert( m_pGrid );
  m_pGrid->ForceRefresh();
  m_pGrid->GetParent()->Layout();
}

void OptionOrderModel_impl::Add( pWatch_t& pWatch, ou::tf::OrderSide::EOrderSide side, int quantity ) {
  const std::string& sName( pWatch->GetInstrumentName() );
  Add(
    OptionOrderRow::EType::underlying, sName, side, quantity,
    [pWatch, side, quantity](){
      return std::make_unique<OptionOrderRow>( pWatch, side, quantity );
    } );
}

void OptionOrderModel_impl::Add( pOption_t& pOption, ou::tf::OrderSide::EOrderSide side, int quantity ) {
  const std::string& sName( pOption->GetInstrumentName() );
  Add(
    OptionOrderRow::EType::option, sName, side, quantity,
    [pOption, side, quantity](){
      return std::make_unique<OptionOrderRow>( pOption, side, quantity );
    } );
}

void OptionOrderModel_impl::Refresh() {

  OptionOrderRow& summary( *m_vOptionOrderRow.back() );

  //boost::fusion::at_c<COL_Quan>(  summary.m_vModelCells ).SetValue(   0 );
  boost::fusion::at_c<COL_Last>(  summary.m_vModelCells ).SetValue( 0.0 ); // should this be here?
  //boost::fusion::at_c<COL_Bid>(   summary.m_vModelCells ).SetValue( 0.0 ); // best - no spread - lower amount
  //boost::fusion::at_c<COL_Ask>(   summary.m_vModelCells ).SetValue( 0.0 ); // full - full spread - higher amount
  //boost::fusion::at_c<COL_Delta>( summary.m_vModelCells ).SetValue( 0.0 );
  //boost::fusion::at_c<COL_Gamma>( summary.m_vModelCells ).SetValue( 0.0 );
  boost::fusion::at_c<COL_IV>(    summary.m_vModelCells ).SetValue( 0.0 );

  int sumQuan_int {};
  double sumQuan_dbl {};
  double sumLast {};
  double sumBid {};
  double sumAsk {};
  double sumDelta {};
  double sumGamma {};
  double sumIV {};

  for ( vOptionOrderRow_t::value_type& p: m_vOptionOrderRow ) {
    OptionOrderRow& item( *p );
    if ( ( OptionOrderRow::EType::option == item.m_type )
      || ( OptionOrderRow::EType::underlying == item.m_type )
    ) {

      double spread_full {};
      double spread_none {};

      const int quan_int = boost::fusion::at_c<COL_Quan>( item.m_vModelCells ).GetValue();
      const double quan_dbl = (double) quan_int;
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
          spread_none = boost::fusion::at_c<COL_Ask>( item.m_vModelCells ).GetValue();
          spread_full = boost::fusion::at_c<COL_Bid>( item.m_vModelCells ).GetValue();
          break;
        default:
          assert( false );
          break;
      }

      {
        sumAsk += multiplier * spread_full;
        boost::fusion::at_c<COL_Ask>( summary.m_vModelCells ).SetValue( sumAsk );
      }

      {
        sumBid += multiplier * spread_none;
        boost::fusion::at_c<COL_Bid>( summary.m_vModelCells ).SetValue( sumBid );
      }

      {
        const double delta = boost::fusion::at_c<COL_Delta>( item.m_vModelCells ).GetValue();
        sumDelta += multiplier * delta;
        boost::fusion::at_c<COL_Delta>( summary.m_vModelCells ).SetValue( sumDelta );
      }

      if ( OptionOrderRow::EType::option == item.m_type ) {
        {
          const double gamma = boost::fusion::at_c<COL_Gamma>( item.m_vModelCells ).GetValue();
          sumGamma += multiplier * gamma;
          boost::fusion::at_c<COL_Gamma>( summary.m_vModelCells ).SetValue( sumGamma );
        }

        {
          //const double iv = boost::fusion::at_c<COL_IV>( item.m_vModelCells ).GetValue();
          //sumIV += quan_dbl * iv;
          //boost::fusion::at_c<COL_IV>( summary.m_vModelCells ).SetValue( sumIV / sumQuan_dbl );
        }
      }
    }
  }

  m_pGrid->ForceRefresh();

}

void OptionOrderModel_impl::SetView( wxGrid* pGrid ) {
  m_pGrid = pGrid;
}

int OptionOrderModel_impl::GetNumberRows() {
  return m_vOptionOrderRow.size();
}

int OptionOrderModel_impl::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

bool OptionOrderModel_impl::IsEmptyCell( int row, int col ) {
  return false;
}

void OptionOrderModel_impl::SetValue( int row, int col, const wxString &value ) {
  assert( false );  // not sure if this is used
}

wxString OptionOrderModel_impl::GetValue( int ixRow, int ixCol ) {

  assert( 0 <= ixRow );
  assert( m_vOptionOrderRow.size() > ixRow );

  wxString s;
  OptionOrderRow& row( *m_vOptionOrderRow[ ixRow ] );

  switch ( row.m_type ) {
    case OptionOrderRow::EType::underlying:
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
        default:
          s = "n/a";
          break;
      }
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
      assert( ( m_vOptionOrderRow.size() - 1 ) == ixRow );
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

wxString OptionOrderModel_impl::GetColLabelValue( int col ) {

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

wxGridCellAttr* OptionOrderModel_impl::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

  int align = wxALIGN_CENTER;

  #define GRID_EMIT_SwitchGetColAlign( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      align = GRID_EXTRACT_COL_DETAILS(z, n, 2 ); \
      break;

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  switch ( kind ) {
    case wxGridCellAttr::wxAttrKind::Any:
    case wxGridCellAttr::wxAttrKind::Cell:
    case wxGridCellAttr::wxAttrKind::Col: // Col type is never used
      switch ( col ) {
        BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColAlign, 0 )
      }
      pAttr->SetAlignment( align, wxALIGN_CENTER_VERTICAL );

      switch ( col ) {
        case COL_Quan:
          pAttr->SetReadOnly( false );
          break;
        default:
          pAttr->SetReadOnly();
          break;
      }
      break;
    case wxGridCellAttr::wxAttrKind::Row:
      assert( true );
      break;
    case wxGridCellAttr::wxAttrKind::Default:
      assert( true );
      break;
  }

  return pAttr;

}

void OptionOrderModel_impl::DeleteOrder( size_t row ) {
  if ( ( m_vOptionOrderRow.size() - 1 ) == row ) {
    //assert( OptionOrderRow::EType::summary == m_vOptionOrderRow.front()->m_type );
    assert( false );  // can't delete summary line
  }
  else {
    m_vOptionOrderRow.erase( m_vOptionOrderRow.begin() + row );
    assert( m_pGrid );
    m_pGrid->DeleteRows( row, 1, false );
    m_pGrid->ForceRefresh();
    m_pGrid->GetParent()->Layout();
  }
}

void OptionOrderModel_impl::Set( fPlaceBagOrder_t&& fPlaceBagOrder ) {
  assert( fPlaceBagOrder );
  m_fPlaceBagOrder = std::move( fPlaceBagOrder );
}

void OptionOrderModel_impl::BagOrderBuy() {
  if ( 1 >= m_vOptionOrderRow.size() ) {
    BOOST_LOG_TRIVIAL(warning) << "no orders present";
  }
  else {
    ou::tf::ib::Bag bag;
    bag.side = ou::tf::OrderSide::Buy;
    BagOrder( bag );
    assert( m_fPlaceBagOrder );
    m_fPlaceBagOrder( bag );
  }
}

void OptionOrderModel_impl::BagOrderSell() {
  if ( 1 >= m_vOptionOrderRow.size() ) {
    BOOST_LOG_TRIVIAL(warning) << "no orders present";
  }
  else {
    ou::tf::ib::Bag bag;
    bag.side = ou::tf::OrderSide::Sell;
    BagOrder( bag );
    assert( m_fPlaceBagOrder );
    m_fPlaceBagOrder( bag );
  }
}

void OptionOrderModel_impl::BagOrder( ou::tf::ib::Bag& bag ) {
  assert( ou::tf::OrderSide::Unknown != bag.side );
  for ( vOptionOrderRow_t::value_type& p: m_vOptionOrderRow ) {

    OptionOrderRow& row( *p );

    if ( OptionOrderRow::EType::summary == row.m_type ) {

      bag.quantity = 1;

      double priceBag {};
      switch ( bag.side ) {
        case ou::tf::OrderSide::Buy:
          priceBag = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetValue(); // start on low side
          break;
        case ou::tf::OrderSide::Sell:
          priceBag = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetValue(); // start on high side
          break;
        default:
          assert( false );
      }

      bag.price = priceBag;
      if ( 0.0 == bag.price ) {
        bag.type = ou::tf::OrderType::Market;
      }
      else {
        bag.type = ou::tf::OrderType::Limit;
      }
    }
    else {
      pInstrument_t pInstrument;
      switch ( row.m_type ) {
        case OptionOrderRow::EType::underlying:
          pInstrument = row.m_pWatch->GetInstrument();
          assert( 0 != pInstrument->GetContract() );
          break;
        case OptionOrderRow::EType::option:
          pInstrument = row.m_pOption->GetInstrument();
          assert( 0 != pInstrument->GetContract() );
          break;
        case OptionOrderRow::EType::summary:
          assert( false );
          break;
      }

      if ( 0 == bag.name.size() ) {
        if ( row.m_pOption ) {
          if ( 0 < row.m_pOption->GetFundamentals().sExchangeRoot.size() ) {
            bag.name = row.m_pOption->GetFundamentals().sExchangeRoot;
          }
          else {
            bag.name = pInstrument->GetInstrumentName();
          }
        }
        if ( row.m_pWatch ) {
          if ( 0 < row.m_pWatch->GetFundamentals().sExchangeRoot.size() ) {
            bag.name = row.m_pWatch->GetFundamentals().sExchangeRoot;
          }
          else {
            bag.name = pInstrument->GetInstrumentName();
          }
        }
      }
      else {
        //bag.name += ',';
      }
      //bag.name += pInstrument->GetInstrumentName();

      const int side = boost::fusion::at_c<COL_OrderSide>( row.m_vModelCells ).GetValue();

      ou::tf::Order::pOrder_t pOrder = std::make_shared<ou::tf::Order>(
        pInstrument
      , ou::tf::OrderType::Market
      , (ou::tf::OrderSide::EOrderSide) side
      , boost::fusion::at_c<COL_Quan>( row.m_vModelCells ).GetValue()
      );

      double priceOrder {};
      switch ( bag.side ) {
        case ou::tf::OrderSide::Buy:
          switch ( side ) {
            case ou::tf::OrderSide::Buy:
              priceOrder = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetValue(); // start on low side
              break;
            case ou::tf::OrderSide::Sell:
              priceOrder = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetValue(); // start on high side
              break;
            default:
              assert( false );
              break;
          }
          break;
        case ou::tf::OrderSide::Sell:
          switch ( side ) {
            case ou::tf::OrderSide::Buy:
              priceOrder = boost::fusion::at_c<COL_Ask>( row.m_vModelCells ).GetValue(); // start on high side
              break;
            case ou::tf::OrderSide::Sell:
              priceOrder = boost::fusion::at_c<COL_Bid>( row.m_vModelCells ).GetValue(); // start on low side
              break;
            default:
              assert( false );
              break;
          }
          break;
        default:
          assert( false );
      }

      BOOST_LOG_TRIVIAL(trace) << "order " << pOrder->GetQuanOrdered() << " " << pInstrument->GetInstrumentName() << " @ " << priceOrder;

      bag.vOrder.push_back( pOrder );
    }
  }

  auto& front_order( bag.vOrder.front() );
  ou::tf::OrderManager::GlobalInstance().SetOrderId( front_order );
  bag.order_id = front_order->GetOrderId();

  ClearOrders();
}

void OptionOrderModel_impl::ClearOrders() {
  while ( OptionOrderRow::EType::summary != m_vOptionOrderRow.front()->m_type ) {
    DeleteOrder( 0 );
  }
}

} // namespace tf
} // namespace ou
