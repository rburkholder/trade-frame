/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfoliostats.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/12/2012 21:42:27
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELPORTFOLIOSTATS_H_
#define _PANELPORTFOLIOSTATS_H_


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
#define ID_PANELPORTFOLIOSTATS 10071
#define ID_TxtPortfolioLow 10072
#define ID_TxtPortfolioCurrent 10073
#define ID_TxtPortfolioHigh 10074
#define SYMBOL_PANELPORTFOLIOSTATS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPORTFOLIOSTATS_TITLE _("PanelPortfolioStats")
#define SYMBOL_PANELPORTFOLIOSTATS_IDNAME ID_PANELPORTFOLIOSTATS
#define SYMBOL_PANELPORTFOLIOSTATS_SIZE wxSize(400, 300)
#define SYMBOL_PANELPORTFOLIOSTATS_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelPortfolioStats class declaration
 */

class PanelPortfolioStats: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelPortfolioStats )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelPortfolioStats();
    PanelPortfolioStats( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE, long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOSTATS_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOSTATS_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOSTATS_SIZE, long style = SYMBOL_PANELPORTFOLIOSTATS_STYLE );

    /// Destructor
    ~PanelPortfolioStats();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelPortfolioStats event handler declarations

////@end PanelPortfolioStats event handler declarations

////@begin PanelPortfolioStats member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelPortfolioStats member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelPortfolioStats member variables
    wxTextCtrl* m_txtPortfolioLow;
    wxTextCtrl* m_txtPortfolioCurrent;
    wxTextCtrl* m_txtPortfolioHigh;
////@end PanelPortfolioStats member variables
};

#endif
    // _PANELPORTFOLIOSTATS_H_
