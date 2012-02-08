/////////////////////////////////////////////////////////////////////////////
// Name:        PanelOptionSelection.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     23/01/2012 21:24:46
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELOPTIONSELECTION_H_
#define _PANELOPTIONSELECTION_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/datectrl.h"
#include "wx/dateevt.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxDatePickerCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PANELOPTIONSPARAMETERS 10052
#define ID_LBL_Underlying 10056
#define ID_TXT_Underlying 10053
#define ID_LBL_OPTIONNEARDATE 10057
#define ID_DATE_NearDate 10054
#define ID_LBL_OPTIONFARDATE 10058
#define ID_DATE_FarDate 10055
#define ID_BTN_START 10059
#define ID_BTN_STOP 10015
#define ID_BTN_SAVE 10028
#define SYMBOL_PANELOPTIONSPARAMETERS_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELOPTIONSPARAMETERS_TITLE _("Panel Options Parameters")
#define SYMBOL_PANELOPTIONSPARAMETERS_IDNAME ID_PANELOPTIONSPARAMETERS
#define SYMBOL_PANELOPTIONSPARAMETERS_SIZE wxSize(400, 300)
#define SYMBOL_PANELOPTIONSPARAMETERS_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelOptionsParameters class declaration
 */

class PanelOptionsParameters: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelOptionsParameters )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelOptionsParameters();
    PanelOptionsParameters( wxWindow* parent, wxWindowID id = SYMBOL_PANELOPTIONSPARAMETERS_IDNAME, const wxPoint& pos = SYMBOL_PANELOPTIONSPARAMETERS_POSITION, const wxSize& size = SYMBOL_PANELOPTIONSPARAMETERS_SIZE, long style = SYMBOL_PANELOPTIONSPARAMETERS_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELOPTIONSPARAMETERS_IDNAME, const wxPoint& pos = SYMBOL_PANELOPTIONSPARAMETERS_POSITION, const wxSize& size = SYMBOL_PANELOPTIONSPARAMETERS_SIZE, long style = SYMBOL_PANELOPTIONSPARAMETERS_STYLE );

    /// Destructor
    ~PanelOptionsParameters();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelOptionsParameters event handler declarations

////@end PanelOptionsParameters event handler declarations

////@begin PanelOptionsParameters member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelOptionsParameters member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelOptionsParameters member variables
    wxTextCtrl* m_txtUnderlying;
    wxDatePickerCtrl* m_ctrlNearDate;
    wxDatePickerCtrl* m_ctrlFarDate;
    wxButton* m_btnStart;
    wxButton* m_btnStop;
    wxButton* m_btnSave;
////@end PanelOptionsParameters member variables
};

#endif
    // _PANELOPTIONSELECTION_H_
