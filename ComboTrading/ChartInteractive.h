/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ChartInteractive.h
 * Author: raymond@burkholder.net
 *
 * Created on October 16, 2016, 5:53 PM
 */

#pragma once

#include <wx/window.h>
#include <wx/timer.h>

#include <OUCharting/ChartMaster.h>

#define SYMBOL_WIN_CHARTINTERACTIVE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WIN_CHARTINTERACTIVE_TITLE _("Window Interactive Chart")
#define SYMBOL_WIN_CHARTINTERACTIVE_IDNAME ID_WINDOW_CHARTINTERACTIVE
#define SYMBOL_WIN_CHARTINTERACTIVE_SIZE wxSize(400, 300)
#define SYMBOL_WIN_CHARTINTERACTIVE_POSITION wxDefaultPosition

class ChartInteractive: public wxWindow {
public:
  
  ChartInteractive();
  ChartInteractive( wxWindow* parent, wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION, 
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE, 
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );

  bool Create( wxWindow* parent, 
    wxWindowID id = SYMBOL_WIN_CHARTINTERACTIVE_IDNAME, 
    const wxPoint& pos = SYMBOL_WIN_CHARTINTERACTIVE_POSITION, 
    const wxSize& size = SYMBOL_WIN_CHARTINTERACTIVE_SIZE, 
    long style = SYMBOL_WIN_CHARTINTERACTIVE_STYLE );
  virtual ~ChartInteractive();
  
protected:
  
  void Init();
  void CreateControls();
  
private:

  enum { 
    ID_Null=wxID_HIGHEST, ID_WINDOW_CHARTINTERACTIVE
  };
  
  //wxWindow* m_winChart;
  ou::ChartMaster m_chartMaster;
  ou::ChartDataView* m_pChartDataView;
  wxTimer m_timerGuiRefresh;
  
  void HandleDrawChart( const MemBlock& );
  void HandlePaint( wxPaintEvent& event );
  void HandleSize( wxSizeEvent& event );
  void OnClose( wxCloseEvent& event );
  
  void HandleGuiRefresh( wxTimerEvent& event );
  
};



