/***********************************************************************
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

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#define GRID_EXTRACT_COL_DETAILS(z, row, col) \
  BOOST_PP_TUPLE_ELEM( \
    GRID_ARRAY_PARAM_COUNT, col, \
      BOOST_PP_ARRAY_ELEM( row, GRID_ARRAY ) \
    )

// if n is 0, then no comma, ie, prepends comma except on first element, col is column number to extract
#define GRID_EXTRACT_ENUM_LIST(z, n, col) \
  BOOST_PP_COMMA_IF(n) \
  GRID_EXTRACT_COL_DETAILS( z, n, col )

// 2015/12/30 change PanelPortfolioPosition_impl to m_grid, then can re-use this macro
//   otherwise figure out how to pass in another define representing GRID instead of hardcoding m_gridPositions
#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_gridPositions->SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_gridPositions->SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

#define COL_ALIGNMENT( z, n, VAR ) \
  m_grid.SetCellAlignment( VAR, GRID_EXTRACT_COL_DETAILS(z, n, 0), GRID_EXTRACT_COL_DETAILS(z, n, 2), wxALIGN_CENTRE );

namespace ou { // One Unified
namespace tf { // TradeFrame

  template<typename ModelCell>
  class CellInfo_t: public ModelCell {
  public:
    CellInfo_t(): m_col( 0 ) {};
    CellInfo_t( int col ): m_col( col ) {};
    virtual ~CellInfo_t() = default;
    int SetCol( int col ) { m_col = col; return m_col; }
    int GetCol() const { return m_col; }
  private:
    int m_col;
  };

} // namespace tf
} // namespace ou

#define COMPOSE_MODEL_CELL(z,n,col)\
  BOOST_PP_COMMA_IF(n)\
  CellInfo_t<GRID_EXTRACT_COL_DETAILS(z,n,col)>

#define VECTOR_DEF BOOST_PP_CAT( vector, GRID_ARRAY_COL_COUNT )

