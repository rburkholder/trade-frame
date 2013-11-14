/////////////////////////////////////////////////////////////////////////////
// Name:        framehedgedbollinger.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/10/2013 21:11:39
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMEHEDGEDBOLLINGER_H_
#define _FRAMEHEDGEDBOLLINGER_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/splitter.h"
#include "wx/treectrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxSplitterWindow;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAMEHEDGEDBOLLINGER 10082
#define ID_SPLITTERWINDOW1 10083
#define ID_TREECTRL 10084
#define ID_PANEL4 10085
#define ID_PANEL5 10086
#define ID_PANEL6 10087
#define ID_PANEL7 10088
#define ID_GAUGE 10089
#define SYMBOL_FRAMEHEDGEDBOLLINGER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMEHEDGEDBOLLINGER_TITLE _("FrameHedgedBollinger")
#define SYMBOL_FRAMEHEDGEDBOLLINGER_IDNAME ID_FRAMEHEDGEDBOLLINGER
#define SYMBOL_FRAMEHEDGEDBOLLINGER_SIZE wxSize(400, 300)
#define SYMBOL_FRAMEHEDGEDBOLLINGER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameHedgedBollinger class declaration
 */

class FrameHedgedBollinger: public wxFrame
{    
    DECLARE_CLASS( FrameHedgedBollinger )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameHedgedBollinger();
    FrameHedgedBollinger( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEHEDGEDBOLLINGER_IDNAME, const wxString& caption = SYMBOL_FRAMEHEDGEDBOLLINGER_TITLE, const wxPoint& pos = SYMBOL_FRAMEHEDGEDBOLLINGER_POSITION, const wxSize& size = SYMBOL_FRAMEHEDGEDBOLLINGER_SIZE, long style = SYMBOL_FRAMEHEDGEDBOLLINGER_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEHEDGEDBOLLINGER_IDNAME, const wxString& caption = SYMBOL_FRAMEHEDGEDBOLLINGER_TITLE, const wxPoint& pos = SYMBOL_FRAMEHEDGEDBOLLINGER_POSITION, const wxSize& size = SYMBOL_FRAMEHEDGEDBOLLINGER_SIZE, long style = SYMBOL_FRAMEHEDGEDBOLLINGER_STYLE );

    /// Destructor
    ~FrameHedgedBollinger();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameHedgedBollinger event handler declarations

////@end FrameHedgedBollinger event handler declarations

////@begin FrameHedgedBollinger member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameHedgedBollinger member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameHedgedBollinger member variables
    wxBoxSizer* m_sizerFrame;
    wxBoxSizer* m_sizerFrameRow1;
    wxSplitterWindow* m_splitterRow1;
    wxPanel* m_panelSplitterRight;
    wxBoxSizer* m_sizerSplitterRight;
    wxPanel* m_panelProvider;
    wxPanel* m_panelLog;
    wxBoxSizer* m_sizerChart;
////@end FrameHedgedBollinger member variables
};

#endif
    // _FRAMEHEDGEDBOLLINGER_H_
