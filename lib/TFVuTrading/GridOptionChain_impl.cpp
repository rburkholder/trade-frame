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

#include <algorithm>
#include <functional>

#include <wx/dnd.h>
#include <wx/cursor.h>

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

  m_details.Bind( wxEVT_GRID_LABEL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );
  m_details.Bind( wxEVT_GRID_CELL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );

  m_details.EnableDragCell( true );

  m_details.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want

  m_details.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

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

void GridOptionChain_impl::Add( double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    iter = m_mapOptionValueRow.insert(
      m_mapOptionValueRow.begin(),
      mapOptionValueRow_t::value_type( strike, OptionValueRow( m_details, strike ) ) );

    struct Reindex {
      size_t ix;
      Reindex(): ix{} {}
      void operator()( OptionValueRow& row ) { row.m_nRow = ix; ix++; }
    };

    Reindex reindex;
    std::for_each(
      m_mapOptionValueRow.rbegin(), m_mapOptionValueRow.rend(),
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
  // TODO: actually enable/disable watch?
}

// replace my the 'option engine'?
void GridOptionChain_impl::HandleGuiRefresh( wxTimerEvent& event ) {
  // need extra validation here, crash when redrawing with active options
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
    [this](mapOptionValueRow_t::value_type& value) {
      if ( m_details.IsVisible( value.second.m_nRow, COL_Strike ) ) {
        value.second.UpdateGui();
      }
    }
  );
}

void GridOptionChain_impl::OnMouseMotion( wxMouseEvent& event ) {
  if ( event.Dragging() ) {

    // 2018/08/02 obsolete?  replaced by LeftClick operations?
    if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

      mapOptionValueRow_t::iterator iter;
      iter = std::find_if(
        m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
        [this]( mapOptionValueRow_t::value_type& vt ){ return m_nRow == vt.second.m_nRow; } );
      assert( m_mapOptionValueRow.end() != iter );

//      std::cout << "column: " << m_nColumn << std::endl;

//      if ( ( 0 <= m_nColumn ) && ( 5 >= m_nColumn ) ) {
//        // call drag and drop
//        ou::tf::DragDropDataInstrument dndCall( iter->second.m_sCallName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndCall );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }

//      if ( ( 7 <= m_nColumn ) && ( 12 >= m_nColumn ) ) {
//        // put drag and drop
//        ou::tf::DragDropDataInstrument dndPut( iter->second.m_sPutName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndPut );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }
    }
  }
}

void GridOptionChain_impl::OnGridRightClick( wxGridEvent& event ) {
  //std::cout << "Notebook Left Click: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  // use to toggle monitoring

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

    if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

      mapOptionValueRow_t::iterator iter;
      iter = std::find_if(
        m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
        [this]( mapOptionValueRow_t::value_type& vt ){ return m_nRow == vt.second.m_nRow; } );
      assert( m_mapOptionValueRow.end() != iter );

      // 2018/08/02 obsolete?  replaced by LeftClick operations?
//      if ( ( 0 <= m_nColumn ) && ( 5 >= m_nColumn ) ) {
//        // call drag and drop
//        ou::tf::DragDropDataInstrument dndCall( iter->second.m_sCallName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndCall );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }

//      if ( ( 7 <= m_nColumn ) && ( 12 >= m_nColumn ) ) {
//        // put drag and drop
//        ou::tf::DragDropDataInstrument dndPut( iter->second.m_sPutName );
//        wxDropSource dragSource( &m_details );
//        dragSource.SetData( dndPut );
//        wxDragResult result = dragSource.DoDragDrop( true );
//      }
    }
}

void GridOptionChain_impl::OnGridLeftClick( wxGridEvent& event ) {
  //std::cout << "Notebook Left Click: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  // use to toggle monitoring

  bool bSkip( true );

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

  if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

    mapOptionValueRow_t::iterator iterOptionValueRow;
    iterOptionValueRow = std::find_if(
      m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
      [this]( mapOptionValueRow_t::value_type& vt ){ return m_nRow == vt.second.m_nRow; } );
    assert( m_mapOptionValueRow.end() != iterOptionValueRow );

    if ( nullptr != m_details.m_fOnRowClicked ) {

      if ( ( 0 <= m_nRow ) && event.ControlDown() ) {
        GridOptionChain::OptionUpdateFunctions funcCall;
        funcCall.sSymbolName = iterOptionValueRow->second.m_sCallName;
        funcCall.fQuote = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallQuote );
        funcCall.fTrade = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallTrade );
        funcCall.fGreek = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdateCallGreeks );

        GridOptionChain::OptionUpdateFunctions funcPut;
        funcPut.sSymbolName = iterOptionValueRow->second.m_sPutName;
        funcPut.fQuote = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutQuote );
        funcPut.fTrade = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutTrade );
        funcPut.fGreek = fastdelegate::MakeDelegate( &iterOptionValueRow->second, &OptionValueRow::UpdatePutGreeks );

        iterOptionValueRow->second.m_bSelected = !iterOptionValueRow->second.m_bSelected;

        m_details.m_fOnRowClicked( iterOptionValueRow->first, iterOptionValueRow->second.m_bSelected, funcCall, funcPut );
      }

    }

  }

  event.Skip( bSkip );
}

