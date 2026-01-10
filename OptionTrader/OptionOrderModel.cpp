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
#include "OptionOrderModel_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionOrderModel::OptionOrderModel()
: wxGridTableBase()
, m_pOptionOrderModel_impl( std::make_unique<OptionOrderModel_impl>() ) // conflicts with OptionChainModel macros
{
}

OptionOrderModel::~OptionOrderModel() {
  m_pOptionOrderModel_impl.reset();
}

void OptionOrderModel::Add( pWatch_t& pWatch, ou::tf::OrderSide::EOrderSide side, int quantity ) {
  m_pOptionOrderModel_impl->Add( pWatch, side, quantity );
}

void OptionOrderModel::Add( pOption_t& pOption, ou::tf::OrderSide::EOrderSide side, int quantity ) {
  m_pOptionOrderModel_impl->Add( pOption, side, quantity );
}

void OptionOrderModel::Refresh() {
  m_pOptionOrderModel_impl->Refresh();
}

void OptionOrderModel::SetView( wxGrid *grid ) {
  m_pOptionOrderModel_impl->SetView( grid );
  wxGridTableBase::SetView( grid );
}

//wxGrid* OptionOrderModel::GetView() const {
//  return wxGridTableBase::GetView();
//}

int OptionOrderModel::GetNumberRows() {
  return m_pOptionOrderModel_impl->GetNumberRows();
}

int OptionOrderModel::GetNumberCols() {
  return m_pOptionOrderModel_impl->GetNumberCols();
}

bool OptionOrderModel::IsEmptyCell( int row, int col ) {
  return m_pOptionOrderModel_impl->IsEmptyCell( row, col );
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
    return true;
  }
  else {
    return false;
  }
}

bool OptionOrderModel::AppendRows( size_t numRows ) {
  if ( GetView() ) {
    wxGridTableMessage msg(
      this,
      wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
      numRows
    );
    GetView()->ProcessTableMessage( msg );
    return true;
  }
  else {
    return false;
  }
}

bool OptionOrderModel::DeleteRows( size_t pos, size_t numRows ) {
  if ( GetView() ) {
    wxGridTableMessage msg(
      this,
      wxGRIDTABLE_NOTIFY_ROWS_DELETED,
      pos,
      numRows
    );
    GetView()->ProcessTableMessage( msg );
    return true;
  }
  else {
    return false;
  }
}

void OptionOrderModel::SetValue( int row, int col, const wxString &value ) {
  m_pOptionOrderModel_impl->SetValue( row, col, value );
}

wxString OptionOrderModel::GetValue( int ixRow, int ixCol ) {
  return m_pOptionOrderModel_impl->GetValue( ixRow, ixCol );
}

wxString OptionOrderModel::GetColLabelValue( int col ) {
  return m_pOptionOrderModel_impl->GetColLabelValue( col );
}

wxGridCellAttr* OptionOrderModel::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {
  return m_pOptionOrderModel_impl->GetAttr( row, col, kind );
}

void OptionOrderModel::Set( fGatherOrderLegs_t&& fGatherOrderLegs ) {
  m_pOptionOrderModel_impl->Set( std::move( fGatherOrderLegs ) );
}

OptionOrderModel::fOrderLeg_t OptionOrderModel::FactoryAddComboOrderLeg() {
  return std::move( m_pOptionOrderModel_impl->FactoryAddComboOrderLeg() );
}

void OptionOrderModel::DeleteOrder( size_t row ) {
  m_pOptionOrderModel_impl->DeleteOrder( row );
}

void OptionOrderModel::PlaceComboOrder() {
  m_pOptionOrderModel_impl->PlaceComboOrder();
}

void OptionOrderModel::ClearCombo() {
  m_pOptionOrderModel_impl->ClearCombo();
}

} // namespace tf
} // namespace ou
