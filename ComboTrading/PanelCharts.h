/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 6, 2015, 1:26 PM

#pragma once

#include <wx/treectrl.h>
#include <wx/panel.h>
#include <wx/event.h>

//#include <wx/scrolwin.h>

#include <OUCharting/ChartMaster.h>

#include <TFBitsNPieces/TreeOps.h>
#include <TFVuTrading/DialogPickSymbol.h>
#include "TreeItem.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANEL_CHARTS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANEL_CHARTS_TITLE _("Panel Chart Data")
#define SYMBOL_PANEL_CHARTS_IDNAME ID_PANEL_CHARTS
#define SYMBOL_PANEL_CHARTS_SIZE wxSize(400, 300)
#define SYMBOL_PANEL_CHARTS_POSITION wxDefaultPosition

class PanelCharts: public wxPanel {
public:
  PanelCharts( void );
  PanelCharts( wxWindow* parent, wxWindowID id = SYMBOL_PANEL_CHARTS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTS_SIZE, 
    long style = SYMBOL_PANEL_CHARTS_STYLE );
  virtual ~PanelCharts();

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_PANEL_CHARTS_IDNAME, 
    const wxPoint& pos = SYMBOL_PANEL_CHARTS_POSITION, 
    const wxSize& size = SYMBOL_PANEL_CHARTS_SIZE, 
    long style = SYMBOL_PANEL_CHARTS_STYLE );
  
  typedef boost::signals2::signal<void(const std::string&, std::string&)> signalLookUpDescription_t;
  typedef signalLookUpDescription_t::slot_type slotLookUpDescription_t;
  signalLookUpDescription_t signalLookUpDescription;

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected: 
  void Init();
  void CreateControls();
private:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  
  enum { 
    ID_Null=wxID_HIGHEST, ID_PANEL_CHARTS, 
    MIRoot, MIGroup, MIInstrument, MIPortfolio, MIPosition
  };
  
  ou::tf::TreeItemResources m_baseResources;
  Resources m_resources;
  ou::tf::TreeOps* m_pTreeOps;
  
  wxWindow* m_winChart;
  ou::ChartMaster m_chartMaster;
  ou::ChartDataView* m_pChartDataView;
  
  void HandleLookUpDescription( const std::string&, std::string& );
  
  pInstrument_t HandleNewInstrumentRequest( void );
  void HandleComposeComposite( ou::tf::DialogPickSymbol::DataExchange* );

  void OnClose( wxCloseEvent& event );
  
  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  
};

} // namespace tf
} // namespace ou
