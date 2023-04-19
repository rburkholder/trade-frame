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

#include "GridOptionChain_impl.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

GridOptionChain_impl::GridOptionChain_impl( GridOptionChain& details )
: wxGridTableBase()
, m_details( details ), m_bTimerActive( false ) {
}

GridOptionChain_impl::~GridOptionChain_impl() {
  m_details.SetTable( nullptr, false, wxGrid::wxGridSelectNone );
  m_vRowIX.clear();
  m_mapOptionValueRow.clear();
}

void GridOptionChain_impl::CreateControls() {

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

  m_details.Bind( wxEVT_GRID_LABEL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );
  m_details.Bind( wxEVT_GRID_CELL_LEFT_CLICK , &GridOptionChain_impl::OnGridLeftClick, this );

  m_details.Bind( wxEVT_GRID_LABEL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );
  m_details.Bind( wxEVT_GRID_CELL_RIGHT_CLICK , &GridOptionChain_impl::OnGridRightClick, this );

  m_details.EnableDragCell( true );

  m_details.Bind( wxEVT_GRID_CELL_BEGIN_DRAG, &GridOptionChain_impl::OnGridCellBeginDrag, this );  // this is the event we really want

  m_details.Bind( wxEVT_MOTION, &GridOptionChain_impl::OnMouseMotion, this );  // already consumed by grid itself

  m_details.EnableEditing( false );

}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Quote& quote ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallQuote( quote );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutQuote( quote );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Trade& trade ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallTrade( trade );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutTrade( trade );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Update( double strike, ou::tf::OptionSide::EOptionSide side, const ou::tf::Greek& greek ) {
  switch ( side ) {
    case ou::tf::OptionSide::Call:
      FindOptionValueRow( strike )->second.UpdateCallGreeks( greek );
      break;
    case ou::tf::OptionSide::Put:
      FindOptionValueRow( strike )->second.UpdatePutGreeks( greek );
      break;
    default:
      assert( false );
  }
}

