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

OptionChainModel::OptionChainModel( mapChains_t::value_type& vt, fBuildOption_t& fBuildOption )
: wxDataViewVirtualListModel( vt.second.Size() )
, m_vt( vt )
, m_fBuildOption( fBuildOption )
{
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel constructed " << ou::tf::Instrument::BuildDate( m_vt.first );
  m_vt.second.Strikes(
    [this]( double strike, chain_t::strike_t& entry ) {
      m_vRow2Entry.push_back( Strike( strike, entry, m_fBuildOption ) );
    } );
}

OptionChainModel::~OptionChainModel() {
  BOOST_LOG_TRIVIAL(trace) << "OptionChainModel destructed " << ou::tf::Instrument::BuildDate( m_vt.first );
}

bool OptionChainModel::IsContainer( const wxDataViewItem& item ) const {
  return false; // not called
}

bool OptionChainModel::IsEnabled( const wxDataViewItem& item, unsigned int col ) const {
  return true;  // is called
}

wxDataViewItem OptionChainModel::GetParent( const wxDataViewItem& item ) const {
  return wxDataViewItem( nullptr ); // not called
}

unsigned int OptionChainModel::GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const {
  return 0; // not called
}

bool OptionChainModel::HasValue ( const wxDataViewItem& item, unsigned col ) const {
  bool bHasValue( 4 == col );  // simple for onw
  return bHasValue;
}

void OptionChainModel::GetValue( wxVariant& value, const wxDataViewItem& item, unsigned int	col	) const {
  const auto row( GetRow( item ) );
  //std::string response( boost::lexical_cast<std::string>( row ) + " - " + boost::lexical_cast<std::string>( col ) );
  std::string response;
  if ( 4 == col ) {
    response = boost::lexical_cast<std::string>( m_vRow2Entry[ row ].strike );
    wxVariant variant;
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
