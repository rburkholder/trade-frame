/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
// Started January 3, 2016, 3:44 PM

#pragma once

#include <memory>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/panel.h>

#include <TFInteractiveBrokers/IBTWS.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_AccountValues_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_AccountValues_TITLE _("Panel IB Account Values")
#define SYMBOL_PANEL_AccountValues_IDNAME ID_PANELACCOUNTVALUES
#define SYMBOL_PANEL_AccountValues_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_AccountValues_POSITION wxDefaultPosition

class PanelIBAccountValues_impl;  // Forward declaration

class PanelIBAccountValues: public wxPanel {
    friend class PanelIBAccountValues_impl;
    friend class boost::serialization::access;
public:
  PanelIBAccountValues(void);
  PanelIBAccountValues( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANEL_AccountValues_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountValues_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountValues_SIZE, 
    long style = SYMBOL_PANEL_AccountValues_STYLE );
  ~PanelIBAccountValues(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_AccountValues_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_AccountValues_POSITION, 
    const wxSize& size = SYMBOL_PANEL_AccountValues_SIZE, 
    long style = SYMBOL_PANEL_AccountValues_STYLE );
  
  void UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& ad );

protected:
    void Init();
private:
  
  enum { ID_Null=wxID_HIGHEST, ID_PANELACCOUNTVALUES, 
    ID_GRID_ACCOUNTVALUES
  };
  
  std::unique_ptr<PanelIBAccountValues_impl> m_pimpl;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };
  
    template<class Archive>
    void serialize(Archive & ar, const unsigned int file_version);
};

} // namespace tf
} // namespace ou
