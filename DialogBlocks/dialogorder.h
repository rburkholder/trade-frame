/////////////////////////////////////////////////////////////////////////////
// Name:        dialogorder.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     13/11/2013 16:16:39
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOGORDER_H_
#define _DIALOGORDER_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/statline.h"
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
#define ID_DIALOGORDER 10117
#define ID_ChcBuySell 10118
#define ID_TxtQuantity 10119
#define ID_ChcDayGtc 10120
#define ID_ChcLmtMktStp 10121
#define ID_TxtPrice1 10122
#define SYMBOL_DIALOGORDER_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGORDER_TITLE _("Order Creation")
#define SYMBOL_DIALOGORDER_IDNAME ID_DIALOGORDER
#define SYMBOL_DIALOGORDER_SIZE wxSize(399, 299)
#define SYMBOL_DIALOGORDER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DialogOrder class declaration
 */

class DialogOrder: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DialogOrder )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DialogOrder();
    DialogOrder( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGORDER_IDNAME, const wxString& caption = SYMBOL_DIALOGORDER_TITLE, const wxPoint& pos = SYMBOL_DIALOGORDER_POSITION, const wxSize& size = SYMBOL_DIALOGORDER_SIZE, long style = SYMBOL_DIALOGORDER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGORDER_IDNAME, const wxString& caption = SYMBOL_DIALOGORDER_TITLE, const wxPoint& pos = SYMBOL_DIALOGORDER_POSITION, const wxSize& size = SYMBOL_DIALOGORDER_SIZE, long style = SYMBOL_DIALOGORDER_STYLE );

    /// Destructor
    ~DialogOrder();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DialogOrder event handler declarations

////@end DialogOrder event handler declarations

////@begin DialogOrder member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DialogOrder member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DialogOrder member variables
    wxChoice* m_choiceBuySell;
    wxTextCtrl* m_txtQuantity;
    wxChoice* m_choiceDayGtc;
    wxChoice* m_choiceLmtMktStp;
    wxTextCtrl* m_txtPrice1;
    wxButton* m_btnSubmit;
////@end DialogOrder member variables
};

#endif
    // _DIALOGORDER_H_
