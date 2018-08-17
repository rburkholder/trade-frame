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
#include <algorithm>

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

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/grid.h>

#include <TFVuTrading/DialogSimpleOneLineOrder.h>
#include <TFVuTrading/DialogNewPortfolio.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "PanelOptionCombo.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelOptionCombo_impl {
//public:

  PanelOptionCombo_impl( PanelOptionCombo& );
  virtual ~PanelOptionCombo_impl( void );

//protected:
//private:
  
  typedef ou::tf::Instrument::idInstrument_t idInstrument_t;
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  
  typedef ou::tf::option::Option::pOption_t pOption_t;

  typedef ou::tf::PortfolioGreek::pPortfolioGreek_t pPortfolioGreek_t;
  typedef ou::tf::PortfolioGreek::pPositionGreek_t pPositionGreek_t;

  void UpdateGui( void );
  void AddPositionGreek( pPositionGreek_t pPositionGreek );
  //void AddInstrumentToPosition( pInstrument_t pInstrument );
  void AddOptionUnderlyingPosition( pInstrument_t pOption_t, pInstrument_t pUnderlying_t );

  void SaveColumnSizes( ou::tf::GridColumnSizer& ) const;
  void SetColumnSizes( ou::tf::GridColumnSizer& );
	
// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 16
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,            1,            2,         3,      4,             */ \
      (COL_Pos      , "Position",   wxALIGN_LEFT,  100, ModelCellString ), \
      (COL_Quan     , "Quan",       wxALIGN_RIGHT,  50, ModelCellInt ),    \
      (COL_Side     , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_ConsVlu  , "ConsValue",  wxALIGN_RIGHT,  70, ModelCellDouble ), \
      (COL_URPL     , "UnRealPL",   wxALIGN_RIGHT,  70, ModelCellDouble ), \
      (COL_RPL      , "RealPL",     wxALIGN_RIGHT,  70, ModelCellDouble ), \
      (COL_Comm     , "Comm",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Bid      , "Bid",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Last     , "Last",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Ask      , "Ask",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_ImpVol   , "ImpVol",     wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Delta    , "Delta",      wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COL_Gamma    , "Gamma",      wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Theta    , "Theta",      wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Vega     , "Vega",       wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Rho      , "Rho",        wxALIGN_RIGHT,  60, ModelCellDouble ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class structPosition { // ======================================== structPosition
    friend class boost::serialization::access;
  public:
    structPosition( pPositionGreek_t pPositionGreek, wxGrid& grid, int row )
      : m_pPositionGreek( pPositionGreek ), m_grid( grid ), m_rowGrid( row ) {
        Init();
    }
    structPosition( const structPosition& rhs )
      : m_pPositionGreek( rhs.m_pPositionGreek ), m_grid( rhs.m_grid ), m_rowGrid( rhs.m_rowGrid ) {
      Init();
    }
    ~structPosition( void ) {
      m_pPositionGreek->OnPositionChanged.Remove( MakeDelegate( this, &structPosition::HandleOnPositionChanged ) );
      m_pPositionGreek->OnExecutionRaw.Remove( MakeDelegate( this, &structPosition::HandleOnExecutionRaw ) );
      m_pPositionGreek->OnCommission.Remove( MakeDelegate( this, &structPosition::HandleOnCommission ) );
      m_pPositionGreek->OnUnRealizedPL.Remove( MakeDelegate( this, &structPosition::HandleOnUnRealizedPL ) );
      m_pPositionGreek->OnQuote.Remove( MakeDelegate( this, &structPosition::HandleOnQuote ) );
      m_pPositionGreek->OnTrade.Remove( MakeDelegate( this, &structPosition::HandleOnTrade ) );
      m_pPositionGreek->OnGreek.Remove( MakeDelegate( this, &structPosition::HandleOnGreek ) );
    }
    void UpdateGui( void ) {
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_grid, m_rowGrid ) );
    }
    const pPositionGreek_t GetPositionGreek( void ) const { return m_pPositionGreek; }
    void SetPrecision( double dbl ) {  // why a call with double, and not being used?
      boost::fusion::for_each( boost::fusion::filter<ModelCellDouble>( m_vModelCells ), ModelCell_ops::SetPrecision( dbl ) );
    }
    int GetGridRow() const { return m_rowGrid; }
    void SetGridRow( int nRow ) { m_rowGrid = nRow; }
    
  private:
    int m_rowGrid;
    wxGrid& m_grid;
    pPositionGreek_t m_pPositionGreek;
    vModelCells_t m_vModelCells;  // needs to be changed to unique_ptr so doesn't change, or use move semantics? (due to background thread processing)
    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      boost::fusion::at_c<COL_Pos>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().sName );
      m_pPositionGreek->OnPositionChanged.Add( MakeDelegate( this, &structPosition::HandleOnPositionChanged ) );
      m_pPositionGreek->OnExecutionRaw.Add( MakeDelegate( this, &structPosition::HandleOnExecutionRaw ) );
      m_pPositionGreek->OnCommission.Add( MakeDelegate( this, &structPosition::HandleOnCommission ) );
      m_pPositionGreek->OnUnRealizedPL.Add( MakeDelegate( this, &structPosition::HandleOnUnRealizedPL ) );
      m_pPositionGreek->OnQuote.Add( MakeDelegate( this, &structPosition::HandleOnQuote ) );
      m_pPositionGreek->OnTrade.Add( MakeDelegate( this, &structPosition::HandleOnTrade ) );
      m_pPositionGreek->OnGreek.Add( MakeDelegate( this, &structPosition::HandleOnGreek ) );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_rowGrid)

      // initialize row of values.
      const Position::TableRowDef& row( m_pPositionGreek->GetRow() );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( row.nPositionPending );
      boost::fusion::at_c<COL_Side>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSidePending ] );
