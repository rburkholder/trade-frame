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

ModelBase::ModelBase(void): wxDataViewModel() {
}

ModelBase::~ModelBase(void) {
}

bool ModelBase::IsContainer(	const wxDataViewItem&	item ) const {
  return item.IsOk();
}

wxDataViewItem ModelBase::GetParent( const wxDataViewItem&	item ) const {
  //return wxDataViewItem(NULL);
  return itemNull;
}

unsigned int ModelBase::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {
  // called when clicking on plus
  //children.
  return 0;
}

unsigned int ModelBase::GetColumnCount( void ) const {
  return m_vColumnNames.size();
}

wxString ModelBase::GetColumnType( unsigned int	col ) const {
  return "string";
}

void ModelBase::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  //wxVariant v(_asString(item));
  wxString s( "test" );
  variant = s;
}

bool ModelBase::SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int col	) {
  //this->
  return true;
}
