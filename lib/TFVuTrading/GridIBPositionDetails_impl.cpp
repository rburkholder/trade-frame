/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
// Started December 30, 2015, 4:51 PM

#include "GridIBPositionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridIBPositionDetails_impl::GridIBPositionDetails_impl( GridIBPositionDetails& pad )
: m_pad( pad ) {
}

void GridIBPositionDetails_impl::CreateControls() {
    //GridIBPositionDetails* itemPanel1 = &m_pad;

    //wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    //itemPanel1->SetSizer(itemBoxSizer2);

    //m_pGrid = new wxGrid( itemPanel1, m_pad.ID_GRID_ACCOUNTDETAILS, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
    m_pad.SetDefaultColSize(50);
    m_pad.SetDefaultRowSize(22);
    m_pad.SetColLabelSize(22);
    m_pad.SetRowLabelSize(50);
    //m_pGrid->CreateGrid(5, 5, wxGrid::wxGridSelectCells);
    //itemBoxSizer2->Add(m_pGrid, 1, wxGROW|wxALL, 2);

  m_pad.CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

// found in ModelCell_macros.h
#ifdef GRID_EMIT_SetColSettings
#undef GRID_EMIT_SetColSettings
#endif

#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_pad.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_pad.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_pad.Bind( wxEVT_CLOSE_WINDOW, &GridIBPositionDetails_impl::OnClose, this );  // start close of windows and controls
}

GridIBPositionDetails_impl::~GridIBPositionDetails_impl() {
}

void GridIBPositionDetails_impl::UpdatePositionDetailRow( const ou::tf::ib::TWS::PositionDetail& pd ) {

  mapPositionDetailRow_t::iterator iter = m_mapPositionDetailRow.find( pd.sLocalSymbol );
  if ( m_mapPositionDetailRow.end() == iter ) {
    iter = m_mapPositionDetailRow.insert( m_mapPositionDetailRow.end(),
      mapPositionDetailRow_t::value_type( pd.sLocalSymbol, PositionDetailRow( m_pad, m_mapPositionDetailRow.size() ) ) );
    try {
      m_pad.AppendRows( 1 );
    }
    catch(...) {
      std::cout << "UpdatePositionDetailRow caught something" << std::endl;
    }
  }

  iter->second.UpdatePositionDetail( pd );
  iter->second.UpdateGui();
}

void GridIBPositionDetails_impl::DestroyControls() {

  //m_timerGuiRefresh.Stop();
  //m_details.Unbind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
