/////////////////////////////////////////////////////////////////////////////
// Name:        panelportfolioposition.cpp
// Purpose:     
// Author:      Raymond P. Burkholder
// Modified by: 
// Created:     27/10/2013 20:40:05
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
////@end includes

#include "panelportfolioposition.h"

////@begin XPM images
////@end XPM images


/*
 * PanelPortfolioPosition type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PanelPortfolioPosition, wxPanel )


/*
 * PanelPortfolioPosition event table definition
 */

BEGIN_EVENT_TABLE( PanelPortfolioPosition, wxPanel )

////@begin PanelPortfolioPosition event table entries
////@end PanelPortfolioPosition event table entries

END_EVENT_TABLE()


/*
 * PanelPortfolioPosition constructors
 */

PanelPortfolioPosition::PanelPortfolioPosition()
{
    Init();
}

PanelPortfolioPosition::PanelPortfolioPosition( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * PanelPortfolioPosition creator
 */

bool PanelPortfolioPosition::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PanelPortfolioPosition creation
    wxPanel::Create( parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PanelPortfolioPosition creation
    return true;
}


/*
 * PanelPortfolioPosition destructor
 */

PanelPortfolioPosition::~PanelPortfolioPosition()
{
////@begin PanelPortfolioPosition destruction
////@end PanelPortfolioPosition destruction
}


/*
 * Member initialisation
 */

void PanelPortfolioPosition::Init()
{
////@begin PanelPortfolioPosition member initialisation
    m_sizerMain = NULL;
    m_sizerPortfolio = NULL;
    m_lblIdPortfolio = NULL;
    m_lblCurrency = NULL;
    m_gridPortfolioStats = NULL;
    m_txtUnRealizedPL = NULL;
    m_txtCommission = NULL;
    m_txtRealizedPL = NULL;
    m_txtTotal = NULL;
    m_lblDescription = NULL;
    m_txtDescription = NULL;
    m_gridPositions = NULL;
////@end PanelPortfolioPosition member initialisation
}


/*
 * Control creation for PanelPortfolioPosition
 */

void PanelPortfolioPosition::CreateControls()
{    
////@begin PanelPortfolioPosition content construction
    PanelPortfolioPosition* itemPanel1 = this;

    m_sizerMain = new wxBoxSizer(wxVERTICAL);
    itemPanel1->SetSizer(m_sizerMain);

    m_sizerPortfolio = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(m_sizerPortfolio, 0, wxALIGN_LEFT|wxALL, 2);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_sizerPortfolio->Add(itemBoxSizer4, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 1);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_LEFT|wxALL, 0);

    m_lblIdPortfolio = new wxStaticText( itemPanel1, ID_LblIdPortfolio, _("portfolio"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblIdPortfolio, 0, wxALIGN_LEFT|wxALL, 5);

    m_lblCurrency = new wxStaticText( itemPanel1, ID_LblCurrency, _("currency"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer5->Add(m_lblCurrency, 0, wxALIGN_LEFT|wxALL, 5);

    m_gridPortfolioStats = new wxFlexGridSizer(2, 4, 0, 0);
    m_sizerPortfolio->Add(m_gridPortfolioStats, 1, wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText9 = new wxStaticText( itemPanel1, ID_LblUnrealizedPL, _("UnRealized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtUnRealizedPL = new wxTextCtrl( itemPanel1, ID_TxtUnRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtUnRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText11 = new wxStaticText( itemPanel1, ID_LblCommission, _("Commission:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtCommission = new wxTextCtrl( itemPanel1, ID_TxtCommission, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtCommission, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText13 = new wxStaticText( itemPanel1, ID_LblRealizedPL, _("Realized PL:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText13, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtRealizedPL = new wxTextCtrl( itemPanel1, ID_TxtRealizedPL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtRealizedPL, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    wxStaticText* itemStaticText15 = new wxStaticText( itemPanel1, ID_LblTotal, _("Total:"), wxDefaultPosition, wxDefaultSize, 0 );
    m_gridPortfolioStats->Add(itemStaticText15, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_txtTotal = new wxTextCtrl( itemPanel1, ID_TxtTotal, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY|wxTE_RIGHT );
    m_gridPortfolioStats->Add(m_txtTotal, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 1);

    m_gridPortfolioStats->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(itemBoxSizer17, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 2);

    m_lblDescription = new wxStaticText( itemPanel1, ID_LblDescription, _("Desc:"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT );
    itemBoxSizer17->Add(m_lblDescription, 0, wxALIGN_TOP|wxALL, 2);

    m_txtDescription = new wxTextCtrl( itemPanel1, ID_TxtDescription, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
    itemBoxSizer17->Add(m_txtDescription, 1, wxGROW|wxLEFT|wxRIGHT, 1);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    m_sizerMain->Add(itemBoxSizer20, 0, wxGROW|wxALL, 5);

    m_gridPositions = new wxGrid( itemPanel1, ID_GridPositions, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE|wxHSCROLL|wxVSCROLL );
    m_gridPositions->SetDefaultColSize(75);
    m_gridPositions->SetDefaultRowSize(22);
    m_gridPositions->SetColLabelSize(22);
    m_gridPositions->SetRowLabelSize(20);
    m_gridPositions->CreateGrid(1, 5, wxGrid::wxGridSelectCells);
    itemBoxSizer20->Add(m_gridPositions, 1, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM|wxADJUST_MINSIZE, 2);

////@end PanelPortfolioPosition content construction
}


/*
 * Should we show tooltips?
 */

bool PanelPortfolioPosition::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap PanelPortfolioPosition::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PanelPortfolioPosition bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PanelPortfolioPosition bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon PanelPortfolioPosition::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PanelPortfolioPosition icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PanelPortfolioPosition icon retrieval
}
