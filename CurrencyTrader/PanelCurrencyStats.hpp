/************************************************************************
 * Copyright(c) 2024, One Unified. All rights reserved.                 *
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
 * File:    PanelCurrencyStats.hpp
 * Author:  raymond@burkholder.net
 * Project: CurrencyTrader
 * Created: May 12, 2024 14:37:24
 */

#pragma once

#include <map>

#include <wx/panel.h>

class wxBoxSizer;
class wxStaticText;

#define SYMBOL_PANELCURRENCYSTATS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELCURRENCYSTATS_TITLE _("Currency Stats")
#define SYMBOL_PANELCURRENCYSTATS_IDNAME ID_PANELCURRENCYSTATS
#define SYMBOL_PANELCURRENCYSTATS_POSITION wxDefaultPosition
#define SYMBOL_PANELCURRENCYSTATS_SIZE wxDefaultSize

class PanelCurrencyStats: public wxPanel {
public:

  PanelCurrencyStats();
  PanelCurrencyStats(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELCURRENCYSTATS_IDNAME,
    const wxPoint& pos = SYMBOL_PANELCURRENCYSTATS_POSITION,
    const wxSize& size = SYMBOL_PANELCURRENCYSTATS_SIZE,
    long style = SYMBOL_PANELCURRENCYSTATS_STYLE );
  virtual ~PanelCurrencyStats();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELCURRENCYSTATS_IDNAME,
    const wxPoint& pos = SYMBOL_PANELCURRENCYSTATS_POSITION,
    const wxSize& size = SYMBOL_PANELCURRENCYSTATS_SIZE,
    long style = SYMBOL_PANELCURRENCYSTATS_STYLE );

  using fUpdateCurrency_t = std::function<void(double amount, double extended)>;
  using fUpdatePair_t = std::function<void(double bid, double ask)>;

  fUpdateCurrency_t AddCurrency( const std::string& sName );
  fUpdatePair_t AddPair( const std::string& sName );

protected:
private:

  enum {
    ID_NULL=wxID_HIGHEST, SYMBOL_PANELCURRENCYSTATS_IDNAME
  };

  wxBoxSizer* m_sizerPanel;
  wxBoxSizer* m_sizerCurrencies;
  wxBoxSizer* m_sizerPairs;

  struct Currency {
    wxBoxSizer* m_sizer;
    wxStaticText* m_textAmount;
    wxStaticText* m_textExtended;
    Currency(): m_sizer( nullptr ), m_textAmount( nullptr ), m_textExtended( nullptr ) {}
  };
  using mapCurrency_t = std::map<std::string, Currency>;
  mapCurrency_t m_mapCurrency;

  struct Pair {
    wxBoxSizer* m_sizer;
    wxStaticText* m_textBid;
    wxStaticText* m_textAsk;
    Pair(): m_sizer( nullptr ), m_textBid( nullptr ), m_textAsk( nullptr ) {}
  };
  using mapPair_t = std::map<std::string, Pair>;
  mapPair_t m_mapPair;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

};
