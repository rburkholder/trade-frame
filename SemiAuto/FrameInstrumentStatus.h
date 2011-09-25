/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include <wx/wx.h>
#include <wx/grid.h>

// wxFrame (wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE, const wxString &name=wxFrameNameStr)

class FrameInstrumentStatus: public wxFrame {
public:

  FrameInstrumentStatus( void );
  FrameInstrumentStatus(
    wxWindow* parent,
   const wxString& title = "Grid", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxDefaultSize, //wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
  virtual ~FrameInstrumentStatus(void);

  bool Create( 
    wxWindow* parent,
   const wxString& title = "InstrumentStatus", 
   const wxPoint& pos=wxDefaultPosition, 
   const wxSize& size=wxDefaultSize, //wxSize(500, 120), 
   long style=wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));

  wxGrid* Grid( void ) { return m_gridStatus; };

protected:
  wxGrid* m_gridStatus;
private:

  void Init( void );
  void CreateControls( void );
  bool ShowToolTips( void ) { return true; };

  void OnClose( wxCloseEvent& event );

};

