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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTrading/Portfolio.h>

#include <TFVuTrading/DialogInstrumentSelect.h>

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

  struct structPosition {
    pPosition_t pPosition;
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
