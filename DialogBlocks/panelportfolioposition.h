/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfolioposition.h
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     27/10/2013 20:40:05
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _PANELPORTFOLIOPOSITION_H_
#define _PANELPORTFOLIOPOSITION_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/grid.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
class wxFlexGridSizer;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_PANELPORTFOLIOPOSITION 10090
#define ID_LblIdPortfolio 10091
#define ID_LblCurrency 10092
#define ID_LblDescription 10093
#define ID_LblUnrealizedPL 10101
#define ID_TxtUnRealizedPL 10094
#define ID_LblCommission 10099
#define ID_TxtCommission 10096
#define ID_LblRealizedPL 10102
#define ID_TxtRealizedPL 10095
#define ID_LblTotal 10100
#define ID_TxtTotal 10097
#define ID_GridPositions 10098
#define SYMBOL_PANELPORTFOLIOPOSITION_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PANELPORTFOLIOPOSITION_TITLE _("PanelPortfolioPosition")
#define SYMBOL_PANELPORTFOLIOPOSITION_IDNAME ID_PANELPORTFOLIOPOSITION
#define SYMBOL_PANELPORTFOLIOPOSITION_SIZE wxSize(400, 300)
#define SYMBOL_PANELPORTFOLIOPOSITION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * PanelPortfolioPosition class declaration
 */

class PanelPortfolioPosition: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( PanelPortfolioPosition )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    PanelPortfolioPosition();
    PanelPortfolioPosition( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOPOSITION_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOPOSITION_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOPOSITION_SIZE, long style = SYMBOL_PANELPORTFOLIOPOSITION_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_PANELPORTFOLIOPOSITION_IDNAME, const wxPoint& pos = SYMBOL_PANELPORTFOLIOPOSITION_POSITION, const wxSize& size = SYMBOL_PANELPORTFOLIOPOSITION_SIZE, long style = SYMBOL_PANELPORTFOLIOPOSITION_STYLE );

    /// Destructor
    ~PanelPortfolioPosition();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin PanelPortfolioPosition event handler declarations

////@end PanelPortfolioPosition event handler declarations

////@begin PanelPortfolioPosition member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end PanelPortfolioPosition member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin PanelPortfolioPosition member variables
    wxBoxSizer* m_sizerMain;
    wxBoxSizer* m_sizerPortfolio;
    wxStaticText* m_lblIdPortfolio;
    wxStaticText* m_lblCurrency;
    wxStaticText* m_lblDescription;
    wxFlexGridSizer* m_gridPortfolioStats;
    wxTextCtrl* m_txtUnRealizedPL;
    wxTextCtrl* m_txtCommission;
    wxTextCtrl* m_txtRealizedPL;
    wxTextCtrl* m_txtTotal;
    wxGrid* m_gridPositions;
////@end PanelPortfolioPosition member variables
};

#endif
    // _PANELPORTFOLIOPOSITION_H_
