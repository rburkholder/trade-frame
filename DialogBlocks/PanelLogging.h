/////////////////////////////////////////////////////////////////////////////
// Name:        PanelLogging.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/03/2012 10:26:41
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELLOGGING_H_
#define _PANELLOGGING_H_


/*!
 * Includes
 */

////@begin includes
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
#define ID_PANELLOGGING 10048
#define ID_TEXTLOGGING 10051
#define SYMBOL_PANELLOGGING_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELLOGGING_TITLE _("PanelLogging")
#define SYMBOL_PANELLOGGING_IDNAME ID_PANELLOGGING
#define SYMBOL_PANELLOGGING_SIZE wxSize(400, 300)
#define SYMBOL_PANELLOGGING_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelLogging class declaration
 */

class PanelLogging: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelLogging )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelLogging();
    PanelLogging( wxWindow* parent, wxWindowID id = SYMBOL_PANELLOGGING_IDNAME, const wxPoint& pos = SYMBOL_PANELLOGGING_POSITION, const wxSize& size = SYMBOL_PANELLOGGING_SIZE, long style = SYMBOL_PANELLOGGING_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELLOGGING_IDNAME, const wxPoint& pos = SYMBOL_PANELLOGGING_POSITION, const wxSize& size = SYMBOL_PANELLOGGING_SIZE, long style = SYMBOL_PANELLOGGING_STYLE );

    /// Destructor
    ~PanelLogging();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelLogging event handler declarations

////@end PanelLogging event handler declarations

////@begin PanelLogging member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelLogging member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelLogging member variables
    wxTextCtrl* m_txtLogging;
////@end PanelLogging member variables
};

#endif
    // _PANELLOGGING_H_
