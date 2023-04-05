/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>
#include <vector>

#include <boost/range/algorithm/for_each.hpp>

#include <wx/dataview.h>

#include "DataViewItem.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

// migrate to CRTP concept
template<class T> // T inheriting class, M is map type
class ModelBase: public wxDataViewModel {
public:

  ModelBase(): wxDataViewModel() {};
  virtual ~ModelBase() {};

  template<class F>
  void IterateColumnNames( F f ) {
    boost::for_each( m_vColumnNames, f );
  }

  // can convert from virtual to CRTP type calls.
  virtual bool IsContainer( const wxDataViewItem& item ) const {
    assert( false );  // need to change this.
    return false;
  };
  virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const {
    assert( false );
    return wxDataViewItem( nullptr );
  };
//  virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const { assert( 0 ); return 0; }; // called when clicking on plus
  virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const {
    //assert( 0 );
    return 0;
  }; // called when clicking on plus
  virtual unsigned int GetColumnCount( void ) const {
    return m_vColumnNames.size();
  };
  virtual wxString GetColumnType( unsigned intcol ) const {
    return "string";
  };
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const {
    assert( false );
  };
  virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col ) {
    return false;
  };

  virtual void ClearItems() {};

protected:

  using vString_t = std::vector<std::string>;

  //static wxDataViewItem m_itemNull;

  vString_t m_vColumnNames;

private:

};

//template<class T>
//wxDataViewItem ModelBase<T>::m_itemNull;

} // namespace tf
} // namespace ou