bool GridOptionChain_impl::StartDragDrop( ou::tf::DragDropInstrument& dddi ) {

#if defined(__WXMSW__)
      wxCursor cursor( wxCURSOR_HAND );
      wxDropSource dragSource( dndCall, &m_details, cursor, cursor, cursor );
#elif defined(__WXGTK__)
      // needs icon: docs.wxwidgets.org/3.0/classwx_drop_source.html
      wxDropSource dragSource( &m_details );
#else
      assert(0);
#endif

      dragSource.SetData( dddi );
      //std::cout << "call drag start " << std::endl;
      wxDragResult result = dragSource.DoDragDrop( true );
      //std::cout << "call drag stop " << std::endl;
      switch ( result ) {
        case wxDragCopy:
        case wxDragMove:
          break;
        default:
          break;
      }
      //bSkip = false;
      return false; // bSkip
}

void GridOptionChain_impl::OnGridCellBeginDrag( wxGridEvent& event ) {
  //std::cout << "Notebook Begin Drag: " << event.GetRow() << std::endl;
  // column header is -1, first row is 0
  // use to toggle monitoring

  bool bSkip( true );

  m_nRow = event.GetRow();
  m_nColumn = event.GetCol();

  if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

    mapOptionValueRow_t::iterator iterOptionValueRow;
    iterOptionValueRow = std::find_if(
      m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
      [this]( mapOptionValueRow_t::value_type& vt ){ return m_nRow == vt.second.m_nRow; } );
    assert( m_mapOptionValueRow.end() != iterOptionValueRow );

    if ( nullptr != m_details.m_fOnOptionUnderlyingRetrieveInitiate ) {

      if ( ( 0 <= m_nColumn ) && ( 5 >= m_nColumn ) ) { // call drag and drop
        ou::tf::DragDropInstrument dndCall( [this,iterOptionValueRow]( GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t&& f ){
          m_details.m_fOnOptionUnderlyingRetrieveInitiate( iterOptionValueRow->second.m_sCallName, iterOptionValueRow->first, std::move( f ) ); // iqfeed name and strike
        } );

        bSkip = StartDragDrop( dndCall );
      }

      if ( ( 7 <= m_nColumn ) && ( 12 >= m_nColumn ) ) { // put drag and drop
        ou::tf::DragDropInstrument dndPut( [this,iterOptionValueRow]( GridOptionChain::fOnOptionUnderlyingRetrieveComplete_t&& f ){
          m_details.m_fOnOptionUnderlyingRetrieveInitiate( iterOptionValueRow->second.m_sPutName, iterOptionValueRow->first, std::move( f ) ); // iqfeed name and strike
        } );

        bSkip = StartDragDrop( dndPut );
      }
    }

  }

  event.Skip( bSkip );
}

void GridOptionChain_impl::StopWatch() {
  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(), [this](mapOptionValueRow_t::value_type& value){
    if ( value.second.m_bSelected ) {
      value.second.m_bSelected = false;

      if ( nullptr != m_details.m_fOnRowClicked ) {

        GridOptionChain::OptionUpdateFunctions funcCall;
        funcCall.sSymbolName = value.second.m_sCallName;

        GridOptionChain::OptionUpdateFunctions funcPut;
        funcPut.sSymbolName = value.second.m_sPutName;

        m_details.m_fOnRowClicked( value.first, value.second.m_bSelected, funcCall, funcPut );
      }
    }
  });
}

void GridOptionChain_impl::DestroyControls() {

  TimerDeactivate();

  m_details.Unbind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );

  m_details.Unbind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );

  m_details.Unbind( wxEVT_GRID_LABEL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );
  m_details.Unbind( wxEVT_GRID_CELL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );

  m_details.Unbind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  //m_details.Unbind( wxEVT_DESTROY, &GridOptionDetails_impl::OnDestroy, this );
}

} // namespace tf
} // namespace ou
