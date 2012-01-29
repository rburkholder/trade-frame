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
#define ID_PANELOPTIONSELECTION 10052
#define ID_LBL_Underlying 10056
#define ID_TXT_Underlying 10053
#define ID_LBL_OPTIONNEARDATE 10057
#define ID_DATE_NearDate 10054
#define ID_LBL_OPTIONFARDATE 10058
#define ID_DATE_FarDate 10055
#define ID_BTN_START 10059
#define SYMBOL_PANELOPTIONSELECTION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELOPTIONSELECTION_TITLE _("Panel Option Selection")
#define SYMBOL_PANELOPTIONSELECTION_IDNAME ID_PANELOPTIONSELECTION
#define SYMBOL_PANELOPTIONSELECTION_SIZE wxSize(400, 300)
#define SYMBOL_PANELOPTIONSELECTION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelOptionSelection class declaration
 */

class PanelOptionSelection: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelOptionSelection )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelOptionSelection();
    PanelOptionSelection( wxWindow* parent, wxWindowID id = SYMBOL_PANELOPTIONSELECTION_IDNAME, const wxPoint& pos = SYMBOL_PANELOPTIONSELECTION_POSITION, const wxSize& size = SYMBOL_PANELOPTIONSELECTION_SIZE, long style = SYMBOL_PANELOPTIONSELECTION_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELOPTIONSELECTION_IDNAME, const wxPoint& pos = SYMBOL_PANELOPTIONSELECTION_POSITION, const wxSize& size = SYMBOL_PANELOPTIONSELECTION_SIZE, long style = SYMBOL_PANELOPTIONSELECTION_STYLE );

    /// Destructor
    ~PanelOptionSelection();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelOptionSelection event handler declarations

////@end PanelOptionSelection event handler declarations

////@begin PanelOptionSelection member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelOptionSelection member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelOptionSelection member variables
    wxTextCtrl* m_txtUnderlying;
    wxDatePickerCtrl* m_ctrlNearDate;
    wxDatePickerCtrl* m_ctrlFarDate;
    wxButton* m_btnStart;
////@end PanelOptionSelection member variables
};

#endif
    // _PANELOPTIONSELECTION_H_
