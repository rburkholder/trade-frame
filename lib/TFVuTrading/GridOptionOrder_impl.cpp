/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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

/*
 * File:    GridOptionOrder_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/23 17:28:15
 */

#include "GridOptionOrder_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionOrder_impl::GridOptionOrder_impl( GridOptionOrder& details )
: wxGridTableBase()
, m_details( details ), m_bTimerActive( false ) {
}

GridOptionOrder_impl::~GridOptionOrder_impl() {
  m_details.SetTable( nullptr, false, wxGrid::wxGridSelectNone );
}

void GridOptionOrder_impl::CreateControls() {

  m_details.SetDefaultColSize(50);
  m_details.SetDefaultRowSize(22);
  m_details.SetColLabelSize(22);
  m_details.SetRowLabelSize(50);

  m_details.SetTable( this, false, wxGrid::wxGridSelectCells );

  // found in ModelCell_macros.h
  #ifdef GRID_EMIT_SetColSettings
  #undef GRID_EMIT_SetColSettings
  #endif

  #define GRID_EMIT_SetColSettings( z, n, VAR ) \
    /* m_details.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); */ \
    m_details.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_details.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  m_details.EnableDragCell( true );

//  m_details.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want

//  m_details.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  m_details.EnableEditing( false );

}

void GridOptionOrder_impl::DestroyControls() {

//  m_details.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionOrder_impl::OnGridCellBeginDrag, this );

//  m_details.Unbind( wxEVT_MOTION, &GridOptionOrder_impl::OnMouseMotion, this );  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

}

} // namespace tf
} // namespace ou
