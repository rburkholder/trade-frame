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

#include <boost/shared_ptr.hpp>

#include <wx/splitter.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/listbox.h>

#include "IndicatorPackage.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELARMSINDEX_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELARMSINDEX_TITLE _("Arms Index")
#define SYMBOL_PANELARMSINDEX_IDNAME ID_PANELARMSINDEX
#define SYMBOL_PANELARMSINDEX_SIZE wxSize(400, 300)
#define SYMBOL_PANELARMSINDEX_POSITION wxDefaultPosition

class PanelArmsIndex: public wxPanel {
public:

  typedef ou::tf::ProviderInterfaceBase::pProvider_t pProvider_t;

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

  void SetProvider( pProvider_t pProvider );  // set once provider is connected

  void UpdateGUI( void );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected:

  void Init();
  void CreateControls();

private:

  typedef ou::tf::ProviderInterfaceBase::pInstrument_t pInstrument_t;

  enum { ID_Null=wxID_HIGHEST, ID_PANELARMSINDEX, ID_SplitterArmsIndex, ID_LbArmsIndex,
    ID_PanelArmsVsIndex, ID_PanelTick, ID_PanelIndex, ID_PANEL8, ID_PANEL9, ID_BtnToggleView
  };

  //typedef std::vector<std::string> vSymbols_t;

  struct instrument_t {
    std::string sName;
    pInstrument_t pInst;
    instrument_t( const std::string& sName_ ) {
      pInst.reset( new ou::tf::Instrument( sName_, ou::tf::InstrumentType::Stock, "SMART" ) );
    }
  };
  struct collection_t {
    std::string sName;
    instrument_t instIndex;
    instrument_t instTick;
    instrument_t instTrin;
    boost::shared_ptr<IndicatorPackage> pip;
    collection_t( pProvider_t pProvider_,
                  const std::string& sName_, const std::string& sIndex_, const std::string& sTick_, const std::string& sTrin_ )
      : sName( sName_ ), instIndex( sIndex_ ), instTick( sTick_ ), instTrin( sTrin_ ), pip( 0 ) {
        pip.reset( new IndicatorPackage( pProvider_, instIndex.pInst, instTick.pInst, instTrin.pInst ) );
    };
    ~collection_t( void ) {};
  };

  typedef std::vector<collection_t> vCollections_t;
  vCollections_t m_vCollections;

  vCollections_t::size_type m_ixActiveChart;


  pProvider_t m_pProvider;


  static std::vector<std::string> m_vDowSymbols;

//  struct Collection {
//    std::string sSymbolIndex;
//    std::string sSymbolTick;
//    std::string sSymbolTrin;
//  };

//  IndicatorPackage* m_pip;
  void DrawChartIndex( const MemBlock& m );
  void DrawChartTick( const MemBlock& m );
  void DrawChartArms( const MemBlock& m );
  void DrawChart( const MemBlock& m, wxPanel* pPanel );

    wxBoxSizer* m_sizerPanelArmsIndex;
    wxSplitterWindow* m_splitterArmsIndex;
    wxListBox* m_lbArmsIndex;
    wxButton* m_btnToggleView;
    wxBoxSizer* m_sizerCharts;
    wxPanel* m_panelIndex;
    wxPanel* m_panelTick;
    wxPanel* m_panelArmsVsIndex;

  void OnClose( wxCloseEvent& event );

  void HandleOnSize( wxSizeEvent& event );
  void HandleListBoxSelection( wxCommandEvent& event );
  void HandleBtnToggleView( wxCommandEvent& event );
};

} // namespace tf
} // namespace ou
