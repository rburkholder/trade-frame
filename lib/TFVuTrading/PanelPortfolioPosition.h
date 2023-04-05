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

#include <memory>
#include <functional>

#include <wx/panel.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/Portfolio.h>

#include <TFBitsNPieces/GridColumnSizer.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define PANEL_PORTFOLIOPOSITION_STYLE wxTAB_TRAVERSAL
#define PANEL_PORTFOLIOPOSITION_TITLE _("Panel Portfolio Position")
#define PANEL_PORTFOLIOPOSITION_IDNAME ID_PANEL_PORTFOLIOPOSITION
#define PANEL_PORTFOLIOPOSITION_SIZE wxSize(-1, -1)
#define PANEL_PORTFOLIOPOSITION_POSITION wxDefaultPosition

class PanelPortfolioPosition_impl;  // Forward declaration

class PanelPortfolioPosition: public wxPanel {
  friend class PanelPortfolioPosition_impl;
public:

  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  typedef ou::tf::Portfolio::idPortfolio_t idPortfolio_t;
  typedef ou::tf::Portfolio::pPortfolio_t pPortfolio_t;

  typedef ou::tf::Portfolio::idPosition_t idPosition_t;
  typedef ou::tf::Portfolio::pPosition_t pPosition_t;

  std::function<pInstrument_t(void)> m_fSelectInstrument;  // Dialog to select Symbol/Instrument

  typedef std::function<void(pPosition_t)> fAddPosition_t;
  typedef std::function<void(pInstrument_t,pPortfolio_t,fAddPosition_t)> fConstructPosition_t;
  typedef std::function<void(PanelPortfolioPosition&, const std::string&, const std::string&)> fConstructPortfolio_t;
  typedef std::function<void(int,int,PanelPortfolioPosition&)> fColumnWidthChanged_t;

  fAddPosition_t m_fAddPosition;  // does not appeared to be used
  fConstructPosition_t m_fConstructPosition;
  fConstructPortfolio_t m_fConstructPortfolio;
  fColumnWidthChanged_t m_fColumnWidthChanged;

  PanelPortfolioPosition();
  PanelPortfolioPosition(
    wxWindow* parent,
    wxWindowID id =      PANEL_PORTFOLIOPOSITION_IDNAME,
    const wxPoint& pos = PANEL_PORTFOLIOPOSITION_POSITION,
    const wxSize& size = PANEL_PORTFOLIOPOSITION_SIZE,
    long style =         PANEL_PORTFOLIOPOSITION_STYLE );
  virtual ~PanelPortfolioPosition();

  bool Create(
    wxWindow* parent,
    wxWindowID id =      PANEL_PORTFOLIOPOSITION_IDNAME,
    const wxPoint& pos = PANEL_PORTFOLIOPOSITION_POSITION,
    const wxSize& size = PANEL_PORTFOLIOPOSITION_SIZE,
    long style =         PANEL_PORTFOLIOPOSITION_STYLE );

  void SetPortfolio( pPortfolio_t pPortfolio );
  pPortfolio_t& GetPortfolio();

  //void SetNameLookup( DelegateNameLookup_t function );
  //void SetConstructPosition( DelegateConstructPosition_t function );
  //void SetConstructPortfolio( DelegateConstructPortfolio_t function );

  void AddPosition( pPosition_t pPosition ); // constructed from supplied symbol name

  void SaveColumnSizes( ou::tf::GridColumnSizer& ) const;
  void SetColumnSizes( ou::tf::GridColumnSizer& );

  void UpdateGui();

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
