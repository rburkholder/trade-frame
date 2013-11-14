/////////////////////////////////////////////////////////////////////////////
// Name:        frameportfolioposition.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     09/11/2013 20:22:22
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMEPORTFOLIOPOSITION_H_
#define _FRAMEPORTFOLIOPOSITION_H_


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
#define ID_FRAMEPORTFOLIOPOSITION 10103
#define ID_SCROLLEDWINDOW 10104
#define SYMBOL_FRAMEPORTFOLIOPOSITION_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMEPORTFOLIOPOSITION_TITLE _("FramePortfolioPosition")
#define SYMBOL_FRAMEPORTFOLIOPOSITION_IDNAME ID_FRAMEPORTFOLIOPOSITION
#define SYMBOL_FRAMEPORTFOLIOPOSITION_SIZE wxSize(400, 300)
#define SYMBOL_FRAMEPORTFOLIOPOSITION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FramePortfolioPosition class declaration
 */

class FramePortfolioPosition: public wxFrame
{    
    DECLARE_CLASS( FramePortfolioPosition )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FramePortfolioPosition();
    FramePortfolioPosition( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEPORTFOLIOPOSITION_IDNAME, const wxString& caption = SYMBOL_FRAMEPORTFOLIOPOSITION_TITLE, const wxPoint& pos = SYMBOL_FRAMEPORTFOLIOPOSITION_POSITION, const wxSize& size = SYMBOL_FRAMEPORTFOLIOPOSITION_SIZE, long style = SYMBOL_FRAMEPORTFOLIOPOSITION_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEPORTFOLIOPOSITION_IDNAME, const wxString& caption = SYMBOL_FRAMEPORTFOLIOPOSITION_TITLE, const wxPoint& pos = SYMBOL_FRAMEPORTFOLIOPOSITION_POSITION, const wxSize& size = SYMBOL_FRAMEPORTFOLIOPOSITION_SIZE, long style = SYMBOL_FRAMEPORTFOLIOPOSITION_STYLE );

    /// Destructor
    ~FramePortfolioPosition();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FramePortfolioPosition event handler declarations

////@end FramePortfolioPosition event handler declarations

////@begin FramePortfolioPosition member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FramePortfolioPosition member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FramePortfolioPosition member variables
////@end FramePortfolioPosition member variables
};

#endif
    // _FRAMEPORTFOLIOPOSITION_H_
