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

#include "GridOptionDetails_impl.h"

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
      
  //m_details.Bind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );

  m_details.Bind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionDetails_impl::OnGridLeftClick, this );
  m_details.Bind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionDetails_impl::OnGridLeftClick, this );

  // this GuiRefresh initialization should come after all else
  m_timerGuiRefresh.SetOwner( &m_details );
  m_details.Bind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  m_timerGuiRefresh.Start( 250 );

  m_details.EnableEditing( false );

}

GridOptionDetails_impl::~GridOptionDetails_impl( void ) {
}

void GridOptionDetails_impl::Add( double strike, ou::tf::OptionSide::enumOptionSide side, const std::string& sSymbol ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    iter = m_mapOptionValueRow.insert( m_mapOptionValueRow.begin(),
      mapOptionValueRow_t::value_type( strike, OptionValueRow( m_details, strike ) ) );
    
    struct Reindex {
      size_t ix;
      Reindex(): ix{} {}
      void operator()( OptionValueRow& row ) { row.m_nRow = ix; ix++; }
    };
    
    Reindex reindex; 
    std::for_each( 
      m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(), 
        [&reindex](mapOptionValueRow_t::value_type& v){ reindex( v.second ); } );
        
    assert( m_details.InsertRows( iter->second.m_nRow ) );
  }
  
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      iter->second.m_sCallName = sSymbol;
      break;
    case ou::tf::OptionSide::Put:
      iter->second.m_sPutName = sSymbol;
      break;
  }
}

GridOptionDetails_impl::mapOptionValueRow_iter
GridOptionDetails_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    assert( 0 );
  }
  return iter;
}

void GridOptionDetails_impl::SetSelected(double strike, bool bSelected) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  wxColour colour = bSelected ? *wxWHITE : m_details.GetDefaultCellBackgroundColour();
  m_details.SetCellBackgroundColour( iter->second.m_nRow, -1, colour );
}

void GridOptionDetails_impl::HandleGuiRefresh( wxTimerEvent& event ) {
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
    [this](mapOptionValueRow_t::value_type& value) {
      if ( m_details.IsVisible( value.second.m_nRow, COL_Strike ) ) {
        value.second.UpdateGui();
      }
    }
    );
}

void GridOptionDetails_impl::OnGridLeftClick( wxGridEvent& event ) {
  // use to toggle monitoring
  int nRow = event.GetRow();
  if ( 0 <= nRow && event.ControlDown() ) {
    assert( nRow < m_mapOptionValueRow.size() );
    mapOptionValueRow_t::iterator iter;
    iter = std::find_if( 
      m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(), 
      [nRow]( mapOptionValueRow_t::value_type& vt ){ return nRow == vt.second.m_nRow; } );
    assert( m_mapOptionValueRow.end() != iter );
    if ( nullptr != m_details.m_fOnRowClicked ) {
      GridOptionDetails::DatumUpdateFunctions functions;
      functions.fCallGreek = std::bind( &OptionValueRow::UpdateCallGreeks, &iter->second, std::placeholders::_1 );
      functions.fCallQuote = std::bind( &OptionValueRow::UpdateCallQuote,  &iter->second, std::placeholders::_1 );
      functions.fCallTrade = std::bind( &OptionValueRow::UpdateCallTrade,  &iter->second, std::placeholders::_1 );
      functions.fPutGreek  = std::bind( &OptionValueRow::UpdatePutGreeks,  &iter->second, std::placeholders::_1 );
      functions.fPutQuote  = std::bind( &OptionValueRow::UpdatePutQuote,   &iter->second, std::placeholders::_1 );
      functions.fPutTrade  = std::bind( &OptionValueRow::UpdatePutTrade,   &iter->second, std::placeholders::_1 );
      m_details.m_fOnRowClicked( iter->first, iter->second.m_sCallName, iter->second.m_sPutName, functions );
    }
  }
  
  //std::cout << "Notebook Left Click: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  event.Skip();
}

void GridOptionDetails_impl::DestroyControls() { 
  
  m_timerGuiRefresh.Stop();
  m_timerGuiRefresh.DeletePendingEvents();
  m_details.Unbind( wxEVT_TIMER, &GridOptionDetails_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );

  m_details.Unbind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionDetails_impl::OnGridLeftClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionDetails_impl::OnGridLeftClick, this );
  
  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou