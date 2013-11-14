/////////////////////////////////////////////////////////////////////////////
// Name:        panelbaskettradingmain.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     22/12/2012 21:48:11
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELBASKETTRADINGMAIN_H_
#define _PANELBASKETTRADINGMAIN_H_


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
#define ID_PANELBASKETTRADINGMAIN 10066
#define ID_BtnStart 10067
#define ID_BtnExitPositions 10068
#define ID_BtnStop 10069
#define ID_BtnSave 10070
#define SYMBOL_PANELBASKETTRADINGMAIN_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELBASKETTRADINGMAIN_TITLE _("PanelBasketTradingMain")
#define SYMBOL_PANELBASKETTRADINGMAIN_IDNAME ID_PANELBASKETTRADINGMAIN
#define SYMBOL_PANELBASKETTRADINGMAIN_SIZE wxSize(400, 300)
#define SYMBOL_PANELBASKETTRADINGMAIN_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelBasketTradingMain class declaration
 */

class PanelBasketTradingMain: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelBasketTradingMain )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelBasketTradingMain();
    PanelBasketTradingMain( wxWindow* parent, wxWindowID id = SYMBOL_PANELBASKETTRADINGMAIN_IDNAME, const wxPoint& pos = SYMBOL_PANELBASKETTRADINGMAIN_POSITION, const wxSize& size = SYMBOL_PANELBASKETTRADINGMAIN_SIZE, long style = SYMBOL_PANELBASKETTRADINGMAIN_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELBASKETTRADINGMAIN_IDNAME, const wxPoint& pos = SYMBOL_PANELBASKETTRADINGMAIN_POSITION, const wxSize& size = SYMBOL_PANELBASKETTRADINGMAIN_SIZE, long style = SYMBOL_PANELBASKETTRADINGMAIN_STYLE );

    /// Destructor
    ~PanelBasketTradingMain();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelBasketTradingMain event handler declarations

////@end PanelBasketTradingMain event handler declarations

////@begin PanelBasketTradingMain member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelBasketTradingMain member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelBasketTradingMain member variables
    wxButton* m_btnStart;
    wxButton* m_btnExitPositions;
    wxButton* m_btnStop;
    wxButton* m_btnSave;
////@end PanelBasketTradingMain member variables
};

#endif
    // _PANELBASKETTRADINGMAIN_H_
