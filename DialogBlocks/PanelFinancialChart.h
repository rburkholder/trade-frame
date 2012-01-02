/////////////////////////////////////////////////////////////////////////////
// Name:        PanelFinancialChart.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     11/11/2011 21:43:29
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELFINANCIALCHART_H_
#define _PANELFINANCIALCHART_H_


/*!
 * Includes
 */

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxWindow;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PANELFINANCIALCHART 10042
#define ID_CHART 10043
#define SYMBOL_PANELFINANCIALCHART_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELFINANCIALCHART_TITLE _("PanelFinancialChart")
#define SYMBOL_PANELFINANCIALCHART_IDNAME ID_PANELFINANCIALCHART
#define SYMBOL_PANELFINANCIALCHART_SIZE wxSize(400, 300)
#define SYMBOL_PANELFINANCIALCHART_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelFinancialChart class declaration
 */

class PanelFinancialChart: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelFinancialChart )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelFinancialChart();
    PanelFinancialChart( wxWindow* parent, wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME, const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION, const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE, long style = SYMBOL_PANELFINANCIALCHART_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELFINANCIALCHART_IDNAME, const wxPoint& pos = SYMBOL_PANELFINANCIALCHART_POSITION, const wxSize& size = SYMBOL_PANELFINANCIALCHART_SIZE, long style = SYMBOL_PANELFINANCIALCHART_STYLE );

    /// Destructor
    ~PanelFinancialChart();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelFinancialChart event handler declarations

    /// wxEVT_SIZE event handler for ID_CHART
    void OnSize( wxSizeEvent& event );

    /// wxEVT_PAINT event handler for ID_CHART
    void OnPaint( wxPaintEvent& event );

////@end PanelFinancialChart event handler declarations

////@begin PanelFinancialChart member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelFinancialChart member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelFinancialChart member variables
    wxWindow* m_winChart;
////@end PanelFinancialChart member variables
};

#endif
    // _PANELFINANCIALCHART_H_
