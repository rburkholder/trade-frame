/////////////////////////////////////////////////////////////////////////////
// Name:        frametest.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/03/2012 13:00:26
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMETEST_H_
#define _FRAMETEST_H_


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
#define ID_FRAMETEST 10063
#define SYMBOL_FRAMETEST_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMETEST_TITLE _("FrameTest")
#define SYMBOL_FRAMETEST_IDNAME ID_FRAMETEST
#define SYMBOL_FRAMETEST_SIZE wxSize(400, 300)
#define SYMBOL_FRAMETEST_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameTest class declaration
 */

class FrameTest: public wxFrame
{    
    DECLARE_CLASS( FrameTest )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameTest();
    FrameTest( wxWindow* parent, wxWindowID id = SYMBOL_FRAMETEST_IDNAME, const wxString& caption = SYMBOL_FRAMETEST_TITLE, const wxPoint& pos = SYMBOL_FRAMETEST_POSITION, const wxSize& size = SYMBOL_FRAMETEST_SIZE, long style = SYMBOL_FRAMETEST_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMETEST_IDNAME, const wxString& caption = SYMBOL_FRAMETEST_TITLE, const wxPoint& pos = SYMBOL_FRAMETEST_POSITION, const wxSize& size = SYMBOL_FRAMETEST_SIZE, long style = SYMBOL_FRAMETEST_STYLE );

    /// Destructor
    ~FrameTest();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameTest event handler declarations
////@end FrameTest event handler declarations

////@begin FrameTest member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameTest member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameTest member variables
////@end FrameTest member variables
};

#endif
    // _FRAMETEST_H_
