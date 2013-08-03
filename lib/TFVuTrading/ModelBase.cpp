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

#include "StdAfx.h"

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

wxDataViewItem ModelBase::m_itemNull;

ModelBase::ModelBase(void): wxDataViewModel() {
}

ModelBase::~ModelBase(void) {
}

bool ModelBase::IsContainer(	const wxDataViewItem&	item ) const {  // makes as tree container, or regular field
//  return item.IsOk();
  return false;
}

wxDataViewItem ModelBase::GetParent( const wxDataViewItem&	item ) const {
  //return wxDataViewItem(NULL);
  return m_itemNull;// will need to generate something better than this once we get rolling  (good enough for list, but not for tree)
}

unsigned int ModelBase::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {
  // called when clicking on plus
  // needs over-ride
  assert( 0 );
  return 0;  // returns number of items, need to be able to search by item
}

unsigned int ModelBase::GetColumnCount( void ) const {
  return m_vColumnNames.size();
}

wxString ModelBase::GetColumnType( unsigned int	col ) const {
  return "string";
}

void ModelBase::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  //wxVariant v(_asString(item));
//  wxString s( "test" );
//  variant = s;
  assert( 0 );
}

bool ModelBase::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col	) {
  return true;
}

} // namespace tf
} // namespace ou