//      boost::fusion::at_c<COL_QuanActv>( m_vModelCells ).SetValue( row.nPositionActive );
//      boost::fusion::at_c<COL_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSideActive ] );
      boost::fusion::at_c<COL_ConsVlu>( m_vModelCells ).SetValue( row.dblConstructedValue );
      boost::fusion::at_c<COL_URPL>( m_vModelCells ).SetValue( row.dblUnRealizedPL );
      boost::fusion::at_c<COL_RPL>( m_vModelCells ).SetValue( row.dblRealizedPL );
      boost::fusion::at_c<COL_Comm>( m_vModelCells ).SetValue( row.dblCommissionPaid );
    }

    void HandleOnPositionChanged( const Position& position ) {
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().nPositionPending );
      boost::fusion::at_c<COL_Side>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPositionGreek->GetRow().eOrderSidePending ] );
//      boost::fusion::at_c<COL_QuanActv>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().nPositionActive );
//      boost::fusion::at_c<COL_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPositionGreek->GetRow().eOrderSideActive ] );
      boost::fusion::at_c<COL_ConsVlu>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().dblConstructedValue );
    }

    void HandleOnExecutionRaw( const Position::execution_pair_t& pair ) {
      boost::fusion::at_c<COL_RPL>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().dblRealizedPL );
    }

    void HandleOnCommission( const Position::PositionDelta_delegate_t& tuple ) {
      boost::fusion::at_c<COL_Comm>( m_vModelCells ).SetValue( m_pPositionGreek->GetRow().dblCommissionPaid );
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
    
    void HandleOnGreek( const ou::tf::Greek& greek ) {
      boost::fusion::at_c<COL_ImpVol>( m_vModelCells ).SetValue( greek.ImpliedVolatility() );
      boost::fusion::at_c<COL_Delta>( m_vModelCells ).SetValue( greek.Delta() ); //Delta – Sensitivity to Underlying's Price
      boost::fusion::at_c<COL_Gamma>( m_vModelCells ).SetValue( greek.Gamma() ); //Gamma – Sensitivity to Delta
      boost::fusion::at_c<COL_Theta>( m_vModelCells ).SetValue( greek.Theta() ); //Theta – Sensitivity to Time Decay
      boost::fusion::at_c<COL_Vega>( m_vModelCells ).SetValue( greek.Vega() ); //Vega – Sensitivity to Underlying's Volatility
      boost::fusion::at_c<COL_Rho>( m_vModelCells ).SetValue( greek.Rho() ); //Rho - Sensitivity to risk-free rate of interest
    }
    
    template<typename Archive>
    void save( Archive& ar, const unsigned int version ) const {
      ar & *m_pPositionGreek;
    }

    template<typename Archive>
    void load( Archive& ar, const unsigned int version ) {
      ar & *m_pPositionGreek;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
  };  // ======================================== structPosition
  

  typedef std::unique_ptr<structPosition> pstructPositionGreek_t;
  typedef std::vector<pstructPositionGreek_t> vPositions_t;
  vPositions_t m_vPositions;  // one to one match on rows in grid

  bool m_bDialogActive;
  int m_nRowRightClick;  // row on which right click occurred

    wxBoxSizer* m_sizerMain;
    wxBoxSizer* m_sizerHeader;
    wxStaticText* m_lblCurrency;
    wxStaticText* m_lblIdPortfolio;
    wxTextCtrl* m_txtDescription;
    wxGrid* m_gridPositions;
    wxGrid* m_gridPortfolioStats;
    
    wxSizerItem* m_siPosition;
    wxSizerItem* m_siPortfolioStats;

    wxMenu* m_menuGridLabelPositionPopUp;
    wxMenu* m_menuGridCellPositionPopUp;

  PanelOptionCombo& m_poc; // passed in on construction 

  pPortfolioGreek_t m_pPortfolioGreek;
  
  typedef std::vector<double> vPortfolioCalcs_t;
  vPortfolioCalcs_t m_vPortfolioCalcs;

  //typedef boost::fusion::vector4<ModelCellDouble,ModelCellDouble,ModelCellDouble,ModelCellDouble> vPortfolioValues_t;
  typedef std::vector<ModelCellDouble> vPortfolioValues_t;
  vPortfolioValues_t m_vPortfolioModelCell;  // holds dblUnRealized, dblRealized, dblCommissionsPaid, dblTotal

  ou::tf::DialogSimpleOneLineOrder::DataExchange m_DialogSimpleOneLineOrder_DataExchange;
  ou::tf::DialogSimpleOneLineOrder* m_pdialogSimpleOneLineOrder;

  ou::tf::DialogNewPortfolio::DataExchange m_DialogNewPortfolio_DataExchange;
  ou::tf::DialogNewPortfolio* m_pdialogNewPortfolio;
  
  void CreateControls();
  void SetPortfolioGreek( pPortfolioGreek_t pPortfolioGreek );

  void OnMouseWheel( wxMouseEvent& event );
  void OnRightClickGridLabel( wxGridEvent& event );
  void OnRightClickGridCell( wxGridEvent& event );
  void OnGridColSize( wxGridSizeEvent& event );
  void OnPositionPopUpAddPosition( wxCommandEvent& event );
  void OnPositionPopUpDeletePosition( wxCommandEvent& event );
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
  
  void HandleWindowDestroy( wxWindowDestroyEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {

    ar & m_vPositions.size();
    std::for_each( m_vPositions.begin(), m_vPositions.end(), 
      [&ar](const vPositions_t::value_type& vt){
        const std::string sO( vt->GetPositionGreek()->GetOption()->GetInstrument()->GetInstrumentName() );
        ar & sO;
        const std::string sU( vt->GetPositionGreek()->GetUnderlying()->GetInstrument()->GetInstrumentName() );
        ar & sU;
        ar & *vt;
    } );
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {

    if ( 2 <= version ) {
      vPositions_t::size_type cntPositions;
      ar & cntPositions;
      for ( vPositions_t::size_type cnt = 0; cnt < cntPositions; cnt++ ) {

        idInstrument_t idOptionInstrument;
        ar & idOptionInstrument;
        pInstrument_t pOptionInstrument;
        m_poc.m_fLookUpInstrument( idOptionInstrument, pOptionInstrument );
        assert( nullptr != pOptionInstrument.get() );

        idInstrument_t idUnderlyingInstrument;
        pInstrument_t pUnderlyingInstrument;
        ar & idUnderlyingInstrument;
        m_poc.m_fLookUpInstrument( idUnderlyingInstrument, pUnderlyingInstrument );
        assert( nullptr != pUnderlyingInstrument.get() );

        AddOptionUnderlyingPosition( pOptionInstrument, pUnderlyingInstrument );
        
        if ( 3 <= version ) {
          ar & (*m_vPositions.back());  // assumes AddPositionGreek does a push back, and is still current
        }
        
        
      }
      //m_poc.Layout();
      //m_sizerMain->CalcMin();
      //m_sizerMain->RecalcSizes();
      m_sizerMain->Layout();
    }

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelOptionCombo_impl, 3)
BOOST_CLASS_VERSION(ou::tf::PanelOptionCombo_impl::structPosition, 1)
