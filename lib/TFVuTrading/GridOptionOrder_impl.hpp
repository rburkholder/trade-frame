/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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

/*
 * File:    GridOptionOrder_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/23 17:28:15
 */

#pragma once

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

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "GridOptionOrder.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct GridOptionOrder_impl: public wxGridTableBase {
//public:
  GridOptionOrder_impl( GridOptionOrder& );
  virtual ~GridOptionOrder_impl();
//protected:

//private:

  GridOptionOrder& m_details;

  // for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
  #define GRID_ARRAY_PARAM_COUNT 5
  #define GRID_ARRAY_COL_COUNT 9
  #define GRID_ARRAY \
    (GRID_ARRAY_COL_COUNT,  \
      ( /* Col 0,         1,            2,       3,      4,          */ \
        (COL_OrderSide, "OSide", wxALIGN_RIGHT,  50, ModelCellString ), \
        (COL_Quan,      "Quan",  wxALIGN_RIGHT,  50, ModelCellInt    ), \
        (COL_Price,     "Price", wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Name,      "Name",  wxALIGN_LEFT ,  70, ModelCellString ), \
        (COL_Bid,       "Bid",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Ask,       "Ask",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_IV,        "IV",    wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Delta,     "Delta", wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Gamma,     "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
        ) \
      )

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  using vModelCells_t = boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  >;

  bool m_bTimerActive;
  wxTimer m_timerGuiRefresh;
  void HandleGuiRefresh( wxTimerEvent& event );

  void CreateControls();
  void DestroyControls();

  void Start();
  void Stop();

  virtual void SetView ( wxGrid *grid );
  virtual wxGrid* GetView() const;

  virtual int GetNumberRows();
  virtual int GetNumberCols();
  virtual bool IsEmptyCell( int row, int col );

  virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 );
  virtual bool AppendRows( size_t numRows = 1 );

  virtual wxString GetValue( int row, int col );
  virtual void SetValue( int row, int col, const wxString &value );

  virtual wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind );

  //virtual void SetValueAsDouble(int row, int col, double value);
  //virtual double GetValueAsDouble(int row, int col);

  virtual wxString GetColLabelValue( int col );

  bool StartDragDrop( DragDropInstrument& );

};

} // namespace tf
} // namespace ou
