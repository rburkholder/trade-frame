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
 * File:    OptionChainModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:59
 */

#pragma once

#include <vector>

#include <wx/dataview.h>

#include "Chains.hpp"

class OptionChainModel
: public wxDataViewVirtualListModel
{
public:

  OptionChainModel( mapChains_t::value_type& );
  ~OptionChainModel();

  virtual bool IsContainer( const wxDataViewItem& item	) const;
  virtual bool IsEnabled( const wxDataViewItem& item, unsigned int col ) const;
  virtual bool HasValue ( const wxDataViewItem& item, unsigned col) const;
  virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const;
  virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const;
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int	col	) const;
  virtual bool HasContainerColumns( const wxDataViewItem& item ) const;

  virtual void GetValueByRow( wxVariant &variant, unsigned int row, unsigned int col ) const;
  virtual bool SetValueByRow( const wxVariant &variant, unsigned int row, unsigned int col );

  unsigned int GetCount() const;

protected:
private:

  mapChains_t::value_type& m_vt;

  struct Strike {
    double strike;
    const chain_t::strike_t& options;
    Strike( double strike_, const chain_t::strike_t& options_ ): strike( strike_ ), options( options_ ) {}
    Strike( const Strike& rhs ): strike( rhs.strike ), options( rhs.options ) {}
  };

  using vRow2Entry_t = std::vector<Strike>;
  vRow2Entry_t m_vRow2Entry;

};