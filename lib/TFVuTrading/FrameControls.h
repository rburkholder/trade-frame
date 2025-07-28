/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    FrameControls.h
 * Author:  raymond@burkholder.net
 * Project: lib/TFVuTrading
 * Created: February 11, 2022 15:42
 */

 // floating frame with controls

#pragma once

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/wx.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_FRAMECONTROLS_IDNAME ID_FRAMECONTROLS
#define SYMBOL_FRAMECONTROLS_TITLE _("Frame Controls")
#define SYMBOL_FRAMECONTROLS_POSITION wxDefaultPosition
#define SYMBOL_FRAMECONTROLS_SIZE wxDefaultSize
//#define SYMBOL_FRAMECONTROLS_STYLE wxRESIZE_BORDER|wxFRAME_TOOL_WINDOW|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP|wxTAB_TRAVERSAL
#define SYMBOL_FRAMECONTROLS_STYLE wxRESIZE_BORDER|wxFRAME_TOOL_WINDOW|wxFRAME_FLOAT_ON_PARENT|wxFRAME_NO_TASKBAR|wxTAB_TRAVERSAL

class FrameControls: public wxFrame {
  friend class boost::serialization::access;
public:

  FrameControls();
  FrameControls(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMECONTROLS_IDNAME,
    const wxString& caption = SYMBOL_FRAMECONTROLS_TITLE,
    const wxPoint& pos = SYMBOL_FRAMECONTROLS_POSITION,
    const wxSize& size = SYMBOL_FRAMECONTROLS_SIZE,
    long style = SYMBOL_FRAMECONTROLS_STYLE );

  bool Create(
    wxWindow* parent,
    wxWindowID id = SYMBOL_FRAMECONTROLS_IDNAME,
    const wxString& caption = SYMBOL_FRAMECONTROLS_TITLE,
    const wxPoint& pos = SYMBOL_FRAMECONTROLS_POSITION,
    const wxSize& size = SYMBOL_FRAMECONTROLS_SIZE,
    long style = SYMBOL_FRAMECONTROLS_STYLE );

  virtual ~FrameControls(void);

  void Attach( wxWindow* );

protected:
private:

  enum { ID_Null=wxID_HIGHEST,
    ID_FRAMECONTROLS
  };

  wxBoxSizer* m_sizerFrame;

  void Init();
  void CreateControls();
  static bool ShowToolTips();

  //void OnDestroy( wxWindowDestroyEvent& event );
  void OnClose( wxCloseEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    wxPoint point = this->GetPosition();
    ar & point.x;
    ar & point.y;
    wxSize size = this->GetSize();
    ar & size.x;
    ar & size.y;
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    int x, y;
    ar & x;
    ar & y;
    wxPoint point( x, y );
    ar & x;
    ar & y;
    wxSize size( x, y );
    SetSize( size );
    SetPosition( point );
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::FrameControls, 1)

