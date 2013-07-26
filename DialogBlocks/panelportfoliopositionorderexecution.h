/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfoliopositionorderexecution.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     03/10/2011 22:09:32
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELPORTFOLIOPOSITIONORDEREXECUTION_H_
#define _PANELPORTFOLIOPOSITIONORDEREXECUTION_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/splitter.h"
#include "wx/treectrl.h"
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
#define ID_PANELPORTFOLIOPOSITIONORDEREXECUTION 10029
#define ID_SPLITTERWINDOW 10030
#define ID_TREECTRL 10075
#define ID_PANEL 10031
#define SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_TITLE _("PanelPortfolioPositionOrderExecution")
#define SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_IDNAME ID_PANELPORTFOLIOPOSITIONORDEREXECUTION
#define SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_SIZE wxSize(400, 300)
#define SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelPortfolioPositionOrderExecution class declaration
 */

class PanelPortfolioPositionOrderExecution: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelPortfolioPositionOrderExecution )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelPortfolioPositionOrderExecution();
    PanelPortfolioPositionOrderExecution( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_SIZE, long style = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_SIZE, long style = SYMBOL_PANELPORTFOLIOPOSITIONORDEREXECUTION_STYLE );

    /// Destructor
    ~PanelPortfolioPositionOrderExecution();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelPortfolioPositionOrderExecution event handler declarations

////@end PanelPortfolioPositionOrderExecution event handler declarations

////@begin PanelPortfolioPositionOrderExecution member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelPortfolioPositionOrderExecution member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelPortfolioPositionOrderExecution member variables
////@end PanelPortfolioPositionOrderExecution member variables
};

#endif
    // _PANELPORTFOLIOPOSITIONORDEREXECUTION_H_
