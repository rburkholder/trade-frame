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
 * File:    OptionChainModel.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:59
 */

#include <string>

#include <boost/log/trivial.hpp>

#include <fmt/core.h>

#include "OptionChainModel.hpp"
#include "OptionChainModel_impl.hpp"

OptionChainModel::OptionChainModel(
  mapChains_t::value_type& vt
, fBuildOption_t& fBuildOption
, fOptionEngineAction_t&& fOptionEngineStart
, fOptionEngineAction_t&& fOptionEngineStop
)
: wxGridTableBase()
, m_pOptionChainModel_impl( std::make_unique<OptionChainModel_impl>(  // conflicts with OptionOrderModel macros
    vt, fBuildOption, std::move( fOptionEngineStart ), std::move( fOptionEngineStop )
  ) )
{
}

OptionChainModel::~OptionChainModel() {
  m_pOptionChainModel_impl.reset();
}

void OptionChainModel::SetView( wxGrid* pGrid ) {
  m_pOptionChainModel_impl->SetView( pGrid );
  wxGridTableBase::SetView( pGrid );
}

void OptionChainModel::HandleTimer( int top_row, int visible_row_count ) {
  m_pOptionChainModel_impl->HandleTimer( top_row, visible_row_count );
}

int OptionChainModel::ClosestStrike( double price ) const {
  return m_pOptionChainModel_impl->ClosestStrike( price );
}

void OptionChainModel::OptionSelected( int row, int col ) {
  m_pOptionChainModel_impl->OptionSelected( row, col );
}

wxString OptionChainModel::GetValue( int row, int col	) {
  return m_pOptionChainModel_impl->GetValue( row, col );
}

int OptionChainModel::GetNumberRows() {
  return m_pOptionChainModel_impl->GetNumberRows();
}

int OptionChainModel::GetNumberCols() {
  return m_pOptionChainModel_impl->GetNumberCols();
}

void OptionChainModel::SetValue( int row, int col, const wxString& s ) {
  m_pOptionChainModel_impl->SetValue( row, col, s );
}

void OptionChainModel::Clear() {
  m_pOptionChainModel_impl->Clear();
}

bool OptionChainModel::IsEmptyCell( int row, int col ) {
  return m_pOptionChainModel_impl->IsEmptyCell( row, col );
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
// wxGridStringTable::InsertRows
bool OptionChainModel::InsertRows( size_t pos, size_t numRows ) {
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

bool OptionChainModel::AppendRows( size_t numRows ) {
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

wxString OptionChainModel::GetColLabelValue( int col ) {
  return m_pOptionChainModel_impl->GetColLabelValue( col );
}

wxGridCellAttr* OptionChainModel::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {
  return m_pOptionChainModel_impl->GetAttr( row, col, kind );
}

