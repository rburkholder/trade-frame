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

#define FUSION_MAX_VECTOR_SIZE 12
//#include <boost/fusion/container/vector.hpp>
//#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/container/vector/vector20.hpp>
#include <boost/fusion/include/vector20.hpp>

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
#include <TFVuTrading/ModelCell.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_PORTFOLIOPOSITION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_PORTFOLIOPOSITION_TITLE _("Panel Portfolio Position")
#define SYMBOL_PANEL_PORTFOLIOPOSITION_IDNAME ID_PANEL_PORTFOLIOPOSITION
#define SYMBOL_PANEL_PORTFOLIOPOSITION_SIZE wxSize(-1, -1)
#define SYMBOL_PANEL_PORTFOLIOPOSITION_POSITION wxDefaultPosition

class PanelPortfolioPosition: public wxPanel {
public:

  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;
  typedef ou::tf::Portfolio::idPortfolio_t idPortfolio_t;
  typedef ou::tf::Portfolio::pPosition_t pPosition_t;
  typedef ou::tf::DialogInstrumentSelect::DelegateNameLookup_t DelegateNameLookup_t;

  typedef FastDelegate1<pPosition_t,void> DelegateAddPosition_t;
  typedef FastDelegate3<const std::string&,pPortfolio_t,DelegateAddPosition_t,void> DelegateConstructPosition_t;

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
  void SetNameLookup( DelegateNameLookup_t function ) { m_DialogInstrumentSelect_DataExchange.lookup = function; };
  void SetConstructPosition( DelegateConstructPosition_t function ) { m_delegateConstructPosition = function; };

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
    ID_TxtUnRealizedPL, ID_TxtCommission, ID_TxtRealizedPL, ID_TxtTotal,
    ID_MenuAddPosition, ID_MenuClosePosition, ID_MenuCancelOrders, ID_MenuAddOrder,
    ID_MenuAddPortfolio, ID_MenuClosePortfolio,
    ID_GridPositions
  };

// wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define COLHDR_POSITION_ARRAY_COL_COUNT 5
#define COLHDR_POSITION_ARRAY_ROW_COUNT 12
#define COLHDR_POSITION_ARRAY \
  (COLHDR_POSITION_ARRAY_ROW_COUNT,  \
    ( /* Col 0,                       1,            2,              3,  4,             */ \
      (COLHDR_POSITION_COL_Pos      , "Position",   wxALIGN_LEFT,  100, ModelCellString ), \
      (COLHDR_POSITION_COL_Side     , "Side",       wxALIGN_LEFT,   50, ModelCellString ), \
      (COLHDR_POSITION_COL_QuanPend , "#Pend",      wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COLHDR_POSITION_COL_QuanActv , "#Active",    wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COLHDR_POSITION_COL_ConsVlu  , "ConsValue",  wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_MktVlu   , "MktValue",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_URPL     , "UnRealPL",   wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_RPL      , "RealPL",     wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_Comm     , "Comm",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_Bid      , "Bid",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_Last     , "Last",       wxALIGN_RIGHT,  50, ModelCellDouble ), \
      (COLHDR_POSITION_COL_Ask      , "Ask",        wxALIGN_RIGHT,  50, ModelCellDouble ), \
      ) \
    ) \
  /**/

#define COLHDR_POSITION_EXTRACT_COL_DETAILS(z, row, col) \
  BOOST_PP_TUPLE_ELEM( \
    COLHDR_POSITION_ARRAY_COL_COUNT, col, \
      BOOST_PP_ARRAY_ELEM( row, COLHDR_POSITION_ARRAY ) \
    )

// if n is 0, then no comma, ie, prepends comma except on first element, col is column number to extract
#define COLHDR_POSITION_EXTRACT_ENUM_LIST(z, n, col) \
  BOOST_PP_COMMA_IF(n) \
  COLHDR_POSITION_EXTRACT_COL_DETAILS( z, n, col )

#define COLHDR_POSITION_EMIT_SetColSettings( z, n, VAR ) \
  m_gridPositions->SetColLabelValue( VAR, _T(COLHDR_POSITION_EXTRACT_COL_DETAILS(z, n, 1) ) ); \
  m_gridPositions->SetColSize( VAR++, COLHDR_POSITION_EXTRACT_COL_DETAILS(z, n, 3) );

#define VECTOR_DEF BOOST_PP_CAT( vector, COLHDR_POSITION_ARRAY_ROW_COUNT )
  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_POSITION_ARRAY), COLHDR_POSITION_EXTRACT_ENUM_LIST, 4 )
  > vModelCells_t;

  struct structPosition {
    pPosition_t pPosition;
    vModelCells_t vModelCells;
    structPosition( pPosition_t pPosition_ ): pPosition( pPosition_ ) {};
  };
  typedef std::vector<structPosition> vPositions_t;

  vPositions_t m_vPositions;  // one to one match on rows in grid

  bool m_bDialogActive;

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
    wxGrid* m_gridPositions;

    wxMenu* m_menuGridLabelPositionPopUp;
    wxMenu* m_menuGridCellPositionPopUp;

  pPortfolio_t m_pPortfolio;
  DelegateConstructPosition_t m_delegateConstructPosition;

  ou::tf::DialogInstrumentSelect::DataExchange m_DialogInstrumentSelect_DataExchange;
  ou::tf::DialogInstrumentSelect* m_pdialogInstrumentSelect;

  void AddPosition( pPosition_t pPosition ); // constructed from supplied symbol name

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

};


} // namespace tf
} // namespace ou
