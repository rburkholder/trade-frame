/////////////////////////////////////////////////////////////////////////////
// Name:        framelivechart.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/09/2013 16:21:50
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _FRAMELIVECHART_H_
#define _FRAMELIVECHART_H_


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
class wxTreeCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_FRAMELIVECHART 10075
#define IDsplitter 10076
#define IDtreeLeft 10077
#define IDpanelRight 10078
#define IDpanelMiniLeft 10079
#define IDpanelMiniRight 10080
#define IDpanelrightBottom 10081
#define SYMBOL_FRAMELIVECHART_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FRAMELIVECHART_TITLE _("FrameLiveChart")
#define SYMBOL_FRAMELIVECHART_IDNAME ID_FRAMELIVECHART
#define SYMBOL_FRAMELIVECHART_SIZE wxSize(800, 600)
#define SYMBOL_FRAMELIVECHART_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameLiveChart class declaration
 */

class FrameLiveChart: public wxFrame
{    
    DECLARE_CLASS( FrameLiveChart )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameLiveChart();
    FrameLiveChart( wxWindow* parent, wxWindowID id = SYMBOL_FRAMELIVECHART_IDNAME, const wxString& caption = SYMBOL_FRAMELIVECHART_TITLE, const wxPoint& pos = SYMBOL_FRAMELIVECHART_POSITION, const wxSize& size = SYMBOL_FRAMELIVECHART_SIZE, long style = SYMBOL_FRAMELIVECHART_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMELIVECHART_IDNAME, const wxString& caption = SYMBOL_FRAMELIVECHART_TITLE, const wxPoint& pos = SYMBOL_FRAMELIVECHART_POSITION, const wxSize& size = SYMBOL_FRAMELIVECHART_SIZE, long style = SYMBOL_FRAMELIVECHART_STYLE );

    /// Destructor
    ~FrameLiveChart();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameLiveChart event handler declarations

////@end FrameLiveChart event handler declarations

////@begin FrameLiveChart member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameLiveChart member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameLiveChart member variables
    wxBoxSizer* sizerMainH;
    wxSplitterWindow* splitter;
    wxTreeCtrl* tree;
    wxPanel* panelSplitterRightPanel;
    wxBoxSizer* sizerRight;
    wxBoxSizer* m_sizerControls;
    wxPanel* m_pPanelProviderControl;
    wxStaticText* staticText;
    wxPanel* m_pPanelLogging;
    wxPanel* m_winChart;
////@end FrameLiveChart member variables
};

#endif
    // _FRAMELIVECHART_H_
