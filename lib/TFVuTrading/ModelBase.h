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

#include <boost/shared_ptr.hpp>

#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <wx/dataview.h>

#include "DataViewItem.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelBase: public wxDataViewModel {
public:

  ModelBase(void);
  ~ModelBase(void);

  template<class F> 
  void IterateColumnNames( F f ) {
    boost::for_each( m_vColumnNames, f );
  }

  virtual bool IsContainer(	const wxDataViewItem&	item ) const;
  virtual wxDataViewItem GetParent( const wxDataViewItem&	item ) const;
  virtual unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  virtual unsigned int GetColumnCount( void ) const;
  virtual wxString GetColumnType( unsigned int	col ) const;
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;
  virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col	);

protected:

  typedef std::vector<std::string> vString_t;

  static wxDataViewItem m_itemNull;

  vString_t m_vColumnNames;

private:
  
};

} // namespace tf
} // namespace ou
