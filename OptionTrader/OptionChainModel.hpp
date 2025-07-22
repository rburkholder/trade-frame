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

// todo:
//   need top row and visible rows from the ctrl
//   need to obtain option and set watch when visible

class OptionChainModel
: public wxDataViewVirtualListModel
{
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fBuildOption_t = std::function<pOption_t( pInstrument_t )>;

  OptionChainModel( mapChains_t::value_type&, fBuildOption_t& );
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

  fBuildOption_t& m_fBuildOption;

  struct Strike {
    bool bWatching;
    double strike;
    chain_t::strike_t& options;
    fBuildOption_t& fBuildOption;

    Strike( double strike_, chain_t::strike_t& options_, fBuildOption_t& fBuildOption_ )
    : strike( strike_ ), options( options_ ), bWatching( false ), fBuildOption( fBuildOption_ )
    {}

    Strike( const Strike& rhs )
    : strike( rhs.strike ), options( rhs.options ), bWatching( false ), fBuildOption( rhs.fBuildOption )
    {}

    bool IsWatching() const { return bWatching; }

    void Start() {
      if ( !bWatching ) {
        bWatching = true;
        if ( options.call.pInstrument ) {
          if ( !options.call.pOption ) {
            options.call.pOption = fBuildOption( options.call.pInstrument );
            // todo: start watch, and batched notification
          }
        }
        if ( options.put.pInstrument ) {
          if ( !options.put.pOption ) {
            options.put.pOption = fBuildOption( options.put.pInstrument );
            // todo: start watch, and batched notification
          }
        }
      }
    }

    void Stop() {
      if ( bWatching ) {
        bWatching = false;
      }
    }

    ~Strike() {
      Stop();
    }
  };

  using vRow2Entry_t = std::vector<Strike>;
  vRow2Entry_t m_vRow2Entry;

};