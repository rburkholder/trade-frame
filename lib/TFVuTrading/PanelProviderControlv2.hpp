/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    PanelProviderControlv2.hpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: June 19, 2022 12:03
 */

#pragma once

#include <map>
#include <memory>

#include <wx/panel.h>

#include <TFTrading/ProviderInterface.h>

class wxBoxSizer;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace v2 { // v2

class ProviderWidgets;

// ===================================

#define SYMBOL_PANELPROVIDERCONTROL_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPROVIDERCONTROL_TITLE _("Provider Control")
#define SYMBOL_PANELPROVIDERCONTROL_IDNAME ID_PANELPROVIDERCONTROL_V2
#define SYMBOL_PANELPROVIDERCONTROL_POSITION wxDefaultPosition
#define SYMBOL_PANELPROVIDERCONTROL_SIZE wxDefaultSize

class PanelProviderControl: public wxPanel {
public:

  using pProvider_t = ou::tf::ProviderInterfaceBase::pProvider_t;
  using fCallBack_t = std::function<void()>;

  PanelProviderControl();
  PanelProviderControl(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION,
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE,
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );
  virtual ~PanelProviderControl();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME,
    const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION,
    const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE,
    long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );

  // since we have provider directly, we can control and track state directly, ie, read the state from the provider directly
  void Add(
    pProvider_t
  , bool bD1, bool bD2, bool bX1, bool bX2
  , fCallBack_t&& fConnecting, fCallBack_t&& fConnected // cross thread events
  , fCallBack_t&& fDisconnecting, fCallBack_t&& fDisconnected // cross thread events
  );

protected:
private:

  enum {
    ID_NULL=wxID_HIGHEST, ID_PANELPROVIDERCONTROL_V2
  };

  wxBoxSizer* m_sizerPanel;

  using pProviderWidgets_t = std::shared_ptr<ProviderWidgets>;
  using mapProviderWidgets_t = std::map<ou::tf::keytypes::eidProvider_t, pProviderWidgets_t>;
  mapProviderWidgets_t m_mapProviderWidgets;

  void Init();
  void CreateControls();
  bool ShowToolTips() { return true; };

  void UpdateProviderButtons();

};

} // namespace v2
} // namespace tf
} // namespace ou
