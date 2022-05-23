/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    PanelLevelIIButtons.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading/MarketDepth
 * Created: May 23 2022, 11:27
 */

#pragma once

#include <functional>

#include <wx/panel.h>

class wxButton;
class wxCheckBox;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace l2 { // market depth

#define SYMBOL_PANELLEVELIIBUTTONS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELLEVELIIBUTTONS_TITLE _("Panel State Buttons")
#define SYMBOL_PANELLEVELIIBUTTONS_IDNAME ID_PANELLEVELIIBUTTONS
#define SYMBOL_PANELLEVELIIBUTTONS_SIZE wxDefaultSize
#define SYMBOL_PANELLEVELIIBUTTONS_POSITION wxDefaultPosition

class PanelLevelIIButtons: public wxPanel {
public:

  PanelLevelIIButtons();
  PanelLevelIIButtons(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELLEVELIIBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELLEVELIIBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELLEVELIIBUTTONS_SIZE,
   long style = SYMBOL_PANELLEVELIIBUTTONS_STYLE
   );
  virtual ~PanelLevelIIButtons();

  bool Create(
   wxWindow* parent,
   wxWindowID id = SYMBOL_PANELLEVELIIBUTTONS_IDNAME,
   const wxPoint& pos = SYMBOL_PANELLEVELIIBUTTONS_POSITION,
   const wxSize& size = SYMBOL_PANELLEVELIIBUTTONS_SIZE,
   long style = SYMBOL_PANELLEVELIIBUTTONS_STYLE
   );

  using fButtonClick_t = std::function<void()>;
  using fButtonArmed_t = std::function<void(bool)>;

  void Set( fButtonArmed_t&&, fButtonClick_t&&, fButtonClick_t&&, fButtonClick_t&& ); // armed, recenter, cancel, close

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST, ID_PANELLEVELIIBUTTONS
  , ID_CB_Arm
  , ID_BTN_Cancel, ID_BTN_Close, ID_BTN_ReCenter
  };

    wxCheckBox* m_cbArm;
    wxButton* m_btnCancel;
    wxButton* m_btnClose;
    wxButton* m_btnReCenter;

  fButtonArmed_t m_fButtonArmed;
  fButtonClick_t m_fButtonReCenter;
  fButtonClick_t m_fButtonCancel;
  fButtonClick_t m_fButtonClose;

  void Init();
  void CreateControls();

  static bool ShowToolTips();

  void OnCBArmClick( wxCommandEvent& event );
  void OnBTNCancelClick( wxCommandEvent& event );
  void OnBTNCloseClick( wxCommandEvent& event );
  void OnBTNReCenterClick( wxCommandEvent& event );

  //void OnCloseWindow( wxCloseEvent& event );
  void OnDestroy( wxWindowDestroyEvent& event );

};

} // market depth
} // namespace tf
} // namespace ou
