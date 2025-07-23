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

OptionChainModel::OptionChainModel( mapChains_t::value_type& vt, fBuildOption_t& fBuildOption )
: wxDataViewVirtualListModel( vt.second.Size() )
, m_vt( vt )
, m_fBuildOption( fBuildOption )
, m_ixFirst( -1 ), m_ixLast( -1 )
{
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel constructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  vRow2Entry_t vRow2Entry; // temporary use for strike reversal
  m_vt.second.Strikes(
    [this,&vRow2Entry]( double strike, chain_t::strike_t& entry ) {
      vRow2Entry.push_back( Strike( strike, entry, m_fBuildOption ) );
    } );
  for ( vRow2Entry_t::reverse_iterator iter = vRow2Entry.rbegin(); iter != vRow2Entry.rend(); ++iter ) {
    m_vRow2Entry.push_back( *iter );
  }
}

OptionChainModel::~OptionChainModel() {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel destructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  m_vRow2Entry.clear();
}

void OptionChainModel::HandleTimer( wxDataViewItem dviTopItem, int nRows ) {
  if ( 0 < nRows ) {

    const int ixFirst( GetRow( dviTopItem ) );
    const int ixLast_( ixFirst + nRows - 1 );
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
        RowChanged( ix );
      }
    }

  }
}

bool OptionChainModel::IsContainer( const wxDataViewItem& item ) const {
  return false; // not called
}

bool OptionChainModel::IsEnabled( const wxDataViewItem& item, unsigned int col ) const {
  const auto row( GetRow( item ) );
  return row < m_vRow2Entry.size();  // is called
}

wxDataViewItem OptionChainModel::GetParent( const wxDataViewItem& item ) const {
  return wxDataViewItem( nullptr ); // not called
}

unsigned int OptionChainModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const {
  return 0; // not called
}

bool OptionChainModel::HasValue ( const wxDataViewItem& item, unsigned col ) const {
  const auto row( GetRow( item ) );
  return row < m_vRow2Entry.size();  // is called
}

void OptionChainModel::GetValue( wxVariant& value, const wxDataViewItem& item, unsigned int	col	) const {
  const auto row( GetRow( item ) );
  const vRow2Entry_t::value_type& vt( m_vRow2Entry[ row ] );
  std::string response;
  switch ( col ) {
    case c_oi:
      if ( vt.options.call.pOption ) {
        response = fmt::format( "{:d}", vt.options.call.pOption->GetSummary().nOpenInterest );
      }
      break;
    case c_bid: // call bid
      if ( vt.options.call.pOption ) {
        response = fmt::format( "{:.{}f}", vt.options.call.pOption->LastQuote().Bid(), 2 );
      }
      break;
    case c_ask: // call ask
      if ( vt.options.call.pOption ) {
        response = fmt::format( "{:.{}f}", vt.options.call.pOption->LastQuote().Ask(), 2 );
      }
      break;
    case strike:  // strike
      response = fmt::format( "{:.{}f}", vt.strike, 2 );
      break;
    case p_bid: // put bid
      if ( vt.options.put.pOption ) {
        response = fmt::format( "{:.{}f}", vt.options.put.pOption->LastQuote().Bid(), 2 );
      }
      break;
    case p_ask: // put ask
      if ( vt.options.put.pOption ) {
        response = fmt::format( "{:.{}f}", vt.options.put.pOption->LastQuote().Ask(), 2 );
      }
      break;
    case p_oi:
      if ( vt.options.put.pOption ) {
        response = fmt::format( "{:d}", vt.options.put.pOption->GetSummary().nOpenInterest );
      }
      break;
  }
  wxVariant placeholder(
    response
  , boost::lexical_cast<std::string>( col )
  );
  value = placeholder;
}

bool OptionChainModel::HasContainerColumns( const wxDataViewItem& item ) const {
  return false; // not called
}

void OptionChainModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel GetValueByRow"; // not called
}

bool OptionChainModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col ) {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel SetValueByRow"; // not called
  return false;
}

unsigned int OptionChainModel::GetCount() const {
  return m_vt.second.Size();
}
