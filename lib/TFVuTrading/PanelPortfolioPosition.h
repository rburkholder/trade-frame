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
#include <memory>

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

class PanelPortfolioPosition_impl;  // Forward declaration

class PanelPortfolioPosition: public wxPanel {
  friend class PanelPortfolioPosition_impl;
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
  pPortfolio_t& GetPortfolio( void );

  void SetNameLookup( DelegateNameLookup_t function );
  void SetConstructPosition( DelegateConstructPosition_t function );
  void SetConstructPortfolio( DelegateConstructPortfolio_t function );

  void AddPosition( pPosition_t pPosition ); // constructed from supplied symbol name

  void UpdateGui( void );

protected:

  void Init();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_PORTFOLIOPOSITION, 
    ID_LblIdPortfolio, ID_LblCurrency, ID_LblDescription, ID_LblUnrealizedPL, ID_LblCommission, ID_LblRealizedPL, ID_LblTotal,
    ID_TxtDescription,
    ID_TxtUnRealizedPL, ID_TxtCommission, ID_TxtRealizedPL, ID_TxtTotal,
    ID_MenuAddPosition, ID_MenuClosePosition, ID_MenuCancelOrders, ID_MenuAddOrder,
    ID_MenuAddPortfolio, ID_MenuClosePortfolio,
    ID_GridPositions
  };

  std::unique_ptr<PanelPortfolioPosition_impl> m_pimpl;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};


} // namespace tf
} // namespace ou
