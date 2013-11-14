/////////////////////////////////////////////////////////////////////////////
// Name:        dialognewportfolio.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     12/11/2013 22:26:40
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DIALOGNEWPORTFOLIO_H_
#define _DIALOGNEWPORTFOLIO_H_


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
#define ID_DIALOGNEWPORTFOLIO 10106
#define ID_LblPortfolioId 10115
#define ID_TxtPortfolioId 10111
#define ID_LblDescription 10116
#define ID_TxtDescription 10112
#define SYMBOL_DIALOGNEWPORTFOLIO_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_DIALOGNEWPORTFOLIO_TITLE _("Add New Portfolio")
#define SYMBOL_DIALOGNEWPORTFOLIO_IDNAME ID_DIALOGNEWPORTFOLIO
#define SYMBOL_DIALOGNEWPORTFOLIO_SIZE wxSize(400, 300)
#define SYMBOL_DIALOGNEWPORTFOLIO_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DialogNewPortfolio class declaration
 */

class DialogNewPortfolio: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DialogNewPortfolio )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    DialogNewPortfolio();
    DialogNewPortfolio( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGNEWPORTFOLIO_IDNAME, const wxString& caption = SYMBOL_DIALOGNEWPORTFOLIO_TITLE, const wxPoint& pos = SYMBOL_DIALOGNEWPORTFOLIO_POSITION, const wxSize& size = SYMBOL_DIALOGNEWPORTFOLIO_SIZE, long style = SYMBOL_DIALOGNEWPORTFOLIO_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DIALOGNEWPORTFOLIO_IDNAME, const wxString& caption = SYMBOL_DIALOGNEWPORTFOLIO_TITLE, const wxPoint& pos = SYMBOL_DIALOGNEWPORTFOLIO_POSITION, const wxSize& size = SYMBOL_DIALOGNEWPORTFOLIO_SIZE, long style = SYMBOL_DIALOGNEWPORTFOLIO_STYLE );

    /// Destructor
    ~DialogNewPortfolio();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin DialogNewPortfolio event handler declarations

////@end DialogNewPortfolio event handler declarations

////@begin DialogNewPortfolio member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DialogNewPortfolio member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DialogNewPortfolio member variables
    wxTextCtrl* m_txtPortfolioId;
    wxTextCtrl* m_txtDescription;
    wxButton* m_btnOk;
    wxButton* m_btnCancel;
////@end DialogNewPortfolio member variables
};

#endif
    // _DIALOGNEWPORTFOLIO_H_
