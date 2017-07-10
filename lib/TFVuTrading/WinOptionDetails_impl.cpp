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

#include "stdafx.h"

#include <functional>

#include "WinOptionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionDetails_impl::GridOptionDetails_impl( GridOptionDetails& details )
: m_details( details ) {
}

void GridOptionDetails_impl::CreateControls() {
  
    //GridOptionDetails* itemPanel1 = &m_details;

    //wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    //itemPanel1->SetSizer(itemBoxSizer2);

    //m_pGrid = new wxGrid( itemPanel1, m_details.ID_GRID_OPTIONDETAILS, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
    m_details.SetDefaultColSize(50);
    m_details.SetDefaultRowSize(22);
    m_details.SetColLabelSize(22);
    m_details.SetRowLabelSize(50);
    //m_pGrid->CreateGrid(5, 5, wxGrid::wxGridSelectCells);
    //itemBoxSizer2->Add(m_pGrid, 1, wxGROW|wxALL, 2);
    
  m_details.CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

// found in ModelCell_macros.h  
#ifdef GRID_EMIT_SetColSettings
#undef GRID_EMIT_SetColSettings
#endif
  
#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_details.SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_details.SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );
  
  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )
      
  m_details.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( &m_details );
  m_details.Bind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

}

GridOptionDetails_impl::~GridOptionDetails_impl( void ) {
}

GridOptionDetails_impl::mapOptionValueRow_iter
GridOptionDetails_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    iter = m_mapOptionValueRow.insert( m_mapOptionValueRow.end(),
      mapOptionValueRow_t::value_type( strike, OptionValueRow( &m_details, m_mapOptionValueRow.size() ) ) );
    m_details.AppendRows( 1 );
  }
  return iter;
}

void GridOptionDetails_impl::UpdateCallGreeks( double strike, ou::tf::Greek& greek ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallGreeks( greek );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::UpdateCallQuote( double strike, ou::tf::Quote& quote ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallQuote( quote );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::UpdateCallTrade( double strike, ou::tf::Trade& trade ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallTrade( trade );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::UpdatePutGreeks( double strike, ou::tf::Greek& greek ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutGreeks( greek );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::UpdatePutQuote( double strike, ou::tf::Quote& quote ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutQuote( quote );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::UpdatePutTrade( double strike, ou::tf::Trade& trade ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutTrade( trade );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void GridOptionDetails_impl::HandleGuiRefresh( wxTimerEvent& event ) {
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
    [](mapOptionValueRow_t::value_type& value) {
      value.second.UpdateGui();
    }
    );
}

void GridOptionDetails_impl::OnDestroy( wxWindowDestroyEvent& event ) {
  
  m_timerGuiRefresh.Stop();
  m_details.Unbind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  
  m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
  
  event.Skip();
}

} // namespace tf
} // namespace ou