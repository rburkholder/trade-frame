/////////////////////////////////////////////////////////////////////////////
// Name:        frametable.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     24/09/2011 18:45:38
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMETABLE_H_
#define _FRAMETABLE_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/grid.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAMETABLE 10026
#define ID_GRID 10027
#define SYMBOL_FRAMETABLE_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMETABLE_TITLE _("FrameTable")
#define SYMBOL_FRAMETABLE_IDNAME ID_FRAMETABLE
#define SYMBOL_FRAMETABLE_SIZE wxSize(400, 300)
#define SYMBOL_FRAMETABLE_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameTable class declaration
 */

class FrameTable: public wxFrame
{    
    DECLARE_CLASS( FrameTable )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameTable();
    FrameTable( wxWindow* parent, wxWindowID id = SYMBOL_FRAMETABLE_IDNAME, const wxString& caption = SYMBOL_FRAMETABLE_TITLE, const wxPoint& pos = SYMBOL_FRAMETABLE_POSITION, const wxSize& size = SYMBOL_FRAMETABLE_SIZE, long style = SYMBOL_FRAMETABLE_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMETABLE_IDNAME, const wxString& caption = SYMBOL_FRAMETABLE_TITLE, const wxPoint& pos = SYMBOL_FRAMETABLE_POSITION, const wxSize& size = SYMBOL_FRAMETABLE_SIZE, long style = SYMBOL_FRAMETABLE_STYLE );

    /// Destructor
    ~FrameTable();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameTable event handler declarations

////@end FrameTable event handler declarations

////@begin FrameTable member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameTable member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameTable member variables
////@end FrameTable member variables
};

#endif
    // _FRAMETABLE_H_
