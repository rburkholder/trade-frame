/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// 2013/08/05 moved content from ModelBase.h

#pragma once

#include <boost/shared_ptr.hpp>

#include <wx/dataview.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

enum ETypes { eUnknown=0, ePortfolioMaster, ePortfolioCurrency, ePortfolio, ePosition, eOrder, eExecution };

struct DataViewItemBase: public wxDataViewItem {
  DataViewItemBase( void* p, DataViewItemBase* pParent_ = 0 ): wxDataViewItem( p ), pParent( pParent_ ) {};
  ~DataViewItemBase( void ) {};
  virtual void AssignFirstColumn( wxVariant& variant ) const {};  // for getting polymorphic stuff for the tree
  virtual bool IsContainer( void ) { return false; };
  ETypes ixTypes;
  DataViewItemBase* pParent;
};

template<class T> 
struct DataViewItem: public DataViewItemBase {
  typedef boost::shared_ptr<T> shared_ptr;
  DataViewItem( shared_ptr& ptr )
    : m_ptr( ptr ), DataViewItemBase( reinterpret_cast<void*>( ptr.get() ) ) {}; // use object directly as ptrs come and go
  DataViewItem( const DataViewItem& item ): m_ptr( item.m_ptr ), DataViewItemBase( item ) {};
  ~DataViewItem( void ) {};
  virtual void AssignFirstColumn( wxVariant& variant ) const {};
  T* Value( void ) const { return reinterpret_cast<T*>( wxDataViewItem::GetID() ); };
  shared_ptr Get() { return m_ptr; };
  shared_ptr m_ptr;  // this is required for reference counting to the original shared item
};

} // namespace tf
} // namespace ou
