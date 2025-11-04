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
  static const char* rColumnLabels[] = {
    "dt"
  , "bid vol"
  , "bid prc"
  , "trd vol"
  , "trd prc"
  , "ask vol"
  , "ask prc"
  };
  static const std::string fmtVolume( "{}" );
  static const std::string fmtPrice( "{:.{}f}" );
}

TimeSeriesModel::TimeSeriesModel()
: wxGridTableBase()
, m_pQuotes( nullptr )
, m_pTrades( nullptr )
, m_pGrid( nullptr )
, m_nRows {}
{
}

TimeSeriesModel::~TimeSeriesModel() {
}

void TimeSeriesModel::Set( const ou::tf::Quotes* pQuotes, const ou::tf::Trades* pTrades ) {
  m_pQuotes = pQuotes;
  m_pTrades = pTrades;
}

// GoToCell()
// IsVisible()
// MakeCellVisible()
// GetFirstFullyVisibleRow()
// CalcCellsExposed (
// GetDefaultRowSize ()
// GetRowMinimalAcceptableHeight ()

void TimeSeriesModel::UpdateDateTime( const boost::posix_time::ptime dt ) {
  if ( m_pGrid ) {
    const auto size = m_pQuotes->Size();
    if ( 0 < size ) {
      m_pGrid->BeginBatch();

      m_vDatum.clear();
      auto iterQuotes = m_pQuotes->AtOrAfter( dt );
      auto iterTrades = m_pTrades->AtOrAfter( dt );

      unsigned int ix {};
      while ( ix < m_nRows ) {
        if ( m_pQuotes->end() == iterQuotes ) break;
        if ( m_pTrades->end() == iterTrades ) break;
        if ( iterQuotes->DateTime() <= iterTrades->DateTime() ) {
          m_vDatum.push_back( *iterQuotes );
          ++iterQuotes;
        }
        else {
          m_vDatum.push_back( *iterTrades );
          ++ iterTrades;
        }
        ++ix;
      }

      m_pGrid->ClearGrid();
      m_pGrid->EndBatch();
    }
  }
}

int TimeSeriesModel::GetNumberRows() {
  return m_nRows;
}

int TimeSeriesModel::GetNumberCols() {
  return EColId::_col_id_count;
}

wxString TimeSeriesModel::GetColLabelValue( int col ) {
  return wxString( rColumnLabels[ col ] );
}

bool TimeSeriesModel::AppendRows( size_t numRows ) {
  m_nRows += numRows;
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
  m_nRows += numRows;
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

bool TimeSeriesModel::DeleteRows( size_t pos, size_t numRows ) {
  assert( m_nRows >= numRows );
  m_nRows -= numRows;
  if ( m_pGrid ) {
    wxGridTableMessage msg(
      this,
      wxGRIDTABLE_NOTIFY_ROWS_DELETED,
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

bool TimeSeriesModel::IsEmptyCell( int row, int col ) {
  return ( row >= m_nRows );
}

wxString TimeSeriesModel::Datum( int col, const ou::tf::Quote& quote ) {
  wxString s;
  switch ( col ) {
    case EColId::dt:
      s = boost::posix_time::to_iso_string( quote.DateTime() ) ;
      break;
    case EColId::bid_vol:
      s = fmt::format( fmtVolume, quote.BidSize() );
      break;
    case EColId::bid_prc:
      s = fmt::format( fmtPrice, quote.Bid(), 2 );
      break;
    case EColId::trd_vol:
      break;
    case EColId::trd_prc:
      break;
    case EColId::ask_vol:
      s = fmt::format( fmtVolume, quote.AskSize() );
      break;
    case EColId::ask_prc:
      s = fmt::format( fmtPrice, quote.Ask(), 2 );
      break;
    default:
      break;
  }
  return s;
}

wxString TimeSeriesModel::Datum( int col, const ou::tf::Trade& trade ) {
  wxString s;
  switch ( col ) {
    case EColId::dt:
      s = boost::posix_time::to_iso_string( trade.DateTime() ) ;
      break;
    case EColId::bid_vol:
      break;
    case EColId::bid_prc:
      break;
    case EColId::trd_vol:
      s = fmt::format( fmtVolume, trade.Volume() );
      break;
    case EColId::trd_prc:
      s = fmt::format( fmtPrice, trade.Price(), 3 );
      break;
    case EColId::ask_vol:
      break;
    case EColId::ask_prc:
      break;
    default:
      break;
  }
  return s;
}

wxString TimeSeriesModel::GetValue( int row, int col ) {

  wxString s;

  if ( row < m_vDatum.size() ) {
    const variantDatum_t datum( m_vDatum[ row ] );
    std::visit( [this,col,&s]( auto&& arg ){ s = Datum( col, arg ); }, datum );
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
        case EColId::trd_vol:
        case EColId::trd_prc:
          pAttr->SetTextColour( wxColour( 0, 255, 0 ) );
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

