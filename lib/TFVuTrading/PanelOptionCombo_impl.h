/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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

// http://herbsutter.com/gotw/_100/pimpl stuff

#pragma once

#include <vector>

#define FUSION_MAX_VECTOR_SIZE 15

#include <boost/fusion/container/vector/vector20.hpp>
#include <boost/fusion/include/vector20.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/algorithm/transformation/filter.hpp>
#include <boost/fusion/include/filter.hpp>

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/grid.h>

#include <TFVuTrading/DialogSimpleOneLineOrder.h>
#include <TFVuTrading/DialogNewPortfolio.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include <TFVuTrading/DragDropInstrumentTarget.h>

#include "PanelOptionCombo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelOptionCombo_impl {
//public:

  PanelOptionCombo_impl( PanelOptionCombo& );
  virtual ~PanelOptionCombo_impl( void );

//protected:
//private:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Portfolio::pPosition_t pPosition_t;

  void UpdateGui( void );
  void AddPosition( pPosition_t pPosition );
  void AddInstrumentToPosition( pInstrument_t pInstrument );

// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 15
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,            1,            2,         3,      4,             */ \
      (COL_Pos      , "Position",   wxALIGN_LEFT,  100, ModelCellString ), \
      (COL_QuanPend , "#Pend",      wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_SidePend , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_QuanActv , "#Active",    wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_SideActv , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_ConsVlu  , "ConsValue",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_URPL     , "UnRealPL",   wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_RPL      , "RealPL",     wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Comm     , "Comm",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Bid      , "Bid",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Last     , "Last",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Ask      , "Ask",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_ImpVol   , "ImpVol",     wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Delta    , "Delta",      wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Gamma    , "Gamma",      wxALIGN_RIGHT,  50, ModelCellDouble ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class structPosition {
  public:
    structPosition( pPosition_t pPosition, wxGrid& grid, int row )
      : m_pPosition( pPosition ), m_grid( grid ), m_row( row ) {
        Init();
    }
    structPosition( const structPosition& rhs )
      : m_pPosition( rhs.m_pPosition ), m_grid( rhs.m_grid ), m_row( rhs.m_row ) {
      Init();
    }
    ~structPosition( void ) {
      m_pPosition->OnPositionChanged.Remove( MakeDelegate( this, &structPosition::HandleOnPositionChanged ) );
      m_pPosition->OnExecutionRaw.Remove( MakeDelegate( this, &structPosition::HandleOnExecutionRaw ) );
      m_pPosition->OnCommission.Remove( MakeDelegate( this, &structPosition::HandleOnCommission ) );
      m_pPosition->OnUnRealizedPL.Remove( MakeDelegate( this, &structPosition::HandleOnUnRealizedPL ) );
      m_pPosition->OnQuote.Remove( MakeDelegate( this, & structPosition::HandleOnQuote ) );
      m_pPosition->OnTrade.Remove( MakeDelegate( this, &structPosition::HandleOnTrade ) );
    }
    void UpdateGui( void ) {
      //m_pGrid->BeginBatch();
      //m_pGrid->Freeze();
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_grid, m_row ) );
      //m_pGrid->Thaw();
      //m_pGrid->EndBatch();
    }
    pPosition_t GetPosition( void ) { return m_pPosition; }
    void SetPrecision( double dbl ) {
      boost::fusion::for_each( boost::fusion::filter<ModelCellDouble>( m_vModelCells ), ModelCell_ops::SetPrecision( 2 ) );
    }
  private:
    int m_row;
    wxGrid& m_grid;
    pPosition_t m_pPosition;
    vModelCells_t m_vModelCells;
    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      boost::fusion::at_c<COL_Pos>( m_vModelCells ).SetValue( m_pPosition->GetRow().sName );
      m_pPosition->OnPositionChanged.Add( MakeDelegate( this, &structPosition::HandleOnPositionChanged ) );
      m_pPosition->OnExecutionRaw.Add( MakeDelegate( this, &structPosition::HandleOnExecutionRaw ) );
      m_pPosition->OnCommission.Add( MakeDelegate( this, &structPosition::HandleOnCommission ) );
      m_pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &structPosition::HandleOnUnRealizedPL ) );
      m_pPosition->OnQuote.Add( MakeDelegate( this, & structPosition::HandleOnQuote ) );
      m_pPosition->OnTrade.Add( MakeDelegate( this, &structPosition::HandleOnTrade ) );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_row)

      // initialize row of values.
      const Position::TableRowDef& row( m_pPosition->GetRow() );
      boost::fusion::at_c<COL_QuanPend>( m_vModelCells ).SetValue( row.nPositionPending );
      boost::fusion::at_c<COL_SidePend>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSidePending ] );
      boost::fusion::at_c<COL_QuanActv>( m_vModelCells ).SetValue( row.nPositionActive );
      boost::fusion::at_c<COL_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSideActive ] );
      boost::fusion::at_c<COL_ConsVlu>( m_vModelCells ).SetValue( row.dblConstructedValue );
      boost::fusion::at_c<COL_URPL>( m_vModelCells ).SetValue( row.dblUnRealizedPL );
      boost::fusion::at_c<COL_RPL>( m_vModelCells ).SetValue( row.dblRealizedPL );
      boost::fusion::at_c<COL_Comm>( m_vModelCells ).SetValue( row.dblCommissionPaid );
    }

    void HandleOnPositionChanged( const Position& position ) {
      boost::fusion::at_c<COL_QuanPend>( m_vModelCells ).SetValue( m_pPosition->GetRow().nPositionPending );
      boost::fusion::at_c<COL_SidePend>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPosition->GetRow().eOrderSidePending ] );
      boost::fusion::at_c<COL_QuanActv>( m_vModelCells ).SetValue( m_pPosition->GetRow().nPositionActive );
      boost::fusion::at_c<COL_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPosition->GetRow().eOrderSideActive ] );
      boost::fusion::at_c<COL_ConsVlu>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblConstructedValue );
    }

    void HandleOnExecutionRaw( const Position::execution_pair_t& pair ) {
      boost::fusion::at_c<COL_RPL>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblRealizedPL );
    }

    void HandleOnCommission( const Position::PositionDelta_delegate_t& tuple ) {
      boost::fusion::at_c<COL_Comm>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblCommissionPaid );
    }


    void HandleOnUnRealizedPL( const Position::PositionDelta_delegate_t& tuple ) {
      boost::fusion::at_c<COL_URPL>( m_vModelCells ).SetValue( boost::tuples::get<2>( tuple ) );
    }

    void HandleOnTrade( const ou::tf::Trade& trade ) {
      boost::fusion::at_c<COL_Last>( m_vModelCells ).SetValue( trade.Price() );
    }

    void HandleOnQuote( const ou::tf::Quote& quote ) {
      boost::fusion::at_c<COL_Bid>( m_vModelCells ).SetValue( quote.Bid() );
      boost::fusion::at_c<COL_Ask>( m_vModelCells ).SetValue( quote.Ask() );
    }
  };  // structPosition

  typedef std::vector<structPosition> vPositions_t;
  vPositions_t m_vPositions;  // one to one match on rows in grid

  bool m_bDialogActive;
  int m_nRowRightClick;  // row on which right click occurred

    wxBoxSizer* m_sizerMain;
    wxBoxSizer* m_sizerPortfolio;
    wxStaticText* m_lblIdPortfolio;
    wxStaticText* m_lblCurrency;
    wxStaticText* m_lblDescription;
    wxFlexGridSizer* m_gridPortfolioStats;
    wxTextCtrl* m_txtUnRealizedPL;
    wxTextCtrl* m_txtCommission;
    wxTextCtrl* m_txtRealizedPL;
    wxTextCtrl* m_txtTotal;
    wxTextCtrl* m_txtDescription;
    wxGrid* m_gridPositions;

    wxMenu* m_menuGridLabelPositionPopUp;
    wxMenu* m_menuGridCellPositionPopUp;

  PanelOptionCombo& m_ppp; // passed in on construction 

  pPortfolio_t m_pPortfolio;

  //typedef boost::fusion::vector4<ModelCellDouble,ModelCellDouble,ModelCellDouble,ModelCellDouble> vPortfolioValues_t;
  typedef std::vector<ModelCellDouble> vPortfolioValues_t;
  vPortfolioValues_t m_vPortfolioValues;

  //ou::tf::DialogInstrumentSelect::DataExchange m_DialogInstrumentSelect_DataExchange;
  //ou::tf::DialogInstrumentSelect* m_pdialogInstrumentSelect;

  ou::tf::DialogSimpleOneLineOrder::DataExchange m_DialogSimpleOneLineOrder_DataExchange;
  ou::tf::DialogSimpleOneLineOrder* m_pdialogSimpleOneLineOrder;

  ou::tf::DialogNewPortfolio::DataExchange m_DialogNewPortfolio_DataExchange;
  ou::tf::DialogNewPortfolio* m_pdialogNewPortfolio;
  
  DragDropInstrumentTarget m_ddDataInstrumentTarget;

  void CreateControls();
  void SetPortfolio( pPortfolio_t pPortfolio );

  void OnClose( wxCloseEvent& event );

  void OnRightClickGridLabel( wxGridEvent& event );
  void OnRightClickGridCell( wxGridEvent& event );
  void OnPositionPopUpAddPosition( wxCommandEvent& event );
  void OnPositionPopUpAddOrder( wxCommandEvent& event );
  void OnPositionPopUpCancelOrders( wxCommandEvent& event );
  void OnPositionPopUpClosePosition( wxCommandEvent& event );
  void OnPositionPopUpAddPortfolio( wxCommandEvent& event );
  void OnPositionPopUpClosePortfolio( wxCommandEvent& event );

  void OnDialogSimpleOneLineOrderDone( ou::tf::DialogBase::DataExchange* );
  void OnDialogNewPortfolioDone( ou::tf::DialogBase::DataExchange* );

  void HandleOnUnRealizedPLUpdate( const Portfolio& );
  void HandleOnExecutionUpdate( const Portfolio& );
  void HandleOnCommissionUpdate( const Portfolio& );

};



} // namespace tf
} // namespace ou
