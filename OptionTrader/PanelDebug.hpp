/************************************************************************
 * Copyright(c) 2026, One Unified. All rights reserved.                 *
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
 * File:    PanelDebug.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: January 26, 2026 19:56:26
 */

#pragma once

#include <map>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/panel.h>

#define SYMBOL_PANELDEBUG_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELDEBUG_TITLE _("Instrument Views")
#define SYMBOL_PANELDEBUG_IDNAME ID_PANELDEBUG
#define SYMBOL_PANELDEBUG_SIZE wxDefaultSize
#define SYMBOL_PANELDEBUG_POSITION wxDefaultPosition

class wxBoxSizer;
class wxStaticText;

namespace ou { // One Unified
namespace tf { // TradeFrame

class PanelDebug
: public wxPanel
{
  friend class boost::serialization::access;
public:

  PanelDebug();
  PanelDebug(
    wxWindow* parent, wxWindowID id = SYMBOL_PANELDEBUG_IDNAME,
    const wxPoint& pos = SYMBOL_PANELDEBUG_POSITION,
    const wxSize& size = SYMBOL_PANELDEBUG_SIZE,
    long style = SYMBOL_PANELDEBUG_STYLE,
    const wxString& name = SYMBOL_PANELDEBUG_TITLE );
  virtual ~PanelDebug();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PANELDEBUG_IDNAME,
    const wxPoint& pos = SYMBOL_PANELDEBUG_POSITION,
    const wxSize& size = SYMBOL_PANELDEBUG_SIZE,
    long style = SYMBOL_PANELDEBUG_STYLE,
    const wxString& name = SYMBOL_PANELDEBUG_TITLE );

  void Update( const std::string&, const std::string& );

protected:
private:

  enum {
    ID_Null=wxID_HIGHEST
  , ID_PANELDEBUG
  };

  wxBoxSizer* m_pSizer;

  using mapFields_t = std::map<std::string, std::pair<wxStaticText*,wxStaticText*> >;
  mapFields_t m_mapFields;

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

BOOST_CLASS_VERSION(ou::tf::PanelDebug, 1)
