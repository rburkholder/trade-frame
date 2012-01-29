/////////////////////////////////////////////////////////////////////////////
// Name:        providers.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/08/2011 23:28:32
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PROVIDERS_H_
#define _PROVIDERS_H_


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
#define ID_PROVIDERS 10015
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
#define SYMBOL_PROVIDERS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_PROVIDERS_TITLE _("Providers")
#define SYMBOL_PROVIDERS_IDNAME ID_PROVIDERS
#define SYMBOL_PROVIDERS_SIZE wxSize(200, 150)
#define SYMBOL_PROVIDERS_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * Providers class declaration
 */

class Providers: public wxWindow
{    
    DECLARE_DYNAMIC_CLASS( Providers )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Providers();
    Providers(wxWindow* parent, wxWindowID id = ID_PROVIDERS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(100, 100), long style = wxSIMPLE_BORDER);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = ID_PROVIDERS, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(100, 100), long style = wxSIMPLE_BORDER);

    /// Destructor
    ~Providers();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Providers event handler declarations

////@end Providers event handler declarations

////@begin Providers member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Providers member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Providers member variables
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
////@end Providers member variables
};

/*!
 * Providers class declaration
 */

class Providers: public wxFrame
{    
    DECLARE_CLASS( Providers )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Providers();
    Providers( wxWindow* parent, wxWindowID id = SYMBOL_PROVIDERS_IDNAME, const wxString& caption = SYMBOL_PROVIDERS_TITLE, const wxPoint& pos = SYMBOL_PROVIDERS_POSITION, const wxSize& size = SYMBOL_PROVIDERS_SIZE, long style = SYMBOL_PROVIDERS_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PROVIDERS_IDNAME, const wxString& caption = SYMBOL_PROVIDERS_TITLE, const wxPoint& pos = SYMBOL_PROVIDERS_POSITION, const wxSize& size = SYMBOL_PROVIDERS_SIZE, long style = SYMBOL_PROVIDERS_STYLE );

    /// Destructor
    ~Providers();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Providers event handler declarations
////@end Providers event handler declarations

////@begin Providers member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Providers member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Providers member variables
////@end Providers member variables
};

#endif
    // _PROVIDERS_H_
