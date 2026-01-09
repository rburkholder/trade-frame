/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    OptionOrderModel_impl.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: 2026/01/08 19:56:36
 */

// loosely based upon lib/TFVuTrading/GridOptionComboOrder_impl.hpp

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

#include <TFTrading/Watch.h>
#include <TFOptions/Option.h>

#include <TFTrading/Order.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class OptionOrderView;
class OptionOrderModel;

class OptionOrderModel_impl {
  friend OptionOrderModel;
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  OptionOrderModel_impl( OptionOrderModel& );
  virtual ~OptionOrderModel_impl();

  void Add( pWatch_t&, ou::tf::OrderSide::EOrderSide, int quantity ); // underlying
  void Add( pOption_t&, ou::tf::OrderSide::EOrderSide, int quantity ); // option

  using fOrderLeg_t = std::function<void(ou::tf::OrderSide::EOrderSide side, int quan, double price, const std::string& sIQFeedName)>;
  fOrderLeg_t FactoryAddComboOrderLeg();

  using fIterateLegs_t = std::function<void( fOrderLeg_t&& )>;
  using fGatherOrderLegs_t = std::function<void( fIterateLegs_t&& )>;
  void Set( fGatherOrderLegs_t&& );

protected:

private:

  OptionOrderModel& m_OptionOrderModel;

  wxGrid* m_pGrid;

  fGatherOrderLegs_t m_fGatherOrderLegs;

  // for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
  #define GRID_ARRAY_PARAM_COUNT 5
  #define GRID_ARRAY_COL_COUNT 9
  #define GRID_ARRAY \
    (GRID_ARRAY_COL_COUNT,  \
      ( /* Col 0,         1,            2,       3,      4,          */ \
        (COL_OrderSide, "OSide", wxALIGN_RIGHT,  50, ModelCellInt    ), \
        (COL_Quan,      "Quan",  wxALIGN_RIGHT,  50, ModelCellInt    ), \
        (COL_Name,      "Name",  wxALIGN_LEFT , 120, ModelCellString ), \
        (COL_Last,      "Last",  wxALIGN_RIGHT , 50, ModelCellDouble ), \
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

  struct OptionOrderRow {

    enum EType { summary, underlying, option } m_type;

    // one or the other depending upon EType
    pWatch_t m_pWatch; // underlying
    pOption_t m_pOption; // option

    vModelCells_t m_vModelCells;

    OptionOrderRow(): m_type( EType::summary ) {}

    OptionOrderRow( pWatch_t pWatch, ou::tf::OrderSide::EOrderSide side, int quantity )
    : m_type( EType::underlying )
    , m_pWatch( pWatch )
    {
      Init();
      boost::fusion::at_c<COL_Name>( m_vModelCells ).SetValue( m_pWatch->GetInstrumentName() );
      boost::fusion::at_c<COL_OrderSide>( m_vModelCells ).SetValue( side );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( quantity );

      m_pWatch->OnTrade.Add( MakeDelegate( this, &OptionOrderRow::UpdateTrade ) );
      m_pWatch->OnQuote.Add( MakeDelegate( this, &OptionOrderRow::UpdateQuote ) );
    }

    OptionOrderRow( pOption_t pOption, ou::tf::OrderSide::EOrderSide side, int quantity )
    : m_type( EType::underlying )
    , m_pOption( pOption )
    {
      Init();
      boost::fusion::at_c<COL_Name>( m_vModelCells ).SetValue( m_pOption->GetInstrumentName() );
      boost::fusion::at_c<COL_OrderSide>( m_vModelCells ).SetValue( side );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( quantity );

      m_pOption->OnTrade.Add( MakeDelegate( this, &OptionOrderRow::UpdateTrade ) );
      m_pOption->OnQuote.Add( MakeDelegate( this, &OptionOrderRow::UpdateQuote ) );
      m_pOption->OnGreek.Add( MakeDelegate( this, &OptionOrderRow::UpdateGreeks ) );
    }

    OptionOrderRow( const OptionOrderRow& rhs ) = delete;

    OptionOrderRow( OptionOrderRow&& rhs )
    : m_type( rhs.m_type )
    , m_pOption( std::move( rhs.m_pOption ) ), m_pWatch( std::move( rhs.m_pWatch ) )
    , m_vModelCells( std::move( rhs.m_vModelCells ) )
    {
      Init();
      boost::fusion::at_c<COL_OrderSide>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_OrderSide>( rhs.m_vModelCells ).GetValue() );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Quan>( rhs.m_vModelCells ).GetValue() );
      boost::fusion::at_c<COL_Name>( m_vModelCells ).SetValue( boost::fusion::at_c<COL_Name>( rhs.m_vModelCells ).GetValue() );
    }

    ~OptionOrderRow() {
      if ( m_pWatch ) {
        m_pWatch->OnTrade.Remove( MakeDelegate( this, &OptionOrderRow::UpdateTrade ) );
        m_pWatch->OnQuote.Remove( MakeDelegate( this, &OptionOrderRow::UpdateQuote ) );
        m_pWatch.reset();
      }
      if ( m_pOption ) {
        m_pOption->OnTrade.Remove( MakeDelegate( this, &OptionOrderRow::UpdateTrade ) );
        m_pOption->OnQuote.Remove( MakeDelegate( this, &OptionOrderRow::UpdateQuote ) );
        m_pOption->OnGreek.Remove( MakeDelegate( this, &OptionOrderRow::UpdateGreeks ) );
        m_pOption.reset();
      }
    }

    void Init() {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
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

    void UpdateTrade( const ou::tf::Trade& trade ) {
      boost::fusion::at_c<COL_Last>( m_vModelCells ).SetValue( trade.Price() );
    }
  };

  using pOptionOrderRow_t = std::unique_ptr<OptionOrderRow>;
  using vOptionOrderRow_t = std::vector<pOptionOrderRow_t>;
  vOptionOrderRow_t m_vOptionOrderRow;

  using fAdd_t = std::function<pOptionOrderRow_t()>;
  void Add( OptionOrderRow::EType, const std::string&, ou::tf::OrderSide::EOrderSide side, int quantity, fAdd_t&& );

  void CreateControls();
  void DestroyControls();

  void Refresh();

  void ClearRows();
  void PlaceComboOrder();

  void SetView ( wxGrid* );

  int GetNumberRows();
  int GetNumberCols();
  bool IsEmptyCell( int row, int col );

  wxString GetValue( int row, int col );
  void SetValue( int row, int col, const wxString &value );

  wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind );

  wxString GetColLabelValue( int col );

};

} // namespace tf
} // namespace ou
