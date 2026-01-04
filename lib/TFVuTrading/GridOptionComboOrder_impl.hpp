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
 * File:    GridOptionComboOrder_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: 2023/04/23 17:28:15
 */

#pragma once

#include <vector>

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

#include <TFTimeSeries/DatedDatum.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "GridOptionComboOrder.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

class GridOptionComboOrder_impl
: public wxGridTableBase {
  friend GridOptionComboOrder;
public:

  GridOptionComboOrder_impl( GridOptionComboOrder& );
  virtual ~GridOptionComboOrder_impl();

  void Add( ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sName );

protected:

private:

  GridOptionComboOrder& m_grid;

  // for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
  #define GRID_ARRAY_PARAM_COUNT 5
  #define GRID_ARRAY_COL_COUNT 9
  #define GRID_ARRAY \
    (GRID_ARRAY_COL_COUNT,  \
      ( /* Col 0,         1,            2,       3,      4,          */ \
        (COL_OrderSide, "OSide", wxALIGN_RIGHT,  50, ModelCellInt    ), \
        (COL_Quan,      "Quan",  wxALIGN_RIGHT,  50, ModelCellInt    ), \
        (COL_Price,     "Price", wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Name,      "Name",  wxALIGN_LEFT , 120, ModelCellString ), \
        (COL_Bid,       "Bid",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Ask,       "Ask",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Delta,     "Delta", wxALIGN_RIGHT,  50, ModelCellDouble ), \
        (COL_Gamma,     "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
        (COL_IV,        "IV",    wxALIGN_RIGHT,  50, ModelCellDouble ), \
        ) \
      )

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  using vModelCells_t = boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  >;

  struct OptionComboOrderRow {

    enum EType { empty, summary, item } m_type;

    int m_nRow;
    vModelCells_t m_vModelCells;

    OptionComboOrderRow()
    : m_type( EType::empty ), m_nRow {}
    {
      Init();
    }
    OptionComboOrderRow( ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sName )
    : m_type( EType::item ), m_nRow {}
    {
      Init();
      boost::fusion::at_c<COL_OrderSide>( m_vModelCells ).SetValue( side );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( quan );
      boost::fusion::at_c<COL_Price>( m_vModelCells ).SetValue( price );
      boost::fusion::at_c<COL_Name>( m_vModelCells ).SetValue( sName );
    }
    OptionComboOrderRow( const OptionComboOrderRow& rhs ) = delete;
    OptionComboOrderRow( OptionComboOrderRow&& rhs )
    : m_type( rhs.m_type ), m_nRow( rhs.m_nRow )
    {
      Init();
      boost::fusion::at_c<COL_OrderSide>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_OrderSide>( rhs.m_vModelCells ).GetValue() );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Quan>( rhs.m_vModelCells ).GetValue() );
      boost::fusion::at_c<COL_Price>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Price>( rhs.m_vModelCells ).GetValue() );
      boost::fusion::at_c<COL_Name>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Name>( rhs.m_vModelCells ).GetValue() );
    }
    ~OptionComboOrderRow() = default;

    void Init() {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      //BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_nRow) // performed elsewhere
      boost::fusion::at_c<COL_IV>( m_vModelCells ).SetPrecision( 3 );
      boost::fusion::at_c<COL_Delta>( m_vModelCells ).SetPrecision( 3 );
      boost::fusion::at_c<COL_Gamma>( m_vModelCells ).SetPrecision( 4 );
    }

    void UpdateGreeks( const ou::tf::Greek& greek ) {
      boost::fusion::at_c<COL_IV>( m_vModelCells ).SetValue( greek.ImpliedVolatility() );
      boost::fusion::at_c<COL_Delta>( m_vModelCells ).SetValue( greek.Delta() );
      boost::fusion::at_c<COL_Gamma>( m_vModelCells ).SetValue( greek.Gamma() );
    }

    void UpdateQuote( const ou::tf::Quote& quote ) {
      boost::fusion::at_c<COL_Bid>( m_vModelCells ).SetValue( quote.Bid() );
      boost::fusion::at_c<COL_Ask>( m_vModelCells ).SetValue( quote.Ask() );
    }

    void UpdateTrade( const ou::tf::Trade& trade ) {}
  };

  using vOptionComboOrderRow_t = std::vector<OptionComboOrderRow>;
  vOptionComboOrderRow_t m_vOptionComboOrderRow;

  void CreateControls();
  void DestroyControls();

  void Refresh();

  void ClearRows();
  void PlaceComboOrder();

  virtual void SetView ( wxGrid *grid ) override;
  virtual wxGrid* GetView() const override;

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;
  virtual bool IsEmptyCell( int row, int col ) override;

  virtual bool InsertRows( size_t pos = 0, size_t numRows = 1 ) override;
  virtual bool AppendRows( size_t numRows = 1 ) override;

  virtual wxString GetValue( int row, int col ) override;
  virtual void SetValue( int row, int col, const wxString &value ) override;

  virtual wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind ) override;

  //virtual void SetValueAsDouble(int row, int col, double value);
  //virtual double GetValueAsDouble(int row, int col);

  virtual wxString GetColLabelValue( int col ) override;

  bool StartDragDrop( DragDropInstrument& );

};

} // namespace tf
} // namespace ou
