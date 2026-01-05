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

OptionChainModel::OptionChainModel(
  mapChains_t::value_type& vt
, fBuildOption_t& fBuildOption
, fOptionEngineAction_t&& fOptionEngineStart
, fOptionEngineAction_t&& fOptionEngineStop
)
: wxGridTableBase()
, m_vt( vt )
, m_fBuildOption( fBuildOption )
, m_fOptionEngineStart( std::move( fOptionEngineStart ) )
, m_fOptionEngineStop( std::move( fOptionEngineStop ) )
, m_ixFirst( -1 ), m_ixLast( -1 )
{
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel constructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  vRow2Entry_t vRow2Entry; // temporary use for strike reversal
  m_vt.second.Strikes(
    [this,&vRow2Entry]( double strike, chain_t::strike_t& entry ) {
      vRow2Entry.push_back( Strike( strike, entry, m_fBuildOption, m_fOptionEngineStart, m_fOptionEngineStop ) );
    } );
  for ( vRow2Entry_t::reverse_iterator iter = vRow2Entry.rbegin(); iter != vRow2Entry.rend(); ++iter ) {
    m_vRow2Entry.push_back( *iter );
  }
}

OptionChainModel::~OptionChainModel() {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel destructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  m_vRow2Entry.clear();
}

void OptionChainModel::SetView( wxGrid *grid ) {
  wxGridTableBase::SetView( grid );
}

wxGrid* OptionChainModel::GetView() const {
  return wxGridTableBase::GetView();
}

void OptionChainModel::HandleTimer( int top_row, int visible_row_count ) {
  if ( 0 < visible_row_count ) {

    const int ixFirst( top_row );
    const int ixLast_( ixFirst + visible_row_count - 1 );
    const int ixLast( std::min<int>( ixLast_, m_vRow2Entry.size() - 1 ) );

    if ( ( ixFirst != m_ixFirst ) || ( ixLast != m_ixLast ) ) {
      //BOOST_LOG_TRIVIAL(trace) << "timer " << ixFirst << "," << ixLast;
      // check for start on in view rows - first
      for ( int ix = ixFirst; ix <= ixLast; ++ix ) {
        m_vRow2Entry[ ix ].Start();
      }

      // check for stop for out of view rows - second
      if ( -1 != m_ixFirst ) {
        assert( -1 != m_ixLast );
        for ( int ix = m_ixFirst; ix <= m_ixLast; ++ix ) {
          m_vRow2Entry[ ix ].Stop();
        }
      }

      m_ixFirst = ixFirst;
      m_ixLast = ixLast;
    }

    // check for rows to update
    for ( int ix = m_ixFirst; ix <= m_ixLast; ++ix ) {
      if ( m_vRow2Entry[ ix ].Updated() ) {
        wxGrid* grid( GetView() );
        if ( grid ) {
          grid->ForceRefresh();
        }
      }
    }

  }
}

int OptionChainModel::ClosestStrike( double price ) const {
  const double strike = m_vt.second.Atm( price );
  // todo: brute force this for now, create a comparator later
  size_t ix {};
  vRow2Entry_t::const_iterator iter( m_vRow2Entry.begin() );
  while ( strike < iter->strike ) {
    if ( m_vRow2Entry.end() == iter ) break;
    ++ix;
    ++iter;
  }
  //wxDataViewItem item = GetItem( ix );
  return ix;
}

void OptionChainModel::OptionSelected( int row, int col ) {

  assert( m_vRow2Entry.size() > row );
  Strike& strike( m_vRow2Entry[ row ] );

  std::string name;

  switch ( col ) {
  case OptionChainModel::col_CallAsk:
  case OptionChainModel::col_CallBid:
    name = strike.options.call.sIQFeedSymbolName;
    break;
  case OptionChainModel::col_PutAsk:
  case OptionChainModel::col_PutBid:
    name = strike.options.put.sIQFeedSymbolName;
    break;
  default:
    break;
  }

  BOOST_LOG_TRIVIAL(trace)
    << "strike " << strike.strike << ": " << name
    ;
}

wxString OptionChainModel::GetValue( int row, int col	) {

  static const std::string fmtDecimal( "{:d}" );
  static const std::string fmtFixed( "{:.{}f}" );

  const vRow2Entry_t::value_type& vt( m_vRow2Entry[ row ] );
  std::string response;
  switch ( col ) {
    case col_CallOi:
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtDecimal, vt.options.call.pOption->GetSummary().nOpenInterest );
      }
      break;
    case col_CallIV:
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtFixed, vt.options.call.pOption->LastGreek().ImpliedVolatility() * 100.0, 3 );
      }
      break;
    case col_CallDelta:
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtFixed, vt.options.call.pOption->LastGreek().Delta(), 3 );
      }
      break;
    case col_CallGamma:
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtFixed, vt.options.call.pOption->LastGreek().Gamma(), 4 );
      }
      break;
    case col_CallBid: // call bid
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtFixed, vt.options.call.pOption->LastQuote().Bid(), 2 );
      }
      break;
    case col_CallAsk: // call ask
      if ( vt.options.call.pOption ) {
        response = fmt::format( fmtFixed, vt.options.call.pOption->LastQuote().Ask(), 2 );
      }
      break;
    case col_Strike:  // strike
      response = fmt::format( fmtFixed, vt.strike, 2 );
      break;
    case col_PutBid: // put bid
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtFixed, vt.options.put.pOption->LastQuote().Bid(), 2 );
      }
      break;
    case col_PutAsk: // put ask
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtFixed, vt.options.put.pOption->LastQuote().Ask(), 2 );
      }
      break;
    case col_PutDelta:
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtFixed, vt.options.put.pOption->LastGreek().Delta(), 3 );
      }
      break;
    case col_PutGamma:
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtFixed, vt.options.put.pOption->LastGreek().Gamma(), 4 );
      }
      break;
    case col_PutIV:
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtFixed, vt.options.put.pOption->LastGreek().ImpliedVolatility() * 100.0, 3 );
      }
      break;
    case col_PutOi:
      if ( vt.options.put.pOption ) {
        response = fmt::format( fmtDecimal, vt.options.put.pOption->GetSummary().nOpenInterest );
      }
      break;
  }

  return response;
}

int OptionChainModel::GetNumberRows() {
  return m_vt.second.Size();
}

int OptionChainModel::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

void OptionChainModel::SetValue( int row, int col, const wxString& ) {
  assert( false );  // not sure if this is used
}

void OptionChainModel::Clear() {
}

bool OptionChainModel::IsEmptyCell( int row, int col ) {
  return false;
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

bool OptionChainModel::DeleteRows( size_t pos, size_t numRows ) {
  return true;
}

bool OptionChainModel::InsertCols( size_t pos, size_t numCols ) {
  return true;
}

bool OptionChainModel::AppendCols( size_t numCols ) {
  return true;
}

bool OptionChainModel::DeleteCols( size_t pos, size_t numCols ) {
  return true;
}

wxString OptionChainModel::GetColLabelValue( int col ) {

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

wxGridCellAttr* OptionChainModel::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

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
        //case COL_Quan:
        //case COL_Price:
        //  switch ( col ) {
        //    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColAlign, 0 )
        //  }
        //  pAttr->SetReadOnly( false );
        //  break;
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

