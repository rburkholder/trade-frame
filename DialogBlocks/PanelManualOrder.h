/////////////////////////////////////////////////////////////////////////////
// Name:        PanelManualOrder.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     29/01/2012 14:36:16
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELMANUALORDER_H_
#define _PANELMANUALORDER_H_


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
#define ID_PANELMANUALORDER 10047
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
#define SYMBOL_PANELMANUALORDER_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELMANUALORDER_TITLE _("PanelManualOrder")
#define SYMBOL_PANELMANUALORDER_IDNAME ID_PANELMANUALORDER
#define SYMBOL_PANELMANUALORDER_SIZE wxSize(400, 300)
#define SYMBOL_PANELMANUALORDER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelManualOrder class declaration
 */

class PanelManualOrder: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelManualOrder )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelManualOrder();
    PanelManualOrder( wxWindow* parent, wxWindowID id = SYMBOL_PANELMANUALORDER_IDNAME, const wxPoint& pos = SYMBOL_PANELMANUALORDER_POSITION, const wxSize& size = SYMBOL_PANELMANUALORDER_SIZE, long style = SYMBOL_PANELMANUALORDER_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELMANUALORDER_IDNAME, const wxPoint& pos = SYMBOL_PANELMANUALORDER_POSITION, const wxSize& size = SYMBOL_PANELMANUALORDER_SIZE, long style = SYMBOL_PANELMANUALORDER_STYLE );

    /// Destructor
    ~PanelManualOrder();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelManualOrder event handler declarations

////@end PanelManualOrder event handler declarations

////@begin PanelManualOrder member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelManualOrder member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelManualOrder member variables
    wxTextCtrl* m_txtInstrumentSymbol;
    wxStaticText* m_txtInstrumentName;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnCancel;
    wxStaticText* m_txtStatus;
////@end PanelManualOrder member variables
};

#endif
    // _PANELMANUALORDER_H_
