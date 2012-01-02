/////////////////////////////////////////////////////////////////////////////
// Name:        dialogmanualorder.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     27/09/2011 21:09:43
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOGMANUALORDER_H_
#define _DIALOGMANUALORDER_H_


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
#define ID_DIALOGMANUALORDER 10028
#define ID_LblInstrumentSymbol 10011
#define ID_TxtInstrumentSymbol 10007
#define ID_LblInstrumentName 10025
#define ID_LblQuantity 10012
#define ID_TxtQuantity 10008
#define ID_LblPrice1 10013
#define ID_TxtPrice1 10009
#define ID_LblPrice2 10014
#define ID_TxtPrice2 10010
#define ID_BtnOrderTypeMarket 10004
#define ID_BtnOrderTypeLimit 10005
#define ID_BtnOrderTypeStop 10006
#define ID_BtnBuy 10001
#define ID_BtnSell 10002
#define ID_BtnCancel 10003
#define SYMBOL_DIALOGMANUALORDER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGMANUALORDER_TITLE _("Manual Order")
#define SYMBOL_DIALOGMANUALORDER_IDNAME ID_DIALOGMANUALORDER
#define SYMBOL_DIALOGMANUALORDER_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGMANUALORDER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DialogManualOrder class declaration
 */

class DialogManualOrder: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DialogManualOrder )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DialogManualOrder();
    DialogManualOrder( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGMANUALORDER_IDNAME, const wxString& caption = SYMBOL_DIALOGMANUALORDER_TITLE, const wxPoint& pos = SYMBOL_DIALOGMANUALORDER_POSITION, const wxSize& size = SYMBOL_DIALOGMANUALORDER_SIZE, long style = SYMBOL_DIALOGMANUALORDER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGMANUALORDER_IDNAME, const wxString& caption = SYMBOL_DIALOGMANUALORDER_TITLE, const wxPoint& pos = SYMBOL_DIALOGMANUALORDER_POSITION, const wxSize& size = SYMBOL_DIALOGMANUALORDER_SIZE, long style = SYMBOL_DIALOGMANUALORDER_STYLE );

    /// Destructor
    ~DialogManualOrder();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DialogManualOrder event handler declarations

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TxtInstrumentSymbol
    void OnTxtInstrumentSymbolTextUpdated( wxCommandEvent& event );

////@end DialogManualOrder event handler declarations

////@begin DialogManualOrder member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DialogManualOrder member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DialogManualOrder member variables
    wxTextCtrl* m_txtInstrumentSymbol;
    wxStaticText* m_txtInstrumentName;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnCancel;
    wxStaticText* m_txtStatus;
////@end DialogManualOrder member variables
};

#endif
    // _DIALOGMANUALORDER_H_
