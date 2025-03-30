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

  using OnActionHandler_t = FastDelegate0<>;

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

  using vpItems_t = std::vector<structMenuItem*>;

  FrameMain();
  FrameMain(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME,
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE,
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION,
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE,
    long style = SYMBOL_FRAMEGENERIC_STYLE );
  virtual ~FrameMain();

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME,
    const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE,
    const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION,
    const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE,
    long style = SYMBOL_FRAMEGENERIC_STYLE );

  int AddFileMenuItem( const wxString& );

  wxMenu* AddDynamicMenu( const std::string& root, const vpItems_t& vItems );

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_FRAMEGENERIC, ID_MENUEXIT, ID_STATUSBAR, ID_PANELMAIN,
    ID_DYNAMIC_MENU_ACTIONS // must be last in line
  };

  int m_ixDynamicMenuItem;  // initialized to ID_DYNAMIC_MENU_ACTIONS and incremented with each new menu entry
  wxMenu* m_menuFile;

  wxMenuBar* m_menuBar;
  wxStatusBar* m_statusBar;

  vpItems_t m_vPtrItems;

  void Init();
  void CreateControls();

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );

  void OnMenuExitClick( wxCommandEvent& );
  void OnDynamicActionClick( wxCommandEvent& );
  void OnDestroy( wxWindowDestroyEvent& );
  void OnClose( wxCloseEvent& );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemResources>(*this);

    wxSize size = GetSize();
    ar & size.x;
    ar & size.y;

    wxPoint point = GetPosition();
    ar & point.x;
    ar & point.y;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemResources>(*this);

    int x, y;

    if ( 1 == version ) {
      ar & x;
      ar & y;
      wxPoint point( x, y );
      CallAfter(
        [this,point](){
          SetPosition( point );
        } );

      ar & x;
      ar & y;
      wxSize size( x, y );
      SetSize( size );
      Layout();

    }
    else {
      ar & x;
      ar & y;
      wxSize size( x, y );
      SetSize( size );

      ar & x;
      ar & y;
      wxPoint position( x, y );
      Move( position );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

BOOST_CLASS_VERSION(FrameMain, 2)

