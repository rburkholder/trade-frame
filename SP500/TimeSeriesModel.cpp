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
  , "imbal"
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

  static const wxColour colourBackgroundRed( 255,   0,   0, 30 );
  static const wxColour colourBackgroundGrn(   0, 255,   0, 30 );
  static const wxColour colourBackgroundBlu(   0,   0, 255, 30 );

  if ( m_pGrid ) {
    const auto size = m_pQuotes->Size();
    if ( 0 < size ) {
      m_pGrid->BeginBatch();

      m_vRow.clear();
      auto iterQuotes = m_pQuotes->AtOrAfter( dt );
      auto iterTrades = m_pTrades->AtOrAfter( dt );

      bool bQuoteFound( false );
      auto iterQuoteLast = m_pQuotes->end();
      bool bTradeFound( false );
      auto iterTradeLast = m_pTrades->end();

      unsigned int ix {};
      while ( ix < m_nRows ) {
        if ( m_pQuotes->end() == iterQuotes ) break;
        if ( m_pTrades->end() == iterTrades ) break;
        if ( iterQuotes->DateTime() <= iterTrades->DateTime() ) {
          const ou::tf::Quote& quote( *iterQuotes );
          Row row;
          row.sDateTime = boost::posix_time::to_iso_string( quote.DateTime() );
          row.sBidVol = fmt::format( fmtVolume, quote.BidSize() );
          row.sBidPrice = fmt::format( fmtPrice, quote.Bid(), 2 );
          row.sAskVol = fmt::format( fmtVolume, quote.AskSize() );
          row.sAskPrice = fmt::format( fmtPrice, quote.Ask(), 2 );
          auto imbalance = quote.Imbalance();
          row.sImbalance = fmt::format( fmtPrice, imbalance, 3 );
          if ( 0.0 == imbalance ) {}
          else {
            if ( 0.0 < imbalance ) {
              row.colourImbalance = colourBackgroundBlu;
            }
            else {
              row.colourImbalance = colourBackgroundRed;
            }
          }
          m_vRow.emplace_back( std::move( row ) );
          iterQuoteLast = iterQuotes;
          bQuoteFound = true;
          ++iterQuotes;
        }
        else {
          const ou::tf::Trade& trade( *iterTrades );
          const double price( trade.Price() );
          Row row;
          row.sDateTime = boost::posix_time::to_iso_string( trade.DateTime() );
          row.sTrdVol = fmt::format( fmtVolume, trade.Volume() );
          row.sTrdPrice = fmt::format( fmtPrice, price, 3 );
          if ( bQuoteFound ) {
            bool result = iterQuoteLast->LeeReady( bTradeFound ? iterTradeLast->Price() : price, price );
            row.colourTrdPrice = result ? colourBackgroundGrn : colourBackgroundRed;
          }
          m_vRow.emplace_back( std::move( row ) );
          iterTradeLast = iterTrades;
          bTradeFound = true;
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

wxString TimeSeriesModel::GetValue( int ixRow, int ixCol ) {

  wxString s;

  if ( ixRow < m_vRow.size() ) {
    const Row& row( m_vRow.at( ixRow ) );
    switch ( ixCol ) {
      case EColId::dt:
        return row.sDateTime;
        break;
      case EColId::bid_vol:
        return row.sBidVol;
        break;
      case EColId::bid_prc:
        return row.sBidPrice;
        break;
      case EColId::trd_vol:
        return row.sTrdVol;
        break;
      case EColId::trd_prc:
        return row.sTrdPrice;
        break;
      case EColId::ask_vol:
        return row.sAskVol;
        break;
      case EColId::ask_prc:
        return row.sAskPrice;
        break;
      case EColId::imbalance:
        return row.sImbalance;
        break;
      default:
        break;
    }
  }

  return s;
}

void TimeSeriesModel::SetValue( int row, int col, const wxString &value ) {
}

wxGridCellAttr* TimeSeriesModel::GetAttr( int ixRow, int ixCol, wxGridCellAttr::wxAttrKind kind ) {

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  if ( ixRow < m_vRow.size() ) {
    const Row& row( m_vRow.at( ixRow ) );
    switch ( kind ) {
      case wxGridCellAttr::wxAttrKind::Any:
      case wxGridCellAttr::wxAttrKind::Cell:
      case wxGridCellAttr::wxAttrKind::Col:
        switch ( ixCol ) {
          case EColId::dt:
            pAttr->SetAlignment( wxALIGN_LEFT, wxALIGN_CENTER_VERTICAL );
            break;
          case EColId::ask_vol:
          case EColId::ask_prc:
            pAttr->SetTextColour( wxColour( 255, 0, 40 ) );
            pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
            break;
          case EColId::trd_vol:
          case EColId::trd_prc:
            //pAttr->SetTextColour( wxColour( 0, 255, 40 ) );
            pAttr->SetBackgroundColour( row.colourTrdPrice );
            pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
            break;
          case EColId::bid_vol:
          case EColId::bid_prc:
            pAttr->SetTextColour( wxColour( 0, 40, 255 ) );
            pAttr->SetAlignment( wxALIGN_RIGHT, wxALIGN_CENTER_VERTICAL );
            break;
          case EColId::imbalance:
            pAttr->SetBackgroundColour( row.colourImbalance );
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
  }

  pAttr->SetReadOnly();

  return pAttr;
}

