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
// Started January 3, 2016, 3:31 PM

#include "GridIBAccountValues_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridIBAccountValues_impl::GridIBAccountValues_impl( GridIBAccountValues& pav )
: m_pav( pav ) {
}

void GridIBAccountValues_impl::CreateControls() {
    //GridIBAccountValues* itemPanel1 = &m_pav;

    //wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    //itemPanel1->SetSizer(itemBoxSizer2);

    //m_pGrid = new wxGrid( itemPanel1, m_pav.ID_GRID_ACCOUNTVALUES, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
    m_pav.SetDefaultColSize(50);
    m_pav.SetDefaultRowSize(22);
    m_pav.SetColLabelSize(22);
    m_pav.SetRowLabelSize(50);
    //m_pGrid->CreateGrid(5, 5, wxGrid::wxGridSelectCells);
    //itemBoxSizer2->Add(&m_pav, 1, wxGROW|wxALL, 2);

  m_pav.CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

// found in ModelCell_macros.h
#ifdef GRID_EMIT_SetColSettings
#undef GRID_EMIT_SetColSettings
#endif

#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_pav.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_pav.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );

  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )

  //m_pav.Bind( wxEVT_CLOSE_WINDOW, &GridIBAccountValues_impl::OnClose, this );  // start close of windows and controls
}

GridIBAccountValues_impl::~GridIBAccountValues_impl() {
}

void GridIBAccountValues_impl::UpdateAccountValueRow( const ou::tf::ib::TWS::AccountValue& av ) {

  mapAccountValueRow_t::iterator iter = m_mapAccountValueRow.find( av.sKey );
  if ( m_mapAccountValueRow.end() == iter ) {
    iter = m_mapAccountValueRow.insert( m_mapAccountValueRow.end(),
      mapAccountValueRow_t::value_type( av.sKey, AccountValueRow( m_pav, m_mapAccountValueRow.size() ) ) );
    m_pav.AppendRows( 1 );
  }

  iter->second.UpdateAccountValue( av );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridIBAccountValues_impl::DestroyControls() {

  //m_timerGuiRefresh.Stop();
  //m_details.Unbind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
