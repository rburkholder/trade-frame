/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    PanelOptionCombo.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: December 28 2025 20:27:57
 */

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#define SYMBOL_PanelOptionCombo_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PanelOptionCombo_TITLE _("Option Combo")
#define SYMBOL_PanelOptionCombo_IDNAME ID_PanelOptionCombo
#define SYMBOL_PanelOptionCombo_SIZE wxDefaultSize
#define SYMBOL_PanelOptionCombo_POSITION wxDefaultPosition

namespace ou { // One Unified
namespace tf { // TradeFrame

class PanelOptionCombo
: public wxPanel
{
  friend class boost::serialization::access;
public:

  PanelOptionCombo();
  PanelOptionCombo(
    wxWindow* parent, wxWindowID id = SYMBOL_PanelOptionCombo_IDNAME,
    const wxPoint& pos = SYMBOL_PanelOptionCombo_POSITION,
    const wxSize& size = SYMBOL_PanelOptionCombo_SIZE,
    long style = SYMBOL_PanelOptionCombo_STYLE,
    const wxString& name = SYMBOL_PanelOptionCombo_TITLE );
  virtual ~PanelOptionCombo();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PanelOptionCombo_IDNAME,
    const wxPoint& pos = SYMBOL_PanelOptionCombo_POSITION,
    const wxSize& size = SYMBOL_PanelOptionCombo_SIZE,
    long style = SYMBOL_PanelOptionCombo_STYLE,
    const wxString& name = SYMBOL_PanelOptionCombo_TITLE );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_PanelOptionCombo
  };

  void Init();
  void CreateControls();
  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  static bool ShowToolTips() { return true; };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelOptionCombo, 1)
