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

#include "PanelOptionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

WinOptionDetails_impl::WinOptionDetails_impl( WinOptionDetails& details )
: m_details( details ), m_pGrid( nullptr ) {
}

void WinOptionDetails_impl::CreateControls() {
    WinOptionDetails* itemPanel1 = &m_details;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_pGrid = new wxGrid( itemPanel1, m_details.ID_GRID_OPTIONDETAILS, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
    m_pGrid->SetDefaultColSize(50);
    m_pGrid->SetDefaultRowSize(22);
    m_pGrid->SetColLabelSize(22);
    m_pGrid->SetRowLabelSize(50);
    //m_pGrid->CreateGrid(5, 5, wxGrid::wxGridSelectCells);
    itemBoxSizer2->Add(m_pGrid, 1, wxGROW|wxALL, 2);
    
  m_pGrid->CreateGrid(0, GRID_ARRAY_COL_COUNT, wxGrid::wxGridSelectCells);

// found in ModelCell_macros.h  
#ifdef GRID_EMIT_SetColSettings
#undef GRID_EMIT_SetColSettings
#endif
  
#define GRID_EMIT_SetColSettings( z, n, VAR ) \
  m_pGrid->SetColLabelValue( VAR, _T(GRID_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_pGrid->SetColSize( VAR++, GRID_EXTRACT_COL_DETAILS(z, n, 3) );
  
  int ix( 0 );
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SetColSettings, ix )
      
  m_details.Bind( wxEVT_CLOSE_WINDOW, &WinOptionDetails_impl::OnClose, this );  // start close of windows and controls
}

WinOptionDetails_impl::~WinOptionDetails_impl( void ) {
}

WinOptionDetails_impl::mapOptionValueRow_iter
WinOptionDetails_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    iter = m_mapOptionValueRow.insert( m_mapOptionValueRow.end(),
      mapOptionValueRow_t::value_type( strike, OptionValueRow( m_pGrid, m_mapOptionValueRow.size() ) ) );
    m_pGrid->AppendRows( 1 );
  }
  return iter;
}

void WinOptionDetails_impl::UpdateCallGreeks( double strike, ou::tf::Greek& greek ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallGreeks( greek );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::UpdateCallQuote( double strike, ou::tf::Quote& quote ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallQuote( quote );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::UpdateCallTrade( double strike, ou::tf::Trade& trade ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdateCallTrade( trade );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::UpdatePutGreeks( double strike, ou::tf::Greek& greek ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutGreeks( greek );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::UpdatePutQuote( double strike, ou::tf::Quote& quote ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutQuote( quote );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::UpdatePutTrade( double strike, ou::tf::Trade& trade ) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  iter->second.UpdatePutTrade( trade );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void WinOptionDetails_impl::OnClose( wxCloseEvent& event ) {

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();
}

} // namespace tf
} // namespace ou