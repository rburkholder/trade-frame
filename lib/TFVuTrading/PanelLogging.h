/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <iostream>

#include <wx/panel.h>

#include <OUCommon/ConsoleStream.h>

class wxTextCtrl;

namespace ou { // One Unified
namespace tf { // TradeFrame

#define SYMBOL_PANELLOGGING_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELLOGGING_TITLE _("Panel Logging")
#define SYMBOL_PANELLOGGING_IDNAME ID_PANELLOGGING
#define SYMBOL_PANELLOGGING_SIZE wxDefaultSize
#define SYMBOL_PANELLOGGING_POSITION wxDefaultPosition

using csb_t = ou::ConsoleStreamBuf<char>;

class ConsoleStringEvent: public wxEvent {
public:
  ConsoleStringEvent( wxEventType eventType, csb_t::Buf* p ): wxEvent( 0, eventType ), m_pBuf( p ) {};
  //ConsoleStringEvent( const ConsoleStringEvent& event): wxEvent( event ), m_pBuf( event.m_pBuf ) {};
  virtual ~ConsoleStringEvent() { m_pBuf = nullptr; };
  ConsoleStringEvent* Clone() const { return new ConsoleStringEvent( *this ); };
  csb_t::Buf* GetBuf() { assert( m_pBuf ); return m_pBuf; };
private:
  csb_t::Buf* m_pBuf;
};

wxDECLARE_EVENT( EVT_ConsoleString, ConsoleStringEvent );

class PanelLogging: public wxPanel {
public:

  PanelLogging();
  PanelLogging(
    wxWindow* parent, wxWindowID id = SYMBOL_PANELLOGGING_IDNAME,
    const wxPoint& pos = SYMBOL_PANELLOGGING_POSITION,
    const wxSize& size = SYMBOL_PANELLOGGING_SIZE,
    long style = SYMBOL_PANELLOGGING_STYLE );
  virtual ~PanelLogging();

  bool Create( wxWindow* parent,
    wxWindowID id = SYMBOL_PANELLOGGING_IDNAME,
    const wxPoint& pos = SYMBOL_PANELLOGGING_POSITION,
    const wxSize& size = SYMBOL_PANELLOGGING_SIZE,
    long style = SYMBOL_PANELLOGGING_STYLE );

  void CreateControls();

protected:
private:

  enum { ID_Null=wxID_HIGHEST, ID_PANELLOGGING, ID_TEXTLOGGING };
  wxTextCtrl* m_txtLogging;
  std::streambuf* m_pOldStreamBuf;
  csb_t m_csb;

  void Init();

  void HandleConsoleLine0( csb_t::Buf* pBuf );
  void HandleConsoleLine1( ConsoleStringEvent& event );

  void OnDestroy( wxWindowDestroyEvent& event );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips() { return true; };
};


} // namespace tf
} // namespace ou
