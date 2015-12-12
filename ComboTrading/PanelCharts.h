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

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };

protected: 
  void Init();
  void CreateControls();
private:
  
  enum { 
    ID_Null=wxID_HIGHEST, ID_PANEL_CHARTS, 
    MIRoot, MIGroup, MIInstrument, MIPortfolio, MIPosition
  };
  
  // 20151206  convert all this to TreeItem derivations, so everything is self contained
  
  struct CustomItemBase: public wxTreeItemData {
    enum enumNodeType { Root, Group, Instrument, Portfolio, Position } m_eNodeType;
    CustomItemBase( void ): m_eNodeType( Root ) {};
    CustomItemBase( enumNodeType type ): m_eNodeType( type ) {};
  };
  
  struct CustomItemGroup: public CustomItemBase {
    // put link to instrument here, once regurgitated
    CustomItemGroup( void ): CustomItemBase( Group ) {};
  };
  
  struct CustomItemInstrument: public CustomItemBase {
    // put link to instrument here, once regurgitated
    CustomItemInstrument( void ): CustomItemBase( Instrument ) {};
  };
  
  struct CustomItemPortfolio: public CustomItemBase {
    // put link to portfolio here, once regurgitated
    CustomItemPortfolio( void ): CustomItemBase( Portfolio ) {};
  };
  
  struct CustomItemPosition: public CustomItemBase {
    // put link to position here, once regurgitated
    CustomItemPosition( void ): CustomItemBase( Position ) {};
  };
  
  wxTreeCtrl* m_pTreeSymbols;  // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html

  wxWindow* m_winChart;
  ou::ChartMaster m_chartMaster;
  ou::ChartDataView* m_pChartDataView;

  void OnClose( wxCloseEvent& event );
  
  void ShowContextMenuRoot( void );
  
  void HandleAddGroup( wxCommandEvent& event );
  void HandleAddInstrument( wxCommandEvent& event );
  void HandleAddPortfolio( wxCommandEvent& event );
  void HandleAddPosition( wxCommandEvent& event );

  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  
  void HandleTreeSelChanged( wxTreeEvent& event );
  void HandleTreeItemRightClick( wxTreeEvent& event );
  //void HandleTreeItemMenu( wxTreeEvent& event );
  void HandleTreeItemActivated( wxTreeEvent& event );

};

} // namespace tf
} // namespace ou
