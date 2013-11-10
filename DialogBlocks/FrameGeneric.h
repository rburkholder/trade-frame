/////////////////////////////////////////////////////////////////////////////
// Name:        FrameGeneric.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     06/11/2011 14:32:35
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMEGENERIC_H_
#define _FRAMEGENERIC_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/statusbr.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxStatusBar;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAMEMAIN 10038
#define ID_PANELMAIN 10044
#define ID_MENUEXIT 10040
#define ID_STATUSBAR 10041
#define SYMBOL_FRAMEGENERIC_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE|wxCLOSE_BOX
#define SYMBOL_FRAMEGENERIC_TITLE _("FrameMain")
#define SYMBOL_FRAMEGENERIC_IDNAME ID_FRAMEMAIN
#define SYMBOL_FRAMEGENERIC_SIZE wxSize(400, 300)
#define SYMBOL_FRAMEGENERIC_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameGeneric class declaration
 */

class FrameGeneric: public wxFrame
{    
    DECLARE_CLASS( FrameGeneric )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameGeneric();
    FrameGeneric( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, long style = SYMBOL_FRAMEGENERIC_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEGENERIC_IDNAME, const wxString& caption = SYMBOL_FRAMEGENERIC_TITLE, const wxPoint& pos = SYMBOL_FRAMEGENERIC_POSITION, const wxSize& size = SYMBOL_FRAMEGENERIC_SIZE, long style = SYMBOL_FRAMEGENERIC_STYLE );

    /// Destructor
    ~FrameGeneric();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameGeneric event handler declarations

////@end FrameGeneric event handler declarations

////@begin FrameGeneric member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameGeneric member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameGeneric member variables
    wxPanel* m_panelMain;
    wxBoxSizer* m_sizerMain;
    wxStatusBar* m_statusBar;
////@end FrameGeneric member variables
};

#endif
    // _FRAMEGENERIC_H_
