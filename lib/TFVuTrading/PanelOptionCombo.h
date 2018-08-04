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

#pragma once

/* reason for existence:
 *   non-registered instruments as they come and go - actually, all will be registered 
 *     -> need to validate in registry prior to creating another instrument
 *   contract id is optional as data comes from IQFeed
 *   no affect on database of positions - positions are transitory
 *   ability to add and delete with destroying database integrity
 */

#include <memory>
#include <functional>

#include <wx/panel.h>

#include <TFTrading/Instrument.h>
#include <TFTrading/PortfolioGreek.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define PANEL_OPTIONCOMBO_STYLE wxTAB_TRAVERSAL
#define PANEL_OPTIONCOMBO_TITLE _("Panel Option Combo")
#define PANEL_OPTIONCOMBO_IDNAME ID_PANEL_OPTIONCOMBO
#define PANEL_OPTIONCOMBO_SIZE wxSize(-1, -1)
#define PANEL_OPTIONCOMBO_POSITION wxDefaultPosition

class PanelOptionCombo_impl;  // Forward declaration

class PanelOptionCombo: public wxPanel {
  friend class PanelOptionCombo_impl;
public:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;

  typedef ou::tf::PortfolioGreek::idPortfolio_t idPortfolio_t;
  typedef ou::tf::PortfolioGreek::pPortfolioGreek_t pPortfolioGreek_t;
  
  typedef ou::tf::PortfolioGreek::idPosition_t idPosition_t;
  typedef ou::tf::PortfolioGreek::pPositionGreek_t pPositionGreek_t;
  
  std::function<pInstrument_t(void)> m_fSelectInstrument;  // Dialog to select Symbol/Instrument
  
  typedef std::function<void(pPositionGreek_t)> fAddPositionGreek_t;
  typedef std::function<void(PanelOptionCombo&, const std::string&, const std::string&)> fConstructPortfolioGreek_t;
  typedef std::function<void(pInstrument_t,pPortfolioGreek_t,fAddPositionGreek_t)> fConstructPositionGreek_t;
  
  fConstructPortfolioGreek_t m_fConstructPortfolioGreek;
  fConstructPositionGreek_t m_fConstructPositionGreek;
  fAddPositionGreek_t m_fAddPosition;

  PanelOptionCombo(void);
  PanelOptionCombo( 
    wxWindow* parent, 
    wxWindowID id =      PANEL_OPTIONCOMBO_IDNAME, 
    const wxPoint& pos = PANEL_OPTIONCOMBO_POSITION, 
    const wxSize& size = PANEL_OPTIONCOMBO_SIZE, 
    long style =         PANEL_OPTIONCOMBO_STYLE );
  virtual ~PanelOptionCombo(void);

  bool Create( 
    wxWindow* parent, 
    wxWindowID id =      PANEL_OPTIONCOMBO_IDNAME, 
    const wxPoint& pos = PANEL_OPTIONCOMBO_POSITION, 
    const wxSize& size = PANEL_OPTIONCOMBO_SIZE, 
    long style =         PANEL_OPTIONCOMBO_STYLE );

  void SetPortfolioGreek( pPortfolioGreek_t pPortfolioGreek );
  pPortfolioGreek_t& GetPortfolioGreek( void );

  void AddPositionGreek( pPositionGreek_t pPositionGreek );

  void UpdateGui( void );

protected:

  void Init();

private:

  enum { ID_Null=wxID_HIGHEST, ID_PANEL_OPTIONCOMBO, 
    ID_LblIdPortfolio, ID_LblCurrency, ID_LblDescription, ID_LblUnrealizedPL, ID_LblCommission, ID_LblRealizedPL, ID_LblTotal,
    ID_TxtDescription,
    ID_TxtUnRealizedPL, ID_TxtCommission, ID_TxtRealizedPL, ID_TxtTotal,
    ID_MenuAddPosition, ID_MenuClosePosition, ID_MenuCancelOrders, ID_MenuAddOrder,
    ID_MenuAddPortfolio, ID_MenuClosePortfolio,
    ID_GridPositions
  };

  std::unique_ptr<PanelOptionCombo_impl> m_pimpl;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

};

} // namespace tf
} // namespace ou
