/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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
// Started 2014/09/21

#include <wx/grid.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace ModelCell_ops {

struct UpdateGui {
  wxGrid* m_pGrid;
  int m_row;
  UpdateGui( wxGrid* pGrid, int row ): m_pGrid( pGrid ), m_row( row ) {};
  template<typename T>
  void operator()( T& t ) const {
    // todo:  deal with flicker by double-buffering?
    if ( t.Changed() ) {
      m_pGrid->SetCellValue( t.GetText(), m_row, t.GetCol() );
    }
  }
};

struct SetCol {
  typedef int result_type;
  SetCol( void ) {};
  template<typename F, typename T>
  int operator()( F& f, T& t ) const {
    return 1 + t.SetCol( f );
  }
};

struct SetPrecision {
  unsigned int m_nPrecision;
  SetPrecision( unsigned int val ): m_nPrecision( val ) {};
  template<typename T>
  void operator()( T& t ) const {
    t.SetPrecision( m_nPrecision );
  }
};

} // ModelCell_ops

} // tf
} // ou
