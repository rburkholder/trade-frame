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

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <wx/frame.h>
#include <wx/statusbr.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#define SYMBOL_FRAMEGENERIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX
#define SYMBOL_FRAMEGENERIC_TITLE _("Frame Main")
#define SYMBOL_FRAMEGENERIC_IDNAME ID_FRAMEGENERIC
#define SYMBOL_FRAMEGENERIC_SIZE wxSize(400, 500)
#define SYMBOL_FRAMEGENERIC_POSITION wxDefaultPosition

class FrameMain: public wxFrame {
public:

  typedef FastDelegate0<> OnActionHandler_t;

  struct structMenuItem: wxObject {
    std::string text;
    OnActionHandler_t OnActionHandler;
    unsigned long ix;
    structMenuItem( void ): ix( 0 ) {};
    structMenuItem( const std::string& text_, OnActionHandler_t oah, unsigned long ix_ = 0 )
      : text( text_ ), OnActionHandler( oah ), ix( ix_ ) {};
    structMenuItem( const structMenuItem& rhs )
      : text( rhs.text ), OnActionHandler( rhs.OnActionHandler ), ix( rhs.ix ) {};
  };

  typedef std::vector<structMenuItem*> vpItems_t;

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

  // re-arrange this so when events added/removed, menu updated with text as well, 
  // ie, pass in structure to build menu

  void AddDynamicMenu( const std::string& root, const vpItems_t& vItems );
  
  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN,
    ID_MENUACTION1, ID_MENUACTION2, ID_MENUACTION3, ID_MENUACTION4, ID_MENUACTION5, ID_DYNAMIC_MENU_ACTIONS
  };

  wxMenuBar* m_menuBar;
  wxStatusBar* m_statusBar;

//  typedef std::vector<structMenuItem*> vPtrItems_t;  // wxWidgets take ownership of object
  vpItems_t m_vPtrItems;

  void Init();
  void CreateControls();

  void OnMenuExitClick( wxCommandEvent& event );
  void OnDynamicActionClick( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );
};

