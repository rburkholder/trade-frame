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

#pragma once

#include <map>
#include <set>

#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/algorithm/transformation/filter.hpp>

#include <boost/fusion/container/vector/vector20.hpp>

#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/include/filter.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/vector20.hpp>

#include <boost/fusion/sequence/intrinsic/at_c.hpp>

#include <wx/timer.h>
#include <wx/stattext.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "GridOptionChain.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct GridOptionChain_impl: public wxGridTableBase {
//public:
  GridOptionChain_impl( GridOptionChain& );
  virtual ~GridOptionChain_impl();
//protected:

//private:

  GridOptionChain& m_details;

// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 13
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,         1,            2,       3,      4,          */ \
      (COL_CallLast , "Last",  wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_CallIV   , "IV",    wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_CallGamma, "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_CallDelta, "Delta", wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_CallAsk  , "Ask",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_CallBid  , "Bid",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Strike   , "Strk",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_PutBid   , "Bid",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_PutAsk   , "Ask",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_PutDelta , "Delta", wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_PutGamma , "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_PutIV    , "IV",    wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_PutLast  , "Last",  wxALIGN_RIGHT,  50, ModelCellDouble ), \
      ) \
    )

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  using vModelCells_t = boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  >;

  struct OptionValueRow {
  //public:
    OptionValueRow( wxGrid& grid, double strike )
      : m_grid( grid ), m_nRow {}, m_bSelected( false )
      {
	      Init();
        boost::fusion::at_c<COL_Strike>( m_vModelCells ).SetValue( strike );
      }
    OptionValueRow( const OptionValueRow& rhs ) = delete;
    OptionValueRow( OptionValueRow&& rhs )
      : m_grid( rhs.m_grid ), m_nRow( rhs.m_nRow ), m_bSelected( rhs.m_bSelected )
    {
      Init();
      boost::fusion::at_c<COL_Strike>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Strike>( rhs.m_vModelCells ).GetValue() );
    }
    ~OptionValueRow() = default;

    void UpdateGui() { // now updated by update events
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_grid, m_nRow ) );
    }
    void UpdateCallGreeks( const ou::tf::Greek& greek ) {
      boost::fusion::at_c<COL_CallIV>( m_vModelCells ).SetValue( greek.ImpliedVolatility() );
      boost::fusion::at_c<COL_CallDelta>( m_vModelCells ).SetValue( greek.Delta() );
      boost::fusion::at_c<COL_CallGamma>( m_vModelCells ).SetValue( greek.Gamma() );
    }
    void UpdateCallQuote( const ou::tf::Quote& quote ) {
      boost::fusion::at_c<COL_CallBid>( m_vModelCells ).SetValue( quote.Bid() );
      boost::fusion::at_c<COL_CallAsk>( m_vModelCells ).SetValue( quote.Ask() );
    }
    void UpdateCallTrade( const ou::tf::Trade& trade ) {
      boost::fusion::at_c<COL_CallLast>( m_vModelCells ).SetValue( trade.Price() );
    }
    void UpdatePutGreeks( const ou::tf::Greek& greek ) {
      boost::fusion::at_c<COL_PutIV>( m_vModelCells ).SetValue( greek.ImpliedVolatility() );
      boost::fusion::at_c<COL_PutDelta>( m_vModelCells ).SetValue( greek.Delta() );
      boost::fusion::at_c<COL_PutGamma>( m_vModelCells ).SetValue( greek.Gamma() );
    }
    void UpdatePutQuote( const ou::tf::Quote& quote ) {
      boost::fusion::at_c<COL_PutBid>( m_vModelCells ).SetValue( quote.Bid() );
      boost::fusion::at_c<COL_PutAsk>( m_vModelCells ).SetValue( quote.Ask() );
    }
    void UpdatePutTrade( const ou::tf::Trade& trade ) {
      boost::fusion::at_c<COL_PutLast>( m_vModelCells ).SetValue( trade.Price() );
    }
    // TODO: add open interest
  //protected:
  //private:

    std::string m_sCallName;
    std::string m_sPutName;

    bool m_bSelected;

    wxGrid& m_grid;
    int m_nRow;
    vModelCells_t m_vModelCells;

    void Init() {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_nRow)
    }
  };  // struct OptionValueRow

  int m_nRow;
  int m_nColumn;

  using mapOptionValueRow_t = std::map<double,OptionValueRow>;
  using mapOptionValueRow_iter = mapOptionValueRow_t::iterator;
  mapOptionValueRow_t m_mapOptionValueRow;

  // fast access into m_mapOptionValueRow by row index
  using vRowIX_t = std::vector<mapOptionValueRow_t::reverse_iterator>;
  vRowIX_t m_vRowIX;

  using setRows_t = std::set<int>;
  setRows_t m_setRowUpdating; // rows with trade/quote/greeks

  mapOptionValueRow_iter FindOptionValueRow( double );

  void Add( double strike, ou::tf::OptionSide::EOptionSide side, const std::string& sSymbol );

  void MakeRowVisible( double strike );

  void SetSelected( double strike, bool bSelected );

  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Quote& );
  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Trade& );
  void Update( double strike, ou::tf::OptionSide::EOptionSide, const ou::tf::Greek& );
  void Clear(  double strike );

  bool m_bTimerActive;
  void Start();
  void Stop();

  void CreateControls();
  //void OnDestroy( wxWindowDestroyEvent& event );  // can't use this

  virtual void SetView ( wxGrid *grid );
  virtual wxGrid* GetView() const;

  virtual int GetNumberRows();
  virtual int GetNumberCols();
  virtual bool IsEmptyCell(int row, int col);

  virtual bool InsertRows(size_t pos=0, size_t numRows=1);

  virtual wxString GetValue(int row, int col);
  virtual void SetValue(int row, int col, const wxString &value);

  virtual wxGridCellAttr* GetAttr (int row, int col, wxGridCellAttr::wxAttrKind kind );

  //virtual void SetValueAsDouble(int row, int col, double value);
  //virtual double GetValueAsDouble(int row, int col);

  virtual wxString GetColLabelValue( int col );

  void FillDelegates( int row, ou::tf::option::Delegates& call, ou::tf::option::Delegates& put );
  void StopStrike( int row );

  void StopWatch();
  void DestroyControls();

  void OnGridLeftClick( wxGridEvent& event );
  void OnGridRightClick( wxGridEvent& event );
  void OnMouseMotion( wxMouseEvent& event );
  void OnGridCellBeginDrag( wxGridEvent& event );

  bool StartDragDrop( DragDropInstrument& );

  wxTimer m_timerGuiRefresh;
  void HandleGuiRefresh( wxTimerEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void GridOptionChain::serialize(Archive & ar, const unsigned int file_version){
    ar & *m_pimpl;
}

} // namespace tf
} // namespace ou