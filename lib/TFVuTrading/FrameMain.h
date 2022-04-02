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
#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/frame.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#define SYMBOL_FRAMEGENERIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxMAXIMIZE_BOX
#define SYMBOL_FRAMEGENERIC_TITLE _("Frame Main")
#define SYMBOL_FRAMEGENERIC_IDNAME ID_FRAMEGENERIC
#define SYMBOL_FRAMEGENERIC_SIZE wxSize(400, 500)
#define SYMBOL_FRAMEGENERIC_POSITION wxDefaultPosition

class wxMenuBar;
class wxStatusBar;

class FrameMain: public wxFrame {
  friend class boost::serialization::access;
public:

  typedef FastDelegate0<> OnActionHandler_t;

  struct structMenuItem: wxObject {
    std::string text;
    OnActionHandler_t OnActionHandler;
    unsigned long ix;  // may no longer be required, replaced by m_ixDynamicMenuItem
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

  int AddFileMenuItem( const wxString& );

  wxMenu* AddDynamicMenu( const std::string& root, const vpItems_t& vItems );

  static bool ShowToolTips() { return true; };
  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN,
    ID_DYNAMIC_MENU_ACTIONS // must be last in line
  };

  int m_ixDynamicMenuItem;  // initialized to ID_DYNAMIC_MENU_ACTIONS and incremented with each new menu entry
  wxMenu* m_menuFile;

  wxMenuBar* m_menuBar;
  wxStatusBar* m_statusBar;

//  typedef std::vector<structMenuItem*> vPtrItems_t;  // wxWidgets take ownership of object
  vpItems_t m_vPtrItems;

  void Init();
  void CreateControls();

  void OnMenuExitClick( wxCommandEvent& event );
  void OnDynamicActionClick( wxCommandEvent& event );
  void OnClose( wxCloseEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);
    wxPoint point = this->GetPosition();
    ar & point.x;
    ar & point.y;
    wxSize size = this->GetSize();
    ar & size.x;
    ar & size.y;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);
    int x, y;
    ar & x;
    ar & y;
    wxPoint point( x, y );
    ar & x;
    ar & y;
    wxSize size( x, y );
    this->SetSize( size );
    this->SetPosition( point ); // seems to work better with this order
    Layout(); // but still not consistently, seems to be a timing problem somewhere

  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(FrameMain, 1)

