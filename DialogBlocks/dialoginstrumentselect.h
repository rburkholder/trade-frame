/////////////////////////////////////////////////////////////////////////////
// Name:        dialoginstrumentselect.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     11/11/2013 11:32:03
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOGINSTRUMENTSELECT_H_
#define _DIALOGINSTRUMENTSELECT_H_


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
#define ID_DIALOGINSTRUMENTSELECT 10105
#define ID_CBSymbol 10110
#define ID_LblDescription 10109
#define SYMBOL_DIALOGINSTRUMENTSELECT_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGINSTRUMENTSELECT_TITLE _("Select Instrument")
#define SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME ID_DIALOGINSTRUMENTSELECT
#define SYMBOL_DIALOGINSTRUMENTSELECT_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGINSTRUMENTSELECT_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DialogInstrumentSelect class declaration
 */

class DialogInstrumentSelect: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DialogInstrumentSelect )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DialogInstrumentSelect();
    DialogInstrumentSelect( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME, const wxString& caption = SYMBOL_DIALOGINSTRUMENTSELECT_TITLE, const wxPoint& pos = SYMBOL_DIALOGINSTRUMENTSELECT_POSITION, const wxSize& size = SYMBOL_DIALOGINSTRUMENTSELECT_SIZE, long style = SYMBOL_DIALOGINSTRUMENTSELECT_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGINSTRUMENTSELECT_IDNAME, const wxString& caption = SYMBOL_DIALOGINSTRUMENTSELECT_TITLE, const wxPoint& pos = SYMBOL_DIALOGINSTRUMENTSELECT_POSITION, const wxSize& size = SYMBOL_DIALOGINSTRUMENTSELECT_SIZE, long style = SYMBOL_DIALOGINSTRUMENTSELECT_STYLE );

    /// Destructor
    ~DialogInstrumentSelect();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DialogInstrumentSelect event handler declarations

////@end DialogInstrumentSelect event handler declarations

////@begin DialogInstrumentSelect member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DialogInstrumentSelect member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DialogInstrumentSelect member variables
    wxComboBox* m_cbSymbol;
    wxStaticText* m_lblDescription;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;
////@end DialogInstrumentSelect member variables
};

#endif
    // _DIALOGINSTRUMENTSELECT_H_
