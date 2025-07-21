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
#include <boost/lexical_cast.hpp>

#include "Chains.hpp"

#include "OptionChainModel.hpp"

OptionChainModel::OptionChainModel( mapChains_t::value_type& vt )
: wxDataViewVirtualListModel( vt.second.Size() )
, m_vt( vt )
{
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel constructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  m_vt.second.Strikes(
    [this]( double strike, const chain_t::strike_t& entry ) {
      m_vRow2Entry.push_back( Strike( strike, entry ) );
    } );
}

OptionChainModel::~OptionChainModel() {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel destructed " << ou::tf::Instrument::BuildDate( m_vt.first );
}

bool OptionChainModel::IsContainer( const wxDataViewItem& item ) const {
  return false;
}

bool OptionChainModel::IsEnabled( const wxDataViewItem& item, unsigned int col ) const {
  return true;
}

wxDataViewItem OptionChainModel::GetParent( const wxDataViewItem& item ) const {
  return wxDataViewItem( nullptr );
}

unsigned int OptionChainModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const {
  return 0;
}

bool OptionChainModel::HasValue ( const wxDataViewItem& item, unsigned col ) const {
  //BOOST_LOG_TRIVIAL(trace) << "OptionChainModel HasValue " << col;
  return true;
}

void OptionChainModel::GetValue( wxVariant& value, const wxDataViewItem& item, unsigned int	col	) const {
  const auto row( GetRow( item ) );
  //std::string response( boost::lexical_cast<std::string>( row ) + " - " + boost::lexical_cast<std::string>( col ) );
  std::string response;
  if ( 4 == col ) {
    //BOOST_LOG_TRIVIAL(trace) << "OptionChainModel GetValue " << response;
    //m_vt.second.GetStrike( 10.0 ).
    //mapChains_t::iterator iter = m_vt.second.
    response = boost::lexical_cast<std::string>( m_vRow2Entry[ row ].strike );
    wxVariant variant;
  }
  wxVariant placeholder(
    response
  , "name col " + boost::lexical_cast<std::string>( col )
  );
  value = placeholder;
}

bool OptionChainModel::HasContainerColumns( const wxDataViewItem& item ) const {
  return false;
}

void OptionChainModel::GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel GetValueByRow";
}

bool OptionChainModel::SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col ) {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel SetValueByRow";
  return false;
}

unsigned int OptionChainModel::GetCount() const {
  return m_vt.second.Size();
}

