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

#include "Common.hpp"

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

  void HandleTimer( wxDataViewItem, int );

protected:
private:

  mapChains_t::value_type& m_vt;

  fBuildOption_t& m_fBuildOption;

  struct Strike {
    size_t nWatching;
    bool bUpdated;  // used by timed scan to generate event to control
    double strike;
    chain_t::strike_t& options;
    fBuildOption_t& fBuildOption;

    Strike( double strike_, chain_t::strike_t& options_, fBuildOption_t& fBuildOption_ )
    : strike( strike_ ), options( options_ ), fBuildOption( fBuildOption_ )
    , nWatching( 0 ), bUpdated( false )
    {}

    Strike( const Strike& rhs )
    : strike( rhs.strike ), options( rhs.options ), fBuildOption( rhs.fBuildOption )
    , nWatching( rhs.nWatching ), bUpdated( rhs.bUpdated )
    {}

    bool IsWatching() const { return 0 < nWatching; }

    void Start() {
      if ( 0 == nWatching ) {
        if ( options.call.pInstrument ) {
          if ( !options.call.pOption ) {
            options.call.pOption = fBuildOption( options.call.pInstrument );
          }
          options.call.pOption->OnQuote.Add( MakeDelegate( this, &Strike::HandleQuote ) );
          //options.call.pOption->OnTrade.Add( MakeDelegate( this, &Strike::HandleTrade ) );
          options.call.pOption->StartWatch();
        }
        if ( options.put.pInstrument ) {
          if ( !options.put.pOption ) {
            options.put.pOption = fBuildOption( options.put.pInstrument );
          }
          options.put.pOption->OnQuote.Add( MakeDelegate( this, &Strike::HandleQuote ) );
          //options.put.pOption->OnTrade.Add( MakeDelegate( this, &Strike::HandleTrade ) );
          options.put.pOption->StartWatch();
        }
      }
      ++nWatching;
    }

    void HandleQuote( const ou::tf::Quote& quote ) {
      bUpdated = true; // todo: confirm with change in value of bid/ask
    }

    void HandleTrade( const ou::tf::Trade& trade ) {
    }

    bool Updated() {
      bool b( bUpdated );
      bUpdated = false;
      return b;
    }

    void Stop() {
      if ( 0 < nWatching ) {
        if ( 1 == nWatching ) {
          bUpdated = false;
          if ( options.call.pInstrument ) {
            if ( options.call.pOption ) {
              options.call.pOption->StopWatch();
              options.call.pOption->OnQuote.Remove( MakeDelegate( this, &Strike::HandleQuote ) );
              //options.call.pOption->OnTrade.Remove( MakeDelegate( this, &Strike::HandleTrade ) );
            }
          }
          if ( options.put.pInstrument ) {
            if ( options.put.pOption ) {
              options.put.pOption->StopWatch();
              options.put.pOption->OnQuote.Remove( MakeDelegate( this, &Strike::HandleQuote ) );
              //options.put.pOption->OnTrade.Remove( MakeDelegate( this, &Strike::HandleTrade ) );
            }
          }
        }
        --nWatching;
      }
    }

    ~Strike() {
      Stop();
      assert( 0 == nWatching );
    }
  };

  int m_ixFirst, m_ixLast;

  using vRow2Entry_t = std::vector<Strike>;
  vRow2Entry_t m_vRow2Entry;

};