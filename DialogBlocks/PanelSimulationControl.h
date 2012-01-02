/////////////////////////////////////////////////////////////////////////////
// Name:        PanelSimulationControl.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     06/11/2011 11:46:53
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELSIMULATIONCONTROL_H_
#define _PANELSIMULATIONCONTROL_H_


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
#define ID_PANELSIMULATIONCONTROL 10032
#define ID_TEXT_INSTRUMENTNAME 10034
#define ID_TEXT_GROUPDIRECTORY 10035
#define ID_BTN_STARTSIM 10033
#define ID_STATIC_RESULT 10036
#define ID_GAUGE_PROGRESS 10037
#define ID_BTN_DRAWCHART 10045
#define SYMBOL_PANELSIMULATIONCONTROL_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELSIMULATIONCONTROL_TITLE _("Simulation Control")
#define SYMBOL_PANELSIMULATIONCONTROL_IDNAME ID_PANELSIMULATIONCONTROL
#define SYMBOL_PANELSIMULATIONCONTROL_SIZE wxSize(400, 300)
#define SYMBOL_PANELSIMULATIONCONTROL_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelSimulationControl class declaration
 */

class PanelSimulationControl: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelSimulationControl )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelSimulationControl();
    PanelSimulationControl( wxWindow* parent, wxWindowID id = SYMBOL_PANELSIMULATIONCONTROL_IDNAME, const wxPoint& pos = SYMBOL_PANELSIMULATIONCONTROL_POSITION, const wxSize& size = SYMBOL_PANELSIMULATIONCONTROL_SIZE, long style = SYMBOL_PANELSIMULATIONCONTROL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELSIMULATIONCONTROL_IDNAME, const wxPoint& pos = SYMBOL_PANELSIMULATIONCONTROL_POSITION, const wxSize& size = SYMBOL_PANELSIMULATIONCONTROL_SIZE, long style = SYMBOL_PANELSIMULATIONCONTROL_STYLE );

    /// Destructor
    ~PanelSimulationControl();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelSimulationControl event handler declarations

////@end PanelSimulationControl event handler declarations

////@begin PanelSimulationControl member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelSimulationControl member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelSimulationControl member variables
    wxTextCtrl* m_txtInstrumentName;
    wxTextCtrl* m_txtGroupDirectory;
    wxStaticText* m_staticResult;
    wxGauge* m_gaugeProgress;
////@end PanelSimulationControl member variables
};

#endif
    // _PANELSIMULATIONCONTROL_H_
