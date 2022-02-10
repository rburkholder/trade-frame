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

#include <memory>

#include <wx/dataview.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

enum EModelType { EMTUnknown=0,
  EMTPortfolioMaster, EMTPortfolioCurrency, EMTPortfolio,
  EMTPosition, EMTOrder, EMTExecution, EMTModelTypeCount };

// 2017/09/17: built a typed version, various flavours of DataVierwItemBase* will be placed here
template<class T>
struct wxDataViewItem_typed: public wxDataViewItem {
  wxDataViewItem_typed() {}
  wxDataViewItem_typed( const wxDataViewItem_typed& item ): wxDataViewItem( (void*) item ) {}
  wxDataViewItem_typed( T* p ): wxDataViewItem( p ) {}
  T* GetID() const { return reinterpret_cast<T*>( wxDataViewItem::GetID() ); }
};

class DataViewItemBase {
public:
  DataViewItemBase( EModelType eModelType, DataViewItemBase* pParent = nullptr )
    : m_pParent( pParent ), m_EModelType( eModelType ) {};
  DataViewItemBase( const DataViewItemBase& rhs )
    : m_EModelType( rhs.m_EModelType ), m_pParent( rhs.m_pParent ) {};
  virtual ~DataViewItemBase( void ) {};

  // get the const back again?
  virtual void AssignFirstColumn( wxVariant& variant ) /* const */ {};  // for getting polymorphic stuff for the tree
  virtual bool IsContainer( void ) const {
    return false;
  };
  EModelType GetModelType() const { return m_EModelType; }
  DataViewItemBase* GetParent() const { return m_pParent; }
  void SetParent( DataViewItemBase* pParent ) { m_pParent = pParent; }
protected:
private:
  EModelType m_EModelType;
  DataViewItemBase* m_pParent; // should try to make this a shared_ptr, but may have issues with 'type'
};

template<class T>
class DataViewItem: public DataViewItemBase {
public:

  using shared_ptr = std::shared_ptr<T>;

  DataViewItem( EModelType eModelType, shared_ptr& ptr, DataViewItemBase* pParent = nullptr )
    : DataViewItemBase( eModelType, pParent ), m_ptr( ptr ) {};
  DataViewItem( const DataViewItem& rhs )
    : DataViewItemBase( rhs ), m_ptr( rhs.m_ptr ) {};
  virtual ~DataViewItem( void ) {};

  shared_ptr GetPtr() { return m_ptr; };

  //virtual void AssignFirstColumn( wxVariant& variant ) const {};

  //T* Value( void ) const { return reinterpret_cast<T*>( wxDataViewItem::GetID() ); };

private:
  shared_ptr m_ptr;
};

} // namespace tf
} // namespace ou
