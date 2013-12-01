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

#pragma once

// Started 2013/11/30

#include <vector>
#include <string>

#include <wx/splitter.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELARMSINDEX_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELARMSINDEX_TITLE _("Arms Index")
#define SYMBOL_PANELARMSINDEX_IDNAME ID_PANELARMSINDEX
#define SYMBOL_PANELARMSINDEX_SIZE wxSize(400, 300)
#define SYMBOL_PANELARMSINDEX_POSITION wxDefaultPosition

class PanelArmsIndex: public wxPanel {
public:

  PanelArmsIndex(void);
  PanelArmsIndex( 
    wxWindow* parent, wxWindowID id = SYMBOL_PANELARMSINDEX_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELARMSINDEX_POSITION, 
    const wxSize& size = SYMBOL_PANELARMSINDEX_SIZE, 
    long style = SYMBOL_PANELARMSINDEX_STYLE );
  ~PanelArmsIndex(void);

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANELARMSINDEX_IDNAME, 
    const wxPoint& pos = SYMBOL_PANELARMSINDEX_POSITION, 
    const wxSize& size = SYMBOL_PANELARMSINDEX_SIZE, 
    long style = SYMBOL_PANELARMSINDEX_STYLE );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:

  void Init();
  void CreateControls();

private:
  enum { ID_Null=wxID_HIGHEST, ID_PANELARMSINDEX, ID_SplitterArmsIndex, ID_LbArmsIndex,
    ID_PanelArmsVsIndex, ID_PanelTick, ID_PANEL8, ID_PANEL9
  };

  typedef std::vector<std::string> vSymbols_t;
  struct collection_t {
    std::string sName;
    vSymbols_t vSymbols;
    collection_t( const std::string& sName_ ): sName( sName_ ) {};
  };

  typedef std::vector<collection_t> vCollections_t;

  vCollections_t m_vCollections;

  static std::vector<std::string> m_vDowSymbols;

  struct Collection {
    std::string sSymbolTrin;
    std::string sSymbolTick;
    std::string sSymbolIndex;
  };

    wxBoxSizer* m_sizerPanelArmsIndex;
    wxSplitterWindow* m_splitterArmsIndex;
    wxListBox* m_lbArmsIndex;
    wxBoxSizer* m_sizerCharts;
    wxPanel* m_panelArmsVsIndex;
    wxPanel* m_panelTick;

  void OnClose( wxCloseEvent& event );
};

} // namespace tf
} // namespace ou
