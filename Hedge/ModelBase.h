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

#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>

#include <wx/dataview.h>

class ModelBase: public wxDataViewModel {
public:

  ModelBase(void);
  ~ModelBase(void);

  template<class F> void IterateColumnNames( F ) const;

  bool IsContainer(	const wxDataViewItem&	item ) const;
  wxDataViewItem GetParent( const wxDataViewItem&	item ) const;
  unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  unsigned int GetColumnCount( void ) const;
  wxString GetColumnType( unsigned int	col ) const;
  void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;
  bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col	);

protected:

  typedef std::vector<std::string> vString_t;

  vString_t m_vColumnNames;

private:
  
};

template<class F> void ModelBase::IterateColumnNames( F f ) const {
  boost::for_each( m_vColumnNames, f );
}
