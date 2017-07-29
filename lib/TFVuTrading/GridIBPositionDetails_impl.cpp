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

#include <wx/sizer.h>

#include "GridIBPositionDetails_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

PanelIBPositionDetails_impl::PanelIBPositionDetails_impl( PanelIBPositionDetails& pad ): m_pad( pad ) {
  m_pGrid = NULL;
}

void PanelIBPositionDetails_impl::CreateControls() {
    PanelIBPositionDetails* itemPanel1 = &m_pad;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    m_pGrid = new wxGrid( itemPanel1, m_pad.ID_GRID_ACCOUNTDETAILS, wxDefaultPosition, wxSize(400, 300), wxHSCROLL|wxVSCROLL );
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
      
  m_pad.Bind( wxEVT_CLOSE_WINDOW, &PanelIBPositionDetails_impl::OnClose, this );  // start close of windows and controls
}

PanelIBPositionDetails_impl::~PanelIBPositionDetails_impl() {
}

void PanelIBPositionDetails_impl::UpdatePositionDetailRow( const ou::tf::IBTWS::PositionDetail& pd ) {
  
  mapPositionDetailRow_t::iterator iter = m_mapPositionDetailRow.find( pd.sLocalSymbol );
  if ( m_mapPositionDetailRow.end() == iter ) {
    iter = m_mapPositionDetailRow.insert( m_mapPositionDetailRow.end(),
      mapPositionDetailRow_t::value_type( pd.sLocalSymbol, PositionDetailRow( *m_pGrid, m_mapPositionDetailRow.size() ) ) );
    try {
      m_pGrid->AppendRows( 1 );
    }
    catch(...) {
      std::cout << "UpdatePositionDetailRow caught something" << std::endl;
    }
  }

  iter->second.UpdatePositionDetail( pd );
  iter->second.UpdateGui();
}

void PanelIBPositionDetails_impl::OnClose( wxCloseEvent& event ) {

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
