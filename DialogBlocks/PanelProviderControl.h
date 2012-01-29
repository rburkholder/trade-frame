/////////////////////////////////////////////////////////////////////////////
// Name:        PanelProviderControl.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/01/2012 14:37:58
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELPROVIDERCONTROL_H_
#define _PANELPROVIDERCONTROL_H_


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
#define ID_PANELPROVIDERCONTROL 10000
#define ID_CB_IQF_D1 10060
#define ID_CB_IQF_D2 10061
#define ID_CB_IQF_X 10062
#define ID_BtnIQFeed 10022
#define wxID_LblIQFeed 10019
#define ID_CB_IB_D1 10018
#define ID_CB_IB_D2 10023
#define ID_CB_IB_X 10049
#define ID_BtnInteractiveBrokers 10016
#define wxID_LblInteractiveBrokers 10020
#define ID_CB_SIM_D1 10024
#define ID_CB_SIM_D2 10046
#define ID_CB_SIM_X 10050
#define ID_BtnSimulation 10017
#define wxID_LblSimulation 10021
#define SYMBOL_PANELPROVIDERCONTROL_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPROVIDERCONTROL_TITLE _("PanelProviderControl")
#define SYMBOL_PANELPROVIDERCONTROL_IDNAME ID_PANELPROVIDERCONTROL
#define SYMBOL_PANELPROVIDERCONTROL_SIZE wxSize(400, 300)
#define SYMBOL_PANELPROVIDERCONTROL_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelProviderControl class declaration
 */

class PanelProviderControl: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelProviderControl )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelProviderControl();
    PanelProviderControl( wxWindow* parent, wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME, const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION, const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE, long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPROVIDERCONTROL_IDNAME, const wxPoint& pos = SYMBOL_PANELPROVIDERCONTROL_POSITION, const wxSize& size = SYMBOL_PANELPROVIDERCONTROL_SIZE, long style = SYMBOL_PANELPROVIDERCONTROL_STYLE );

    /// Destructor
    ~PanelProviderControl();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelProviderControl event handler declarations

////@end PanelProviderControl event handler declarations

////@begin PanelProviderControl member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelProviderControl member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelProviderControl member variables
    wxCheckBox* m_cbIQFeedD1;
    wxCheckBox* m_cbIQFeedD2;
    wxCheckBox* m_cbIQFeedX;
    wxButton* m_btnIQFeed;
    wxCheckBox* m_cbIBD1;
    wxCheckBox* m_cbIBD2;
    wxCheckBox* m_cbIBX;
    wxButton* m_btnIB;
    wxCheckBox* m_cbSimD1;
    wxCheckBox* m_cbSimD2;
    wxCheckBox* m_cbSimX;
    wxButton* m_btnSimulator;
////@end PanelProviderControl member variables
};

#endif
    // _PANELPROVIDERCONTROL_H_
