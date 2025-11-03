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
 * File:    TimeSeriesModel.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 1, 2025 19:12:28
 */

// todo: use GetDefaultRowSize 	() with window hiehgt to find number of rows

#define FMT_HEADER_ONLY 1
#include <fmt/core.h>

//#include "OUCommon/Colour.h"

#include "TimeSeriesModel.hpp"

namespace {
  static const size_t c_nRows( 20 );
  static const char* rColumnLabels[] = {
    "dt"
  , "bid vol"
  , "bid prc"
  , "trd vol"
  , "trd prc"
  , "ask vol"
  , "ask prc"
  };
}

TimeSeriesModel::TimeSeriesModel()
: wxGridTableBase()
, m_pQuotes( nullptr )
, m_pTrades( nullptr )
, m_pGrid( nullptr )
, m_bIteratorAvailable( false )
{
}

TimeSeriesModel::~TimeSeriesModel() {
}

void TimeSeriesModel::Set( const ou::tf::Quotes* pQuotes, const ou::tf::Trades* pTrades ) {
  m_pQuotes = pQuotes;
  m_pTrades = pTrades;
}

void TimeSeriesModel::UpdateDateTime( const boost::posix_time::ptime dt ) {
  if ( m_pGrid ) {
    //const auto rows = m_pGrid->GetNumberRows();
    m_pGrid->ClearGrid();
    //m_pGrid->ForceRefresh();
    const auto size = m_pQuotes->Size();
    auto iter = m_pQuotes->AtOrAfter( dt );
    m_iterQuotes = iter;
    m_bIteratorAvailable = true;
  }
}

int TimeSeriesModel::GetNumberRows() {
  int nRows( c_nRows );
  if ( m_pQuotes ) nRows = m_pQuotes->Size();
  return nRows;
}

int TimeSeriesModel::GetNumberCols() {
  return EColId::_col_id_count;
}

wxString TimeSeriesModel::GetColLabelValue( int col ) {
  return wxString( rColumnLabels[ col ] );
}

bool TimeSeriesModel::AppendRows( size_t numRows ) {
  if ( m_pGrid ) {
    wxGridTableMessage msg(
      this,
      wxGRIDTABLE_NOTIFY_ROWS_APPENDED,
      numRows
    );
    GetView()->ProcessTableMessage( msg );
  }
return true;
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
bool TimeSeriesModel::InsertRows( size_t pos, size_t numRows ) {
  if ( m_pGrid ) {
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

void TimeSeriesModel::SetView( wxGrid *pGrid ) {
  wxGridTableBase::SetView( pGrid );
  m_pGrid = pGrid;
}

//wxGrid* TimeSeriesModel::GetView() const {
//}

//bool TimeSeriesModel::CanHaveAttributes() {
//  return false;
//}

//bool TimeSeriesModel::CanMeasureColUsingSameAttr( int col ) const {
//  return false;
//}

bool TimeSeriesModel::IsEmptyCell( int row, int col ) {
  return ( row >= c_nRows );
}

wxString TimeSeriesModel::GetValue( int row, int col ) {
  wxString s;
  if ( m_bIteratorAvailable ) {
    ou::tf::Quotes::const_iterator iter = m_iterQuotes + row;
    const ou::tf::Quote& quote( *iter );
    switch ( col ) {
      case EColId::dt:
        s = boost::posix_time::to_iso_string( quote.DateTime() ) ;
        break;
      case EColId::bid_vol:
        s = fmt::format( "{}", quote.BidSize() );
        break;
      case EColId::bid_prc:
        s = fmt::format( "{:.{}f}", quote.Bid(), 2 );
        break;
      case EColId::trd_vol:
        //s = fmt::format( "{}", quote.BidSize() );
        break;
      case EColId::trd_prc:
        //s = fmt::format( "{}", quote.BidSize() );
        break;
      case EColId::ask_vol:
         s = fmt::format( "{}", quote.AskSize() );
       break;
      case EColId::ask_prc:
        s = fmt::format("{:.{}f}", quote.Ask(), 2 );
        break;
      default:
        break;
    }
  }
  return s;
}

void TimeSeriesModel::SetValue( int row, int col, const wxString &value ) {
}

wxGridCellAttr* TimeSeriesModel::GetAttr( int row, int col, wxGridCellAttr::wxAttrKind kind ) {

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  switch ( kind ) {
    case wxGridCellAttr::wxAttrKind::Any:
    case wxGridCellAttr::wxAttrKind::Cell:
    case wxGridCellAttr::wxAttrKind::Col:
      switch ( col ) {
        case EColId::dt:
          pAttr->SetAlignment( wxALIGN_LEFT, wxALIGN_CENTER_VERTICAL );
          break;
        case EColId::ask_vol:
        case EColId::ask_prc:
          pAttr->SetTextColour( wxColour( 255, 0, 0 ) );
          pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
          break;
        case EColId::bid_vol:
        case EColId::bid_prc:
          pAttr->SetTextColour( wxColour( 0, 0, 255 ) );
          pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
          break;
        default:
          pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
          break;
      }
      break;
    case wxGridCellAttr::wxAttrKind::Row:
      break;
    case wxGridCellAttr::wxAttrKind::Default:
      break;
    default:
      break;
  }

  pAttr->SetReadOnly();

  return pAttr;
}

