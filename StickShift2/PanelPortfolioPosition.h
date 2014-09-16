/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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
// Started 2013/11/06

#pragma once

#include <vector>

#include <wx/grid.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

// may need to hide this away, there are other panels with other field counts to be loaded
#ifdef FUSION_MAX_VECTOR_SIZE
#undef FUSION_MAX_VECTOR_SIZE
#endif

#define FUSION_MAX_VECTOR_SIZE 15

//#include <boost/fusion/container/vector.hpp>
//#include <boost/fusion/include/vector.hpp>
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

#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/array/elem.hpp>
#include <boost/preprocessor/array/size.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/cat.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTrading/Portfolio.h>

#include <TFVuTrading/DialogInstrumentSelect.h>
#include <TFVuTrading/DialogSimpleOneLineOrder.h>
#include <TFVuTrading/DialogNewPortfolio.h>
#include <TFVuTrading/ModelCell.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_PORTFOLIOPOSITION_TITLE _("Panel Portfolio Position")
#define SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME ID_PANEL_PORTFOLIOPOSITION
#define SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE wxSize(-1, -1)
#define SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION wxDefaultPosition

namespace PanelPortfolioPosition_detail {

  struct UpdateGui {
    wxGrid* m_pGrid;
    int m_row;
    UpdateGui( wxGrid* pGrid, int row ): m_pGrid( pGrid ), m_row( row ) {};
    template<typename T>
    void operator()( T& t ) const {
      // todo:  deal with flicker by double-buffering?
      if ( t.Changed() ) {
        m_pGrid->SetCellValue( t.GetText(), m_row, t.GetCol() );
      }
    }
  };

  struct SetCol {
    typedef int result_type;
    SetCol( void ) {};
    template<typename F, typename T>
    int operator()( F& f, T& t ) const {
      return 1 + t.SetCol( f );
    }
  };

  struct SetPrecision {
    unsigned int m_nPrecision;
    SetPrecision( unsigned int val ): m_nPrecision( val ) {};
    template<typename T>
    void operator()( T& t ) const {
      t.SetPrecision( m_val );
    }
  };

}

class PanelPortfolioPosition: public wxPanel {
public:

  typedef ou::tf::Portfolio::idPortfolio_t idPortfolio_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Portfolio::idPosition_t idPosition_t;
  typedef ou::tf::Portfolio::pPosition_t pPosition_t;
  typedef ou::tf::DialogInstrumentSelect::DelegateNameLookup_t DelegateNameLookup_t;

  typedef FastDelegate1<pPosition_t,void> DelegateAddPosition_t;
  typedef FastDelegate3<const std::string&,pPortfolio_t,DelegateAddPosition_t,void> DelegateConstructPosition_t;
  typedef FastDelegate3<PanelPortfolioPosition&,const std::string&, const std::string&> DelegateConstructPortfolio_t;

  PanelPortfolioPosition(void);
  PanelPortfolioPosition( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION, 
    const wxSize& size = SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE, 
    long style = SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE );
  ~PanelPortfolioPosition(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION, 
    const wxSize& size = SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE, 
    long style = SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE );

  void SetPortfolio( pPortfolio_t pPortfolio );
  pPortfolio_t& GetPortfolio( void ) { return m_pPortfolio; }

  void SetNameLookup( DelegateNameLookup_t function ) { m_DialogInstrumentSelect_DataExchange.lookup = function; };
  void SetConstructPosition( DelegateConstructPosition_t function ) { m_delegateConstructPosition = function; };
  void SetConstructPortfolio( DelegateConstructPortfolio_t function ) { m_delegateConstructPortfolio = function; };

  void AddPosition( pPosition_t pPosition ); // constructed from supplied symbol name

  void UpdateGui( void );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:

