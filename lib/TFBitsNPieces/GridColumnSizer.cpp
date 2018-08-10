/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   GridColumnSizer.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on August 9, 2018, 2:07 PM
 */

#include <exception>

#include "GridColumnSizer.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridColumnSizer::GridColumnSizer( ) { }

GridColumnSizer::~GridColumnSizer( ) { }

void GridColumnSizer::SaveColumnSizes( const wxGrid& grid ) {
  m_vColumnSize.clear();
  int size = grid.GetCols();
  m_vColumnSize.reserve( size );
  for ( int ix = 0; ix < size; ix++ ) {
    m_vColumnSize.push_back( grid.GetColSize( ix ) );
  }
};

void GridColumnSizer::SetColumnSizes( wxGrid& grid ) {
  size_t size( m_vColumnSize.size() );
  if ( 0 == size ) {
    SaveColumnSizes( grid );
  }
  else {
    if ( size != grid.GetCols() ) {
      std::runtime_error( "column counts don't match" );
    }
    else {
      for ( size_t ix = 0; ix < size; ix++ ) {
        grid.SetColumnWidth( ix, m_vColumnSize[ ix ] );
      }
    }
  }
};

} // namespace tf
} // namespace ou