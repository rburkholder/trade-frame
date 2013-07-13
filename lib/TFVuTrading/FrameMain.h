/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// Generic frame for wx based applications

#pragma once

#include <wx/frame.h>
#include <wx/statusbr.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#define SYMBOL_FRAMEGENERIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMEGENERIC_TITLE _("Strategy Runner")
#define SYMBOL_FRAMEGENERIC_IDNAME ID_FRAMEGENERIC
#define SYMBOL_FRAMEGENERIC_SIZE wxSize(400, 500)
#define SYMBOL_FRAMEGENERIC_POSITION wxDefaultPosition

class FrameMain: public wxFrame {
public:
  FrameMain(void);
  FrameMain( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, 
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, 
    long style = SYMBOL_FRAMEGENERIC_STYLE );
  ~FrameMain(void);

  bool Create( 
    wxWindow* parent, 
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, 
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, 
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, 
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, 
    long style = SYMBOL_FRAMEGENERIC_STYLE );

  void Init();
  void CreateControls();

  // re-arrange this so when events added/removed, menu updated with text as well, 
  // ie, pass in structure to build menu
  typedef FastDelegate0<> OnActionHandler;
  void SetOnAction1Handler( OnActionHandler function ) {
    OnAction1 = function;
  }
  void SetOnAction2Handler( OnActionHandler function ) {
    OnAction2 = function;
  }
  void SetOnAction3Handler( OnActionHandler function ) {
    OnAction3 = function;
  }
  void SetOnAction4Handler( OnActionHandler function ) {
    OnAction4 = function;
  }
  void SetOnAction5Handler( OnActionHandler function ) {
    OnAction5 = function;
  }

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN,
    ID_MENUACTION1, ID_MENUACTION2, ID_MENUACTION3, ID_MENUACTION4, ID_MENUACTION5
  };

  wxMenuBar* m_menuBar;
  wxStatusBar* m_statusBar;

  OnActionHandler OnAction1;
  OnActionHandler OnAction2;
  OnActionHandler OnAction3;
  OnActionHandler OnAction4;
  OnActionHandler OnAction5;

  void OnMenuExitClick( wxCommandEvent& event );
  void OnMenuAction1Click( wxCommandEvent& event );
  void OnMenuAction2Click( wxCommandEvent& event );
  void OnMenuAction3Click( wxCommandEvent& event );
  void OnMenuAction4Click( wxCommandEvent& event );
  void OnMenuAction5Click( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );
};