  void Init();
  void CreateControls();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_PORTFOLIOPOSITION, 
    ID_LblIdPortfolio, ID_LblCurrency, ID_LblDescription, ID_LblUnrealizedPL, ID_LblCommission, ID_LblRealizedPL, ID_LblTotal,
    ID_TxtDescription,
    ID_TxtUnRealizedPL, ID_TxtCommission, ID_TxtRealizedPL, ID_TxtTotal,
    ID_MenuAddPosition, ID_MenuClosePosition, ID_MenuCancelOrders, ID_MenuAddOrder,
    ID_MenuAddPortfolio, ID_MenuClosePortfolio,
    ID_GridPositions
  };

// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_POSITION_ARRAY_PARAM_COUNT 5
#define GRID_POSITION_ARRAY_COL_COUNT 15
#define GRID_POSITION_ARRAY \
  (GRID_POSITION_ARRAY_COL_COUNT,  \
    ( /* Col 0,                       1,            2,         3,      4,             */ \
      (GRID_POSITION_Pos      , "Position",   wxALIGN_LEFT,  100, ModelCellString ), \
      (GRID_POSITION_QuanPend , "#Pend",      wxALIGN_RIGHT,  50, ModelCellInt ), \
      (GRID_POSITION_SidePend , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (GRID_POSITION_QuanActv , "#Active",    wxALIGN_RIGHT,  50, ModelCellInt ), \
      (GRID_POSITION_SideActv , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (GRID_POSITION_ConsVlu  , "ConsValue",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (GRID_POSITION_URPL     , "UnRealPL",   wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (GRID_POSITION_RPL      , "RealPL",     wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (GRID_POSITION_Comm     , "Comm",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_Bid      , "Bid",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_Last     , "Last",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_Ask      , "Ask",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_ImpVol   , "ImpVol",     wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_Delta    , "Delta",      wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (GRID_POSITION_Gamma    , "Gamma",      wxALIGN_RIGHT,  50, ModelCellDouble ), \
      ) \
    ) \
  /**/

#define GRID_POSITION_EXTRACT_COL_DETAILS(z, row, col) \
  BOOST_PP_TUPLE_ELEM( \
    GRID_POSITION_ARRAY_PARAM_COUNT, col, \
      BOOST_PP_ARRAY_ELEM( row, GRID_POSITION_ARRAY ) \
    )

// if n is 0, then no comma, ie, prepends comma except on first element, col is column number to extract
#define GRID_POSITION_EXTRACT_ENUM_LIST(z, n, col) \
  BOOST_PP_COMMA_IF(n) \
  GRID_POSITION_EXTRACT_COL_DETAILS( z, n, col )

#define GRID_POSITION_EMIT_SetColSettings( z, n, VAR ) \
  m_gridPositions->SetColLabelValue( VAR, _T(GRID_POSITION_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_gridPositions->SetColSize( VAR++, GRID_POSITION_EXTRACT_COL_DETAILS(z, n, 3) );

#define GRID_POSITION_CELL_ALIGNMENT( z, n, VAR ) \
  m_pGrid->SetCellAlignment( VAR, GRID_POSITION_EXTRACT_COL_DETAILS(z, n, 0), GRID_POSITION_EXTRACT_COL_DETAILS(z, n, 2), wxALIGN_CENTRE );

  enum {
    BOOST_PP_REPEAT(GRID_POSITION_ARRAY_COL_COUNT,GRID_POSITION_EXTRACT_ENUM_LIST,0)
  };

  template<typename ModelCell>
  class CellInfo_t: public ModelCell {
  public:
    CellInfo_t( void ): m_col( 0 ) {};
    CellInfo_t( int col ): m_col( col ) {};
    virtual ~CellInfo_t( void ) {  }
    int SetCol( int col ) { m_col = col; return m_col; }
    int GetCol( void ) const { return m_col; }
  private:
    int m_col;
  };

#define COMPOSE_MODEL_CELL(z,n,col)\
  BOOST_PP_COMMA_IF(n)\
  CellInfo_t<GRID_POSITION_EXTRACT_COL_DETAILS(z,n,col)>

#define VECTOR_DEF BOOST_PP_CAT( vector, GRID_POSITION_ARRAY_COL_COUNT )

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_POSITION_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class structPosition {
  public:
    structPosition( pPosition_t pPosition, wxGrid* pGrid, int row )
      : m_pPosition( pPosition ), m_pGrid( pGrid ), m_row( row ) {
        Init();
    }
    structPosition( const structPosition& rhs )
      : m_pPosition( rhs.m_pPosition ), m_pGrid( rhs.m_pGrid ), m_row( rhs.m_row ) {
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
      boost::fusion::for_each( m_vModelCells, PanelPortfolioPosition_detail::UpdateGui( m_pGrid, m_row ) );
      //m_pGrid->Thaw();
      //m_pGrid->EndBatch();
    }
    pPosition_t GetPosition( void ) { return m_pPosition; }
    void SetPrecision( double dbl ) {
      boost::fusion::for_each( boost::fusion::filter<ModelCellDouble>( m_vModelCells ), PanelPortfolioPosition_detail::SetPrecision( 2 ) );
      
    }
  private:
    int m_row;
    wxGrid* m_pGrid;
    pPosition_t m_pPosition;
    vModelCells_t m_vModelCells;
    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, PanelPortfolioPosition_detail::SetCol() );
      boost::fusion::at_c<GRID_POSITION_Pos>( m_vModelCells ).SetValue( m_pPosition->GetRow().sName );
      m_pPosition->OnPositionChanged.Add( MakeDelegate( this, &structPosition::HandleOnPositionChanged ) );
      m_pPosition->OnExecutionRaw.Add( MakeDelegate( this, &structPosition::HandleOnExecutionRaw ) );
      m_pPosition->OnCommission.Add( MakeDelegate( this, &structPosition::HandleOnCommission ) );
      m_pPosition->OnUnRealizedPL.Add( MakeDelegate( this, &structPosition::HandleOnUnRealizedPL ) );
      m_pPosition->OnQuote.Add( MakeDelegate( this, & structPosition::HandleOnQuote ) );
      m_pPosition->OnTrade.Add( MakeDelegate( this, &structPosition::HandleOnTrade ) );
      BOOST_PP_REPEAT(GRID_POSITION_ARRAY_COL_COUNT,GRID_POSITION_CELL_ALIGNMENT,m_row)

      // initialize row of values.
      const Position::TableRowDef& row( m_pPosition->GetRow() );
      boost::fusion::at_c<GRID_POSITION_QuanPend>( m_vModelCells ).SetValue( row.nPositionPending );
      boost::fusion::at_c<GRID_POSITION_SidePend>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSidePending ] );
      boost::fusion::at_c<GRID_POSITION_QuanActv>( m_vModelCells ).SetValue( row.nPositionActive );
      boost::fusion::at_c<GRID_POSITION_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ row.eOrderSideActive ] );
      boost::fusion::at_c<GRID_POSITION_ConsVlu>( m_vModelCells ).SetValue( row.dblConstructedValue );
      boost::fusion::at_c<GRID_POSITION_URPL>( m_vModelCells ).SetValue( row.dblUnRealizedPL );
      boost::fusion::at_c<GRID_POSITION_RPL>( m_vModelCells ).SetValue( row.dblRealizedPL );
      boost::fusion::at_c<GRID_POSITION_Comm>( m_vModelCells ).SetValue( row.dblCommissionPaid );
    }

    void HandleOnPositionChanged( const Position& position ) {
      boost::fusion::at_c<GRID_POSITION_QuanPend>( m_vModelCells ).SetValue( m_pPosition->GetRow().nPositionPending );
      boost::fusion::at_c<GRID_POSITION_SidePend>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPosition->GetRow().eOrderSidePending ] );
      boost::fusion::at_c<GRID_POSITION_QuanActv>( m_vModelCells ).SetValue( m_pPosition->GetRow().nPositionActive );
      boost::fusion::at_c<GRID_POSITION_SideActv>( m_vModelCells ).SetValue( OrderSide::Name[ m_pPosition->GetRow().eOrderSideActive ] );
      boost::fusion::at_c<GRID_POSITION_ConsVlu>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblConstructedValue );
    }

    void HandleOnExecutionRaw( const Position::execution_pair_t& pair ) {
      boost::fusion::at_c<GRID_POSITION_RPL>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblRealizedPL );
    }

    void HandleOnCommission( const Position::PositionDelta_delegate_t& tuple ) {
      boost::fusion::at_c<GRID_POSITION_Comm>( m_vModelCells ).SetValue( m_pPosition->GetRow().dblCommissionPaid );
    }


    void HandleOnUnRealizedPL( const Position::PositionDelta_delegate_t& tuple ) {
      boost::fusion::at_c<GRID_POSITION_URPL>( m_vModelCells ).SetValue( boost::tuples::get<2>( tuple ) );
    }

    void HandleOnTrade( const ou::tf::Trade& trade ) {
      boost::fusion::at_c<GRID_POSITION_Last>( m_vModelCells ).SetValue( trade.Price() );
    }

    void HandleOnQuote( const ou::tf::Quote& quote ) {
      boost::fusion::at_c<GRID_POSITION_Bid>( m_vModelCells ).SetValue( quote.Bid() );
      boost::fusion::at_c<GRID_POSITION_Ask>( m_vModelCells ).SetValue( quote.Ask() );
    }
  };

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

  pPortfolio_t m_pPortfolio;
  DelegateConstructPosition_t m_delegateConstructPosition;  // used to construct the Position
  DelegateConstructPortfolio_t m_delegateConstructPortfolio;  // used to construct the Portfolio

  //typedef boost::fusion::vector4<ModelCellDouble,ModelCellDouble,ModelCellDouble,ModelCellDouble> vPortfolioValues_t;
  typedef std::vector<ModelCellDouble> vPortfolioValues_t;
  vPortfolioValues_t m_vPortfolioValues;

  ou::tf::DialogInstrumentSelect::DataExchange m_DialogInstrumentSelect_DataExchange;
  ou::tf::DialogInstrumentSelect* m_pdialogInstrumentSelect;

  ou::tf::DialogSimpleOneLineOrder::DataExchange m_DialogSimpleOneLineOrder_DataExchange;
  ou::tf::DialogSimpleOneLineOrder* m_pdialogSimpleOneLineOrder;

  ou::tf::DialogNewPortfolio::DataExchange m_DialogNewPortfolio_DataExchange;
  ou::tf::DialogNewPortfolio* m_pdialogNewPortfolio;

  void OnClose( wxCloseEvent& event );

  void OnRightClickGridLabel( wxGridEvent& event );
  void OnRightClickGridCell( wxGridEvent& event );
  void OnPositionPopUpAddPosition( wxCommandEvent& event );
  void OnPositionPopUpAddOrder( wxCommandEvent& event );
  void OnPositionPopUpCancelOrders( wxCommandEvent& event );
  void OnPositionPopUpClosePosition( wxCommandEvent& event );
  void OnPositionPopUpAddPortfolio( wxCommandEvent& event );
  void OnPositionPopUpClosePortfolio( wxCommandEvent& event );

  void OnDialogInstrumentSelectDone( ou::tf::DialogBase::DataExchange* );
  void OnDialogSimpleOneLineOrderDone( ou::tf::DialogBase::DataExchange* );
  void OnDialogNewPortfolioDone( ou::tf::DialogBase::DataExchange* );

  void HandleOnUnRealizedPLUpdate( const Portfolio& );
  void HandleOnExecutionUpdate( const Portfolio& );
  void HandleOnCommissionUpdate( const Portfolio& );

};


} // namespace tf
} // namespace ou
