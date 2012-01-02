/////////////////////////////////////////////////////////////////////////////
// Name:        trading.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     04/09/2011 15:19:36
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _TRADING_H_
#define _TRADING_H_


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
#define ID_TRADING 10000
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
#define SYMBOL_FRAMEMANUALORDER_STYLE wxDEFAULT_FRAME_STYLE|wxSIMPLE_BORDER
#define SYMBOL_FRAMEMANUALORDER_TITLE _("Manual Order")
#define SYMBOL_FRAMEMANUALORDER_IDNAME ID_TRADING
#define SYMBOL_FRAMEMANUALORDER_SIZE wxSize(500, 120)
#define SYMBOL_FRAMEMANUALORDER_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * FrameManualOrder class declaration
 */

class FrameManualOrder: public wxFrame
{    
    DECLARE_CLASS( FrameManualOrder )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    FrameManualOrder();
    FrameManualOrder( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEMANUALORDER_IDNAME, const wxString& caption = SYMBOL_FRAMEMANUALORDER_TITLE, const wxPoint& pos = SYMBOL_FRAMEMANUALORDER_POSITION, const wxSize& size = SYMBOL_FRAMEMANUALORDER_SIZE, long style = SYMBOL_FRAMEMANUALORDER_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FRAMEMANUALORDER_IDNAME, const wxString& caption = SYMBOL_FRAMEMANUALORDER_TITLE, const wxPoint& pos = SYMBOL_FRAMEMANUALORDER_POSITION, const wxSize& size = SYMBOL_FRAMEMANUALORDER_SIZE, long style = SYMBOL_FRAMEMANUALORDER_STYLE );

    /// Destructor
    ~FrameManualOrder();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin FrameManualOrder event handler declarations

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TxtInstrumentSymbol
    void OnTxtInstrumentSymbolTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_ENTER event handler for ID_TxtInstrumentSymbol
    void OnTxtInstrumentSymbolEnter( wxCommandEvent& event );

    /// wxEVT_IDLE event handler for ID_TxtInstrumentSymbol
    void OnIdle( wxIdleEvent& event );

    /// wxEVT_LEFT_UP event handler for ID_BtnOrderTypeMarket
    void OnLeftUp( wxMouseEvent& event );

////@end FrameManualOrder event handler declarations

////@begin FrameManualOrder member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FrameManualOrder member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FrameManualOrder member variables
    wxTextCtrl* m_txtInstrumentSymbol;
    wxStaticText* m_txtInstrumentName;
    wxButton* m_btnBuy;
    wxButton* m_btnSell;
    wxButton* m_btnCancel;
////@end FrameManualOrder member variables
};

#endif
    // _TRADING_H_
