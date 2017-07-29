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

#include <wx/sizer.h>

#include "PanelIBAccountValues_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelIBAccountValues_impl::PanelIBAccountValues_impl( PanelIBAccountValues& pav )
: m_pav( pav ), m_pGrid( nullptr ) {
}

void PanelIBAccountValues_impl::CreateControls() {
    PanelIBAccountValues* itemPanel1 = &m_pav;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_pGrid = new wxGrid( itemPanel1, m_pav.ID_GRID_ACCOUNTVALUES, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
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
      
  m_pav.Bind( wxEVT_CLOSE_WINDOW, &PanelIBAccountValues_impl::OnClose, this );  // start close of windows and controls
}

PanelIBAccountValues_impl::~PanelIBAccountValues_impl() {
}

void PanelIBAccountValues_impl::UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& av ) {
  
  mapAccountValueRow_t::iterator iter = m_mapAccountValueRow.find( av.sKey );
  if ( m_mapAccountValueRow.end() == iter ) {
    iter = m_mapAccountValueRow.insert( m_mapAccountValueRow.end(),
      mapAccountValueRow_t::value_type( av.sKey, AccountValueRow( *m_pGrid, m_mapAccountValueRow.size() ) ) );
    m_pGrid->AppendRows( 1 );
  }

  iter->second.UpdateAccountValue( av );
  iter->second.UpdateGui();  // TODO:  do a timed update
}

void PanelIBAccountValues_impl::OnClose( wxCloseEvent& event ) {

  // todo:  don't close if dialog is still open.

//  if ( 0 != m_menuGridLabelPositionPopUp ) {
//    delete m_menuGridLabelPositionPopUp;
//  }

//  if ( 0 != m_menuGridCellPositionPopUp ) {
//    delete m_menuGridCellPositionPopUp;
//  }

  // Exit Steps: #2 -> FrameMain::OnClose
//  if ( 0 != OnPanelClosing ) OnPanelClosing();
  // event.Veto();  // possible call, if needed
  // event.CanVeto(); // if not a 
  event.Skip();  // auto followed by Destroy();

}
} // namespace tf
} // namespace ou
