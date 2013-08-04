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

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelBase: public wxDataViewModel {
public:

  ModelBase(void);
  ~ModelBase(void);

  template<class F> void IterateColumnNames( F );

  struct DataViewItemBase: public wxDataViewItem {
    DataViewItemBase( void* p ): wxDataViewItem( p ) {};
    ~DataViewItemBase( void ) {};
    virtual void GetFirstColumn( wxVariant& variant ) const {};  // for getting polymorphic stuff for the tree
  };

  template<class T> struct DataViewItem: public DataViewItemBase {
    typedef boost::shared_ptr<T> shared_ptr;
    DataViewItem( shared_ptr& ptr )
      : m_ptr( ptr ), DataViewItemBase( reinterpret_cast<void*>( ptr.get() ) ) {}; // use object directly as ptrs come and go
    DataViewItem( const DataViewItem& item ): m_ptr( item.m_ptr ), DataViewItemBase( item ) {};
    ~DataViewItem( void ) {};
    virtual void GetFirstColumn( wxVariant& variant ) const {};
    T* Value( void ) const { return reinterpret_cast<T*>( wxDataViewItem::GetID() ); };
    shared_ptr Get() { return m_ptr; };
    shared_ptr m_ptr;  // this is required for reference counting to the original shared item
  };

  virtual bool IsContainer(	const wxDataViewItem&	item ) const;
  wxDataViewItem GetParent( const wxDataViewItem&	item ) const;
  virtual unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  unsigned int GetColumnCount( void ) const;
  wxString GetColumnType( unsigned int	col ) const;
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;
  bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col	);

protected:

  typedef std::vector<std::string> vString_t;

  static wxDataViewItem m_itemNull;

  vString_t m_vColumnNames;

private:
  
};

template<class F> void ModelBase::IterateColumnNames( F f ) {
  boost::for_each( m_vColumnNames, f );
}

} // namespace tf
} // namespace ou