void GridOptionChain_impl::Clear(  double strike ) {
  //m_mapOptionValueRow[ strike ].Init();  ?
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
    auto pair = m_mapOptionValueRow.emplace(
      mapOptionValueRow_t::value_type( strike, OptionValueRow( m_details, strike ) ) );
    assert( pair.second );
    iter = pair.first;

    struct Reindex {
      size_t ix;
      Reindex(): ix{} {}
      void operator()( OptionValueRow& row ) { row.m_nRow = ix; ix++; }
    };

    Reindex reindex;
    m_vRowIX.clear();

    for ( auto iter = m_mapOptionValueRow.rbegin(); iter != m_mapOptionValueRow.rend(); iter++ ) {
      reindex( iter->second );
      m_vRowIX.push_back( iter );
    }

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

void GridOptionChain_impl::MakeRowVisible( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  m_details.MakeCellVisible( iter->second.m_nRow, 0 );
}

GridOptionChain_impl::mapOptionValueRow_iter
GridOptionChain_impl::FindOptionValueRow( double strike ) {
  mapOptionValueRow_iter iter = m_mapOptionValueRow.find( strike );
  if ( m_mapOptionValueRow.end() == iter ) {
    assert( false );
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
  // TODO: redo this section by using the wxGridTableMessage to trigger a refresh of the visible grid
  // need extra validation here, crash when redrawing with active options
//  std::for_each( m_mapOptionValueRow.begin(), m_mapOptionValueRow.end(),
//    [this](mapOptionValueRow_t::value_type& value) {
//      if ( m_details.IsVisible( value.second.m_nRow, COL_Strike ) ) {
//        value.second.UpdateGui();
//      }
//    }
//  );
}

void GridOptionChain_impl::OnMouseMotion( wxMouseEvent& event ) {
  if ( event.Dragging() ) {

    // 2018/08/02 obsolete?  replaced by LeftClick operations?
    if ( ( 0 < m_nRow ) && ( m_nRow < m_mapOptionValueRow.size() ) ) {

      assert( m_nRow < m_vRowIX.size() );
      mapOptionValueRow_t::reverse_iterator iter = m_vRowIX[ m_nRow ];
      assert( m_mapOptionValueRow.rend() != iter );

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

      assert( m_nRow < m_vRowIX.size() );
      mapOptionValueRow_t::reverse_iterator iter = m_vRowIX[ m_nRow ];
      assert( m_mapOptionValueRow.rend() != iter );

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

    assert( m_nRow < m_vRowIX.size() );
    mapOptionValueRow_t::reverse_iterator iterOptionValueRow = m_vRowIX[ m_nRow ];
    assert( m_mapOptionValueRow.rend() != iterOptionValueRow );

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

    assert( m_nRow < m_vRowIX.size() );
    mapOptionValueRow_t::reverse_iterator iterOptionValueRow = m_vRowIX[ m_nRow ];
    assert( m_mapOptionValueRow.rend() != iterOptionValueRow );

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

void GridOptionChain_impl::SetView( wxGrid *grid ) {
  wxGridTableBase::SetView( grid );
}

wxGrid* GridOptionChain_impl::GetView() const {
  return wxGridTableBase::GetView();
}

int GridOptionChain_impl::GetNumberRows() {
  return m_mapOptionValueRow.size();
}

int GridOptionChain_impl::GetNumberCols() {
  return GRID_ARRAY_COL_COUNT;
}

bool GridOptionChain_impl::IsEmptyCell(int row, int col ) {
  return false;
}

// https://github.com/wxWidgets/wxWidgets/blob/master/src/generic/grid.cpp
// wxGridStringTable::InsertRows
bool GridOptionChain_impl::InsertRows( size_t pos, size_t numRows ) {
  //return wxGridTableBase::InsertRows( pos, numRows ); // don't do this
    if ( GetView() ) {
      wxGridTableMessage msg(
        this,
        wxGRIDTABLE_NOTIFY_ROWS_INSERTED,
        pos,
        numRows
      );
      GetView()->ProcessTableMessage( msg );
    }
  return true;
}

wxString GridOptionChain_impl::GetValue( int row, int col ) {

  wxString s;

  #define GRID_EMIT_SwitchGetValue( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      s = boost::fusion::at_c<GRID_EXTRACT_COL_DETAILS(z, n, 0)>( m_vRowIX[row]->second.m_vModelCells ).GetText(); \
      break;

  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetValue, 0 )
  }

  return s;

}

void GridOptionChain_impl::SetValue(int row, int col, const wxString &value ) {
}

//void GridOptionChain_impl::SetValueAsDouble(int row, int col, double value ) {
//}

//double GridOptionChain_impl::GetValueAsDouble(int row, int col ) {
//  return 0.0;
//}

wxString GridOptionChain_impl::GetColLabelValue( int col ) {

  wxString s;

  #define GRID_EMIT_SwitchGetColLabel( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      s = wxString( GRID_EXTRACT_COL_DETAILS(z, n, 1 ) ); \
      break;

  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColLabel, 0 )
  }

  return s;
}

wxGridCellAttr* GridOptionChain_impl::GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind ) {

  #define GRID_EMIT_SwitchGetColAlign( z, n, data ) \
    case GRID_EXTRACT_COL_DETAILS(z, n, 0):  \
      align = GRID_EXTRACT_COL_DETAILS(z, n, 2 ); \
      break;

  wxGridCellAttr* pAttr = new wxGridCellAttr();

  int align = wxALIGN_CENTER;
  switch ( col ) {
    BOOST_PP_REPEAT(BOOST_PP_ARRAY_SIZE( GRID_ARRAY ), GRID_EMIT_SwitchGetColAlign, 0 )
  }
  pAttr->SetAlignment( align, wxALIGN_CENTER_VERTICAL );

  switch ( kind ) {
    case wxGridCellAttr::wxAttrKind::Cell:
    case wxGridCellAttr::wxAttrKind::Col:
      break;
    case wxGridCellAttr::wxAttrKind::Row:
      break;
    case wxGridCellAttr::wxAttrKind::Default:
      break;
  }

  return pAttr;

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
