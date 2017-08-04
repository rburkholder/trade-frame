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

#include "GridOptionChain_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionChain_impl::GridOptionChain_impl( GridOptionChain& details )
: m_details( details ), m_bTimerActive( false ) {
}

void GridOptionChain_impl::CreateControls() {
  
    m_details.SetDefaultColSize(50);
    m_details.SetDefaultRowSize(22);
    m_details.SetColLabelSize(22);
    m_details.SetRowLabelSize(50);
    
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

  m_details.Bind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Bind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  
  m_details.EnableEditing( false );

}

GridOptionChain_impl::~GridOptionChain_impl( void ) {
}

void GridOptionChain_impl::TimerActivate() {
  if ( !m_bTimerActive ) {
    m_bTimerActive = true;
    // this GuiRefresh initialization should come after all else
    m_timerGuiRefresh.SetOwner( &m_details );
    m_details.Bind( wxEVT_TIMER, &GridOptionChain_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
    m_timerGuiRefresh.Start( 250 );
  }
}

void GridOptionChain_impl::TimerDeactivate() {
  if ( m_bTimerActive ) {
    m_bTimerActive = false;
    m_timerGuiRefresh.Stop();
    m_timerGuiRefresh.DeletePendingEvents();
    m_details.Unbind( wxEVT_TIMER, &GridOptionChain_impl::HandleGuiRefresh, this, m_timerGuiRefresh.GetId() );
  }
}

void GridOptionChain_impl::Add( double strike, ou::tf::OptionSide::enumOptionSide side, const std::string& sSymbol ) {
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

GridOptionChain_impl::mapOptionValueRow_iter
GridOptionChain_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    assert( 0 );
  }
  return iter;
}

void GridOptionChain_impl::SetSelected(double strike, bool bSelected) {
  mapOptionValueRow_iter iter = FindOptionValueRow( strike );
  wxColour colour = bSelected ? *wxWHITE : m_details.GetDefaultCellBackgroundColour();
  m_details.SetCellBackgroundColour( iter->second.m_nRow, -1, colour );
}

void GridOptionChain_impl::HandleGuiRefresh( wxTimerEvent& event ) {
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
    [this](mapOptionValueRow_t::value_type& value) {
      if ( m_details.IsVisible( value.second.m_nRow, COL_Strike ) ) {
        value.second.UpdateGui();
      }
    }
    );
}

void GridOptionChain_impl::OnGridLeftClick( wxGridEvent& event ) {
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
      
      GridOptionChain::OptionUpdateFunctions funcCall;
      funcCall.sSymbolName = iter->second.m_sCallName;
      funcCall.fQuote = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdateCallQuote );
      funcCall.fTrade = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdateCallTrade );
      funcCall.fGreek = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdateCallGreeks );
      
      GridOptionChain::OptionUpdateFunctions funcPut;
      funcPut.sSymbolName = iter->second.m_sPutName;
      funcPut.fQuote = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdatePutQuote );
      funcPut.fTrade = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdatePutTrade );
      funcPut.fGreek = fastdelegate::MakeDelegate( &iter->second, &OptionValueRow::UpdatePutGreeks );
      m_details.m_fOnRowClicked( iter->first, funcCall, funcPut );
    }
  }
  
  //std::cout << "Notebook Left Click: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  event.Skip();
}

void GridOptionChain_impl::DestroyControls() { 

  TimerDeactivate();  
  
  m_details.Unbind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  
  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou