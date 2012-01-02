/////////////////////////////////////////////////////////////////////////////
// Name:        providers2.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/08/2011 23:29:20
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PROVIDERS2_H_
#define _PROVIDERS2_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
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
#define ID_PROVIDERS2 10016
#define SYMBOL_PROVIDERS2_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PROVIDERS2_TITLE _("Providers2")
#define SYMBOL_PROVIDERS2_IDNAME ID_PROVIDERS2
#define SYMBOL_PROVIDERS2_SIZE wxSize(400, 300)
#define SYMBOL_PROVIDERS2_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * Providers2 class declaration
 */

class Providers2: public wxFrame
{    
    DECLARE_CLASS( Providers2 )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Providers2();
    Providers2( wxWindow* parent, wxWindowID id = SYMBOL_PROVIDERS2_IDNAME, const wxString& caption = SYMBOL_PROVIDERS2_TITLE, const wxPoint& pos = SYMBOL_PROVIDERS2_POSITION, const wxSize& size = SYMBOL_PROVIDERS2_SIZE, long style = SYMBOL_PROVIDERS2_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PROVIDERS2_IDNAME, const wxString& caption = SYMBOL_PROVIDERS2_TITLE, const wxPoint& pos = SYMBOL_PROVIDERS2_POSITION, const wxSize& size = SYMBOL_PROVIDERS2_SIZE, long style = SYMBOL_PROVIDERS2_STYLE );

    /// Destructor
    ~Providers2();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Providers2 event handler declarations
////@end Providers2 event handler declarations

////@begin Providers2 member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Providers2 member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Providers2 member variables
////@end Providers2 member variables
};

#endif
    // _PROVIDERS2_H_
