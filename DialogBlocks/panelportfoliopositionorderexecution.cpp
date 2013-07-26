/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfoliopositionorderexecution.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     03/10/2011 22:09:32
// RCS-ID:      
// Copyright:   (c) 2011 One Unified
// Licence:     
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "panelportfoliopositionorderexecution.h"

////@begin XPM images
////@end XPM images


/*
 * PanelPortfolioPositionOrderExecution type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelPortfolioPositionOrderExecution, wxPanel )


/*
 * PanelPortfolioPositionOrderExecution event table definition
 */

BEGIN_EVENT_TABLE( PanelPortfolioPositionOrderExecution, wxPanel )

////@begin PanelPortfolioPositionOrderExecution event table entries
////@end PanelPortfolioPositionOrderExecution event table entries

END_EVENT_TABLE()


/*
 * PanelPortfolioPositionOrderExecution constructors
 */

PanelPortfolioPositionOrderExecution::PanelPortfolioPositionOrderExecution()
{
    Init();
}

PanelPortfolioPositionOrderExecution::PanelPortfolioPositionOrderExecution( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelPortfolioPositionOrderExecution creator
 */

bool PanelPortfolioPositionOrderExecution::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelPortfolioPositionOrderExecution creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelPortfolioPositionOrderExecution creation
    return true;
}


/*
 * PanelPortfolioPositionOrderExecution destructor
 */

PanelPortfolioPositionOrderExecution::~PanelPortfolioPositionOrderExecution()
{
////@begin PanelPortfolioPositionOrderExecution destruction
////@end PanelPortfolioPositionOrderExecution destruction
}


/*
 * Member initialisation
 */

void PanelPortfolioPositionOrderExecution::Init()
{
////@begin PanelPortfolioPositionOrderExecution member initialisation
////@end PanelPortfolioPositionOrderExecution member initialisation
}


/*
 * Control creation for PanelPortfolioPositionOrderExecution
 */

void PanelPortfolioPositionOrderExecution::CreateControls()
{    
////@begin PanelPortfolioPositionOrderExecution content construction
    PanelPortfolioPositionOrderExecution* itemPanel1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(itemBoxSizer2);

    wxSplitterWindow* itemSplitterWindow3 = new wxSplitterWindow( itemPanel1, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    itemSplitterWindow3->SetMinimumPaneSize(0);

    wxTreeCtrl* itemTreeCtrl4 = new wxTreeCtrl( itemSplitterWindow3, ID_TREECTRL, wxDefaultPosition, wxSize(100, 100), wxTR_SINGLE );

    wxPanel* itemPanel5 = new wxPanel( itemSplitterWindow3, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxVERTICAL);
    itemPanel5->SetSizer(itemBoxSizer6);

    wxBoxSizer* itemBoxSizer7 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer7, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer8 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer6->Add(itemBoxSizer8, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    itemSplitterWindow3->SplitHorizontally(itemTreeCtrl4, itemPanel5, 50);
    itemBoxSizer2->Add(itemSplitterWindow3, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

////@end PanelPortfolioPositionOrderExecution content construction
}


/*
 * Should we show tooltips?
 */

bool PanelPortfolioPositionOrderExecution::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelPortfolioPositionOrderExecution::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelPortfolioPositionOrderExecution bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelPortfolioPositionOrderExecution bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelPortfolioPositionOrderExecution::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelPortfolioPositionOrderExecution icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelPortfolioPositionOrderExecution icon retrieval
}
